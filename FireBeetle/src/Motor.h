#pragma once

#include <AccelStepper.h>

class classMotor {
private:
    AccelStepper *pstepper;

    int positionToSteps(int position);
    void storePosition(int position);
    int fetchPosition();

public:
    void begin(int, int, int, int, int);
    void moveBy(int delta);

    // Record that the pointer is now at icon 0
    void setZero();

    void moveToIcon(int target);
};

extern classMotor Motor;
