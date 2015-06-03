/*
 * LEDControl.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#include <Arduino.h>

#include "GolDuino.h"
#include "LEDControl.h"

static bool ledOn = false;

void LEDControl::inboundSetIO(GolDuinoSetIOResultSender *resultSender, GolDuino_setIO_reqArg *arg)
{
    // Serial.println("Received from: '" + String(resultSender->getSender()) + "'");


    setHandsetId(resultSender->getSender());

    ledOn = arg->getIoState()->getLedState();
    // Serial.println("ledOn: " + String(ledOn));
    ledService();
    resultSender->success();
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

