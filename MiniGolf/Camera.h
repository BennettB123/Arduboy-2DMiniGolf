#pragma once

#include "ArduboyG.h"
#include "Ball.h"
#include "Map.h"

class Camera
{
private:
    ArduboyG _arduboy;
    int16_t _cameraX;
    int16_t _cameraY;
    int16_t _mapWidth;
    int16_t _mapHeight;
    bool _mapExplorerIndicatorToggle = false;

    static constexpr uint8_t HalfScreenWidth = ArduboyG::width() / 2;
    static constexpr uint8_t HalfScreenHeight = ArduboyG::height() / 2;
    static constexpr uint8_t MaxPowerLineLength = 40;
    static constexpr uint8_t MinPowerLineLength = 10;

public:
    Camera() = default;
    Camera(ArduboyG arduboy, int16_t x, int16_t y, int16_t maxX, int16_t maxY)
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

    void DrawMap(const Map &map)
    {
        // draw walls
        for (uint8_t i = 0; i < MaxNumWalls; i++)
        {
            Point p1 = map.walls[i].p1;
            Point p2 = map.walls[i].p2;
            _arduboy.drawLine(p1.x - _cameraX,
                              p1.y - _cameraY,
                              p2.x - _cameraX,
                              p2.y - _cameraY,
                              LIGHT_GRAY);
        }

        // draw texture (dots on ground)
        for (int i = 0; i < _mapWidth; i += 15)
        {
            for (int j = 0; j < _mapHeight; j += 15)
            {
                _arduboy.drawPixel(i - _cameraX, j - _cameraY, LIGHT_GRAY);
            }
        }

        DrawHole(map.end);
    }

    void DrawMapExplorerIndicator()
    {
        // if (_arduboy.everyXFrames(30)) {
        //     _mapExplorerIndicatorToggle = !_mapExplorerIndicatorToggle;
        //     Serial.println("toggle");
        // }

        _mapExplorerIndicatorToggle = true;

        if (_mapExplorerIndicatorToggle)
        {
            _arduboy.setTextColor(DARK_GRAY);
            _arduboy.setCursor(1, ArduboyG::height() - 8);
            _arduboy.print("View Map");
        }
    }

    void DrawBall(const Ball &ball)
    {
        _arduboy.fillCircle(static_cast<int16_t>(ball.x) - _cameraX,
                            static_cast<int16_t>(ball.y) - _cameraY,
                            Ball::Radius);
    }

    void DrawAimHud(const Ball &ball)
    {
        float lineLength = map(ball.power, Ball::MinPower, Ball::MaxPower, MinPowerLineLength, MaxPowerLineLength);

        float x = ball.x + cos(ball.direction) * lineLength;
        float y = ball.y + -(sin(ball.direction) * lineLength);

        _arduboy.drawLine(ball.x - _cameraX,
                          ball.y - _cameraY,
                          x - _cameraX,
                          y - _cameraY,
                          WHITE);
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
    void DrawHole(const Point &hole)
    {
        _arduboy.fillCircle(hole.x - _cameraX,
                            hole.y - _cameraY,
                            Map::HoleRadius,
                            LIGHT_GRAY);
    }

    void KeepInBounds()
    {
        _cameraX = constrain(_cameraX, 0, _mapWidth - ArduboyG::width() + 1);
        _cameraY = constrain(_cameraY, 0, _mapHeight - ArduboyG::height() + 1);
    }
};