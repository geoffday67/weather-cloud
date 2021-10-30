#include "Forecast.h"

#include <string.h>
#include <time.h>

#include "Motor.h"
#include "main.h"

classForecastManager ForecastManager;

int mapIcon(const char *picon) {
    if (!strncmp(picon, "10n", 3)) {
        return 9;
    } else if (!strncmp(picon, "02n", 3)) {
        return 10;
    } else if (!strncmp(picon, "01n", 3)) {
        return 11;
    } else if (!strncmp(picon, "01d", 3)) {
        return 0;
    } else if (!strncmp(picon, "02d", 3)) {
        return 1;
    } else if (!strncmp(picon, "10d", 3)) {
        return 5;
    } else if (!strncmp(picon, "03", 2)) {
        return 2;
    } else if (!strncmp(picon, "04", 2)) {
        return 3;
    } else if (!strncmp(picon, "09", 2)) {
        return 4;
    } else if (!strncmp(picon, "11", 2)) {
        return 6;
    } else if (!strncmp(picon, "13", 2)) {
        return 7;
    } else if (!strncmp(picon, "50", 2)) {
        return 8;
    }

    return 0;
}

void Forecast::addHour(unsigned long timestamp, const char *picon) {
    if (hourCount >= MAX_HOURS) {
        return;
    }

    hours[hourCount].timestamp = timestamp;
    strcpy(hours[hourCount].icon, picon);
    hourCount++;
}

void Forecast::reset() { hourCount = 0; }

void Forecast::indicate(int targetHour) {
    int hour;

    Serial.printf("Indicating forecast for %s\n", location);

    // Find entry which is at least the target hour, if there is one.
    time_t target = time(NULL) + (targetHour * 3600);
    Serial.printf("Target time %ld\n", target);
    for (hour = 0; hour < hourCount; hour++) {
        if (hours[hour].timestamp >= target) {
            break;
        }
    }

    // Map icon name to position on dial.
    Serial.printf("Indicating icon %s for timestamp %d\n", hours[hour].icon, hours[hour].timestamp);
    Motor.moveToIcon(mapIcon(hours[hour].icon));
}

void Forecast::save() {
    char key[64];

    makeKey(key);
    preferences.putBytes(key, this, sizeof(Forecast));
    Serial.printf("Saving %d bytes of forecast %s\n", sizeof(Forecast), key);
}

void Forecast::load() {
    char key[64];

    makeKey(key);
    preferences.getBytes(key, this, sizeof(Forecast));
    Serial.printf("Loading %d bytes of forecast %s\n", sizeof(Forecast), key);
}

void Forecast::makeKey(char *pdestination) {
    char c, *pc;

    for (pc = location; *pc; pc++) {
        c = tolower(*pc);
        if (c == ' ') {
            c = '-';
        } else if (c < 'a' || c > 'z') {
            continue;
        }
        *pdestination++ = c;
    }
    *pdestination = 0;
}

void classForecastManager::begin() {
    strcpy(forecasts[0].location, "Home");
    forecasts[0].load();
    forecasts[0].latitude = 51.4273415;
    forecasts[0].longitude = -2.2233908;

    strcpy(forecasts[1].location, "High Wycombe");
    forecasts[1].load();
    forecasts[1].latitude = 51.628611;
    forecasts[1].longitude = -0.748229;

    strcpy(forecasts[2].location, "Shepherd's Bush");
    forecasts[2].load();
    forecasts[2].latitude = 51.5051913;
    forecasts[2].longitude = -0.22469;

    currentHour = preferences.getInt("hour");
    currentForecast = preferences.getInt("forecast");

    //strcpy(forecasts[0].hours[0].icon, "01d.png");      // TESTING
    //strcpy(forecasts[1].hours[0].icon, "09d.png");      // TESTING
    //strcpy(forecasts[2].hours[0].icon, "10n.png");      // TESTING
}

void classForecastManager::indicateCurrent() {
    forecasts[currentForecast].indicate(currentHour);
}

void classForecastManager::setCurrentHour(int hour) {
    this->currentHour = hour;
    preferences.putInt("hour", hour);
}

void classForecastManager::setCurrentForecast(int forecast) {
    this->currentForecast = forecast;
    preferences.putInt("forecast", forecast);
}
