#pragma once

#include "Vector.h"
#include <Arduboy2.h>

struct Ball
{
    float x, y;
    Vector velocity;    // used for when the ball is in motion
    float direction;    // used for choosing which direction to hit the ball
    float power = (MaxPower + MinPower) / 2;    // how hard to hit the ball

    static constexpr uint8_t Radius = 2;
    static constexpr float Friction = .60;           // percentage to reduce velocity by every second
    static constexpr float MinVelocityThreshold = 3; // stop the ball when velocity is below this threshold
    static constexpr uint8_t MaxPower = 150;
    static constexpr uint8_t MinPower = 25;
    static constexpr uint8_t PowerChangePerSecond = 50;

    Ball() = default;
    Ball(float x, float y) : x(x), y(y) {}

    // TODO: take into account secondsDelta
    void RotateDirection(float delta)
    {
        direction += delta;
        if (direction < 0)
            direction = TWO_PI + direction;
        else if (direction > TWO_PI)
            direction = direction - TWO_PI;
    }

    void IncreasePower(float secondsDelta)
    {
        power += PowerChangePerSecond * secondsDelta;
        power = constrain(power, MinPower, MaxPower);
    }

    void DecreasePower(float secondsDelta)
    {
        power -= PowerChangePerSecond * secondsDelta;
        power = constrain(power, MinPower, MaxPower);
    }

    void StartHit()
    {
        velocity.x = cos(direction) * power;
        velocity.y = -sin(direction) * power;
    }

    void Move(float secondsDelta)
    {
        x += velocity.x * secondsDelta;
        y += velocity.y * secondsDelta;

        velocity.x -= velocity.x * Friction * secondsDelta;
        velocity.y -= velocity.y * Friction * secondsDelta;

        if (velocity.Length() < MinVelocityThreshold)
            velocity = {0, 0};
    }

    bool Stopped()
    {
        return velocity.x == 0 && velocity.y == 0;
    }
};