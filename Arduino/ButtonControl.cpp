/*
 * ButtonControl.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#include <Arduino.h>
#include <GolDuino.h>
#include <ButtonControl.h>

static int buttonState = 0;
static int oldButtonState = -1;

class ButtonPressedResultReceiver: public GolDuinoButtonPressedResultReceiver{
public:
    ButtonPressedResultReceiver(void);
    ~ButtonPressedResultReceiver(void);
    void success(void);
    void errorWithGolgiException(GolgiException *golgiException);
};



ButtonPressedResultReceiver::ButtonPressedResultReceiver(void){

}


void ButtonPressedResultReceiver::success(void){
    Serial.println("buttonPressed: SUCCESS");
    delete this;
}

void ButtonPressedResultReceiver::errorWithGolgiException(GolgiException *gex){
    Serial.println("buttonPressed: FAILURE(" + String(gex->getErrType()) + "/" + String(gex->getErrText()));
    delete this;
}

ButtonPressedResultReceiver::~ButtonPressedResultReceiver(void){

}


class ButtonReleasedResultReceiver: public GolDuinoButtonReleasedResultReceiver{
public:
    ButtonReleasedResultReceiver(void);
    ~ButtonReleasedResultReceiver(void);
    void success(void);
    void errorWithGolgiException(GolgiException *golgiException);
};


ButtonReleasedResultReceiver::ButtonReleasedResultReceiver(void){

}

void ButtonReleasedResultReceiver::success(void){
    Serial.println("buttonReleased: SUCCESS");
    delete this;
}

void ButtonReleasedResultReceiver::errorWithGolgiException(GolgiException *gex){
    Serial.println("buttonReleased: FAILURE(" + String(gex->getErrType()) + "/" + String(gex->getErrText()));
    delete this;
}

ButtonReleasedResultReceiver::~ButtonReleasedResultReceiver(void){

}


static unsigned long downTime = 0;

void sendButtonState(void){
    const char *id = getHandsetId();
    if(id == NULL){
        Serial.println("We don't know the ID of the phone yet");
    }
    else if(buttonState != 0){
        downTime = millis();
        GolDuino_buttonPressed_reqArg *arg = new GolDuino_buttonPressed_reqArg();
        arg->setT((int)0);
        Serial.println("Send to " + String(id));
        GolDuinoButtonPressed_sendTo(new ButtonPressedResultReceiver(), id, arg);

        delete arg;
    }
    else{
        int elapsed = (int)(millis() - downTime);
        GolDuino_buttonReleased_reqArg *arg = new GolDuino_buttonReleased_reqArg();
        arg->setT(elapsed);
        Serial.println("Send to " + String(id) + " with value: " + String(elapsed));
        GolDuinoButtonReleased_sendTo(new ButtonReleasedResultReceiver(), id, arg);

        delete arg;
    }
}




void buttonSetup(void)
{
    pinMode(BUTTON_PIN, INPUT);
}


void buttonService(void)
{
    buttonState = digitalRead(BUTTON_PIN);

    if(buttonState != oldButtonState){
        oldButtonState = buttonState;
        Serial.println("BUTTON: " + String((buttonState) ? "PRESSED" : "RELEASED"));
        sendButtonState();
    }
}



