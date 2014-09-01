#ifndef SCREEN_H
#define SCREEN_H

#include "roguelike.h"

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }

    void Clear() { Clear({{0,0},{SCREEN_WIDTH,SCREEN_HEIGHT}}); }

    void Clear(const Rect & rect)
    {
        for(int2 c=rect.a; c.y<rect.b.y; ++c.y)
        {
            for(c.x=rect.a.x; c.x<rect.b.x; ++c.x)
            {
                PutGlyph(c, {});
            }
        }
    }

    void PutGlyph(const int2 & coord, Glyph glyph)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = glyph;
    }

    void PutString(int2 coord, Color color, const std::string & text)
    {
        for(auto ch : text)
        {
            PutGlyph(coord, {color,ch});
            ++coord.x;
        }
    }
};

#endif