#pragma once

#include "FX/ArduboyFX.h"
#include "FX/fxdata.h"
#include <Arduboy2.h>

constexpr uint8_t MaxNumWalls = 20;
constexpr uint8_t NumMaps = 1;

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
    Wall walls[MaxNumWalls];
    const char *name;

    static constexpr uint8_t HoleRadius = 3;
};

const char* const mapNames[NumMaps] PROGMEM = {
    "Squiggly Lane",
};

Map GetMap1()
{
    Map map;

    // read map from FX data
    FX::seekData(Maps);
    for (uint8_t i = 0; i < NumMaps; i++)
    {
        FX::readObject(map);
    }
    FX::readEnd();

    map.name = mapNames[0];

    return map;
}
