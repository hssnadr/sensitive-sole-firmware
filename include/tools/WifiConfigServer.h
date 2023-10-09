#if !defined(_TOOLS_WIFISETUP_AP_)
#define _TOOLS_WIFISETUP_AP_

#include <ESPAsyncWebServer.h>

#include "globals.h"
#include "../ESP32/FilesManager.h"

void startWifiConfigServer()
{
    AsyncWebServer *server = new AsyncWebServer(80);

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wificonfig.html", "text/html"); });
    server->serveStatic("/", SPIFFS, "/");
    server->on("/", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                        for (int i = 0; i < request->params(); i++)
                        {
                            AsyncWebParameter *p = request->getParam(i);
                            Serial.println(p->name());
                            Serial.print('\t');
                            Serial.print(p->value());
                            if (p->isPost())
                            {
                                if (p->name() == "ssid")
                                    writeFile(FILE_SSID, p->value().c_str());
                                
                                if (p->name() == "pass")
                                    writeFile(FILE_PASSWORD, p->value().c_str());
                                
                                if (p->name() == "ip")
                                    writeFile(FILE_IP, p->value().c_str());

                                if (p->name() == "outip")
                                    writeFile(FILE_IP_OUT, p->value().c_str());
                                
                                if (p->name() == "gateway")
                                    writeFile(FILE_GATEWAY, p->value().c_str());
                            }
                        }
                        
                        request->send(200, "text/plain", "Done. Movuino will restart and connect to " + readFile(FILE_SSID) + " with IP address: " + readFile(FILE_IP));
                        delay(3000);
                        ESP.restart(); });

    server->begin();
}

#endif