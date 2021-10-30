#pragma once

#include "EventManager.h"
#include "EventReceiver.h"

class MenuItem {
public:
  char text[32];
};

class Menu: public EventReceiver {
public:
  Menu(int);
  virtual ~Menu();

  int items;
  MenuItem **ppitems;
  void start();

  void setCurrentChoice(int);
  virtual bool onEvent(Event* pevent);

private:
  int previousPosition, currentChoice;

  void next();
  void previous();
  void showCurrentChoice();
  void handleEncoderEvent(EncoderEvent*);
  void handleSwitchEvent(SwitchEvent*);
};
