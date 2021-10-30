#pragma once

#include "JsonHandler.h"
#include "Forecast.h"

class WeatherHandler: public JsonHandler {
private:
    bool inHour;
    unsigned long hourStart;
    bool documentEnded;
    Forecast *pForecast;
    int hourIndex;

public:
    virtual ~WeatherHandler() {}

    void startDocument() override;
    void endDocument() override;
    void startObject(ElementPath path) override {}
    void endObject(ElementPath path) override {}
    void startArray(ElementPath path) override {}
    void endArray(ElementPath path) override {}
    void whitespace(char c) override {}
    void value(ElementPath path, ElementValue value) override;

    void setForecast(Forecast *pforecast);
    bool isEnded();
};