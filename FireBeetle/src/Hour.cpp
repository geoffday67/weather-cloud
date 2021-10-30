#include "Hour.h"

#include "Forecast.h"
#include "MainMenu.h"
#include "main.h"

classHour Hour;

int classHour::menuChoiceFromHours(int hour) {
    int count = countof(availableHours);
    for (int n = 0; n < count; n++) {
        if (availableHours[n] == hour) {
            return n;
        }
    }
    return 0;
}

void classHour::activate() {
    int count = countof(availableHours);
    pmenu = new Menu(count + 1);
    for (int n = 0; n < count; n++) {
        sprintf(pmenu->ppitems[n]->text, "%d", availableHours[n]);
    }
    strcpy(pmenu->ppitems[count]->text, "Main Menu");
    pmenu->setCurrentChoice(menuChoiceFromHours(ForecastManager.getCurrentHour()));
    pmenu->start();

    EventManager.addListener(EVENT_MENU, this);
}

void classHour::deactivate() {
    EventManager.removeListener(this);

    delete pmenu;
}

bool classHour::onEvent(Event *pevent) {
    switch (pevent->type) {
        case EVENT_MENU:
            handleMenuEvent((MenuEvent *)pevent);
            break;
    }
    return true;
}

void classHour::handleMenuEvent(MenuEvent *pevent) {
    Serial.printf("Hour %d chosen\n", availableHours[pevent->choice]);

    int count = countof(availableHours);
    if (pevent->choice < count) {
        ForecastManager.setCurrentHour(availableHours[pevent->choice]);
        ForecastManager.indicateCurrent();
    } else {
        MainMenu.activate();
        this->deactivate();
    }
}
