<<<<<<< HEAD
=======
#define ETHERNET 1
#define WIFI     0
>>>>>>> ada3e4b444f776821c466c9ac9e7a9265694791c

#include <SPI.h>

<<<<<<< HEAD
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
=======
// #include "../../WIFI_PARAMS.h"
>>>>>>> ada3e4b444f776821c466c9ac9e7a9265694791c

#include <libgolgi.h>

#include "GolDuino.h"
#include <libgolgi.h>

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

    new GolgiAPIImpl(netIf, GOLGI_APPKEY, GOLGI_DEVKEY, "BK-DUINO");
    golgiAPIImpl = GolgiAPIImpl::getInstance();
    (new LEDControl())->registerReceivers();
}

//The setup function is called once at startup of the sketch
void _setup()
{
    Serial.begin(115200);
    SerialUSB.begin(115200);
    SerialUSB.println("Mary Had A Little Lamb");
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
}

long prev = 0;
long prev1 = 0;

// The loop function is called in an endless loop
void _loop()
{
    delay(1000);
    SerialUSB.println("Scooby Dooby Doo");
    Serial.println("<BARK>");

}

void loop()
{
    long now = millis() / 1000;
    if(now != prev){
        prev = now;
        Serial.print('.');
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
