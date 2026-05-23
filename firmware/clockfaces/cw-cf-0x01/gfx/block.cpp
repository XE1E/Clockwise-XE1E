#include "block.h"

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

  if (millis() - coinMillis >= 80) {
    int coinX = _x + 7;

    // Use _firstY (resting position) not _y (changes during bounce)
    // Coin starts at row 1 (just below LED row 0) and stays there briefly
    int coinDrawY = 1;

    coinFrame++;

    if (coinFrame <= 6) {
      // Draw alternating coin frames at fixed position above block
      if (coinFrame % 2 == 1) {
        Locator::getDisplay()->drawRGBBitmap(coinX, coinDrawY, COIN_FRAME1, 5, 7);
      } else {
        // Clear and redraw rotated frame
        Locator::getDisplay()->fillRect(coinX, coinDrawY, 5, 7, SKY_COLOR);
        Locator::getDisplay()->drawRGBBitmap(coinX + 1, coinDrawY, COIN_FRAME2, 3, 7);
      }
    } else {
      // Animation finished - clear coin area
      Locator::getDisplay()->fillRect(coinX, coinDrawY, 5, 7, SKY_COLOR);
      coinActive = false;
      coinFrame = 0;
    }
    coinMillis = millis();
  }
} 

void Block::hit() {
  if (_state != HIT && _state != USED) {
    _lastState = _state;
    _state = HIT;
    direction = UP;

    coinActive = true;
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

      if ((_firstY - _y) >= MAX_MOVE_HEIGHT) {
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
  if (event == EventType::MOVE) {
    if (this->collidedWith(caller)) {
      hit();
      Locator::getEventBus()->broadcast(EventType::COLLISION, this);
    }
  }
}


const char* Block::name() {
  return "BLOCK";
}