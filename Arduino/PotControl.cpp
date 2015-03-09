/*
 * PotControl.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#include <Arduino.h>
#include <GolDuino.h>
#include <PotControl.h>

static int potValue = -1;
static int lastCheck = 0;

class SetPotValueResultReceiver: public GolDuinoSetPotValueResultReceiver{
public:
    SetPotValueResultReceiver(void);
    ~SetPotValueResultReceiver(void);
    void success(void);
    void errorWithGolgiException(GolgiException *golgiException);
};


SetPotValueResultReceiver::SetPotValueResultReceiver(void){

}

void SetPotValueResultReceiver::success(void){
    // Serial.println("setPotValue: SUCCESS");
    delete this;
}

void SetPotValueResultReceiver::errorWithGolgiException(GolgiException *gex){
    Serial.println("setPotValue: FAILURE(" + String(gex->getErrType()) + "/" + String(gex->getErrText()));
    delete this;
}

SetPotValueResultReceiver::~SetPotValueResultReceiver(void){
    Serial.println("Bye Bye result receiver");
}

void sendPotValue(void){
    const char *id = getHandsetId();
    if(id == NULL){
        Serial.println("We don't know the ID of the phone yet");
    }
    else{
        GolDuino_setPotValue_reqArg *arg = new GolDuino_setPotValue_reqArg();
        arg->setV(potValue);
        GolDuinoSetPotValue_sendTo(new SetPotValueResultReceiver(), id, arg);
        delete arg;
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
        int val = analogRead(0);
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
