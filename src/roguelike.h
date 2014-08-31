#ifndef ROGUELIKE_H
#define ROGUELIKE_H

#include "common.h"

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const int MAP_WIDTH = 60;
const int MAP_HEIGHT = 22;
const int MAP_OFFSET_X = 0;
const int MAP_OFFSET_Y = 3;

enum class Color : uint8_t { Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, DkGray, LtBlue, LtGreen, LtCyan, LtRed, LtMagenta, Yellow, White };
enum class Direction : uint8_t { None, North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest };

struct Glyph
{
    Color   color; 
    char    character; 
};

struct int2 
{ 
    int     x,y;

            int2()                              : x(),y() {} 
            int2(int x, int y)                  : x(x),y(y) {}
            int2(Direction dir)                 { static const int2 directions[] = {{0,0}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}}; *this = directions[(int)dir]; }        

    bool    operator == (const int2 & r) const  { return x==r.x && y==r.y; }
    bool    operator != (const int2 & r) const  { return x!=r.x || y!=r.y; }
    int2    operator + () const                 { return {+x, +y}; }
    int2    operator - () const                 { return {-x, -y}; }
    int2    operator + (const int2 & r) const   { return {x+r.x, y+r.y}; }
    int2    operator - (const int2 & r) const   { return {x-r.x, y-r.y}; }
    int2 &  operator += (const int2 & r)        { return *this = *this+r; }
    int2 &  operator -= (const int2 & r)        { return *this = *this-r; }
};

struct Rect
{
    int2    a,b;

    int2    GetSize() const                     { return b-a; }
    int     GetArea() const                     { auto s = GetSize(); return s.x*s.y; }
    bool    IsEmpty() const                     { return GetArea() == 0; }
};

inline int2 max(const int2 & a, const int2 & b) { return {std::max(a.x,b.x), std::max(a.y,b.y)}; }
inline int2 min(const int2 & a, const int2 & b) { return {std::min(a.x,b.x), std::min(a.y,b.y)}; }
inline int mag2(const int2 & a) { return a.x*a.x + a.y*a.y; }
inline int dist2(const int2 & a, const int2 & b) { return mag2(b - a); }

void SetTitle(const char * title);
void WriteOutput(const Glyph (&glyphs)[SCREEN_HEIGHT][SCREEN_WIDTH], const int2 & cursor);
int ReadInput();

extern int GameMain();

#endif
