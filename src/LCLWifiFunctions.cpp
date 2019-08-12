#include <LCLWifiFunctions.h>

// This file can probably be omitted entirely. TestWifi is stupid. StartAccesspoint was also stupid.

//******************************************************************************************************************************
// testWiFi - if not connected to local network, start as Access Point
//******************************************************************************************************************************
int testWiFi() {
  uint8_t connectionAttempts = 0;
  uint8_t blinkLED = 0;

  Serial.println("Waiting for Wifi to connect.");
  while ( connectionAttempts < maxConnectionAttempts ) {
    Serial.print(".");
    blinkLED++;
    if (blinkLED > 1) {
      blinkLED = 0;
      //digitalWrite(2, HIGH);
    } else {
      //digitalWrite(2, LOW);
    }
    delay(500);
    connectionAttempts++;
    if (WiFi.status() == WL_CONNECTED) {
      //digitalWrite(2, HIGH);
      return 0;
    }
  }
  Serial.println("");
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  Serial.println("Connection to WiFi failed, starting Access Point");
  //digitalWrite(2, HIGH);
  return 1;
}//end testWiFi()
//******************************************************************************************************************************
// Set Up Wifi as an Access Point
//******************************************************************************************************************************
void startAccessPoint() {
  /*String tempName = "";
  String checkName = "";
  String buildName = "";*/
  //WiFi.disconnect();
  /* WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("SCANNING for available Networks");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }*/
  //  WiFi.softAP(apSSID, apPASS);
  //WiFi.enableSTA(false);
  //WiFi.mode(WIFI_AP);
  //WiFi.enableAP(true);
  boolean result = WiFi.softAP("Camper","password");
  if(result == true)
  {
    Serial.print("Access Point Started. IP: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Access Point Startup failed!");
    return;
  }

  /*
  // Form a string that is LCLGDI_ + mac address (without colons)
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  tempName = WiFi.softAPmacAddress().c_str();
  tempName.replace(":", "");
  buildName = tempName;
  tempName = AP_PREFIX + buildName;
  checkName = apSSID;
  if (tempName.equals(apSSID)) 
  {   // Should only run once, when default settings file is loaded to SPIFFS
    Serial.print("ESP name is:"); Serial.println(tempName);
  } 
  else {
    Serial.println("AP SSID is default setting. Updating to include mac address.");
    tempName.toCharArray(apSSID, tempName.length() + 1);
    Serial.print("New apSSID is:"); Serial.println(apSSID);
    //saveConfig();
    Serial.println("Config Saved, Resetting ESP");
    //ESP.reset();
  }*/
}//end startAccessPoint()
