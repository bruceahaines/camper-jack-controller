#include <jsonFunctions.h>


/* This file isn't json specific. It is more about web sockets. Should be moved to LCLWebsockets.cpp*/


//******************************************************************************************************************************
//  Parse rx's websocket text and check if it's a valid request
//******************************************************************************************************************************
void parseWebsocketText(String text, uint8_t num) {
  Serial.println("Parseing Web Socket.");
  String message = "{\"echo\":\"Invalid Message Received.\"}";

  DynamicJsonDocument jsonDoc(1024);
  // Deserialize json message.
  auto error = deserializeJson(jsonDoc, text);
  if (error) {
    Serial.println("Failed to parse web socket");
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  JsonObject jsonData = jsonDoc.as<JsonObject>();  
  message = processGPIO(jsonData);

  Serial.println("Sending Websocket Message:");
  Serial.println(message);
  webSocketServer.sendTXT(num, message.c_str(), message.length());  // Send update data
}//end parseWebsocketText()
//******************************************************************************************************************************
// Echo GPIO status to Client
//******************************************************************************************************************************
String processGPIO(JsonObject jsonData) {

  int pinNum;
  int pinState;
  int pinReadState;
  String jsonLocalName = "";
  
  // Get pin# and from JSON data
  Serial.println("Processing pinNum and pinState");
  pinNum = atoi(jsonData["pinNum"]);  
  pinState = atoi(jsonData["pinState"]);

  Serial.print("pin is: "); Serial.println(pinNum);
  Serial.print("pin state is: "); Serial.println(pinState);
  
  if (pinNum == 99) {
    turnOffAllJacks();
    Serial.println("STOP BUTTON PRESSED");
  }
  else {
    turnOffAllJacks();
    Serial.print("Processing GPIO: "); Serial.println(pinNum);
    digitalWrite(pinNum, HIGH); // High is on.
  }
  
  //Serial.print("Reading GPIO gives: "); Serial.println(digitalRead(valGPIO));
/*   if (digitalRead(pinNum) == 1) {  // active low LED on WEMOS D1 MINI!
    pinReadState = "ON";
  } 
  else {
    pinReadState = "OFF";
  } */
  //
  const size_t bufferSize = JSON_OBJECT_SIZE(3); // 3 is number of key:value pairs
  DynamicJsonDocument jsonDoc(1024);
  JsonObject root = jsonDoc.as<JsonObject>();  
  root["echo"] = "Pin read state";
  root["pinNum"] = pinNum;
  root["pinReadState"] = digitalRead(pinNum);
  
  serializeJson(root, jsonLocalName);  
  Serial.println("Sending processed GPIO status");
  return jsonLocalName;
}//end processGPIO()
