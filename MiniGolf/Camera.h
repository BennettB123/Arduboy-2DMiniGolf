#pragma once

#include "Ball.h"
#include "Map.h"
#include <Arduboy2.h>

constexpr uint8_t HalfScreenWidth = Arduboy2::width() / 2;
constexpr uint8_t HalfScreenHeight = Arduboy2::height() / 2;

class Camera
{
private:
    Arduboy2 _arduboy;
    int16_t _cameraX;
    int16_t _cameraY;
    int16_t _mapWidth;
    int16_t _mapHeight;

    bool _mapExplorerIndicatorToggle = false;

public:
    Camera() = default;
    Camera(Arduboy2 arduboy, int16_t x, int16_t y, int16_t maxX, int16_t maxY)
        : _arduboy(arduboy), _mapWidth(maxX), _mapHeight(maxY)
    {
        FocusOn(x, y);
    }

    void FocusOn(int16_t x, int16_t y)
    {
        _cameraX = x - HalfScreenWidth;
        _cameraY = y - HalfScreenHeight;

        KeepInBounds();
    }

    void DrawMap(Map map)
    {
        // draw walls
        for (uint8_t i = 0; i < MaxNumWalls; i++)
        {
            Point p1 = map.walls[i].p1;
            Point p2 = map.walls[i].p2;
            _arduboy.drawLine(p1.x - _cameraX,
                              p1.y - _cameraY,
                              p2.x - _cameraX,
                              p2.y - _cameraY);
        }

        // draw texture
        for (int i = 0; i < _mapWidth; i += 15)
        {
            for (int j = 0; j < _mapHeight; j += 15)
            {
                _arduboy.drawPixel(i - _cameraX, j - _cameraY);
            }
        }
    }

    void DrawMapExplorerIndicator()
    {
        if (_arduboy.everyXFrames(30))
            _mapExplorerIndicatorToggle = !_mapExplorerIndicatorToggle;

        if (_mapExplorerIndicatorToggle)
        {
            _arduboy.setCursor(1, Arduboy2::height() - 8);
            _arduboy.print("View Map");
        }
    }

    void DrawBall(Ball ball)
    {
        _arduboy.fillCircle(static_cast<int16_t>(ball.x) - _cameraX,
                            static_cast<int16_t>(ball.y) - _cameraY,
                            2);
    }

    void DrawHole(Point hole)
    {
        _arduboy.fillCircle(hole.x - _cameraX,
                            hole.y - _cameraY,
                            3);
    }

    void DrawAimHud(Ball ball)
    {
        float x = ball.x + cos(ball.direction) * 25;
        float y = ball.y + -(sin(ball.direction) * 25);

        _arduboy.drawLine(ball.x - _cameraX,
                          ball.y - _cameraY,
                          x - _cameraX,
                          y - _cameraY);
    }

    void MoveUp()
    {
        _cameraY -= 2;
        KeepInBounds();
    }

    void MoveDown()
    {
        _cameraY += 2;
        KeepInBounds();
    }

    void MoveLeft()
    {
        _cameraX -= 2;
        KeepInBounds();
    }

    void MoveRight()
    {
        _cameraX += 2;
        KeepInBounds();
    }

private:
    void KeepInBounds()
    {
        _cameraX = constrain(_cameraX, 0, _mapWidth - Arduboy2::width() + 1);
        _cameraY = constrain(_cameraY, 0, _mapHeight - Arduboy2::height() + 1);
    }
};