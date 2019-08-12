#ifndef LCL_WEBSOCKETS_H
#define LCL_WEBSOCKETS_H

#include <Arduino.h>
#include <WebSockets.h>
#include <ESP8266WebServer.h>

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

#endif //LCL_WEBSOCKETS_H