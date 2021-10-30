#include "Location.h"

#include "Forecast.h"
#include "MainMenu.h"
#include "Motor.h"

classLocation Location;

void classLocation::activate() {
    pmenu = new Menu(FORECAST_COUNT + 1);
    for (int n = 0; n < FORECAST_COUNT; n++) {
        strcpy(pmenu->ppitems[n]->text, ForecastManager.forecasts[n].location);
    }
    strcpy(pmenu->ppitems[FORECAST_COUNT]->text, "Main Menu");
    pmenu->setCurrentChoice(ForecastManager.getCurrentForecast());
    pmenu->start();

    EventManager.addListener(EVENT_MENU, this);
}

void classLocation::deactivate() {
    EventManager.removeListener(this);

    delete pmenu;
}

bool classLocation::onEvent(Event *pevent) {
    switch (pevent->type) {
        case EVENT_MENU:
            handleMenuEvent((MenuEvent *)pevent);
            break;
    }
    return true;
}

void classLocation::handleMenuEvent(MenuEvent *pevent) {
    Serial.printf("Location %d chosen\n", pevent->choice);

    if (pevent->choice < FORECAST_COUNT) {
        ForecastManager.setCurrentForecast(pevent->choice);
        ForecastManager.indicateCurrent();
    } else {
        MainMenu.activate();
        this->deactivate();
    }
}