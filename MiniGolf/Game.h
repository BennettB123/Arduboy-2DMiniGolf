#pragma once

#include "Ball.h"
#include "Camera.h"
#include "CollisionHandler.h"
#include "Map.h"
#include <Arduboy2.h>

enum class GameState
{
    MapSummary,
    Aiming,
    ChoosingPower,
    MapExplorer,
    BallInMotion,
    HoleComplete
};

class Game
{
private:
    Arduboy2 _arduboy;
    Map _map;
    Camera _camera;
    Ball _ball;
    GameState _gameState;
    uint8_t _strokes;
    float _secondsDelta;

public:
    Game(Arduboy2 arduboy) : _arduboy(arduboy)
    {
        _map = GetMap1();
        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
        _gameState = GameState::MapSummary;
        _secondsDelta = 0;
    }

    void Reset()
    {
        _map = GetMap1();
        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
        _gameState = GameState::Aiming;
        _strokes = 0;
        _secondsDelta = 0;
    }

    void Tick(float secondsDelta)
    {
        Serial.println(_ball.power);

        _secondsDelta = secondsDelta;

        HandleInput();

        if (_gameState == GameState::ChoosingPower)
        {
            _ball.TickPower(secondsDelta);
        }

        if (_gameState == GameState::BallInMotion)
        {
            _ball.Move(secondsDelta);

            if (_ball.IsStopped())
            {
                _gameState = GameState::Aiming;
                _ball.ResetPower();
            }

            CollisionHandler::HandleCollisions(_ball, _map);

            if (CollisionHandler::BallInHole(_ball, _map))
            {
                _ball.x = _map.end.x;
                _ball.y = _map.end.y;
                _ball.velocity = {0, 0};
                _gameState = GameState::HoleComplete;
            }
        }

        if (_gameState != GameState::MapExplorer)
            _camera.FocusOn(_ball.x, _ball.y);
    }

    void Display()
    {
        _camera.DrawMap(_map);
        _camera.DrawBall(_ball);

        if (_gameState == GameState::Aiming ||
            _gameState == GameState::ChoosingPower ||
            _gameState == GameState::MapExplorer)
            _camera.DrawAimHud(_ball);

        if (_gameState == GameState::MapSummary)
            _camera.DrawMapSummary(_map);


        if (_gameState == GameState::MapExplorer)
            _camera.DrawMapExplorerIndicator();

        if (_gameState == GameState::HoleComplete)
            _camera.DrawMapComplete(_map, _strokes);
    }

private:
    void HandleInput()
    {
        switch (_gameState)
        {
            case GameState::MapSummary:
                HandleInputMapSummary();
                break;
            case GameState::Aiming:
                HandleInputAiming();
                break;
            case GameState::ChoosingPower:
                HandleInputChoosingPower();
                break;
            case GameState::MapExplorer:
                HandleInputMapExplorer();
                break;
            case GameState::HoleComplete:
                HandleInputHoleComplete();
                break;
        }
    }

    void HandleInputMapSummary()
    {
        if (AnyButtonPressed(_arduboy))
            _gameState = GameState::Aiming;
    }

    void HandleInputAiming()
    {
        if (_arduboy.justPressed(B_BUTTON))
            _gameState = GameState::MapExplorer;
        if (_arduboy.justPressed(A_BUTTON))
            _gameState = GameState::ChoosingPower;
        if (_arduboy.pressed(LEFT_BUTTON))
            _ball.RotateDirection(0.02);
        if (_arduboy.pressed(RIGHT_BUTTON))
            _ball.RotateDirection(-0.02);
    }

    void HandleInputChoosingPower()
    {
        if (_arduboy.justPressed(B_BUTTON))
        {
            _gameState = GameState::Aiming;
            return;
        }
        if (_arduboy.justPressed(A_BUTTON))
        {
            _gameState = GameState::BallInMotion;
            _ball.StartHit();
            _strokes++;
            return;
        }
    }

    void HandleInputMapExplorer()
    {
        if (_arduboy.pressed(UP_BUTTON))
            _camera.MoveUp();
        if (_arduboy.pressed(DOWN_BUTTON))
            _camera.MoveDown();
        if (_arduboy.pressed(LEFT_BUTTON))
            _camera.MoveLeft();
        if (_arduboy.pressed(RIGHT_BUTTON))
            _camera.MoveRight();
        if (_arduboy.justPressed(B_BUTTON))
        {
            _gameState = GameState::Aiming;
            _camera.FocusOn(_ball.x, _ball.y);
        }
    }

    void HandleInputHoleComplete()
    {
        if (AnyButtonPressed(_arduboy))
            Reset();
    }

private:
    static bool AnyButtonPressed(Arduboy2 arduboy)
    {
        return (arduboy.justPressed(UP_BUTTON) ||
                arduboy.justPressed(DOWN_BUTTON) ||
                arduboy.justPressed(LEFT_BUTTON) ||
                arduboy.justPressed(RIGHT_BUTTON) ||
                arduboy.justPressed(A_BUTTON) ||
                arduboy.justPressed(B_BUTTON));
    }
};