/*
 * This file was copied from https://github.com/filmote/Font4x6
 *  
 *  Modifications:
 *  - added setCursorX method
 *  - added setCursorY method
 *  - added getCursorX method
 *  - added getCursorY method
*/


#pragma once

#include <Arduino.h>
#include <Print.h>

class Font4x6 : public Print {
    
  public:

    Font4x6(uint8_t lineSpacing = 8);   

    virtual size_t write(uint8_t); // used by the Arduino Print class
    void printChar(const char c, const int8_t x, int8_t y);

    void setCursor(const int8_t x, const int8_t y);
    void setCursorX(const int8_t x);
    void setCursorY(const int8_t y);

    int8_t getCursorX();
    int8_t getCursorY();

    void setTextColor(const uint8_t color);
    void setHeight(const uint8_t color);
    

  private:

    int8_t _cursorX;    // Default is 0.
    int8_t _baseX;      // needed for linebreak.
    int8_t _cursorY;    // Default is 0.

    int8_t _textColor;  // BLACK == 0, everything else is WHITE. Default is WHITE.

    uint8_t _letterSpacing;  // letterSpacing controls the distance between letters. Default is 1.
    uint8_t _lineHeight;     // lineHeight controls the height between lines breakend by \n. Default is 8.

};
