#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

// Fonts
#include "hour_font.h"
#include "picopixel.h"

// GFX Engine
#include <Locator.h>
#include <Game.h>

// Commons
#include "IClockface.h"
#include "CWDateTime.h"

// Sprites
#include "pacman.h"

// Simple coordinate struct for BFS
struct Point {
  int x, y;
};

class PacmanClockface: public IClockface {
  private:
    static const int MAP_SIZE = 12;
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    bool pacmanState = true;
    bool show_seconds = true;

    const char* _weekDayWords = "DOM\0LUN\0MAR\0MIE\0JUE\0VIE\0SAB\0";
    const char* _monthWords = "ENE\0FEB\0MAR\0ABR\0MAY\0JUN\0JUL\0AGO\0SEP\0OCT\0NOV\0DIC\0";

    // Maze colors (muted/pastel - not too bright)
    const uint16_t MAZE_COLORS[6] = {
      0x0016,  // Dark blue (original)
      0x2104,  // Dark purple
      0x0208,  // Dark green
      0x4000,  // Dark red
      0x4208,  // Dark brown/orange
      0x0210   // Dark cyan
    };
    uint8_t currentMazeColor = 0;
    uint16_t wall_color = 0x0016;

    // Fruit system
    bool fruitActive = false;
    unsigned long fruitTimer = 0;
    uint8_t fruitType = 0;  // 0=cherry, 1=strawberry
    const uint16_t FRUIT_CHERRY = 0xF800;   // Red
    const uint16_t FRUIT_STRAWBERRY = 0xF81F; // Pink/magenta

    // Ghost positions (static decoration) - on top and bottom path rows
    const int GHOST_POSITIONS[4][2] = {{7, 2}, {52, 2}, {7, 57}, {52, 57}};
    const uint16_t GHOST_COLORS[4] = {0xF800, 0xFBE0, 0x07FF, 0xF81F};  // Red, Orange, Cyan, Pink

    // Super food animation
    bool superFoodBlink = false;
    char weekDayTemp[4]= "\0";
    char monthTemp[4]= "\0";

   // BFS related members
   static const int MAX_QUEUE_SIZE = MAP_SIZE * MAP_SIZE;
   Point queue[MAX_QUEUE_SIZE];
   int queueFront, queueRear;
   bool visited[MAP_SIZE][MAP_SIZE];
   Point parent[MAP_SIZE][MAP_SIZE];

    enum MapBlock {
      EMPTY = 0,
      FOOD = 1,
      WALL = 2,
      GATE = 3,
      SUPER_FOOD = 4,
      CLOCK = 5,
      GHOST = 6,
      PACMAN = 7,
      OUT_OF_MAP = 99
    };

    const byte _MAP_CONST[12][12] = {
      {4,1,1,1,1,1,7,1,1,1,1,4},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {4,1,1,1,1,1,1,1,1,1,1,4}
    };

    byte _MAP[12][12] = {
      {4,1,1,1,1,1,7,1,1,1,1,4},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {4,1,1,1,1,1,1,1,1,1,1,4}
    };

    const byte MAP_BORDER_SIZE = 2;
    const byte MAP_MIN_POS = 0 + MAP_BORDER_SIZE;
    const byte MAP_MAX_POS = 64 - MAP_BORDER_SIZE;

    const int PACMAN_MOVING_BLOCKS[4] = {3, MapBlock::EMPTY, MapBlock::FOOD, MapBlock::GATE};
    const int PACMAN_BLOCKING_BLOCKS[4] = {3, MapBlock::OUT_OF_MAP, MapBlock::WALL, MapBlock::CLOCK};

    void drawMap();
    void drawGhosts();
    void drawFruit();
    void updateSuperFoodBlink();
    PacmanClockface::MapBlock nextBlock(Direction dir);
    PacmanClockface::MapBlock nextBlock();
    void turnRandom();
    int countBlocks(PacmanClockface::MapBlock elem);
    bool contains(int v, const int* values);
    void resetMap();
    void directionDecision(MapBlock nextBlk, bool moving_axis_x);
    bool isValid(int r, int c);
    bool isTarget(int r, int c);
    bool findShortestPath(int startX, int startY, Direction& nextMove);
    void reconstructPath(Point start, Point end, Direction& nextMove);
    void updateClock();
    const char* weekDayName(int weekday);
    const char* monthName(int month);

  public:
    PacmanClockface(Adafruit_GFX* display);
    void setup(CWDateTime *dateTime, bool showSplash = true) override;
    void update() override;
};
