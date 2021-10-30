#pragma once

#define MAX_HOURS       48

class Entry {
public:
    unsigned long timestamp;    // UTC
    char icon[16];
};

class Forecast {
public:
    char location[64];
    double latitude, longitude;
    Entry hours[MAX_HOURS];
    int hourCount = 0;

    void addHour(unsigned long timestamp, const char *picon);
    void reset();
    void indicate(int position);
    void save();
    void load();

private:
    void makeKey(char *pdestination);
};

#define FORECAST_COUNT      3

class classForecastManager {
private:
    int currentHour = 0, currentForecast = 0;

public:
    void begin();
    
    void setCurrentHour(int);
    int getCurrentHour() { return currentHour; }
    void setCurrentForecast(int);
    int getCurrentForecast() { return currentForecast; }

    Forecast forecasts[FORECAST_COUNT];
    void indicateCurrent();
};

extern classForecastManager ForecastManager;
