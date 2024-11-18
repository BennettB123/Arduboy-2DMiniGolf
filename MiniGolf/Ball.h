#pragma once

struct Ball
{
    float x;
    float y;
    float _velocity;
    uint8_t _direction; // unit is brads (degrees but mapped to a 0-255 range)

    Ball() = default;
    Ball(float x, float y) : x(x), y(y) {}
};