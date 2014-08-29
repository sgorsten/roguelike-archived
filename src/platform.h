#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <string>

struct Platform
{
    struct Color { uint8_t red, green, blue; };
    struct Cell { int character, attribute; };

    static const int NUM_COLORS = 16;
    static const int WIDTH = 80, HEIGHT = 25;

    virtual void SetCaption(const std::string & title) = 0;
    virtual void SetPalette(const Color (&colors)[NUM_COLORS]) = 0;
    virtual void SetCursor(int x, int y) = 0;
    virtual void ShowScreen(const Cell (&cells)[WIDTH*HEIGHT]) = 0;
    virtual int GetChar() = 0;
};

extern int GameMain(Platform & platform);

#endif