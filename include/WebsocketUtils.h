#ifndef WEBSOCKET_UTILS_H
#define WEBSOCKET_UTILS_H

#include <Arduino.h>
//#include <WebSockets.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

extern WebSocketsServer webSocketServer;
extern void turnOffAllJacks();

String processGPIO(JsonObject jsonData);
void parseWebsocketText(String text, uint8_t num);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

#endif //WEBSOCKET_UTILS_H