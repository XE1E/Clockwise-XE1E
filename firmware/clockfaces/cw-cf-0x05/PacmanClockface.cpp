#include "PacmanClockface.h"

unsigned long lastMillisPacman = 0;
unsigned long lastMillisTimePacman = 0;
unsigned long lastMillisSecPacman = 0;
unsigned long lastMillisSuperFood = 0;
unsigned long lastMillisFruit = 0;

Pacman *pacman;

PacmanClockface::PacmanClockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(display);
}

void PacmanClockface::setup(CWDateTime *dateTime, bool showSplash) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setFont(&hourFont);
  randomSeed(dateTime->getMilliseconds() + millis());
  drawMap();
  updateClock();
}

void PacmanClockface::update()
{
  // Seconds blink
  if ((millis() - lastMillisSecPacman) >= 1000) {

    if (show_seconds) {
      Locator::getDisplay()->fillRect(31, 24, 2, 2, 0xFE40);
      Locator::getDisplay()->fillRect(31, 29, 2, 2, 0xFE40);
    } else  {
      Locator::getDisplay()->fillRect(31, 24, 2, 2, 0);
      Locator::getDisplay()->fillRect(31, 29, 2, 2, 0);
    }

    show_seconds = !show_seconds;
    lastMillisSecPacman = millis();
  }

  // Super food blink animation (every 300ms)
  if ((millis() - lastMillisSuperFood) >= 300) {
    updateSuperFoodBlink();
    lastMillisSuperFood = millis();
  }

  // Fruit spawn/despawn logic (every 15-30 seconds, visible for 5 seconds)
  if (!fruitActive && (millis() - lastMillisFruit) >= 15000 + random(15000)) {
    fruitActive = true;
    fruitTimer = millis();
    fruitType = random(2);  // 0=cherry, 1=strawberry
    drawFruit();
    lastMillisFruit = millis();
  } else if (fruitActive && (millis() - fruitTimer) >= 5000) {
    // Clear fruit after 5 seconds
    fruitActive = false;
    Locator::getDisplay()->fillRect(30, 44, 5, 5, 0x0000);
  }

  // Clock
  if (millis() - lastMillisTimePacman >= 60000) {
    updateClock();
    lastMillisTimePacman = millis();
  }

  // Pacman
  if (millis() - lastMillisPacman >= 100) {

    bool fullBlock =
                     ((pacman->_direction == Direction::LEFT || pacman->_direction == Direction::RIGHT) && (pacman->getX()-2) % 5 == 0) ||
                     ((pacman->_direction == Direction::UP || pacman->_direction == Direction::DOWN) && (pacman->getY()-2) % 5 == 0);

    if (fullBlock) {
      int currentMapR = (pacman->getY() - 2) / 5;
      int currentMapC = (pacman->getX() - 2) / 5;

      // Teleport tunnel logic - GATE blocks at edges (teleport when entering tunnel)
      if (_MAP[currentMapR][currentMapC] == MapBlock::GATE ||
          _MAP_CONST[currentMapR][currentMapC] == MapBlock::GATE) {
        if (currentMapC == 0) {
          // At left tunnel - teleport to right side
          Locator::getDisplay()->fillRect(pacman->getX(), pacman->getY(), 5, 5, 0);
          pacman->setPosition((11 * 5) + 2, pacman->getY());
          pacman->turn(Direction::LEFT);  // Continue moving left after teleport
          _MAP[currentMapR][0] = MapBlock::EMPTY;
          _MAP[currentMapR][11] = MapBlock::EMPTY;
          pacman->update();
          lastMillisPacman = millis();
          return;  // Skip rest of this frame
        } else if (currentMapC == 11) {
          // At right tunnel - teleport to left side
          Locator::getDisplay()->fillRect(pacman->getX(), pacman->getY(), 5, 5, 0);
          pacman->setPosition(2, pacman->getY());
          pacman->turn(Direction::RIGHT);  // Continue moving right after teleport
          _MAP[currentMapR][0] = MapBlock::EMPTY;
          _MAP[currentMapR][11] = MapBlock::EMPTY;
          pacman->update();
          lastMillisPacman = millis();
          return;  // Skip rest of this frame
        }
      }

      MapBlock currentBlockContent = static_cast<MapBlock>(_MAP[currentMapR][currentMapC]);
      _MAP[currentMapR][currentMapC] = MapBlock::EMPTY;

      if (currentBlockContent == MapBlock::SUPER_FOOD) {
        pacman->setState(Pacman::State::INVENCIBLE);
      }

      // Check if Pac-Man ate the fruit
      if (fruitActive) {
        int fruitMapR = 8;  // Fruit position row (44-2)/5 = 8
        int fruitMapC = 6;  // Fruit position col (30-2)/5 = 5.6 ~ 6
        if (currentMapR >= 7 && currentMapR <= 9 && currentMapC >= 5 && currentMapC <= 7) {
          fruitActive = false;
          Locator::getDisplay()->fillRect(30, 44, 5, 5, 0x0000);
          // Bonus: extend invincibility or just visual feedback
          pacman->setState(Pacman::State::INVENCIBLE);
        }
      }

      MapBlock nextBlk = nextBlock();
      directionDecision(nextBlk, (pacman->_direction == Direction::LEFT || pacman->_direction == Direction::RIGHT));

      if (countBlocks(MapBlock::FOOD) == 0 && countBlocks(MapBlock::SUPER_FOOD) == 0) {
         resetMap();
      }
    }

    pacman->update();
    lastMillisPacman = millis();
  }
}

