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
    uint8_t _mapWidth;
    uint8_t _mapHeight;
    bool _textFlashToggle = false;

    static constexpr uint8_t FontWidth = 5;
    static constexpr uint8_t HalfScreenWidth = Arduboy2::width() / 2;
    static constexpr uint8_t HalfScreenHeight = Arduboy2::height() / 2;
    static constexpr uint8_t MaxPowerLineLength = 40;
    static constexpr uint8_t MinPowerLineLength = 10;
    static constexpr uint8_t MaxBoundaryPadding = 5;

public:
    Camera() = default;
    Camera(Arduboy2 arduboy, uint8_t x, uint8_t y, uint8_t mapWidth, uint8_t mapHeight)
        : _arduboy(arduboy), _mapWidth(mapWidth), _mapHeight(mapHeight)
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
        for (auto wall : map.walls)
        {
            _arduboy.drawLine(wall.p1.x - _cameraX,
                              wall.p1.y - _cameraY,
                              wall.p2.x - _cameraX,
                              wall.p2.y - _cameraY);
        }

        // draw circles
        for (auto c : map.circles)
        {
            _arduboy.drawCircle(c.location.x - _cameraX,
                                c.location.y - _cameraY,
                                c.radius);
        }

        // draw texture (dots on ground)
        uint8_t dotSpacing = 16;
        for (int i = dotSpacing/2; i < _mapWidth; i += dotSpacing)
        {
            for (int j = dotSpacing/2; j < _mapHeight; j += dotSpacing)
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

        MoveTextCursorDown(5);
        PrintlnCentered("par " + String(map.par));
        PrintlnCentered("strokes: " + String(strokes));

        MoveTextCursorDown(5);
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
    void DrawHole(const Point8 &hole)
    {
        _arduboy.drawCircle(hole.x - _cameraX,
                            hole.y - _cameraY,
                            Map::HoleRadius);
    }

    void KeepInBounds()
    {
        int16_t maxX = _mapWidth - Arduboy2::width() + MaxBoundaryPadding;
        int16_t maxY = _mapHeight - Arduboy2::height() + MaxBoundaryPadding;

        if (_cameraX > maxX) _cameraX = maxX;
        if (_cameraY > maxY) _cameraY = maxY;
        if (_cameraX < -MaxBoundaryPadding) _cameraX = -MaxBoundaryPadding;
        if (_cameraY < -MaxBoundaryPadding) _cameraY = -MaxBoundaryPadding;
    }

    void PrintlnCentered(const String &text)
    {
        uint8_t textWidth = (text.length() * FontWidth);
        textWidth += text.length() - 1; // include pixel between chars
        uint8_t offset = HalfScreenWidth - (textWidth / 2);
        _arduboy.setCursorX(offset);
        _arduboy.println(text);
    }

    void MoveTextCursorDown(uint8_t offset)
    {
        _arduboy.setCursorY(_arduboy.getCursorY() + offset);
    }
};