#include "Menu.h"

#include "EventManager.h"
#include "Output.h"
#include "main.h"

Menu::Menu(int items) {
    this->items = items;
    ppitems = new MenuItem *[items];
    for (int n = 0; n < items; n++) {
        ppitems[n] = new MenuItem;
    }
    currentChoice = 0;
}

Menu::~Menu() {
    for (int n = 0; n < items; n++) {
        delete ppitems[n];
    }
    delete[] ppitems;

    EventManager.removeListener(this);
}

void Menu::setCurrentChoice(int choice) {
    this->currentChoice = choice;
}

void Menu::showCurrentChoice() {
    Output.clear();

    MenuItem *pitem = ppitems[currentChoice];
    char *ptext = strdup(pitem->text);
    char *pspace = NULL;
    Output.setSize(FONT_LARGE);
    if (Output.getStringWidth(ptext) > SCREEN_WIDTH) {
        Output.setSize(FONT_MEDIUM);
        pspace = strchr(ptext, ' ');
    }
    if (pspace == NULL) {
        Output.addText(CENTRED, CENTRED, ptext);
    } else {
        *pspace = '\0';
        Output.addText(0, 13, ptext);
        Output.addText(0, 33, pspace + 1);
    }
    if (currentChoice < items - 1) {
        Output.arrowUp(SCREEN_WIDTH / 2, 0);
    }
    if (currentChoice > 0) {
        Output.arrowDown(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 1);
    }
    free(ptext);

    Output.flush();
}

void Menu::next() {
    if (currentChoice >= items - 1) {
        return;
    }

    currentChoice++;
    showCurrentChoice();
}

void Menu::previous() {
    if (currentChoice <= 0) {
        return;
    }

    currentChoice--;
    showCurrentChoice();
}

void Menu::start() {
    previousPosition = getEncoderPosition();
    showCurrentChoice();

    EventManager.addListener(EVENT_ENCODER, this);
    EventManager.addListener(EVENT_SWITCH, this);
}

bool Menu::onEvent(Event *pevent) {
    switch (pevent->type) {
        case EVENT_ENCODER:
            handleEncoderEvent((EncoderEvent *)pevent);
            break;
        case EVENT_SWITCH:
            handleSwitchEvent((SwitchEvent *)pevent);
            break;
    }
    return true;
}

void Menu::handleEncoderEvent(EncoderEvent *pevent) {
    if (pevent->position > previousPosition) {
        next();
    } else {
        previous();
    }
    previousPosition = pevent->position;
}

void Menu::handleSwitchEvent(SwitchEvent *pevent) {
    if (pevent->pressed) {
        EventManager.queueEvent(new MenuEvent(currentChoice));
    }
}
