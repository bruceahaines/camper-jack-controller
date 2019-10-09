/*
  Camper Jack Controller
  GPIO of ESP32 are connected to a relay driver IC, driving 4 dual relays each used to actuate a ~100W
  DC12V motor in forward and reverse directions.
  The electric motors control a Bigfoot Camper's stabilizer jacks.

  This project has been tested as working on the ESP32 DOIT Devkit V1 development board.
  To compile the project, use VSCode with the PlatformIO extension.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPUpdate.h>
#include <Wire.h>
#include <FileUtils.h>
//#include <config.h>
#include <WebsocketUtils.h>
#include <globalDefines.h>


//char password[30] = "";

char hostName[30] = "camper";

//WifiServer wifiServer
WebServer webServer(80);   
// For OTA firmware updates                        
HTTPUpdate httpUpdateServer;
WebSocketsServer webSocketServer = WebSocketsServer(81);

//*****************************************************************************
// Setup                                                                      *
//*****************************************************************************
void setup(void) {

  Serial.begin(115200);
  Serial.printf("\n");
  Serial.setDebugOutput(false);  // Enable WiFi serial debug output
  Serial.printf("Camper Jack Controller is starting...\n");
  SPIFFS.begin();

    // GPIO 2 is also the Builtin LED on WEMOS D1 Mini and is active low.
  pinMode(LF_UP_PIN, OUTPUT);
  pinMode(LF_DOWN_PIN, OUTPUT);
  pinMode(RF_UP_PIN, OUTPUT);
  pinMode(RF_DOWN_PIN, OUTPUT);
  pinMode(LR_UP_PIN, OUTPUT);
  pinMode(LR_DOWN_PIN, OUTPUT);
  pinMode(RR_UP_PIN, OUTPUT);
  pinMode(RR_DOWN_PIN, OUTPUT);
  turnOffAllJacks();

  File dir = SPIFFS.open("/");
  while (dir.openNextFile()) {
    String fileName = dir.name();
    size_t fileSize = dir.size();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), getSizeSuffix(fileSize).c_str());
  }
  Serial.printf("\n");

  if (!MDNS.begin(hostName)) { //http://camper.local
    Serial.println("Error setting up MDNS responder!");
  }

  Serial.printf("Open http://%s.local/edit to see the file browser\n", hostName);
  Serial.printf("Open http://%s.local/list?dir= to list contents of file system\n", hostName);

  // Start access point
  boolean result = WiFi.softAP("Camper","password");
  if(result == true){
    Serial.print("Access Point Started. IP: ");
    Serial.println(WiFi.softAPIP());
  }
  else{
    Serial.println("Access Point Startup failed!");
    return;
  }

  // Start Websocket Server
  Serial.println("Connecting Websocket.");
  webSocketServer.begin();
  webSocketServer.onEvent(webSocketEvent);
  webServer.begin();
  Serial.println("Websocket connected.");
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS service started.");
  //Handle requests from client
  //list directory
  webServer.on("/list", HTTP_GET, handleFileList);
  //load editor
  webServer.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) webServer.send(404, "text/plain", "FileNotFound");
  });
  //create file
  webServer.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  webServer.on("/edit", HTTP_POST, []() {
    webServer.send(200, "text/plain", "");
  }, handleFileUpload);
  //called when the url is not defined here
  //use it to load content from SPIFFS
  webServer.onNotFound([]() {
    if (!handleFileRead(webServer.uri()))
      webServer.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  // TODO: serialize this data properly.
  webServer.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    //json += ", \"analog\":" + String(analogRead(A0));
    //json += ", \"gpio\":" + String((uint32_t)(((GPIO_IN_REG | GPIO_IN1_REG) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    webServer.send(200, "text/json", json);
    json = String();
  });
  webServer.begin();
  Serial.println("HTTP web server started.\n");

  turnOffAllJacks();

}//end setup()

//*****************************************************************************
// Loop                                                                       *
//*****************************************************************************
void loop(void) {

  wifi_sta_list_t curr_sta_list;
  static uint8_t previousNumStations;

  webServer.handleClient();
  webSocketServer.loop();
  ArduinoOTA.handle();

  esp_wifi_ap_get_sta_list(&curr_sta_list);

  if (curr_sta_list.num != previousNumStations) {
    Serial.print("Current number of connected stations is: ");
    Serial.println(curr_sta_list.num);
    if(curr_sta_list.num == 0) {
      Serial.println("No stations connected. Turning off all jacks.");
      turnOffAllJacks();
    }
    previousNumStations = curr_sta_list.num;
  }
}//end loop()
