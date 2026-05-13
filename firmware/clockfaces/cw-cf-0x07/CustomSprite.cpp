#include "CustomSprite.h"

CustomSprite::CustomSprite(int8_t x, int8_t y) {
  _x = x;
  _y = y;
}

void CustomSprite::incFrame()
{
  _currentFrame++;

  if (_currentFrame >= _totalFrames)
    _currentFrame = 0;
}

void CustomSprite::setDimensions(uint8_t width, uint8_t height) {
  _width = width;
  _height = height;
}

int8_t CustomSprite::getX() {
  return _x;
}
int8_t CustomSprite::getY() {
  return _y;
}
uint8_t CustomSprite::getWidth() {
  return _width;
}
uint8_t CustomSprite::getHeight() {
  return _height;
}

const char* CustomSprite::name() {
  return "CUSTOM";
}
