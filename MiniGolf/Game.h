#pragma once

#include "Ball.h"
#include "Camera.h"
#include "Map.h"
#include <Arduboy2.h>

enum class GameState
{
    Aiming,
    MapExplorer,
    BallInMotion,
};

class Game
{
private:
    Arduboy2 _arduboy;
    Map _map;
    Camera _camera;
    Ball _ball;
    GameState _gameState;

public:
    Game(Arduboy2 arduboy) : _arduboy(arduboy)
    {
        _map = GetMap1();
        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
        _gameState = GameState::Aiming;
    }

    void Tick(float secondsDelta)
    {
        HandleInput(secondsDelta);

        if (_gameState == GameState::BallInMotion)
        {
            _ball.Tick(secondsDelta);
            if (_ball.Stopped())
                _gameState = GameState::Aiming;
        }

        if (_gameState != GameState::MapExplorer)
            _camera.FocusOn(_ball.x, _ball.y);
    }

    void Display()
    {
        _camera.DrawMap(_map);
        _camera.DrawBall(_ball);
        _camera.DrawHole(_map.end);

        if (_gameState == GameState::Aiming)
            _camera.DrawAimHud(_ball);

        if (_gameState == GameState::MapExplorer)
            _camera.DrawMapExplorerIndicator();
    }

private:
    void HandleInput(float secondsDelta)
    {
        switch (_gameState)
        {
            case GameState::Aiming:
                if (_arduboy.justPressed(B_BUTTON))
                    _gameState = GameState::MapExplorer;
                if (_arduboy.justPressed(A_BUTTON))
                {
                    _gameState = GameState::BallInMotion;
                    _ball.StartHit();
                }
                if (_arduboy.pressed(LEFT_BUTTON))
                    _ball.RotateDirection(0.02);
                if (_arduboy.pressed(RIGHT_BUTTON))
                    _ball.RotateDirection(-0.02);
                break;

            case GameState::MapExplorer:
                if (_arduboy.justPressed(B_BUTTON))
                {
                    _gameState = GameState::Aiming;
                    _camera.FocusOn(_ball.x, _ball.y);
                }
                if (_arduboy.pressed(UP_BUTTON))
                    _camera.MoveUp();
                if (_arduboy.pressed(DOWN_BUTTON))
                    _camera.MoveDown();
                if (_arduboy.pressed(LEFT_BUTTON))
                    _camera.MoveLeft();
                if (_arduboy.pressed(RIGHT_BUTTON))
                    _camera.MoveRight();
                break;
        }
    }
};