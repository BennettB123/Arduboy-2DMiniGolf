#pragma once

#include "Ball.h"
#include "Font4x6/Font4x6.h"
#include "Map.h"
#include "MapManager.h"
#include <Arduboy2.h>

class Camera
{
private:
    Arduboy2Base _arduboy;
    int16_t _cameraX;
    int16_t _cameraY;
    uint8_t _mapWidth;
    uint8_t _mapHeight;
    Font4x6 font4x6;
    bool _textFlashToggle = false;

    static constexpr uint8_t FontWidth = 4;
    static constexpr uint8_t FontHeight = 7;
    static constexpr uint8_t HalfScreenWidth = Arduboy2Base::width() / 2;
    static constexpr uint8_t HalfScreenHeight = Arduboy2Base::height() / 2;
    static constexpr uint8_t MaxPowerLineLength = 40;
    static constexpr uint8_t MinPowerLineLength = 10;
    static constexpr uint8_t MaxBoundaryPadding = 5;

public:
    Camera() = default;
    Camera(Arduboy2Base arduboy, uint8_t x, uint8_t y, uint8_t mapWidth, uint8_t mapHeight)
        : _arduboy(arduboy), _mapWidth(mapWidth), _mapHeight(mapHeight)
    {
        font4x6 = Font4x6();
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
            _arduboy.fillCircle(c.location.x - _cameraX,
                                c.location.y - _cameraY,
                                c.radius);
        }

        // draw texture (dots on ground)
        uint8_t dotSpacing = 16;
        for (int i = dotSpacing / 2; i < _mapWidth; i += dotSpacing)
        {
            for (int j = dotSpacing / 2; j < _mapHeight; j += dotSpacing)
            {
                _arduboy.drawPixel(i - _cameraX, j - _cameraY);
            }
        }

        _arduboy.drawCircle(map.end.x - _cameraX,
                            map.end.y - _cameraY,
                            Map::HoleRadius);
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
        font4x6.setCursor(0, 25);
        PrintCenteredWithBackground(map.name +
                                    String(F("\npar ")) + String(map.par));
    }

    void DrawMapExplorerIndicator()
    {
        if (_arduboy.everyXFrames(30))
            _textFlashToggle = !_textFlashToggle;

        if (_textFlashToggle)
        {
            font4x6.setCursor(0, Arduboy2::height() - FontHeight - 1);

            String str = String(F("View Map"));

            Rect bgRect = Rect(font4x6.getCursorX() - 1,
                               font4x6.getCursorY(),
                               GetTextPixelWidth(str) + 2,
                               FontHeight + 1);

            Rect borderRect = ExpandRect(bgRect, 1);

            DrawCheckeredBorder(borderRect);
            _arduboy.fillRect(bgRect.x, bgRect.y, bgRect.width, bgRect.height, BLACK);
            font4x6.print(str);
        }
    }

    void DrawMapComplete(const Map &map, uint8_t strokes)
    {
        font4x6.setCursorY(10);
        PrintCenteredWithBackground(String(F("Hole Complete!\n")) +
                                    String(F("par ")) + map.par +
                                    String(F("\ntook ")) + String(strokes) + String(F(" strokes\n")) +
                                    String(F("Press A to continue")));
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
        int16_t maxX = _mapWidth - Arduboy2::width() + MaxBoundaryPadding;
        int16_t maxY = _mapHeight - Arduboy2::height() + MaxBoundaryPadding;

        if (_cameraX > maxX)
            _cameraX = maxX;
        if (_cameraY > maxY)
            _cameraY = maxY;
        if (_cameraX < -MaxBoundaryPadding)
            _cameraX = -MaxBoundaryPadding;
        if (_cameraY < -MaxBoundaryPadding)
            _cameraY = -MaxBoundaryPadding;
    }

    // Prints the provided text centered on the screen
    // with a checkered background
    void PrintCenteredWithBackground(const String &text)
    {
        Rect backgroundRect = GetBoundingRectOfCenteredText(text);
        backgroundRect = ExpandRect(backgroundRect, 2);
        DrawCheckeredBorder(backgroundRect);

        int8_t lineLength = 0;
        String temp = String(text);

        do
        {
            lineLength = temp.indexOf('\n');
            if (lineLength < 0)
                lineLength = temp.length();

            auto line = temp.substring(0, lineLength);
            temp = temp.substring(lineLength + 1);

            uint8_t textWidth = GetTextPixelWidth(line);
            uint8_t offset = HalfScreenWidth - (textWidth / 2);

            font4x6.setCursorX(offset);
            PrintlnCenteredOverBlack(line);
        } while (temp.length() > 0);
    }

    // Prints the provided text centered on the screen with a black background
    void PrintlnCenteredOverBlack(const String &text)
    {
        Rect rect = GetBoundingRectOfCenteredText(text);
        _arduboy.fillRect(rect.x, rect.y, rect.width, rect.height, BLACK);
        font4x6.println(text);
    }

    // Draws a black rectangle with the provided Rect with a
    // checkered background 2 pixels wide
    void DrawCheckeredBorder(Rect rect)
    {
        uint8_t margin = 2;
        _arduboy.fillRect(rect.x, rect.y, rect.width, rect.height, BLACK);

        for (int16_t i = 0; i < rect.width; i++)
        {
            for (int16_t j = 0; j < rect.height; j++)
            {
                // only draw if we're on the border
                if (i >= margin && i < rect.width - margin &&
                    j >= margin && j < rect.height - margin)
                        continue;

                if (i % 2 == 1 ^ j % 2 == 0)
                    _arduboy.drawPixel(i + rect.x, j + rect.y, WHITE);
            }
        }
    }

    // Returns a Rect that represents the boundary of a block of text.
    // Margin of 2 pixels on left/right, and margin of 1 pixel on top/bottom.
    // Provided text can contain multiple lines.
    Rect GetBoundingRectOfCenteredText(const String &text)
    {
        int8_t lineLength = 0;
        uint8_t largestStr = 0;
        uint8_t numLines = 0;
        String temp = String(text);

        do
        {
            // get next line of text
            lineLength = temp.indexOf('\n');
            if (lineLength < 0)
                lineLength = temp.length();

            auto line = temp.substring(0, lineLength);
            temp = temp.substring(lineLength + 1);

            largestStr = max(largestStr, line.length());
            numLines++;
        } while (temp.length() > 0);

        uint8_t width = (largestStr * FontWidth);
        width += largestStr - 1; // include pixel between chars
        width += 4;              // account for 2 pixel margin on left/right

        uint8_t x = (HalfScreenWidth - (width / 2));
        uint8_t y = font4x6.getCursorY();
        uint8_t height = (numLines * FontHeight) + (numLines - 1);
        height += 2; // account for 1 pixel margin on top/bottom

        return Rect(x, y, width, height);
    }

    Rect ExpandRect(const Rect &rect, uint8_t i)
    {
        return Rect(rect.x - i, rect.y - i, rect.width + i * 2, rect.height + i * 2);
    }

    static uint8_t GetTextPixelWidth(const String &text)
    {
        uint8_t textWidth = (text.length() * FontWidth);
        textWidth += text.length() - 1; // include pixel between chars

        return textWidth;
    }

    void MoveTextCursorDown(uint8_t offset)
    {
        font4x6.setCursorY(font4x6.getCursorY() + offset);
    }
};