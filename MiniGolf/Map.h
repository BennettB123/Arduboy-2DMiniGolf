#pragma once

#include <Arduboy2.h>

constexpr uint8_t MaxNumWalls = 20;

typedef struct Point8
{
    uint8_t x;
    uint8_t y;

    Point8() = default;
    Point8 (uint8_t x, uint8_t y): x(x), y(y) { }
};

typedef struct Wall
{
    Point8 p1;
    Point8 p2;

    Wall() = default;
    Wall(int8_t x1, int8_t y1, int8_t x2, int8_t y2)
    {
        p1 = Point8(x1, y1);
        p2 = Point8(x2, y2);
    }
};

typedef struct Map
{
    char *name;
    uint8_t par;
    int8_t width;
    int8_t height;
    Point8 start;
    Point8 end;
    Wall walls[MaxNumWalls];

    static constexpr uint8_t HoleRadius = 3;
};

Map GetMap1()
{
    // square map
    int8_t width = Arduboy2::width() - 1;
    int8_t height = width;

    int8_t wall1_x = 42;
    int8_t wall2_x = 84;
    int8_t cornerSize = 20;

    return {
        .name = "Squiggly Lane",
        .par = 5,
        .width = width,
        .height = height,
        .start = Point8(5, 5),
        .end = Point8(width - 10, height - 10),
        .walls =
            {
                // top borders
                Wall(0, 0, wall1_x - cornerSize, 0),
                Wall(wall1_x + cornerSize, 0, width - cornerSize, 0),

                // right border
                Wall(width, cornerSize, width, height),

                // bottom borders
                Wall(cornerSize, height, wall2_x - cornerSize, height),
                Wall(wall2_x + cornerSize, height, width, height),

                // left border
                Wall(0, 0, 0, height - cornerSize),

                // large walls
                Wall(wall1_x, cornerSize, wall1_x, 94),
                Wall(wall2_x, 32, wall2_x, height - cornerSize),

                // corners
                Wall(wall1_x - cornerSize, 0, wall1_x, cornerSize),
                Wall(0, height - cornerSize, cornerSize, height),
                Wall(wall1_x, cornerSize, wall1_x + cornerSize, 0),
                Wall(wall2_x - cornerSize, height, wall2_x, height - cornerSize),
                Wall(width - cornerSize, 0, width, cornerSize),
                Wall(wall2_x, height - cornerSize, wall2_x + cornerSize, height),

                // extra wall for an obstacle
                Wall(width / 2, height / 2, width / 6 * 5, height / 2),
            }};
};