void PacmanClockface::updateSuperFoodBlink() {
  superFoodBlink = !superFoodBlink;
  uint16_t spcfood_color = superFoodBlink ? 0xFBE0 : 0x7920;  // Orange / darker orange

  for (int i = 0; i < MAP_SIZE; i++) {
    for (int j = 0; j < MAP_SIZE; j++) {
      if (_MAP[j][i] == MapBlock::SUPER_FOOD) {
        Locator::getDisplay()->fillRect((i*5)+3, (j*5)+3, 3, 3, spcfood_color);
      }
    }
  }
}

void PacmanClockface::drawFruit() {
  int fx = 30;  // Center of screen X
  int fy = 44;  // Below clock area Y

  if (fruitType == 0) {
    // Cherry - two red circles with green stem
    Locator::getDisplay()->fillRect(fx, fy+2, 2, 2, FRUIT_CHERRY);
    Locator::getDisplay()->fillRect(fx+3, fy+2, 2, 2, FRUIT_CHERRY);
    Locator::getDisplay()->drawPixel(fx+1, fy+1, 0x07E0);  // Stem
    Locator::getDisplay()->drawPixel(fx+2, fy, 0x07E0);
    Locator::getDisplay()->drawPixel(fx+3, fy+1, 0x07E0);
  } else {
    // Strawberry - pink/red with seeds
    Locator::getDisplay()->fillRect(fx+1, fy, 3, 4, FRUIT_STRAWBERRY);
    Locator::getDisplay()->drawPixel(fx, fy+1, FRUIT_STRAWBERRY);
    Locator::getDisplay()->drawPixel(fx, fy+2, FRUIT_STRAWBERRY);
    Locator::getDisplay()->drawPixel(fx+4, fy+1, FRUIT_STRAWBERRY);
    Locator::getDisplay()->drawPixel(fx+4, fy+2, FRUIT_STRAWBERRY);
    // Green top
    Locator::getDisplay()->drawPixel(fx+2, fy-1, 0x07E0);
    // Yellow seeds
    Locator::getDisplay()->drawPixel(fx+1, fy+1, 0xFFE0);
    Locator::getDisplay()->drawPixel(fx+3, fy+2, 0xFFE0);
  }
}

const char* PacmanClockface::weekDayName(int weekday) {
  strncpy(weekDayTemp, _weekDayWords + (weekday*4), 4);
  return weekDayTemp;
}

const char* PacmanClockface::monthName(int month) {
  strncpy(monthTemp, _monthWords + ((month-1)*4), 4);
  return monthTemp;
}

void PacmanClockface::updateClock() {
    Locator::getDisplay()->fillRect(14, 19, 36, 26, 0x0000);

    Locator::getDisplay()->setFont(&Picopixel);
    Locator::getDisplay()->setTextColor(0xAD55);
    Locator::getDisplay()->setCursor(20, 41);
    Locator::getDisplay()->print(this->_dateTime->getDay());
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(monthName(this->_dateTime->getMonth()));

    Locator::getDisplay()->setFont(&hourFont);
    Locator::getDisplay()->setTextColor(0xFE40);
    Locator::getDisplay()->setCursor(15, 28);

    Locator::getDisplay()->print(this->_dateTime->getHour("00"));
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(this->_dateTime->getMinute("00"));
}

