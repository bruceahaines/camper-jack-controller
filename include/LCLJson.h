#ifndef LCL_JSON_H
#define LCL_JSON_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

extern char ssid[];
extern char password[];
extern char hostName[];
extern char apSSID[];
extern char apPASS[];

extern File fsUploadFile;

bool loadConfig();
bool saveConfig();

#endif //LCL_JSON_H