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

    const char* _weekDayWords = "SUN\0MON\0TUE\0WED\0THU\0FRI\0SAT\0";
    const char* _monthWords = "JAN\0FEB\0MAR\0APR\0MAY\0JUN\0JUL\0AUG\0SEP\0OCT\0NOV\0DEC\0";
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
