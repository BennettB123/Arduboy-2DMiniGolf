#pragma once

#include <Arduboy2.h>

constexpr uint8_t MaxNumWalls = 20;

struct Point8
{
    uint8_t x;
    uint8_t y;

    Point8() = default;
    Point8(uint8_t x, uint8_t y) : x(x), y(y) {}
};

struct Wall
{
    Point8 p1;
    Point8 p2;

    Wall() = default;
    Wall(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
    {
        p1 = Point8(x1, y1);
        p2 = Point8(x2, y2);
    }
};

struct Map
{
    uint8_t par;
    uint8_t width;
    uint8_t height;
    Point8 start;
    Point8 end;
    const char *name;
    Wall walls[MaxNumWalls];

    static constexpr uint8_t HoleRadius = 3;
};
