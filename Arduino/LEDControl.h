/*
 * LEDControl.h
 *
 *  Created on: Mar 6, 2015
 *      Author: brian
 */

#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#include <libgolgi.h>
#include "GolgiGen.h"

void ledSetup(void);
void ledService(void);

class LEDControl: public GolDuinoSetIORequestReceiver{
public:
    LEDControl();
    void registerReceivers(void);
    void inboundSetIO(GolDuinoSetIOResultSender *resultSender, GolDuino_setIO_reqArg *arg);
    virtual ~LEDControl();
};

#endif /* LEDCONTROL_H_ */
