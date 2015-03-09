#define ETHERNET 0
#define WIFI     1

#include <GolDuino.h>
#include <libgolgi.h>

#include <LEDControl.h>
#include <ButtonControl.h>
#include <PotControl.h>
#include <GOLGI_KEYS.h>



#if WIFI

//
// Put a file somewhere (outside of the build area so you don't ship it
// by accident) dthat looks like the two definitions below and then
// #include it here
//
/*

#ifndef __WIFI_PARAMS_H__
#define __WIFI_PARAMS_H__

char ssid[] = "SSID_OF_YOUR_NETWORK";
char pass[] = "PASSWORD_OF_YOUR_NETWORK";    // your network password

#endif

 */

#include "../../WIFI_PARAMS.h"

#endif


#if ETHERNET

byte mac[] = { 0x90, 0xa2, 0xda, 0x0e, 0xc7, 0x89 };

#endif

static GolgiAPIImpl *golgiAPIImpl;

static const char *handsetId = NULL;

void setHandsetId(const char *id)
{
    if(handsetId != NULL){
        free((void *)handsetId);
    }

    handsetId = strdup(id);
}

const char *getHandsetId(void)
{
    return handsetId;
}

void golgiSetup()
{
    GolgiNetInterface *netIf;

#if WIFI
    netIf = new GolgiNetWifi();
#endif
#if ETHERNET
    netIf = new GolgiNetEther();
#endif

    new GolgiAPIImpl(netIf, GOLGI_APPKEY, GOLGI_DEVKEY, "BK-DUINO");
    golgiAPIImpl = GolgiAPIImpl::getInstance();
    (new LEDControl())->registerReceivers();
}

//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(9600);

    //
    //
    //

    int i, total;
    for(i = total = 0; i < 4; i++){
        total += analogRead(i);
    }
    randomSeed(total);

#if WIFI

    Serial.print("Waiting:");
    for(i = 0; i < 5; i++){
        Serial.print(".");
        delay(1000);
    }
    Serial.println("DONE");

    Serial.println("Attempting to connect to WPA network...");

    int wifiStatus = WL_IDLE_STATUS;
    wifiStatus = WiFi.begin(ssid, pass);

    // if you're not connected, stop here:

    if ( wifiStatus != WL_CONNECTED) {
      Serial.println(String("Couldn't get a wifi connection ") + String(wifiStatus)) ;
      while(true);
    }
#endif

#if ETHERNET
    int rc = Ethernet.begin(mac);
    if(rc == 0){
        Serial.println("Couldn't setup Ethernet");
        while(true);
    }

#endif

    ledSetup();
    buttonSetup();
    potSetup();
    golgiSetup();
}

// The loop function is called in an endless loop
void loop()
{
    golgiAPIImpl->service();
    ledService();
    buttonService();
    potService();
    delay(20);
}
