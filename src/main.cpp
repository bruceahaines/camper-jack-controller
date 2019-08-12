/*
  This is derivitive of FSWebServer - Example WebServer with SPIFFS backend for esp8266.
  Serves webpages as requested, also processes Websocket requests
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiAP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
// Added to include JSON file support
#include <ArduinoJson.h>
// Added to include OTA support
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//Added to include OTA update via web browser
#include <ESP8266HTTPUpdateServer.h>
#include <Wire.h>
//#include <stdlib.h>

#include <LCLFileFunctions.h>
#include <jsonFunctions.h>
#include <LCLJson.h>
#include <LCLWebsockets.h>
//#include <LCLWifiFunctions.h>

#define DEBUG // Comment out this line to disable debug serial prints

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(x) Serial.printf(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x)
#endif

//#define status0 0x80
//#define status1 0x40
//#define status2 0x20
//#define status3 0x10

#define redLED 0x08
#define yellowLED 0x04
#define greenLED 0x02
#define blueLED 0x01

// Define pins

// Left side of Wemos D1 Mini Pro as seen aligned with top writing.
#define LF_UP_PIN D0 // GPIO16 //Left Front Up. Up corresponds to +12V on the RED wire.
#define LF_DOWN_PIN D5 // GPIO14 
#define RF_UP_PIN D6 // GPIO12 
#define RF_DOWN_PIN D7 // GPIO13 
// Right side of Wemos D1 Mini Pro as seen aligned with top writing.
#define LR_UP_PIN D1 // GPIO5 
#define LR_DOWN_PIN D2 // GPIO4
#define RR_UP_PIN D3 // GPIO0
#define RR_DOWN_PIN D4 // GPIO2


#define addressMCP23017 0x20 // I2C Address of the MCP23017 IO expander
#define addressTCN75A 0x48   // I2C address of the TCN75A temperature sensor
#define greenButtonLED 14
#define redButtonLED 16

#define AP_PREFIX "LCLGDI_"
#define maxConnectionAttempts 120 // 1 minute to attempt connection

#define mcp23017PortA 0
#define mcp23017PortB 1
#define mcp23017Reset 2
#define setPins 1
#define clearPins 0

#define i2cSDA 12
#define i2cSCL 13

#define debounceCount 25

//class SPIFFSConfig;

extern void turnOffAllJacks();

struct inputData
{
  uint8_t state;
  uint8_t count;
  uint8_t current;
  uint8_t lock;
  uint8_t flag;
  uint32_t currentMillis; // change to uint32_t ??
  uint32_t previousMillis;
  uint8_t mode;
  uint8_t active;
  uint8_t set;
  uint8_t show;
  uint32_t onTime;
};


char gdiVersion[30] = "";
char ssid[30] = "";
char password[30] = "";
char hostName[30] = "camper";
char temperatureChoice[10] = "";

char apSSID[30] = "";
char apPASS[30] = "";
char rxBuffer[64]; // Recieved data serial buffer
String inSerial;   // String to hold rx'd data for string operations
String currentTemperature = "-100.0";
String ioStatus = "";
String htmlButtonNames[] = {"1", "2", "3", "4"};

unsigned char mac[6];
unsigned long currentStatusMillis = 0;
unsigned long previousStatusMillis = 0;
#define delayStatus 500
#define doorDelay 1000
#define lightDelay 5000

int temperatureReading = 0;
char ioPortA = 0xFF;
char ioPortB = 0xFF;
int pwmRedButton, pwmGreenButton = 0;
uint16_t panelButtonPWM[2];

inputData buttons[4];
inputData inputs[4];
uint8_t currentInput = 0;
uint8_t lightON = 0;

uint8_t blinkCount = 0;
uint8_t loopCounter = 0;
//uint8_t flagRedLED = 0;
//uint8_t flagYellowLED = 0;
//uint8_t flagGreenLED = 0;
//uint8_t flagBlueLED = 0;
uint8_t redLEDFlag = 0;
uint8_t yellowLEDFlag = 0;
uint8_t greenLEDFlag = 0;
uint8_t blueLEDFlag = 0;

// *SIGNED* variables.  Positive and negative values are valid.
int8_t tempSlope = 0;
int8_t tempIntercept = 0;

uint8_t WiFiType = 0;
uint8_t started = 0; // 0 = startup, 1 = loop

// Timers auxiliary variables
long now = millis();
long lastMeasure = 0;

ESP8266WebServer webServer(80);                           // start a htpp server named server on port 80
ESP8266HTTPUpdateServer httpUpdateServer;                 // start a http update server (for OTA updates)
WebSocketsServer webSocketServer = WebSocketsServer(81);  // start a web socket server named webSocketServer on port 81

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

  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
  // read config settings from JSON file
/*   if (!loadConfig()) {
    Serial.println("No config file found.  Using Defaults.");
  } */

  //WiFi.softAPdisconnect(true);

  //WIFI INIT
  /* Serial.println("Checking if first boot");
  Serial.print("SSID and PASSWORD ARE: "); Serial.print(ssid); Serial.print(":"); Serial.println(password);
  if ((strcmp(ssid, "default") == 0  ) &&  (strcmp(password, "default") == 0  )) {
    wifiStatus = WiFi.begin(ssid, password);
    WiFiType = 1; // AP_MODE
    Serial.printf("First boot - default values detected. Starting as Access Point.\n");
  } 
  else {
    Serial.println("not first boot");
    Serial.print("Connecting to "); Serial.print(ssid);
    Serial.print(" with Password "); Serial.println(password);
    wifiStatus = WiFi.begin(ssid, password);
    Serial.printf("Testing WiFi type\n");

    if (wifiStatus != WL_CONNECTED)
      WiFiType = 1; // AP MODE
    //WiFiType = testWiFi();
  }*/
  /* if (WiFiType == 0) {
    Serial.print("WiFi type is: "); Serial.println(WiFiType);
    Serial.print("\n");
    Serial.printf("Starting Server\n");
    //setupServer();
    WiFi.softAPdisconnect(true);
    Serial.print("Connected! IP address: "); Serial.println(WiFi.localIP());
    // Start mDNS Service
    MDNS.begin(hostName);
    Serial.printf("Open http://%s.local/edit to see the file browser\n", hostName);
    Serial.printf("Open http://%s.local/list?dir= to list contents of file system\n", hostName);
  } else {
    Serial.print("WiFi type is: "); Serial.println(WiFiType);
    Serial.println("Starting Access Point");
    delay(100);
    if (WiFiType == 1) {
      startAccessPoint();
    }
  }*/
  MDNS.begin(hostName);
  Serial.printf("Open http://%s.local/edit to see the file browser\n", hostName);
  Serial.printf("Open http://%s.local/list?dir= to list contents of file system\n", hostName);
  //Start HTTP OTA Service
  httpUpdateServer.setup(&webServer); // For web based OTA Updates
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n\n", hostName);

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
  Serial.println("Starting mDNS service.");
  MDNS.addService("http", "tcp", 80);
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
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
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

  static uint8 currentNumStations;
  static uint8 previousNumStations = 0;

  webServer.handleClient();
  webSocketServer.loop();
  MDNS.update();
  ArduinoOTA.handle();
  currentNumStations = wifi_softap_get_station_num();

  if (currentNumStations != previousNumStations) {
    Serial.print("Current number of connected stations is: ");
    Serial.println(currentNumStations);
    if(currentNumStations == 0) {
      Serial.println("No stations connected. Turning off all jacks.");
      turnOffAllJacks();
    }
    previousNumStations = currentNumStations;
  }
}//end loop()
