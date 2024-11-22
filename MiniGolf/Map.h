#pragma once

#include <Arduboy2.h>

constexpr uint8_t MaxNumWalls = 20;

typedef struct Wall
{
    Point p1;
    Point p2;

    Wall() = default;
    Wall(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    {
        p1 = Point(x1, y1);
        p2 = Point(x2, y2);
    }
};

typedef struct Map
{
    int16_t width;
    int16_t height;
    Point start;
    Point end;
    Wall walls[MaxNumWalls];
};

Map GetMap1()
{
    int16_t width = 175;
    int16_t height = 125;

    return {
        .width = width,
        .height = height,
        .start = Point(10, 10),
        .end = Point(width - 10, height - 10),
        .walls =
            {
                // Border
                // TODO: remove border walls and have it just be implicit
                Wall(0, 0, width, 0),           // top border
                Wall(width, 0, width, height),  // right border
                Wall(0, height, width, height), // bottom border
                Wall(0, 0, 0, height),          // left border

                // Triangle 1
                Wall(32, 32, 60, 60),
                Wall(32, 32, 60, 32),
                Wall(60, 32, 60, 60),

                // Polygon 1
                Wall(85, 75, 110, 75),
                Wall(110, 75, 100, 100),
                Wall(100, 100, 75, 100),
                Wall(75, 100, 85, 75),
            }};
};