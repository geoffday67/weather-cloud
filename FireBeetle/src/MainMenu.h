#pragma once

#include "Screen.h"
#include "EventManager.h"
#include "EventReceiver.h"
#include "Menu.h"

class classMainMenu: public Screen, EventReceiver {
public:
  virtual void activate();
  virtual void deactivate();
  virtual bool onEvent(Event* pevent);

private:
  Menu *pmenu;
  void handleMenuEvent(MenuEvent *pevent);
};

extern classMainMenu MainMenu;

