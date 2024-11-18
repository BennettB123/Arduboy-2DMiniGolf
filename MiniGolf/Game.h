#pragma once

#include "Ball.h"
#include "Camera.h"
#include "Map.h"
#include <Arduboy2.h>

class Game
{
private:
    Arduboy2 _arduboy;
    Map _map;
    Camera _camera;
    Ball _ball;

public:
    Game(Arduboy2 arduboy) : _arduboy(arduboy)
    {
        _map = GetMap1();
        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
    }

    void Tick(float secondsDelta)
    {
        if (_arduboy.pressed(UP_BUTTON))
            _camera.MoveUp();
        if (_arduboy.pressed(DOWN_BUTTON))
            _camera.MoveDown();
        if (_arduboy.pressed(LEFT_BUTTON))
            _camera.MoveLeft();
        if (_arduboy.pressed(RIGHT_BUTTON))
            _camera.MoveRight();
    }

    void Display()
    {
        _camera.DrawMap(_map);
        _camera.DrawBall(_ball);
        _camera.DrawHole(_map.end);
    }
};