bool PacmanClockface::isValid(int r, int c) {
    if (r < 0 || r >= MAP_SIZE || c < 0 || c >= MAP_SIZE) {
        return false;
    }
    MapBlock block = static_cast<MapBlock>(_MAP[r][c]);
    return contains(block, PACMAN_MOVING_BLOCKS) || block == MapBlock::SUPER_FOOD;
}

bool PacmanClockface::isTarget(int r, int c) {
    if (r < 0 || r >= MAP_SIZE || c < 0 || c >= MAP_SIZE) {
        return false;
    }
    MapBlock block = static_cast<MapBlock>(_MAP[r][c]);
    return block == MapBlock::FOOD || block == MapBlock::SUPER_FOOD;
}

void PacmanClockface::reconstructPath(Point start, Point end, Direction& nextMove) {
    Point current = end;
    Point prev = parent[current.x][current.y];

    while (!(prev.x == start.x && prev.y == start.y)) {
        current = prev;
        prev = parent[current.x][current.y];
        if (current.x == -1 || current.y == -1) return;
    }

    if (current.x > start.x) nextMove = Direction::DOWN;
    else if (current.x < start.x) nextMove = Direction::UP;
    else if (current.y > start.y) nextMove = Direction::RIGHT;
    else if (current.y < start.y) nextMove = Direction::LEFT;
}

bool PacmanClockface::findShortestPath(int startR, int startC, Direction& nextMove) {
    queueFront = 0;
    queueRear = -1;
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            visited[i][j] = false;
            parent[i][j] = {-1, -1};
        }
    }

    Point startPoint = {startR, startC};
    visited[startR][startC] = true;
    queue[++queueRear] = startPoint;

    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    while (queueFront <= queueRear) {
        Point current = queue[queueFront++];

        if (isTarget(current.x, current.y)) {
            reconstructPath(startPoint, current, nextMove);
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            int nextR = current.x + dRow[i];
            int nextC = current.y + dCol[i];

            if (isValid(nextR, nextC) && !visited[nextR][nextC]) {
                visited[nextR][nextC] = true;
                parent[nextR][nextC] = current;
                queue[++queueRear] = {nextR, nextC};

                if (queueRear >= MAX_QUEUE_SIZE - 1) {
                    return false;
                }
            }
        }
    }

    return false;
}

void PacmanClockface::directionDecision(MapBlock nextBlk, bool moving_axis_x) {
    int currentMapR = (pacman->getY() - 2) / 5;
    int currentMapC = (pacman->getX() - 2) / 5;
    Direction nextMove = pacman->_direction;

    if (findShortestPath(currentMapR, currentMapC, nextMove)) {
        if (nextMove != pacman->_direction) {
             pacman->turn(nextMove);
        }
    } else {
         if (!contains(nextBlock(), PACMAN_MOVING_BLOCKS)) {
            turnRandom();
         }
         MapBlock immediateNext = nextBlock();
         if (contains(immediateNext, PACMAN_BLOCKING_BLOCKS)) {
             turnRandom();
         }
    }
}

void PacmanClockface::resetMap() {
  memcpy( _MAP, _MAP_CONST, sizeof(_MAP_CONST) );

  // Change maze color every 30 resets
  resetCounter++;
  if (resetCounter >= 30) {
    resetCounter = 0;
    currentMazeColor = (currentMazeColor + 1) % 6;
    wall_color = MAZE_COLORS[currentMazeColor];
  }

  // Reset fruit state
  fruitActive = false;
  fruitTimer = millis();

  drawMap();
  updateClock();
}

int PacmanClockface::countBlocks(PacmanClockface::MapBlock elem) {
  int count = 0;
  for (int i = 0; i<MAP_SIZE; i++) {
    for (int j = 0; j<MAP_SIZE; j++) {
      if (_MAP[i][j] == elem)
        count++;
    }
  }
  return count;
}

void PacmanClockface::turnRandom() {
  int dir = random(4);

  do {
    pacman->turn(static_cast<Direction>(dir));
    dir = random(4);
  } while (!contains(nextBlock(), PACMAN_MOVING_BLOCKS));
}

PacmanClockface::MapBlock PacmanClockface::nextBlock() {
  return nextBlock(pacman->_direction);
}

