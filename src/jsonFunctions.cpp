#include <jsonFunctions.h>


/* This file isn't json specific. It is more about web sockets. Should be moved to LCLWebsockets.cpp*/


//******************************************************************************************************************************
//  Parse rx's websocket text and check if it's a valid request
//******************************************************************************************************************************
void parseWebsocketText(String text, uint8_t num) {
  Serial.println("Parseing Web Socket.");
  String message = "{\"echo\":\"Invalid Message Received.\"}";

  DynamicJsonDocument jsonDoc(1024);

  auto error = deserializeJson(jsonDoc, text);
  if (error) {
    Serial.println("Failed to parse web socket");
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  JsonObject jsonData = jsonDoc.as<JsonObject>();  
  //const char* type = jsonData["type"];
  //const char* data = jsonData["data"];
  if ((jsonData["type"]) == "queryGPIO") {
    Serial.println("Processing queryGPIO");
    message = processGPIO(jsonData);
  } else {
    Serial.println("Unknown Request Type");
  }
  Serial.println("Sending Websocket Message:");
  Serial.println(message);
  webSocketServer.sendTXT(num, message.c_str(), message.length());  // Send update data
}//end parseWebsocketText()
//******************************************************************************************************************************
// Echo GPIO status to Client
//******************************************************************************************************************************
String processGPIO(JsonObject jsonData) {
  const char* GPIO = "";
  String state = "";
  int valGPIO = 0;
  String jsonLocalName = "";
  GPIO = jsonData["data"];
  Serial.print("GPIO from Data is: "); Serial.println(GPIO);
  //  GPIO = GPIO.substring(6,7);
  valGPIO = atoi(GPIO);
  Serial.print("Processing GPIO: "); Serial.println(valGPIO);
  //Serial.print("Reading GPIO gives: "); Serial.println(digitalRead(valGPIO));
/*   if (digitalRead(valGPIO) == 1) {  // active low LED on WEMOS D1 MINI!
    state = "OFF";
  } 
  else {
    state = "ON";
  } */
  
  const size_t bufferSize = JSON_OBJECT_SIZE(2); // 2 is number of key:value pairs
  DynamicJsonDocument jsonDoc(1024);
  JsonObject root = jsonDoc.as<JsonObject>();  
  root["echo"] = "Status GPIO";
  root["status"] = state;
  
  serializeJson(root, jsonLocalName);  
  Serial.println("Sending processed GPIO status");
  return jsonLocalName;
}//end processGPIO()
