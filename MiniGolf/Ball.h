#pragma once

#include "Vector.h"

struct Ball
{
private:
    bool _powerIncreasing = true;

public:
    float x = 0, y = 0;
    Vector velocity = {0, 0};   // used for when the ball is in motion
    float direction = 0;        // used for choosing which direction to hit the ball
    float power = DefaultPower; // how hard to hit the ball

    static constexpr uint8_t Radius = 2;
    static constexpr float Friction = .60;           // percentage to reduce velocity by every second
    static constexpr float MinVelocityThreshold = 3; // stop the ball when velocity is below this threshold
    static constexpr uint8_t MaxPower = 150;
    static constexpr uint8_t MinPower = 20;
    static constexpr uint8_t DefaultPower = (MaxPower + MinPower) / 2;
    static constexpr uint8_t PowerChangePerSecond = 100;

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

    void ResetPower()
    {
        power = DefaultPower;
        _powerIncreasing = true;
    }

    void TickPower(float secondsDelta)
    {
        if (_powerIncreasing)
        {
            power += PowerChangePerSecond * secondsDelta;
            if (power > MaxPower)
            {
                power = MaxPower;
                _powerIncreasing = false;
            }
        }
        else
        {
            power -= PowerChangePerSecond * secondsDelta;
            if (power < MinPower)
            {
                power = MinPower;
                _powerIncreasing = true;
            }
        }
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

    bool IsStopped()
    {
        return velocity.x == 0 && velocity.y == 0;
    }
};