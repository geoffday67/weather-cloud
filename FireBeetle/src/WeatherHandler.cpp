#include "WeatherHandler.h"
#include "Forecast.h"

bool WeatherHandler::isEnded() {
    return documentEnded;
}

void WeatherHandler::setForecast(Forecast *pforecast) {
    this->pForecast = pforecast;
}

void WeatherHandler::startDocument() {
    Serial.println("Start document");
    documentEnded = false;
    inHour = false;
    hourIndex = 0;
}

void WeatherHandler::endDocument() {
    Serial.println("End document");
    documentEnded = true;
}

void WeatherHandler::value(ElementPath path, ElementValue value) {
    if (documentEnded) {
        return;
    }

    // If we get a "dt" then we're starting a new hour.
    if (!strcmp(path.getKey(), "dt")) {
        inHour = true;
        hourStart = value.getInt();
        return;
    }

    // If we get an icon and we're processing an hour then record it as a forecast element.
    if (inHour && !strcmp(path.getKey(), "icon")) {
        if (pForecast && hourIndex < MAX_HOURS) {
            pForecast->addHour(hourStart, value.getString());
            hourIndex++;
        }

        inHour = false;
        return;
    }
}