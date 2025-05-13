#include <Arduino.h>
#include "mb_slave.h"

//#include "LittleFS.h"
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);
IPAddress ap_ip{10,10,10,10};
IPAddress ap_mask{255,255,255,0};
IPAddress ap_gateway{10,10,10,1};
const char *ap_ssid = "EspTD3";
const char *ap_pass = "MARTES00";

MBSlave slave;

void home_handler(AsyncWebServerRequest *request)
{
//    request->send(200, "text/html", "<html><title>TD3 2025</title><body><a>TD3 ESP32 TESTS WEB SERVER</a></body></html>");
    request->send(SPIFFS,"/home.html");
}

void configMB_handler(AsyncWebServerRequest *request)
{
//    request->send(200, "text/html", "<html><title>TD3 2025</title><body><a>TD3 ESP32 TESTS WEB SERVER</a></body></html>");
    request->send(SPIFFS,"/mb_config.html");
}

void setup()
{
  //PHY Config
  slave.setBaudrate(9600);
  slave.setParity(MBSlave::Parity::none);
  slave.setStopBits(MBSlave::StopBit::one);

  slave.serialInit();

  //MB Config
  slave.setID(25);

  slave.start();

  //wifi config
  WiFi.softAPConfig(ap_ip,ap_gateway,ap_mask);
  WiFi.softAP(ap_ssid,ap_pass);

  //FS config
  if( !SPIFFS.begin() )
    Serial.println("FS Error");

  //web server config
  server.on("/home",home_handler);
  server.on("/configMB",configMB_handler);
  server.begin();
}

void loop()
{
  slave.task();
}
