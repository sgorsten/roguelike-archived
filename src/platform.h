#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <cstring>

struct Screen
{
    struct Color { uint8_t red,green,blue; };
    struct Cell { int character, attribute; };
    
    static const int NUM_COLORS = 16;
    static const int WIDTH = 80, HEIGHT = 25;
    
    const char * caption;
    Color colors[NUM_COLORS];
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
    virtual void Show(const Screen & screen) = 0;
};

extern int GameMain(Platform & platform);

#endif