#include <Arduino.h>
#include "mb_slave.h"

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

String processor(const String& arg)
{
  if( arg == "PAGE_TITLE" )
  {
    return "MB Master Config";
  }
  if( arg == "SERVER_IP" )
  {
    return WiFi.localIP().toString();
  }
}

void configMB_handler(AsyncWebServerRequest *request)
{
    request->send(SPIFFS,"/mb_config.html","",false,processor);
}
void start_handler(AsyncWebServerRequest *request)
{
  request->send(200,"application/json","{\"data\":10}\n\n");
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
//  server.on("/turn_on_pump2",onPump2_handler);
  server.on("/start",start_handler);

//  server.on("/request", /*HTTP_POST,*/ handle_request);
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
