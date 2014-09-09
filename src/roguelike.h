#ifndef ROGUELIKE_H
#define ROGUELIKE_H

#include "common.h"

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const int MAP_WIDTH = 60;
const int MAP_HEIGHT = 22;
const int MAP_OFFSET_X = 0;
const int MAP_OFFSET_Y = 3;

struct Color
{
    float r,g,b;

    static Color FromHSV(float hueInDegrees, float saturation, float value)
    {
        float phase = hueInDegrees/60;
        float chroma = saturation * value;
        float x = chroma * (1 - std::abs(std::fmod(phase, 2.0f) - 1));
        switch((int)phase % 6)
        {
        case 0: return {chroma,x,0};
        case 1: return {x,chroma,0};
        case 2: return {0,chroma,x};
        case 3: return {0,x,chroma};
        case 4: return {x,0,chroma};
        case 5: return {chroma,0,x};
        }
    }

    static const Color Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, DkGray, LtBlue, LtGreen, LtCyan, LtRed, LtMagenta, Yellow, White;
};

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

    int2    Abs() const                         { return {std::abs(x), std::abs(y)}; }
    int     Dist2(const int2 & b) const         { return (b - *this).Mag2(); }
    int     Dot(const int2 & r) const           { return x*r.x + y*r.y; }
    int     Mag2() const                        { return Dot(*this); }
    int2    Max(const int2 & r) const           { return {std::max(x,r.x), std::max(y,r.y)}; }
    int2    Min(const int2 & r) const           { return {std::min(x,r.x), std::min(y,r.y)}; }

    bool    operator == (const int2 & r) const  { return x==r.x && y==r.y; }
    bool    operator != (const int2 & r) const  { return x!=r.x || y!=r.y; }
    int2    operator + () const                 { return {+x, +y}; }
    int2    operator - () const                 { return {-x, -y}; }
    int2    operator + (const int2 & r) const   { return {x+r.x, y+r.y}; }
    int2    operator - (const int2 & r) const   { return {x-r.x, y-r.y}; }
    int2    operator * (int r) const            { return {x*r, y*r}; }
    int2    operator / (int r) const            { return {x/r, y/r}; }
    int2 &  operator += (const int2 & r)        { return *this = *this+r; }
    int2 &  operator -= (const int2 & r)        { return *this = *this-r; }
    int2 &  operator *= (int r)                 { return *this = *this*r; }
    int2 &  operator /= (int r)                 { return *this = *this/r; }
};

struct Rect
{
    int2    a,b;

    int2    GetSize() const                     { return b-a; }
    int     GetArea() const                     { auto s = GetSize(); return s.x*s.y; }
    bool    IsEmpty() const                     { return GetArea() == 0; }
    bool    Intersects(const Rect & r) const    { return a.x < r.b.x && a.y < r.b.y && r.a.x < b.x && r.a.y < b.y; }
};

void SetTitle(const char * title);
void WriteOutput(const Glyph (&glyphs)[SCREEN_HEIGHT][SCREEN_WIDTH], const int2 & cursor);
int ReadInput();

std::vector<int2> EvaluateBresenhamLine(int2 a, bool includeA, int2 b, bool includeB);

extern int GameMain();

#endif
