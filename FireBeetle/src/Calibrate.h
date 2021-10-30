#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"

class classCalibrate: public Screen, EventReceiver {
public:
    virtual void activate();
    virtual void deactivate();
    virtual bool onEvent(Event* pevent);

private:
    int previousEncoder;
    void handleEncoderEvent (EncoderEvent *pevent);
    void handleSwitchEvent (SwitchEvent *pevent);
};

extern classCalibrate Calibrate;
