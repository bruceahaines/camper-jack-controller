#include <LCLWebsockets.h>
#include <jsonFunctions.h>


// ******************************************************************************************************************************
// Websockets Event handler function
// ****************************************************************************************************************************
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:                     // Add code here to execute if disconnected
      Serial.println("Websocket disconnected");
      break;
    case WStype_CONNECTED:                        // Add code here to execute when connected
      {
        //getConfig();
        IPAddress ip = webSocketServer.remoteIP(num);   // Get ip address of connection
        Serial.print("Websocket IP Address is: "); Serial.println(ip);
      }
      break;
    case WStype_TEXT:                             // Execute code here to match text etc
      {
        String text = String((char *) &payload[0]);
        Serial.print("WStype_TEXT recieved was: ");
        Serial.println(text);
        parseWebsocketText(text, num);
      }
      break;
    case WStype_BIN:
      Serial.print("WStype_BIN recieved.");
      hexdump(payload, length);                 // echo data back to browser
      webSocketServer.sendBIN(num, payload, length);
      break;

    default:
      break;
  }
}//end webSocketEvent()