/*
 * LEDControl.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#include <Arduino.h>
#include <GolDuino.h>
#include <LEDControl.h>

static bool ledOn = false;

void LEDControl::inboundSetIO(GolDuinoSetIOResultSender *resultSender, GolDuino_setIO_reqArg *arg)
{
    GolDuino_setIO_rspArg *rspArg = new GolDuino_setIO_rspArg(false);

    Serial.println("Received from: '" + String(resultSender->getSender()) + "'");


    setHandsetId(resultSender->getSender());

    ledOn = arg->getIoState()->getLedState();
    Serial.println("ledOn: " + String(ledOn));
    ledService();

    // buzzerOn = arg->getIoState()->getBuzzerState();

    rspArg->setInternalSuccess_(1);
    resultSender->sendResult(rspArg);
    delete rspArg;

}

LEDControl::LEDControl() {
}


void LEDControl::registerReceivers(void){
    GolDuinoRegisterSetIORequestReceiver(this);
}

LEDControl::~LEDControl() {
}

void ledSetup(void)
{
    pinMode(LED_PIN, OUTPUT);
}


void ledService(void)
{
    if(ledOn){
        digitalWrite(LED_PIN, HIGH);
    }
    else {
        digitalWrite(LED_PIN, LOW);
    }
}

