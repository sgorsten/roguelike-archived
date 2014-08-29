#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <cstring>

struct Color
{ 
    static const int NUM_COLORS = 16;
    uint8_t red, green, blue; 
};

struct Screen
{
    struct Cell { int character, attribute; };
    
    static const int WIDTH = 80, HEIGHT = 25;
    
    const char * caption;
    Cell cells[WIDTH * HEIGHT];
    int cursorX,cursorY;

    void PutChar(int x, int y, int character, int attribute)
    {
        cells[y * WIDTH + x] = {character,attribute};
    }

    void SetCursor(int x, int y)
    {
        cursorX = x;
        cursorY = y;
    }
};

struct Platform
{
    virtual void SetPalette(const Color (&colors)[Color::NUM_COLORS]) = 0;
    virtual void ShowScreen(const Screen & screen) = 0;
    virtual int GetChar() = 0;
};

extern int GameMain(Platform & platform);

#endif