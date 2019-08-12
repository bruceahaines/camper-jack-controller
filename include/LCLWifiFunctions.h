#ifndef LCL_WIFI_FUNCTIONS_H
#define LCL_WIFI_FUNCTIONS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LCLJson.h>

#ifndef maxConnectionAttempts
#define maxConnectionAttempts 120 // 1 minute to attempt connection
#endif

#ifndef AP_PREFIX
#define AP_PREFIX "LCLGDI_"
#endif

extern char ssid[];
extern char password[];
extern char hostName[];
extern char apSSID[];
extern char apPASS[];

int testWiFi();
void startAccessPoint();


#endif/* !LCL_WIFI_FUNCTIONS_H */