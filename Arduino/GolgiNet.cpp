

/*
 * GolgiNet.cpp
 */

#include <Arduino.h>
#include <libgolgi.h>
#include "GolgiNet.h"

#define ETHERNET    0
#define WIFI        0
#define SERIALIF    0
#define SMS         0



#if WIFI

/*

#ifndef __WIFI_PARAMS_H__
#define __WIFI_PARAMS_H__

char ssid[] = "SSID_OF_YOUR_NETWORK";
char pass[] = "PASSWORD_OF_YOUR_NETWORK";    // your network password

#endif
*/

#include "/Users/brian/Documents/Development/WIFI_PARAMS.h"

#endif


#if ETHERNET

byte mac[] = { 0x90, 0xa2, 0xda, 0x0e, 0xc7, 0x89 };

#endif


#if ETHERNET
#define NET_SELECTED

GolgiNetInterface *golgiNetworkSetup(void)
{

    int rc = Ethernet.begin(mac);
    if(rc == 0){
        Serial.println("Couldn't setup Ethernet");
        while(true);
    }

    return new GolgiNetEther();
}
#endif



#if WIFI
#define NET_SELECTED

GolgiNetInterface *golgiNetworkSetup(void)
{

    Serial.print("Waiting for WiFi shield to spin up:");
    for(int i = 0; i < 5; i++){
        Serial.print(".");
        delay(1000);
    }
    Serial.println("DONE");
    Serial.println("Firmware Version: " + String(WiFi.firmwareVersion()));

    Serial.println("Attempting to connect to WPA network: '" + String(ssid) + "'...");

    int wifiStatus = WL_IDLE_STATUS;
    wifiStatus = WiFi.begin(ssid, pass);

    // if you're not connected, stop here:

    if ( wifiStatus != WL_CONNECTED) {
      Serial.println(String("Couldn't get a wifi connection ") + String(wifiStatus)) ;
      while(true);
    }



    return  new GolgiNetWifi();
}
#endif

#if SERIALIF
#define NET_SELECTED

class SerialIF : public GolgiSerialInterface
{
public:
    void begin(void){
        pinMode(19, INPUT_PULLUP);
        Serial1.begin(38400);
    };

    int available(void){
        return Serial1.available();
    };

    void write(const uint8_t *data, int len){
        Serial1.write(data, len);
    };

    int readBytes(char *buf, int max){
        return Serial1.readBytes(buf, max);
    };

};

GolgiNetInterface *golgiNetworkSetup(void)
{
    return  new GolgiNetSerial(new SerialIF());
}
#endif

#if SMS
#define NET_SELECTED

/*
#ifndef __SMS_PARAMS_H__
#define __SMS_PARAMS_H__

#define SMS_NUMBER "THE-NUMBER-OF-YOUR-SMS-GATEWAY"

#endif
*/

#include "/Users/brian/Documents/Development/SMS_PARAMS.h"

#if !defined(SMS_NUMBER)
#error No SMS_NUMBER defined
#endif

class SMSSerialIF : public GolgiSerialInterface
{
public:
    void begin(void){
        Serial1.begin(19200);
        gsm_init((char *)GOLGI_DEVKEY,
		 (char *)GOLGI_APPKEY);
        gsm_modem_init(SMS_NUMBER);
    };

    int available(void){
        return Serial1.available();
    };

    void write(const uint8_t *data, int len){
        Serial1.write(data, len);
    };

    int readBytes(char *buf, int max){
        return Serial1.readBytes(buf, max);
    };

};


GolgiNetInterface *golgiNetworkSetup(void)
{
    return  new GolgiNetSMS(new SMSSerialIF());
}
#endif




#ifndef NET_SELECTED
#error No Network Technology Specified
#endif

