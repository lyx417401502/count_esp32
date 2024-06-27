#ifndef  OTA_HPP
#define  OTA_HPP

#include <Arduino.h>

  #include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <WebSocketsServer.h>





class OTA
{
private:

    String m_ssid ;
   String m_password ;

    

public:

    WebServer server;

    static String logData;

    void handleRoot();
    void addLogData(const char* data);

    



    OTA(const String ssid,const String password);
    ~OTA();
};


#endif