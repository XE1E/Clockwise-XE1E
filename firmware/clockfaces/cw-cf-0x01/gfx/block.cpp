#include "block.h"

//String &Block::_text;

Block::Block(int x, int y) {
  _x = x;
  _y = y;
  _firstY = y;
  _width = 19;
  _height = 19;
}

void Block::idle() {
  if (_state != IDLE) {
    _lastState = _state;
    _state = IDLE;
    _y = _firstY;
  }
}

void Block::drawUsedBlock() {
  // Draw block with darker "used" color
  Locator::getDisplay()->fillRect(_x, _y, _width, _height, USED_BLOCK_COLOR);
  Locator::getDisplay()->drawRect(_x, _y, _width, _height, 0x0000);
  setTextBlock();
}

void Block::updateCoin() {
  if (!coinActive) return;

  if (millis() - coinMillis >= 40) {
    // Clear previous coin position
    int coinX = _x + 7;
    int prevCoinY = _y - 10 + coinY;
    Locator::getDisplay()->fillRect(coinX, prevCoinY, 5, 7, SKY_COLOR);

    coinY -= 2;  // Move coin up
    coinFrame = (coinFrame + 1) % 2;

    if (coinY < -20) {
      // Coin animation finished
      coinActive = false;
      coinY = 0;
    } else {
      // Draw coin at new position
      int newCoinY = _y - 10 + coinY;
      if (coinFrame == 0) {
        Locator::getDisplay()->drawRGBBitmap(coinX, newCoinY, COIN_FRAME1, 5, 7);
      } else {
        Locator::getDisplay()->drawRGBBitmap(coinX + 1, newCoinY, COIN_FRAME2, 3, 7);
      }
    }
    coinMillis = millis();
  }
} 

void Block::hit() {
  if (_state != HIT && _state != USED) {
    _lastState = _state;
    _state = HIT;
    _lastY = _y;
    direction = UP;

    // Start coin animation
    coinActive = true;
    coinY = 0;
    coinFrame = 0;
    coinMillis = millis();
  }
}

void Block::setTextBlock() {
  Locator::getDisplay()->setTextColor(0x0000);       
  
  
  if (_text.length() == 1) {
    Locator::getDisplay()->setCursor(_x+6, _y+12);
  }  else {
    Locator::getDisplay()->setCursor(_x+2, _y+12);
  }

  Locator::getDisplay()->print(_text);
}

void Block::setText(String text) {
  _text = text;
}

void Block::init() {
  Locator::getEventBus()->subscribe(this);
  Locator::getDisplay()->drawRGBBitmap(_x, _y, BLOCK, _width, _height);
  setTextBlock();  
}

void Block::update() {
  // Update coin animation
  updateCoin();

  if (_state == IDLE && _lastState != _state) {
    Locator::getDisplay()->drawRGBBitmap(_x, _y, BLOCK, _width, _height);
    setTextBlock();
    _lastState = _state;

  } else if (_state == USED) {
    // Check if 1 second has passed to return to normal
    if (millis() - usedMillis >= 1000) {
      _lastState = _state;
      _state = IDLE;
      Locator::getDisplay()->drawRGBBitmap(_x, _y, BLOCK, _width, _height);
      setTextBlock();
    }

  } else if (_state == HIT) {
    if (millis() - lastMillis >= 60) {
      Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);

      _y = _y + (MOVE_PACE * (direction == UP ? -1 : 1));

      // Draw used block color while bouncing
      drawUsedBlock();

      if (floor(_firstY - _y) >= MAX_MOVE_HEIGHT) {
        direction = DOWN;
      }

      if (_y >= _firstY && direction == DOWN) {
        // Transition to USED state instead of IDLE
        _lastState = _state;
        _state = USED;
        usedMillis = millis();
        _y = _firstY;
        drawUsedBlock();
      }

      lastMillis = millis();
    }
  }
}


void Block::execute(EventType event, Sprite* caller) {
  //Serial.println("Checking collision");

  if (event == EventType::MOVE) {
    if (this->collidedWith(caller)) {
      Serial.println("Collision detected");
      hit();
      Locator::getEventBus()->broadcast(EventType::COLLISION, this);
    }
  }
  
}


const char* Block::name() {
  return "BLOCK";
}