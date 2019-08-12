#ifndef LCL_FILE_FUNCTIONS_H
#define LCL_FILE_FUNCTIONS_H

#include <Arduino.h>
//#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <stdlib.h>

extern ESP8266WebServer webServer;

void turnOffAllJacks();
String formatBytes(size_t bytes);
String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void handleFileCreate();
void handleFileList();

#endif //LCL_FILE_FUNCTIONS_H