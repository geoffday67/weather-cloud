#include "Calibrate.h"
#include "Motor.h"
#include "MainMenu.h"
#include "main.h"

classCalibrate Calibrate;

void classCalibrate::activate() {
    EventManager.addListener(EVENT_ENCODER, this);
    EventManager.addListener(EVENT_SWITCH, this);

    previousEncoder = getEncoderPosition();

    Output.clear();
    Output.addTitle("Setup");
    Output.setSize(FONT_MEDIUM);
    Output.addText(0, 26, "Move the pointer");
    Output.addText(0, 44, "to the top.");
    Output.flush();
}

void classCalibrate::deactivate() {
    EventManager.removeListener(this);
}

bool classCalibrate::onEvent(Event* pevent) {
    switch (pevent->type) {
        case EVENT_ENCODER:
            handleEncoderEvent((EncoderEvent*) pevent);
            break;
        case EVENT_SWITCH:
            handleSwitchEvent((SwitchEvent*) pevent);
            break;
    }
    return true;
}

void classCalibrate::handleEncoderEvent (EncoderEvent *pevent) {
    if (pevent->position > previousEncoder) {
        Motor.moveBy(1);
    } else {
        Motor.moveBy(-1);
    }
    previousEncoder = pevent->position;
}

void classCalibrate::handleSwitchEvent (SwitchEvent *pevent) {
    if (pevent->pressed) {
        Motor.setZero();
        MainMenu.activate();
        this->deactivate();
    }
}
