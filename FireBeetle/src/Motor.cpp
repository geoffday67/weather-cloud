#include <Arduino.h>
#include "Motor.h"
#include "Pins.h"
#include "main.h"

#define MOTOR_TOPIC  "weather/motor"

classMotor Motor;

void classMotor::begin(int steps_per_revolution, int in1, int in2, int in3, int in4) {
  pstepper = new AccelStepper(AccelStepper::FULL4WIRE, in1, in2, in3, in4);
  pstepper->setSpeed(600.0);
  pstepper->setMaxSpeed(600.0);
  pstepper->setAcceleration(1000.0);
  pstepper->setCurrentPosition(positionToSteps(fetchPosition()));
}

void classMotor::moveBy(int delta) {
  if (!pstepper) {
      return;
  }
  pstepper->move(delta * 10);
  pstepper->runToPosition();
  pstepper->disableOutputs();
}

void classMotor::storePosition(int position) {
  preferences.putInt("position", position);
}

int classMotor::fetchPosition() {
  return preferences.getInt("position");
}

void classMotor::setZero() {
  pstepper->setCurrentPosition(0);
  storePosition(0);
}

void classMotor::moveToIcon(int target) {
  char s[128];

  if (target < 0 || target > 11) {
    return;
  }

  int targetSteps = positionToSteps(target);
  int currentSteps = pstepper->currentPosition();
  int delta = targetSteps - currentSteps;
  if (delta > STEPPER_STEPS_PER_REVOLUTION / 2) {
    delta -= STEPPER_STEPS_PER_REVOLUTION;
  } else if (delta < -STEPPER_STEPS_PER_REVOLUTION / 2) {
    delta += STEPPER_STEPS_PER_REVOLUTION;
  }
  pstepper->move(delta);
  pstepper->runToPosition();
  pstepper->disableOutputs();

  storePosition(target);
}

int classMotor::positionToSteps(int position) {
  return -position * STEPPER_STEPS_PER_REVOLUTION / 12;
}
