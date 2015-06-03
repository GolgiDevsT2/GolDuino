#include <SPI.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>




#include "GolDuino.h"

#include "libgolgi.h"

#include "GolgiNet.h"
#include "LEDControl.h"
#include "ButtonControl.h"
#include "PotControl.h"
#include "GOLGI_KEYS.h"



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

extern GolgiNetInterface *golgiNetworkSetup(void);

static GolgiNetInterface *netIf = NULL;

void golgiSetup()
{

    netIf = golgiNetworkSetup();

    new GolgiAPIImpl(netIf, GOLGI_APPKEY, GOLGI_DEVKEY, "Due-#2");
    golgiAPIImpl = GolgiAPIImpl::getInstance();
    (new LEDControl())->registerReceivers();
}

void setup()
{
    Serial.begin(115200);

    // SerialUSB.begin(115200);

    //
    //
    //

    Serial.println("** STARTUP *** " + String(sizeof(char)));

    int i, total;
    for(i = total = 0; i < 4; i++){
        total += analogRead(i);
    }
    randomSeed(total);

    ledSetup();
    buttonSetup();
    potSetup();
    golgiSetup();
    
    pinMode(4, OUTPUT);

}

long prev = 0;
long prev1 = 0;

extern unsigned long nopFlightTime;

void loop()
{
    long now = millis() / 1000;
    if(now != prev){
        prev = now;
        Serial.print('.');
        if(nopFlightTime > 10000) {
              if(now & 1){
                  digitalWrite(4 , HIGH);
              }
              else {
                digitalWrite(4, LOW);
              }
        }
        else{
                digitalWrite(4, LOW);          
        }
    }
    now /= 10;
    if(now != prev1){
        prev1 = now;
        //
    }

    golgiAPIImpl->service();
    ledService();
    buttonService();
    potService();
    netIf->service();
}
