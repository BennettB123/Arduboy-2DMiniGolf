#pragma once

#include "Ball.h"
#include "Camera.h"
#include "CollisionHandler.h"
#include "Map.h"
#include "MapManager.h"
#include <Arduboy2.h>

enum class GameState
{
    MapSummary,
    Aiming,
    ChoosingPower,
    MapExplorer,
    BallInMotion,
    MapComplete
};

class Game
{
private:
    Arduboy2 _arduboy;
    uint8_t _mapIndex;
    Map _map;
    Camera _camera;
    Ball _ball;
    GameState _gameState;
    uint8_t _strokes;
    float _secondsDelta;

public:
    Game(Arduboy2 arduboy) : _arduboy(arduboy)
    {
    }

    void Init()
    {
        _mapIndex = 0;
        _map = MapManager::LoadMap(_mapIndex);
        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
        _gameState = GameState::MapSummary;
        _strokes = 0;
        _secondsDelta = 0;
    }

    void Tick(float secondsDelta)
    {
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

            CollisionHandler::HandleAllCollisions(_ball, _map);

            if (CollisionHandler::BallInHole(_ball, _map))
            {
                _ball.X = _map.end.x;
                _ball.Y = _map.end.y;
                _ball.Velocity = {0, 0};
                _gameState = GameState::MapComplete;
            }
        }

        if (_gameState != GameState::MapExplorer)
            _camera.FocusOn(_ball.X, _ball.Y);
    }

    void Display()
    {
        switch (_gameState)
        {
            case GameState::MapSummary:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                _camera.DrawMapSummary(_map);
                break;
            case GameState::Aiming:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                _camera.DrawAimHud(_ball);
                break;
            case GameState::ChoosingPower:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                _camera.DrawAimHud(_ball);
                break;
            case GameState::MapExplorer:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                _camera.DrawAimHud(_ball);
                _camera.DrawMapExplorerIndicator();
                break;
            case GameState::BallInMotion:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                break;
            case GameState::MapComplete:
                _camera.DrawMap(_map);
                _camera.DrawBall(_ball);
                _camera.DrawMapComplete(_map, _strokes);
                break;
        }
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
            case GameState::MapComplete:
                HandleInputMapComplete();
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
            _ball.RotateDirectionCounterClockwise(_secondsDelta);
        if (_arduboy.pressed(RIGHT_BUTTON))
            _ball.RotateDirectionClockwise(_secondsDelta);
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
            _camera.FocusOn(_ball.X, _ball.Y);
        }
    }

    void HandleInputMapComplete()
    {
        if (AnyButtonPressed(_arduboy))
            LoadNextMap();
    }

    void LoadNextMap()
    {
        _mapIndex += 1;
        _map = MapManager::LoadMap(_mapIndex);

        _camera = Camera(_arduboy, 0, 0, _map.width, _map.height);
        _ball = Ball(static_cast<float>(_map.start.x), static_cast<float>(_map.start.y));
        _gameState = GameState::MapSummary;
        _strokes = 0;
        _secondsDelta = 0;
    }

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