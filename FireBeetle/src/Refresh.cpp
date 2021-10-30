#include "Refresh.h"

#include <ArduinoStreamParser.h>
#include <JsonStreamingParser2.h>

#include "MainMenu.h"
#include "Motor.h"
#include "Forecast.h"
#include "WeatherHandler.h"
#include "main.h"

classRefresh Refresh;

#define NTP_SERVER "time.google.com"
#define NTP_PORT 123
#define NTP_OFFSET 2208988800UL

void classRefresh::syncTime() {
    byte ntpBuffer[48];
    IPAddress address;
    uint32_t ntpTime;
    uint32_t unixTime;
    unsigned long start;

    ntpUDP.begin(NTP_PORT);

    if (!WiFi.hostByName(NTP_SERVER, address)) {
        Serial.println("Error looking up time server address");
        return;
    }
    Serial.printf("Time server address %s\n", address.toString().c_str());

    memset(ntpBuffer, 0, 48);
    ntpBuffer[0] = 0b11100011;
    ntpUDP.beginPacket(address, NTP_PORT);
    ntpUDP.write(ntpBuffer, 48);
    ntpUDP.endPacket();

    Serial.print("Waiting for NTP response ... ");
    start = millis();
    while (ntpUDP.parsePacket() == 0) {
        if (millis() - start > 10000) {
            Serial.println("timeout");
            return;
        }
        delay(10);
    }

    ntpUDP.read(ntpBuffer, 48);
    ntpTime = (ntpBuffer[40] << 24) | (ntpBuffer[41] << 16) | (ntpBuffer[42] << 8) | ntpBuffer[43];
    unixTime = ntpTime - NTP_OFFSET;

    Serial.printf("Unix time now %d\n", unixTime);

    struct timeval tv;
    tv.tv_sec = unixTime;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
}

void classRefresh::activate() {
    bool ok = false;

    showWaiting();

    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    connectWiFi();

    syncTime();
    if (!getForecasts()) {
        goto exit;
    }

    ForecastManager.indicateCurrent();
    ok = true;

exit:
    if (ok) {
        showSuccess();
    } else {
        showFailure();
    }
    delay(2000);
}

void classRefresh::deactivate() {
}

bool classRefresh::onEvent(Event *pevent) {
    return true;
}

bool classRefresh::getForecast(Forecast *pforecast) {
    bool result = false;

    // Form URL
    char *purl = new char[sizeof(WEATHER_URL) + 64];
    sprintf(purl, "%s&lat=%f&lon=%f", WEATHER_URL, pforecast->latitude, pforecast->longitude);
    Serial.printf("Refreshing forecast from: %s\n", purl);

    ArudinoStreamParser *pparser = new ArudinoStreamParser();
    WeatherHandler *phandler = new WeatherHandler();

    phandler->setForecast(pforecast);
    pparser->setHandler(phandler);

    // Get the weather data.
    wifiClient.setInsecure();
    httpClient.setReuse(false);
    httpClient.begin(wifiClient, purl);
    int code = httpClient.GET();
    if (code != 200) {
        Serial.printf("ERROR Status code %d received\n", code);
        goto exit;
    }

    // Parse it, building the forecast.
    pforecast->reset();
    int c;
    while (true) {
        c = wifiClient.read();
        if (c == -1) {
            Serial.println("ERROR End of data reached");
            goto exit;
        }
        pparser->parse(c);
        if (phandler->isEnded()) {
            break;
        }
    }
    pforecast->save();

    if (pforecast->hourCount != 48) {
        goto exit;
    }

    result = true;

exit:
    httpClient.end();
    wifiClient.stop();

    delete pparser;
    delete phandler;
    delete[] purl;

    return result;
}

bool classRefresh::getForecasts() {
    Forecast *pforecast;
    bool result = false;

    for (int n = 0; n < FORECAST_COUNT; n++) {
        pforecast = ForecastManager.forecasts + n;
        if (!getForecast(pforecast)) {
            goto exit;
        }
        Serial.printf("Fetched %d hours forecast for %s\n", pforecast->hourCount, pforecast->location);
    }

    result = true;

exit:
    return result;
}

void classRefresh::connectWiFi() {
    int warioStrength = -999;
    int hobbyHouseStrength = -999;
    Serial.println("Scanning for networks");
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == "Wario") {
            warioStrength = WiFi.RSSI(i);
        }

        if (WiFi.SSID(i) == "HobbyHouse") {
            hobbyHouseStrength = WiFi.RSSI(i);
        }
    }

    if (warioStrength > hobbyHouseStrength) {
        WiFi.begin("Wario", "mansion1");
        Serial.print("Connecting to Wario ");
    } else {
        WiFi.begin("HobbyHouse", "mansion1");
        Serial.print("Connecting to HobbyHouse ");
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}
