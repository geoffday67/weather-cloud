#include "Refresh.h"

#include <ArduinoStreamParser.h>
#include <JsonStreamingParser2.h>
#include <RTClib.h>

#include "Forecast.h"
#include "MainMenu.h"
#include "Motor.h"
#include "WeatherHandler.h"
#include "main.h"

classRefresh Refresh;

#define NTP_SERVER "time.google.com"
#define NTP_PORT 123
#define NTP_OFFSET 2208988800UL

#define MQTT_SERVER "Geoffs-Mac-Mini"
#define MQTT_PORT 1883
#define MQTT_CLIENT "weather-cloud"
#define TIME_TOPIC "weather/time"
#define URL_TOPIC "weather/url"
#define FORECAST_TOPIC "weather/forecast"

void classRefresh::log(const char *ptopic, const char *pmessage) {
    return;

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, not logging");
        return;
    }

    if (mqtt.connect(MQTT_CLIENT)) {
        mqtt.publish(ptopic, pmessage, true);
        mqtt.disconnect();
        Serial.printf("Logged topic %s\n", ptopic);
    }
}

void classRefresh::syncTime() {
    byte ntpBuffer[48];
    IPAddress address;
    uint32_t ntpTime;
    uint32_t unixTime;
    unsigned long start;
    char s[64];
    DateTime dt;

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

    dt = DateTime(unixTime);
    sprintf(s, "Time adjusted to %s UTC", dt.timestamp(DateTime::TIMESTAMP_FULL).c_str());
    Serial.println(s);
    log(TIME_TOPIC, s);

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
    if (!connectWiFi()) {
        Serial.println("Error connecting WiFi");
        goto exit;
    }

    mqtt.setClient(wifiClientInsecure);
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);

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
    char s[256];

    // Form URL
    char *purl = new char[sizeof(WEATHER_URL) + 64];
    sprintf(purl, "%s&lat=%f&lon=%f", WEATHER_URL, pforecast->latitude, pforecast->longitude);
    sprintf(s, "Refreshing forecast from: %s", purl);
    log(URL_TOPIC, s);
    Serial.println(s);

    ArudinoStreamParser *pparser = new ArudinoStreamParser();
    WeatherHandler *phandler = new WeatherHandler();

    phandler->setForecast(pforecast);
    pparser->setHandler(phandler);

    // Get the weather data.
    wifiClientSecure.setInsecure();
    httpClient.setReuse(false);
    httpClient.begin(wifiClientSecure, purl);
    int code = httpClient.GET();
    if (code != 200) {
        Serial.printf("ERROR Status code %d received\n", code);
        goto exit;
    }

    // Parse it, building the forecast.
    pforecast->reset();
    int c;
    while (true) {
        c = wifiClientSecure.read();
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
    wifiClientSecure.stop();

    delete pparser;
    delete phandler;
    delete[] purl;

    return result;
}

bool classRefresh::getForecasts() {
    char s[128];
    Forecast *pforecast;
    bool result = false;

    for (int n = 0; n < FORECAST_COUNT; n++) {
        pforecast = ForecastManager.forecasts + n;
        if (!getForecast(pforecast)) {
            goto exit;
        }
        sprintf(s, "Fetched %d hours forecast for %s", pforecast->hourCount, pforecast->location);
        log(FORECAST_TOPIC, s);
        Serial.println(s);
    }

    result = true;

exit:
    return result;
}

bool classRefresh::connectWiFi() {
    bool result = false;
    unsigned long start;

    WiFi.hostname("Weather cloud");
    WiFi.begin("Wario", "mansion1");
    Serial.print("Connecting to Wario ");

    start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > 20000) {
            Serial.println();
            Serial.println("Timed out connecting to access point");
            goto exit;
        }
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    result = true;

exit:
    return result;
}
