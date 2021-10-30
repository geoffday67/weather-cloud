#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"
#include "Menu.h"

class classHour: public Screen, EventReceiver {
public:
    virtual void activate();
    virtual void deactivate();
    virtual bool onEvent(Event* pevent);

private:
    const int availableHours[6] = {1, 2, 4, 8, 12, 24};

    Menu *pmenu;
    void handleMenuEvent(MenuEvent *pevent);
    int menuChoiceFromHours(int hour);
};

extern classHour Hour;
