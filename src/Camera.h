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
    uint8_t _treadmillFrame = 0;
    Font4x6 _font4x6;
    bool _textFlashToggle = false;

    static constexpr uint8_t FontWidth = 4;
    static constexpr uint8_t FontHeight = 7;
    static constexpr uint8_t HalfScreenWidth = WIDTH / 2;
    static constexpr uint8_t HalfScreenHeight = HEIGHT / 2;
    static constexpr uint8_t MaxPowerLineLength = 40;
    static constexpr uint8_t MinPowerLineLength = 10;
    static constexpr uint8_t MaxBoundaryPadding = 5;

public:
    Camera() = default;
    Camera(Arduboy2Base arduboy, uint8_t x, uint8_t y, uint8_t mapWidth, uint8_t mapHeight)
        : _arduboy(arduboy), _mapWidth(mapWidth), _mapHeight(mapHeight)
    {
        _font4x6 = Font4x6();
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
        // draw floor texture (dots on ground)
        uint8_t dotSpacing = 16;
        for (int i = dotSpacing / 2; i < _mapWidth; i += dotSpacing)
        {
            for (int j = dotSpacing / 2; j < _mapHeight; j += dotSpacing)
            {
                _arduboy.drawPixel(i - _cameraX, j - _cameraY);
            }
        }

        // draw circles
        for (auto circle : map.circles)
        {
            if (circle.IsEmpty())
                continue;

            if (circle.location.x + circle.radius - _cameraX < 0 || circle.location.x - circle.radius -  _cameraX > WIDTH ||
                circle.location.y + circle.radius - _cameraY < 0 || circle.location.y - circle.radius - _cameraY > HEIGHT)
                continue;

            _arduboy.fillCircle(circle.location.x - _cameraX,
                                circle.location.y - _cameraY,
                                circle.radius);
        }

        // draw sand traps
        // TODO: turn into 8x8 sprite (like treadmill)
        uint8_t gridWidth = 5;
        for (auto sandtrap : map.sandTraps)
        {
            if (sandtrap.IsEmpty())
                continue;

            DrawDottedBorder(Rect(sandtrap.x - _cameraX, sandtrap.y - _cameraY, sandtrap.width, sandtrap.height));

            for (uint8_t i = gridWidth; i < sandtrap.width - 1; i += gridWidth)
            {
                uint8_t offset = 0;

                // shift even rows down
                if (i % (gridWidth * 2) == 0)
                    offset = -1;
                // shift odd rows down
                else if (i % gridWidth == 0)
                    offset = 1;

                for (uint8_t j = gridWidth + offset; j < sandtrap.height - 1; j += gridWidth)
                {
                    _arduboy.drawPixel(i + sandtrap.x - _cameraX, (j + sandtrap.y) - _cameraY, WHITE);
                }
            }
        }

        // draw treadmills
        for (auto tread : map.treadmills)
        {
            if (tread.IsEmpty())
                continue;
            
            uint24_t sprite = 0;
            switch (tread.direction){
                case Direction::Up: 
                    sprite = TreadmillUp;
                    break;
                case Direction::Down: 
                    sprite = TreadmillDown;
                    break;
                case Direction::Left: 
                    sprite = TreadmillLeft;
                    break;
                case Direction::Right: 
                    sprite = TreadmillRight;
                    break;
            }

            for (uint8_t x = 0; x < tread.width; x += TreadmillUpWidth) {
                int16_t drawX = x + tread.x - _cameraX;
                if (drawX < -(int8_t)TreadmillUpWidth || drawX > WIDTH)
                    continue;

                for (uint8_t y = 0; y < tread.height; y += TreadmillUpHeight){
                    int16_t drawY = y + tread.y - _cameraY;
                    if (drawY < -(int8_t)TreadmillUpHeight || drawY > HEIGHT)
                        continue;

                    FX::drawBitmap(drawX,
                                   drawY,
                                   sprite, _treadmillFrame, dbmMasked); // only dmbMasked works (bug?) other modes draw wrong sprite
                }
            }
        }

        // draw walls
        // TODO: don't draw if off screen
        for (auto wall : map.walls)
        {
            if (wall.IsEmpty())
                continue;

            _arduboy.drawLine(wall.p1.x - _cameraX,
                              wall.p1.y - _cameraY,
                              wall.p2.x - _cameraX,
                              wall.p2.y - _cameraY);
        }

        // draw hole
        _arduboy.drawCircle(map.end.x - _cameraX,
                            map.end.y - _cameraY,
                            Map::HoleRadius);

        // cycle sprite frames
        if (_arduboy.everyXFrames(5))
            _treadmillFrame = ++_treadmillFrame % TreadmillUpFrames;
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
        _font4x6.setCursor(0, 25);
        PrintCenteredWithBackground(map.name +
                                    String(F("\npar ")) + String(map.par));
    }

    void DrawMapExplorerIndicator()
    {
        if (_arduboy.everyXFrames(30))
            _textFlashToggle = !_textFlashToggle;

        if (_textFlashToggle)
            DrawTextBottomLeft("View Map");
    }

    void DrawDoubleSpeedIndicator()
    {
        if (_arduboy.everyXFrames(30))
            _textFlashToggle = !_textFlashToggle;

        if (_textFlashToggle)
            DrawTextBottomLeft("2x");
    }

    void DrawMapComplete(const Map &map, uint8_t strokes)
    {
        _font4x6.setCursorY(10);
        PrintCenteredWithBackground(String(F("Hole Complete!\n")) +
                                    String(F("par ")) + map.par +
                                    String(F("\ntook ")) + String(strokes) + String(F(" strokes\n")) +
                                    String(F("Press A to continue")));
    }

    void DrawGameSummary(uint8_t (&strokes)[MapManager::NumMaps], uint8_t totalPar)
    {
        uint16_t totalStrokes = 0;
        for (uint8_t i = 0; i < MapManager::NumMaps; i++)
            totalStrokes += strokes[i];

        _font4x6.setCursorY(12);
        PrintCenteredWithBackground(String(F("All 18 Holes Completed!\n\n")) +
                                    String(F("Final Score\n")) +
                                    String(F("Total Par:     ")) + totalPar + String(F("\n")) +
                                    String(F("Total Strokes: ")) + totalStrokes);
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
        int16_t maxX = _mapWidth - WIDTH + MaxBoundaryPadding;
        int16_t maxY = _mapHeight - HEIGHT + MaxBoundaryPadding;

        if (_cameraX > maxX)
            _cameraX = maxX;
        if (_cameraY > maxY)
            _cameraY = maxY;
        if (_cameraX < -MaxBoundaryPadding)
            _cameraX = -MaxBoundaryPadding;
        if (_cameraY < -MaxBoundaryPadding)
            _cameraY = -MaxBoundaryPadding;
    }

    void DrawTextBottomLeft(const String &text)
    {
        _font4x6.setCursor(0, HEIGHT - FontHeight - 1);

        Rect bgRect = Rect(_font4x6.getCursorX() - 1,
                           _font4x6.getCursorY(),
                           GetTextPixelWidth(text) + 2,
                           FontHeight + 1);

        Rect borderRect = ExpandRect(bgRect, 1);

        DrawDottedBorder(borderRect);
        _arduboy.fillRect(bgRect.x, bgRect.y, bgRect.width, bgRect.height, BLACK);
        _font4x6.print(text);
    }

    // Prints the provided text centered on the screen
    // with a checkered background
    void PrintCenteredWithBackground(const String &text)
    {
        Rect backgroundRect = GetBoundingRectOfCenteredText(text);
        backgroundRect = ExpandRect(backgroundRect, 2);
        DrawDottedBorder(backgroundRect);

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

            _font4x6.setCursorX(offset);
            PrintlnCenteredOverBlack(line);
        } while (temp.length() > 0);
    }

    // Prints the provided text centered on the screen with a black background
    void PrintlnCenteredOverBlack(const String &text)
    {
        Rect rect = GetBoundingRectOfCenteredText(text);
        _arduboy.fillRect(rect.x, rect.y, rect.width, rect.height, BLACK);
        _font4x6.println(text);
    }

    // Draws a black rectangle on the provided Rect with a dotted border
    void DrawDottedBorder(const Rect &rect)
    {
        _arduboy.fillRect(rect.x, rect.y, rect.width, rect.height, BLACK);

        uint16_t x = 0;
        uint16_t y = 0;

        // Top border
        for (x = 0; x < rect.width; x++)
        {
            if ((x % 2 == 1) ^ true)
                _arduboy.drawPixel(x + rect.x, rect.y);
        }

        // Bottom border
        for (x = 0; x < rect.width; x++)
        {
            y = rect.height - 1;
            if ((x % 2 == 1) ^ (y % 2 == 0))
                _arduboy.drawPixel(x + rect.x, y + rect.y);
        }

        // Left border
        for (y = 0; y < rect.height; y++)
        {
            if ((y % 2 == 0) ^ false)
                _arduboy.drawPixel(rect.x, y + rect.y);
        }

        // Right border
        for (y = 0; y < rect.height; y++)
        {
            x = rect.width - 1;
            if (x % 2 == 1 ^ y % 2 == 0)
                _arduboy.drawPixel(x + rect.x, y + rect.y);
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
        uint8_t y = _font4x6.getCursorY();
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
        _font4x6.setCursorY(_font4x6.getCursorY() + offset);
    }
};