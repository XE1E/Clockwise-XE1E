#pragma once

#include <Arduino.h>
#include <Game.h>
#include <Locator.h>
#include <EventTask.h>
#include "assets.h"

const uint8_t MOVE_PACE = 2;
const uint8_t MAX_MOVE_HEIGHT = 4;

class Block: public Sprite, public EventTask {
  private:
    enum State {
      IDLE,
      HIT,
      USED
    };

    Direction direction;

    String _text;

    unsigned long lastMillis = 0;
    unsigned long usedMillis = 0;
    unsigned long coinMillis = 0;
    State _state = IDLE;
    State _lastState = IDLE;
    uint8_t _lastY;
    uint8_t _firstY;

    // Coin animation
    bool coinActive = false;
    int8_t coinY = 0;
    int8_t coinFrame = 0;

    void idle();
    void hit();
    void setTextBlock();
    void drawUsedBlock();
    void updateCoin();

  public:
    Block(int x, int y);
    void setText(String text);
    void init();
    void update();    
    const char* name();
    void execute(EventType event, Sprite* caller);

};
