#include <Arduino.h>
#include "mb_slave.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);
AsyncEventSource    events("/events"); // SSE endpoint

IPAddress ap_ip{10,10,10,10};
IPAddress ap_mask{255,255,255,0};
IPAddress ap_gateway{10,10,10,1};
const char *ap_ssid = "EspTD2025_Other";
const char *ap_pass = "MARTES00";

MBSlave slave;

Tim32_ms tim;
void sendAsyncData()
{
    events.send("hello");
}

void home_handler(AsyncWebServerRequest *request)
{
    request->send(SPIFFS,"/home.html");
}

void configMB_handler(AsyncWebServerRequest *request)
{
    request->send(SPIFFS,"/mb_config.html");
}

void start_handler(AsyncWebServerRequest *request)
{
  Serial.printf("params: %d\n",request->params());
  Serial.printf("args: %d\n",request->args());
  for( int i=0 ; i<request->args() ; i++ )
    Serial.println(request->argName(i) + ": " + request->arg(i));

  JsonDocument doc;
  String jsonBody = request->arg("json");
  Serial.println("jsonBody: " + jsonBody);
  deserializeJson(doc, jsonBody);
  if( doc.containsKey("value") )
    Serial.println("value: " + doc["value"].as<String>());
  String response;
  if (digitalRead(2))
  {
      doc["status"] = "Lleno";
      doc["level"]  = "full";
      doc["liters"] = 1000;
  }
  else
  {
      doc["status"] = "Vacio";
      doc["level"]  = "empty";
      doc["liters"] = 0;
  }
  serializeJson(doc, response);
  request->send(200, "application/json", response);
}


void setup()
{
  //PHY Config
  slave.setBaudrate(9600);
  slave.setParity(MBSlave::Parity::none);
  slave.setStopBits(MBSlave::StopBit::one);

  slave.serialInit();

  //FS config
  while( !SPIFFS.begin() )
  {
    Serial.println("FS Error");
    delay(5000);
  }

  //MB Config
  slave.setID(25);

  slave.start();

  //wifi config
  WiFi.softAPConfig(ap_ip,ap_gateway,ap_mask);
  WiFi.softAP(ap_ssid,ap_pass);

  //web server config
  server.on("/home",home_handler);
  server.on("/configMB",configMB_handler);
  server.on("/start",start_handler);

  server.addHandler(&events);

  server.begin();


  tim.start();
}

void loop()
{
  slave.task();
  if( tim > 2s )
  {
    tim.restart();
    sendAsyncData();
  }
}
