#pragma once

struct Ball
{
    float x;
    float y;
    float velocity;
    float direction; // radians

    Ball() = default;
    Ball(float x, float y) : x(x), y(y) {}

    void RotateDirection(float delta)
    {
        direction += delta;
        if (direction < 0)
            direction = TWO_PI + direction;
        else if (direction > TWO_PI)
            direction = direction - TWO_PI;

        Serial.println(direction);
    }
};