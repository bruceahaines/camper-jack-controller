#include <WebsocketUtils.h>
#include <GeneralUtils.h>

// ******************************************************************************************************************************
// Websockets Event handler function
// ****************************************************************************************************************************
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    // Add code here to execute if disconnected
    case WStype_DISCONNECTED:                     
      Serial.println("Websocket disconnected");
      break;
    // Add code here to execute when connected  
    case WStype_CONNECTED:                        
      {
        //Configuration save/load is a feature in development
        //getConfig();
        // Get ip address of client connected to this WebSocketServer
        IPAddress ip = webSocketServer.remoteIP(num);   
        Serial.print("Websocket IP Address is: "); Serial.println(ip);
      }
      break;
    // Execute code here to match text etc  
    case WStype_TEXT:                             
      {
        String text = String((char *) &payload[0]);
        Serial.print("WStype_TEXT recieved was: ");
        Serial.println(text);
        parseWebsocketText(text, num);
      }
      break;
    case WStype_BIN:
      Serial.print("WStype_BIN recieved.");
      GeneralUtils::hexDump(payload, (uint32_t)length);                 
      webSocketServer.sendBIN(num, payload, length);
      break;

    default:
      break;
  }
}//end webSocketEvent()

//******************************************************************************************************************************
//  Parse rx's websocket text and check if it's a valid request
//******************************************************************************************************************************
void parseWebsocketText(String text, uint8_t num) {
  Serial.println("Parsing Web Socket.");
  String message = "{\"echo\":\"Invalid Message Received.\"}";

  // 2 is number of key:value pairs, 22 extra for string input duplication
  const int bufferSize = JSON_OBJECT_SIZE(2) + 22; 
  DynamicJsonDocument jsonDoc(bufferSize);

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

  // Get pin# and state from JSON data
  Serial.println("Processing pinNum and pinState");

  auto pinNum = jsonData["pinNum"].as<int>();
  auto pinState = jsonData["pinState"].as<String>();

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

  // 3 is number of key:value pairs, 45 extra for string input duplication
  const int bufferSize = JSON_OBJECT_SIZE(3) + 45; 
  String jsonLocalName = "";

  DynamicJsonDocument jsonDoc(bufferSize);
  jsonDoc["echo"] = "Pin read state";
  jsonDoc["pinNum"] = pinNum;
  jsonDoc["pinReadState"] = digitalRead(pinNum);
  
  serializeJson(jsonDoc, jsonLocalName);  
  Serial.println("Sending processed GPIO status");
  return jsonLocalName;
}//end processGPIO()