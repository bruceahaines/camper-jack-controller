#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

extern char ssid[];
extern char password[];
extern char hostName[];
extern char apSSID[];
extern char apPASS[];

bool loadConfig();
bool saveConfig();

#endif //CONFIG_H