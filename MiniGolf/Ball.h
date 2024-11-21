#pragma once

#include <Arduboy2.h>
#include "Vector.h"

struct Ball
{
    float x;
    float y;
    Vector velocity; // used for when the ball is in motion
    float direction; // only used for choosing which direction to hit the ball

    static constexpr uint8_t Radius = 2;
    static constexpr float Friction = 0.95;
    static constexpr float MinVelocityCutoff = 1;

    Ball() = default;
    Ball(float x, float y) : x(x), y(y) {}

    void RotateDirection(float delta)
    {
        direction += delta;
        if (direction < 0)
            direction = TWO_PI + direction;
        else if (direction > TWO_PI)
            direction = direction - TWO_PI;
    }

    void StartHit() {
        float power = 100;
        velocity.x = cos(direction) * power;
        velocity.y = -sin(direction) * power;
    }

    void Tick(float secondsDelta) {
        x += velocity.x * secondsDelta;
        y += velocity.y * secondsDelta;

        // apply friction
        velocity.x -= (velocity.x * Friction) * secondsDelta;
        velocity.y -= (velocity.y * Friction) * secondsDelta;

        // check for minimum velocity cutoff
        if (abs(velocity.x) <= MinVelocityCutoff)
            velocity.x = 0;
        if (abs(velocity.y) <= MinVelocityCutoff)
            velocity.y = 0;
    }

    bool Stopped() {
        return velocity.x == 0 && velocity.y == 0;
    }
};