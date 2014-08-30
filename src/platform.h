#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>

enum
{
    CONSOLE_WIDTH = 80,
    CONSOLE_HEIGHT = 25 
};

enum class Color : uint8_t { Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, DkGray, LtBlue, LtGreen, LtCyan, LtRed, LtMagenta, Yellow, White };

struct Glyph { Color color; char character; };

void SetTitle(const char * title);
void WriteOutput(const Glyph (&glyphs)[CONSOLE_WIDTH * CONSOLE_HEIGHT], int cursorX, int cursorY);
int ReadInput();

extern int GameMain();

#endif