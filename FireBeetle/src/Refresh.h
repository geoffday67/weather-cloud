#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"
#include "Forecast.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

#define WEATHER_URL     "https://api.openweathermap.org/data/2.5/onecall?appid=5177e49f435a1cd5af7d5a96d50c7e53&exclude=current,minutely,daily,alerts&units=metric"

class classRefresh: public Screen, EventReceiver {
public:
    virtual void activate();
    virtual void deactivate();
    virtual bool onEvent(Event* pevent);

private:
    WiFiClientSecure wifiClientSecure;
    WiFiClient wifiClientInsecure;
    HTTPClient httpClient;
    WiFiUDP ntpUDP;
    PubSubClient mqtt;

    void syncTime();
    bool getForecast(Forecast*);
    bool getForecasts();
    bool connectWiFi();
    void log(const char *ptopic, const char *pmessage);
};

extern classRefresh Refresh;
