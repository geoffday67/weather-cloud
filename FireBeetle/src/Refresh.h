#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"
#include "Forecast.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define WEATHER_URL     "https://api.openweathermap.org/data/2.5/onecall?appid=5177e49f435a1cd5af7d5a96d50c7e53&exclude=current,minutely,daily,alerts&units=metric"

class classRefresh: public Screen, EventReceiver {
public:
    virtual void activate();
    virtual void deactivate();
    virtual bool onEvent(Event* pevent);

private:
    WiFiClientSecure wifiClient;
    HTTPClient httpClient;
    WiFiUDP ntpUDP;

    void syncTime();
    bool getForecast(Forecast*);
    bool getForecasts();
    void connectWiFi();
};

extern classRefresh Refresh;
