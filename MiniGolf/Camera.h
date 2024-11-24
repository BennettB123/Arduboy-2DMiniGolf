#pragma once

#include "Ball.h"
#include "Map.h"
#include <Arduboy2.h>

class Camera
{
private:
    Arduboy2 _arduboy;
    int16_t _cameraX;
    int16_t _cameraY;
    int16_t _mapWidth;
    int16_t _mapHeight;
    bool _textFlashToggle = false;

    static constexpr uint8_t FontWidth = 5;
    static constexpr uint8_t HalfScreenWidth = Arduboy2::width() / 2;
    static constexpr uint8_t HalfScreenHeight = Arduboy2::height() / 2;
    static constexpr uint8_t MaxPowerLineLength = 40;
    static constexpr uint8_t MinPowerLineLength = 10;

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
                              p2.y - _cameraY);
        }

        // draw texture (dots on ground)
        for (int i = 0; i < _mapWidth; i += 15)
        {
            for (int j = 0; j < _mapHeight; j += 15)
            {
                _arduboy.drawPixel(i - _cameraX, j - _cameraY);
            }
        }

        DrawHole(map.end);
    }

    void DrawBall(const Ball &ball)
    {
        _arduboy.fillCircle(static_cast<int16_t>(ball.X) - _cameraX,
                            static_cast<int16_t>(ball.Y) - _cameraY,
                            Ball::Radius);
    }

    void DrawAimHud(const Ball &ball)
    {
        float lineLength = map(ball.Power, Ball::MinPower, Ball::MaxPower, MinPowerLineLength, MaxPowerLineLength);

        float x = ball.X + cos(ball.Direction) * lineLength;
        float y = ball.Y + -(sin(ball.Direction) * lineLength);

        _arduboy.drawLine(ball.X - _cameraX,
                          ball.Y - _cameraY,
                          x - _cameraX,
                          y - _cameraY);
    }

    void DrawMapSummary(const Map &map)
    {
        _arduboy.setCursorY(25);
        PrintlnCentered(map.name);
        PrintlnCentered("par: " + String(map.par));
    }

    void DrawMapExplorerIndicator()
    {
        if (_arduboy.everyXFrames(30))
            _textFlashToggle = !_textFlashToggle;

        if (_textFlashToggle)
        {
            _arduboy.setCursor(1, Arduboy2::height() - 8);
            _arduboy.print(F("View Map"));
        }
    }

    void DrawMapComplete(const Map &map, uint8_t strokes)
    {
        _arduboy.setCursorY(4);
        PrintlnCentered(F("Hole"));
        PrintlnCentered(F("Complete!"));

        MoveCursorDown(5);
        PrintlnCentered("par " + String(map.par));
        PrintlnCentered("strokes: " + String(strokes));

        MoveCursorDown(5);
        PrintlnCentered(F("Press any button"));
        PrintlnCentered(F("to restart"));
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
        _arduboy.drawCircle(hole.x - _cameraX,
                            hole.y - _cameraY,
                            Map::HoleRadius);
    }

    void KeepInBounds()
    {
        _cameraX = constrain(_cameraX, 0, _mapWidth - Arduboy2::width() + 1);
        _cameraY = constrain(_cameraY, 0, _mapHeight - Arduboy2::height() + 1);
    }

    void PrintlnCentered(const String &text)
    {
        uint8_t textWidth = (text.length() * FontWidth);
        textWidth += text.length() - 1; // include pixel between chars
        uint8_t offset = HalfScreenWidth - (textWidth / 2);
        _arduboy.setCursorX(offset);
        _arduboy.println(text);
    }

    void MoveCursorDown(uint8_t offset)
    {
        _arduboy.setCursorY(_arduboy.getCursorY() + offset);
    }
};