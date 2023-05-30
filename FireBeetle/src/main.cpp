#include "main.h"

#include <Arduino.h>
#include <Debouncer.h>
#include <RotaryEncoder.h>

#include "Calibrate.h"
#include "EventManager.h"
#include "Forecast.h"
#include "MainMenu.h"
#include "Motor.h"
#include "Output.h"
#include "Pins.h"
#include "Refresh.h"
#include "main.h"

#define REFRESH_DELAY (30 * 60 * 1000)
#define IDLE_TIMEOUT 10000

Preferences preferences;

RotaryEncoder encoder(ROTARY_DATA1_PIN, ROTARY_DATA2_PIN, RotaryEncoder::LatchMode::FOUR3);
unsigned long idleStart = 0;

int getEncoderPosition() {
    return encoder.getPosition();
}

void resetIdle() {
    idleStart = millis();
}

int getEncoder() {
    return digitalRead(ROTARY_SWITCH_PIN);
}

void encoderChanged(int value) {
    resetIdle();
    EventManager.queueEvent(new SwitchEvent(value == 0));
}

Debouncer encoderDebouncer(getEncoder, encoderChanged, 100);

void enablePower() {
    digitalWrite(POWER_PIN, 1);
    delay(100);
}

void disablePower() {
    digitalWrite(POWER_PIN, 0);
}

void startSleep() {
    Output.clear();
    Output.flush();

    preferences.end();
    disablePower();

    esp_sleep_enable_ext0_wakeup((gpio_num_t)ROTARY_SWITCH_PIN, 0);
    esp_sleep_enable_timer_wakeup(REFRESH_DELAY * 1000);
    esp_deep_sleep_start();
}

IRAM_ATTR void checkEncoder() {
    encoder.tick();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting");

    preferences.begin("weather");
    Serial.println("Preferences initialised");

    pinMode(POWER_PIN, OUTPUT);
    Serial.println("Power initialised");

    enablePower();
    Serial.println("Power enabled");

    Output.begin();
    Serial.println("Display initialised");

    Motor.begin(STEPPER_STEPS_PER_REVOLUTION, STEPPER_BLUE_PIN, STEPPER_YELLOW_PIN, STEPPER_PINK_PIN, STEPPER_ORANGE_PIN);
    Serial.println("Motor initialised");

    ForecastManager.begin();
    Serial.println("Forecast manager initialised");

    resetIdle();

    // If we're starting because of the sleep timer then start a forecast refresh cycle.
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER ||
        esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
        Serial.println("Waking up for forecast refresh");
        Refresh.activate();
        startSleep();
    }

    Serial.println("Waking up for user interaction");

    // Initialise encoder
    attachInterrupt(digitalPinToInterrupt(ROTARY_DATA1_PIN), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ROTARY_DATA2_PIN), checkEncoder, CHANGE);
    pinMode(ROTARY_SWITCH_PIN, INPUT);
    Serial.println("Encoder initialised");

    MainMenu.activate();
}

int encoderPosition = 0;

void loop() {
    int position;

    // Check if we've been idle and go to sleep if so.
    if (millis() - idleStart > IDLE_TIMEOUT) {
        Serial.println("Idle timeout, going to sleep");
        startSleep();
    }

    // Check for encoder activity
    encoderDebouncer.loop();
    encoder.tick();
    position = encoder.getPosition();
    if (position != encoderPosition) {
        resetIdle();
        EventManager.queueEvent(new EncoderEvent(position));
        encoderPosition = position;
    }

    EventManager.processEvents();
}