#pragma once

#include <Arduino.h>
#include <Sprite.h>

class CustomSprite: public Sprite {
  public:
    CustomSprite(int8_t x, int8_t y);
    uint8_t _totalFrames = 0;
    uint8_t _currentFrame = 0;
    uint8_t _spriteReference = 0;
    uint8_t _currentFrameCount = 0;
    unsigned long _lastMillisSpriteFrames = 0;
    unsigned long _lastResetTime = 0;
    unsigned long _lastResetMoveTime = 0;
    bool _moving = false;
    unsigned long _moveStartTime = 1;
    unsigned long _moveDuration = 0;
    int8_t _moveInitialX = 0;
    int8_t _moveInitialY = 0;
    int8_t _moveTargetX = -1;
    int8_t _moveTargetY = -1;
    bool _shouldReturnToOrigin = false;
    bool _isReversing = false;

    // Cached JSON values (populated once at creation)
    bool _hourBased = false;  // If true, frame is selected by hour (0-23) instead of animating
    uint32_t _cachedLoopDelay = 0;
    uint16_t _cachedFrameDelay = 0;
    uint32_t _cachedMoveStartTime = 1;
    uint32_t _cachedMoveDuration = 0;
    int8_t _cachedMoveTargetX = -1;
    int8_t _cachedMoveTargetY = -1;
    int8_t _cachedOriginX = 0;
    int8_t _cachedOriginY = 0;
    bool _cachedShouldReturn = false;

    void setDimensions(uint8_t width, uint8_t height);
    int8_t getX();
    int8_t getY();
    uint8_t getWidth();
    uint8_t getHeight();

    const char* name();
    void incFrame();

  // Start moving the sprite to a new position
  void startMoving(int8_t targetX, int8_t targetY, unsigned long duration, bool shouldReturnToOrigin) {
    _moveStartTime = millis();
    _moveDuration = duration;
    _moveInitialX = getX();  // Current X position
    _moveInitialY = getY();  // Current Y position
    _moveTargetX = targetX;
    _moveTargetY = targetY;
    _shouldReturnToOrigin = shouldReturnToOrigin;
    _moving = true;
    _isReversing = false;
  }

  void reverseMoving(int8_t targetX, int8_t targetY) {
    _moveStartTime = millis();
    _moveDuration = _moveDuration;
    _moveInitialX = getX();  // Current X position
    _moveInitialY = getY();  // Current Y position
    _moveTargetX = targetX;
    _moveTargetY = targetY;
    _shouldReturnToOrigin = false;
    _moving = true;
    _isReversing = true;
  }

  // Stop the sprite's movement
  void stopMoving() {
    _moving = false;
  }

  // Check if the sprite is currently moving
  bool isMoving() {
    return _moving;
  }

  // Linear interpolation
  int8_t lerp(int8_t start, int8_t end, float t) {
    return start + static_cast<int8_t>(t * (end - start));
  }

  // Set the X position
  void setX(int8_t newX) {
    _x = newX;
  }  

  // Set the Y position
  void setY(int8_t newY) {
    _y = newY;
  }

  // Check if the sprite should return to its origin
  bool shouldReturnToOrigin() {
    return _shouldReturnToOrigin;
  }
};