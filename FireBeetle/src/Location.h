#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"
#include "Menu.h"

class classLocation: public Screen, EventReceiver {
public:
    virtual void activate();
    virtual void deactivate();
    virtual bool onEvent(Event* pevent);

private:
    Menu *pmenu;
    void handleMenuEvent(MenuEvent *pevent);
};

extern classLocation Location;
