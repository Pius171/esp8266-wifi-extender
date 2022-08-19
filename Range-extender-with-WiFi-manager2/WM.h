#ifndef WM_H
#define WM_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
//
//AsyncWebServer server(80);
//DynamicJsonDocument Config(2048);


#if (ARDDUINO >=100)
  #include "Arduino.h"
#endif

class WM {
  public:
  
//  JsonObject obj = Config.as<JsonObject>();
  void listDir(const char * dirname);
  void begin_server();
  void create_server();
  String get_credentials(int a);
  
};

#endif
