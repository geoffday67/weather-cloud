#include "MainMenu.h"
#include "Calibrate.h"
#include "Hour.h"
#include "Location.h"

classMainMenu MainMenu;

void classMainMenu::activate() {
  pmenu = new Menu(3);
  strcpy(pmenu->ppitems[0]->text, "Hour");
  strcpy(pmenu->ppitems[1]->text, "Location");
  strcpy(pmenu->ppitems[2]->text, "Setup");
  pmenu->start();

  EventManager.addListener(EVENT_MENU, this);
}

void classMainMenu::deactivate() {
  EventManager.removeListener(this);

  delete pmenu;
}

bool classMainMenu::onEvent(Event *pevent) {
  switch (pevent->type) {
    case EVENT_MENU:
      handleMenuEvent((MenuEvent*) pevent);
      break;
  }
  return true;
}

void classMainMenu::handleMenuEvent(MenuEvent *pevent) {
  Serial.printf("Option %d chosen\n", pevent->choice);
  switch (pevent->choice) {
    case 0:
      Hour.activate();
      this->deactivate();
      break;
    case 1:
      Location.activate();
      this->deactivate();
      break;
    case 2:
      Calibrate.activate();
      this->deactivate();
      break;
  }
}
