#include <FileUtils.h>

//******************************************************************************************************************************
// turnOffAllJacks() - This function is called before enabling any GPIO to ensure only one 
// pin can be high at any given time. Otherwise the control board fuse will be blown.
//******************************************************************************************************************************
void turnOffAllJacks()
{
    digitalWrite(LF_UP_PIN, LOW);
    digitalWrite(LF_DOWN_PIN, LOW);
    digitalWrite(RF_UP_PIN, LOW);
    digitalWrite(RF_DOWN_PIN, LOW);
    digitalWrite(LR_UP_PIN, LOW);
    digitalWrite(LR_DOWN_PIN, LOW);
    digitalWrite(RR_UP_PIN, LOW);
    digitalWrite(RR_DOWN_PIN, LOW);
}

//******************************************************************************************************************************
// Given size in bytes, return string representing units suffix (ie "KB")
//******************************************************************************************************************************
String getSizeSuffix(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}//end getSizeSuffix()

//******************************************************************************************************************************
// Given a filename, return a String representing the type of file.
//******************************************************************************************************************************
String getContentType(String filename) {
  if (webServer.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}//end getContentType()

//******************************************************************************************************************************
// Read requested file from SPIFFS, extracting .gzip contents if required
//******************************************************************************************************************************
bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "home.html";
  }

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  String pathIcons = "/icons" + path;  

  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(pathIcons) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }
    if (SPIFFS.exists(pathIcons)) {
      path = "/icons" + path;
    }
    // load requested file from SPIFFS
    File file = SPIFFS.open(path, "r");
    if (!file) {
      // Should never get here. Show 404 page instead.
      Serial.println("SPIFFS open failed.  Show 404.");
      delay(500);
      return false;
    }
    webServer.streamFile(file, contentType);   // stream file to web reader.
    file.close();
    Serial.println(String("\tSent file: ") + path);
    return true;

  } else {
    path = "/404.html";
    Serial.println("File not found, Load 404 Page.");
    contentType = "text/html";
    return false;
  }
  Serial.print("Content Type is : "); Serial.print(contentType); Serial.print(" and path is : "); Serial.println(path);
}//end handleFileRead()

//******************************************************************************************************************************
// Handle file upload
//******************************************************************************************************************************
void handleFileUpload() {
  if (webServer.uri() != "/edit") return;
  File fsUploadFile;
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}//end handleFileUpload()

//******************************************************************************************************************************
// Handle file delete
//******************************************************************************************************************************
void handleFileDelete() {
  if (webServer.args() == 0) return webServer.send(500, "text/plain", "BAD ARGS");
  String path = webServer.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
    return webServer.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return webServer.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  webServer.send(200, "text/plain", "");
  path = String();
}//end handleFileDelete()

//******************************************************************************************************************************
// Handle file create
//******************************************************************************************************************************
void handleFileCreate() {
  if (webServer.args() == 0)
    return webServer.send(500, "text/plain", "BAD ARGS");
  String path = webServer.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/")
    return webServer.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return webServer.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return webServer.send(500, "text/plain", "CREATE FAILED");
  //(200, "text/plain", "");
  path = String();
}//end handleFileCreate()

void handleFileList() {
  if (!webServer.hasArg("dir")) {
    webServer.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String path = webServer.arg("dir");
  Serial.println("handleFileList: " + path);

  File root = SPIFFS.open(path);
  path = String();

  String output = "[";
  if(root.isDirectory()){
      File file = root.openNextFile();
      while(file){
          if (output != "[") {
            output += ',';
          }
          output += "{\"type\":\"";
          output += (file.isDirectory()) ? "dir" : "file";
          output += "\",\"name\":\"";
          output += String(file.name()).substring(1);
          output += "\"}";
          file = root.openNextFile();
      }
  }
  output += "]";
  webServer.send(200, "text/json", output);
}//end handleFileList */