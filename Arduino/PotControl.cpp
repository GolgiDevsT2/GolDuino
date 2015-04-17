/*
 * PotControl.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#include <Arduino.h>

#include "GolDuino.h"
#include "PotControl.h"

static int potValue = -1;
static int lastCheck = 0;
static int inFlight = 0;
static int sendPending = 0;

class SetPotValueResultReceiver: public GolDuinoSetPotValueResultReceiver{
public:
    SetPotValueResultReceiver(void);
    ~SetPotValueResultReceiver(void);
    void success(void);
    void errorWithGolgiException(GolgiException *golgiException);
};

void sendPotValueWorker(void);

SetPotValueResultReceiver::SetPotValueResultReceiver(void){

}

void SetPotValueResultReceiver::success(void){
    // Serial.println("setPotValue: SUCCESS");
    inFlight--;
    if(sendPending){
        sendPotValueWorker();
    }
    delete this;
}

void SetPotValueResultReceiver::errorWithGolgiException(GolgiException *gex){
    Serial.println("setPotValue: FAILURE(" + String(gex->getErrType()) + "/" + String(gex->getErrText()));
    inFlight--;
    if(sendPending){
        sendPotValueWorker();
    }

    delete this;
}

SetPotValueResultReceiver::~SetPotValueResultReceiver(void){
}

void sendPotValueWorker(void)
{
    GolDuino_setPotValue_reqArg *arg = new GolDuino_setPotValue_reqArg();
    arg->setV(potValue);
    inFlight++;
    sendPending = 0;
    GolDuinoSetPotValue_sendTo(new SetPotValueResultReceiver(), getHandsetId(), arg);
    delete arg;
}

void sendPotValue(void){
    const char *id = getHandsetId();
    if(id == NULL){
        Serial.println("We don't know the ID of the phone yet");
    }
    else if(inFlight < 1){
        sendPotValueWorker();
    }
    else{
        sendPending = 1;
    }
}

void potSetup(void)
{

}

void potService(void)
{
    //
    // FSD (Full Scale Deflection) is 1024
    //
    int now = millis() / 250;
    if(now != lastCheck){
        lastCheck = now;
        int val = analogRead(POT_INPUT);
        if(val != potValue){
            int diff = val - potValue;
            if(diff > 50 || diff < -50 || val == 0 || val >= 1023){
                potValue = val;
                Serial.println("AVAL: " + String(val));
                sendPotValue();
            }
        }
    }
}
