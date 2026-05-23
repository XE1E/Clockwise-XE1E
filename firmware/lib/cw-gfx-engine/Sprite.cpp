#include "Sprite.h"

boolean Sprite::collidedWith(Sprite* sprite) {
  return (this->_x < sprite->_x + sprite->_width &&
          this->_x + this->_width > sprite->_x &&
          this->_y < sprite->_y + sprite->_height &&
          this->_y + this->_height > sprite->_y);
}
