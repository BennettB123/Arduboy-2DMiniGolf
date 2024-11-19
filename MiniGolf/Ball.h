#pragma once

constexpr float Friction = 40;

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
    }

    void StartHit() {
        velocity = 120;
    }

    void Tick(float secondsDelta) {
        float xDelta = secondsDelta * cos(direction) * velocity;
        float yDelta = -(secondsDelta * sin(direction) * velocity);

        x += xDelta;
        y += yDelta;

        velocity -= Friction * secondsDelta;
        if (velocity < 0)
            velocity = 0;
    }

    bool Stopped() {
        return velocity == 0;
    }
};