#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <Arduino.h>
#include <WebServer.h>
#include <stdlib.h>
#include <FS.h>
#include <SPIFFS.h>
#include <globalDefines.h>

extern WebServer webServer;

void turnOffAllJacks();
String getSizeSuffix(size_t bytes);
String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void handleFileCreate();
void handleFileList();

#endif //FILE_UTILS_H