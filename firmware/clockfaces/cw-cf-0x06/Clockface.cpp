
#include "Clockface.h"


#define LIGHT_GREEN 0x754d
#define DARK_GREEN 0x0264

#define DARK_BLUE 0x016D
#define LIGHT_BLUE 0x24fe

#define LIGHT_BLACK 0X10c4

unsigned long lastMillis = 0;

const uint16_t* const pokemons[] PROGMEM = {pokemon1, pokemon2, pokemon3, pokemon4, pokemon5, pokemon6, pokemon7,};

Clockface::Clockface(Adafruit_GFX* display)
{
  _display = display;
  Locator::provide(display);
}

void Clockface::setup(CWDateTime *dateTime) {
  this->_dateTime = dateTime;

  randomSeed(dateTime->getMilliseconds() + millis());

  // Clear screen
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);

  // Draw background
  Locator::getDisplay()->drawRGBBitmap(0, 0, POKEDEX_BG, 64, 64);

  Locator::getDisplay()->setFont(&PKMN_RBYGSC4pt7b);

  refreshTime();
  refreshDate(this->_dateTime->getWeekday(), DARK_BLUE);
  updatePokemon();
  
}


void Clockface::update() 
{
  if (millis() - lastMillis >= 1000) {

    uint8_t seconds = this->_dateTime->getSecond();

    if (seconds == 0) {
      refreshTime();
      updatePokemon();
    }

    if (_dateTime->getMinute() == 0 && _dateTime->getSecond() == 0) {
      uint8_t wd = this->_dateTime->getWeekday();
      Serial.println(wd);
      //clean up the previous square
      refreshDate((wd == 0 ? 6 : wd-1), LIGHT_BLUE);
      // update date
      refreshDate(wd, DARK_BLUE);
    }

    updateLoadingBar(seconds);

    // Update blink seconds
    uint16_t color = random(LONG_MAX);
    Locator::getDisplay()->fillRect(5, 4, 2, 4, color);
    Locator::getDisplay()->fillRect(4, 5, 4, 2, color);

    lastMillis = millis();
  }

}

void Clockface::refreshDate(uint8_t weekday, uint16_t color) {
  // Update weekday
  uint8_t x = 36 + ((weekday > 3 ? (weekday-4) : weekday) * 6);
  uint8_t y = 35 + (weekday > 3 ? 5 : 0);

  Locator::getDisplay()->fillRect(x, y, 5, 4, color);
}


void Clockface::refreshTime() { 

  // Clean up the clock area
  Locator::getDisplay()->fillRect(35, 17, 26, 14, LIGHT_BLACK);

  snprintf(minutes, sizeof(minutes), "%02d", _dateTime->getMinute());

  Locator::getDisplay()->setCursor(35, 22);
  Locator::getDisplay()->print(_dateTime->getHour());

  Locator::getDisplay()->setCursor(46, 30);
  Locator::getDisplay()->print(minutes);

  if (!_dateTime->is24hFormat())
    Locator::getDisplay()->drawBitmap(55, 18, (_dateTime->isAM() ? AM_SIGN : PM_SIGN), 4, 4, 0xffff);
}

void Clockface::updatePokemon() { 
  Locator::getDisplay()->drawRGBBitmap(8, 21, pokemons[random(sizeof(pokemons)/4)], 16, 16);
}

void Clockface::updateLoadingBar(uint8_t seconds) {

  if (seconds == 0) {
    Locator::getDisplay()->fillRect(9, 53, 11, 5, LIGHT_GREEN);
  } else {
    Locator::getDisplay()->fillRect(9, 53, ((10*seconds) / 59)+1, 5, DARK_GREEN);
  }
  
}