PacmanClockface::MapBlock PacmanClockface::nextBlock(Direction dir) {
  PacmanClockface::MapBlock map_block = PacmanClockface::MapBlock::OUT_OF_MAP;

  if (dir == Direction::RIGHT) {
    if (pacman->getX()+pacman->SPRITE_SIZE < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[(pacman->getY()-2)/5][((pacman->getX()-2)/5)+1]);
    }
  } else if (dir == Direction::DOWN) {
    if (pacman->getY()+pacman->SPRITE_SIZE < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[((pacman->getY()-2)/5)+1][(pacman->getX()-2)/5]);
    }
  } else if (dir == Direction::LEFT) {
    if ((pacman->getX()-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[(pacman->getY()-2)/5][((pacman->getX()-2)/5)-1]);
    }
  } else if (dir == Direction::UP) {
    if ((pacman->getY()-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[((pacman->getY()-2)/5)-1][((pacman->getX()-2)/5)]);
    }
  }

  return map_block;
}

bool PacmanClockface::contains(int v, const int* values) {
  for (int i = 1; i<values[0]+1; i++) {
    if (v == values[i])
      return true;
  }
  return false;
}

void PacmanClockface::drawMap()
{
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);

  uint16_t food_color = 0xB58C;
  uint16_t spcfood_color = 0xFBE0;

  Locator::getDisplay()->drawRect(0,0,64,64,wall_color);
  Locator::getDisplay()->drawRect(1,1,62,62,wall_color);

  for (int i=0; i<MAP_SIZE; i++) {
    for (int j=0; j<MAP_SIZE; j++) {
      if (_MAP[j][i] == MapBlock::FOOD) {
        Locator::getDisplay()->fillRect((i*5)+3,(j*5)+4,3,1,food_color);
      } else if (_MAP[j][i] == MapBlock::WALL) {
        Locator::getDisplay()->fillRect((i*5)+2,(j*5)+2,5,5,wall_color);
      } else if (_MAP[j][i] == MapBlock::CLOCK) {
        Locator::getDisplay()->fillRect((i*5)+2,(j*5)+2,5,5,wall_color);
      } else if (_MAP[j][i] == MapBlock::GATE) {
        // Tunnel entrance - don't draw anything (empty passage)
      } else if (_MAP[j][i] == MapBlock::SUPER_FOOD) {
        Locator::getDisplay()->fillRect((i*5)+3,(j*5)+3,3,3,spcfood_color);
      } else if (_MAP[j][i] == MapBlock::PACMAN) {
        pacman = new Pacman((i*5)+2,(j*5)+2);
      }
    }
  }

  // Draw static decorative ghosts
  drawGhosts();
}

void PacmanClockface::drawGhosts() {
  // Simple 5x5 ghost shape - body with eyes
  for (int g = 0; g < 4; g++) {
    int gx = GHOST_POSITIONS[g][0];
    int gy = GHOST_POSITIONS[g][1];
    uint16_t color = GHOST_COLORS[g];

    // Ghost body (rounded top, wavy bottom)
    Locator::getDisplay()->fillRect(gx+1, gy, 3, 4, color);   // Main body
    Locator::getDisplay()->drawPixel(gx, gy+1, color);        // Left curve
    Locator::getDisplay()->drawPixel(gx, gy+2, color);
    Locator::getDisplay()->drawPixel(gx, gy+3, color);
    Locator::getDisplay()->drawPixel(gx+4, gy+1, color);      // Right curve
    Locator::getDisplay()->drawPixel(gx+4, gy+2, color);
    Locator::getDisplay()->drawPixel(gx+4, gy+3, color);
    // Wavy bottom
    Locator::getDisplay()->drawPixel(gx, gy+4, color);
    Locator::getDisplay()->drawPixel(gx+2, gy+4, color);
    Locator::getDisplay()->drawPixel(gx+4, gy+4, color);

    // Eyes (white with blue pupils)
    Locator::getDisplay()->drawPixel(gx+1, gy+1, 0xFFFF);     // Left eye white
    Locator::getDisplay()->drawPixel(gx+3, gy+1, 0xFFFF);     // Right eye white
    Locator::getDisplay()->drawPixel(gx+1, gy+2, 0x001F);     // Left pupil
    Locator::getDisplay()->drawPixel(gx+3, gy+2, 0x001F);     // Right pupil
  }
}
