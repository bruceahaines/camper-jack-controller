#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

extern WebSocketsServer webSocketServer;

extern void turnOffAllJacks();

String processGPIO(JsonObject jsonData);
void parseWebsocketText(String text, uint8_t num);

#endif //JSON_FUNCTIONS_H