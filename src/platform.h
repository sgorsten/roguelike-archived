#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <algorithm>

enum
{
    CONSOLE_WIDTH = 80,
    CONSOLE_HEIGHT = 25 
};

struct int2 { int x,y; int2() : x(),y() {} int2(int x, int y) : x(x),y(y) {} };
enum class Color : uint8_t { Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, DkGray, LtBlue, LtGreen, LtCyan, LtRed, LtMagenta, Yellow, White };
struct Glyph { Color color; char character; };

inline bool operator == (const int2 & a, const int2 & b) { return a.x==b.x && a.y==b.y; }
inline bool operator != (const int2 & a, const int2 & b) { return a.x!=b.x || a.y!=b.y; }
inline int2 operator + (const int2 & a) { return {+a.x, +a.y}; }
inline int2 operator - (const int2 & a) { return {-a.x, -a.y}; }
inline int2 operator + (const int2 & a, const int2 & b) { return {a.x+b.x, a.y+b.y}; }
inline int2 operator - (const int2 & a, const int2 & b) { return {a.x-b.x, a.y-b.y}; }
inline int2 & operator += (int2 & a, const int2 & b) { return a=a+b; }
inline int2 & operator -= (int2 & a, const int2 & b) { return a=a-b; }
inline int2 max(const int2 & a, const int2 & b) { return {std::max(a.x,b.x), std::max(a.y,b.y)}; }
inline int2 min(const int2 & a, const int2 & b) { return {std::min(a.x,b.x), std::min(a.y,b.y)}; }

void SetTitle(const char * title);
void WriteOutput(const Glyph (&glyphs)[CONSOLE_WIDTH * CONSOLE_HEIGHT], const int2 & cursor);
int ReadInput();

extern int GameMain();

#endif