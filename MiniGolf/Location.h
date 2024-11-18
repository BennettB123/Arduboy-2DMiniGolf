#pragma once

struct Location
{
    float x;
    float y;

    Location() = default;
    Location(float x, float y) : x(x), y(y) {}
};