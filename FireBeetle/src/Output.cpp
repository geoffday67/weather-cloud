#include "Output.h"

classOutput Output;

classOutput::classOutput() {}

classOutput::~classOutput() {}

void classOutput::begin() {
  pdisplay = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2);
  pdisplay->begin();
  pdisplay->sendBuffer();
  pdisplay->setDisplayRotation(U8G2_R2);
  setSize(FONT_MEDIUM);
  pdisplay->setFontMode(0);
  displayWidth = pdisplay->getDisplayWidth();
  displayHeight = pdisplay->getDisplayHeight();
}

void classOutput::clear() {
  pdisplay->clearBuffer();
}

void classOutput::flush() {
  pdisplay->sendBuffer();
}

void classOutput::addText(int x, int y, char c) {
  char s[2];
  s[0] = c;
  s[1] = 0;
  addText(x, y, s);
}

void classOutput::addText(int x, int y, const char *ptext) {
  int useX, useY;

  pdisplay->setFontPosTop();

  if (x == CENTRED) {
    useX = (displayWidth - pdisplay->getStrWidth(ptext)) / 2;
  } else {
    useX = x;
  }

  if (y == CENTRED) {
    useY = (displayHeight - pdisplay->getMaxCharHeight()) / 2;
  } else {
    useY = y;
  }
  pdisplay->drawStr(useX, useY, ptext);
}

void classOutput::setSize(int size) {
  fontSize = size;

  switch (size) {
    case FONT_TITLE:
    case FONT_LARGE:
      pdisplay->setFont(u8g2_font_helvR18_tr);
      break;
    case FONT_MEDIUM:
      pdisplay->setFont(u8g2_font_helvR12_tr);
      break;
    case FONT_SMALL:
      pdisplay->setFont(u8g2_font_profont15_tr);
      break;
  }
}

void classOutput::addFooter(const char *pleft, const char *pright) {
  setSize(FONT_SMALL);
  // Set baseline alignment, this means that text will be missing its descenders
  pdisplay->setFontPosBaseline();
  pdisplay->drawStr(0, displayHeight, pleft);
  pdisplay->drawStr(displayWidth - pdisplay->getStrWidth(pright), displayHeight, pright);
}

void classOutput::addTitle(const char *ptitle) {
  setSize(FONT_TITLE);
  //pdisplay->drawHLine(0, pdisplay->getMaxCharHeight() / 2, displayWidth);
  addText(CENTRED, 0, ptitle);
}

int classOutput::getCharWidth() {
  return pdisplay->getMaxCharWidth();
}

int classOutput::getCharHeight() {
  return pdisplay->getMaxCharHeight();
}

int classOutput::getDisplayWidth() {
  return pdisplay->getDisplayWidth();
}

int classOutput::getBufferSize() {
  return 8 * pdisplay->getBufferTileHeight() * pdisplay->getBufferTileWidth();
}

void classOutput::drawBitmap(void *pdata) {
  pdisplay->drawXBM(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (uint8_t*) pdata);
}

void classOutput::setBlank(bool blank) {
  this->blank = blank;
  if (blank) {
    delete[] pSnapshot;
    pSnapshot = new uint8_t[getBufferSize()];
    memcpy(pSnapshot, pdisplay->getBufferPtr(), getBufferSize());
    pdisplay->clearDisplay();
  } else {
    memcpy(pdisplay->getBufferPtr(), pSnapshot, getBufferSize());
    pdisplay->sendBuffer();
  }
}

void classOutput::enable(bool enabled) {
  pdisplay->setPowerSave(!enabled);
}

void classOutput::arrowUp(int x, int y) {
  pdisplay->drawTriangle(x, y, x - 10, y + 10, x + 10, y + 10);
}

void classOutput::arrowDown(int x, int y) {
  pdisplay->drawTriangle(x, y, x - 10, y - 10, x + 10, y - 10);
}

int classOutput::getStringWidth(const char* ptext) {
  return pdisplay->getStrWidth(ptext);
}

void classOutput::test() {
  clear();
  setSize(FONT_LARGE);
  addText(0, 0, "Geoff is great!");
  flush();

  pdisplay->sendF("caaaaaac", 0x026, 0, 5, 7, 6, 0, 255, 0x2f);

}