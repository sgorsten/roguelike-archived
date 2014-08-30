#include "platform.h"

#include <algorithm>

struct int2 
{ 
    int x,y; 
    int2() : x(),y() {}
    int2(int x, int y) : x(x),y(y) {}
};
bool operator == (const int2 & a, const int2 & b) { return a.x==b.x && a.y==b.y; }
bool operator != (const int2 & a, const int2 & b) { return a.x!=b.x || a.y!=b.y; }
int2 operator + (const int2 & a) { return {+a.x, +a.y}; }
int2 operator - (const int2 & a) { return {-a.x, -a.y}; }
int2 operator + (const int2 & a, const int2 & b) { return {a.x+b.x, a.y+b.y}; }
int2 operator - (const int2 & a, const int2 & b) { return {a.x-b.x, a.y-b.y}; }
int2 & operator += (int2 & a, const int2 & b) { return a=a+b; }
int2 & operator -= (int2 & a, const int2 & b) { return a=a-b; }
int2 max(const int2 & a, const int2 & b) { return {std::max(a.x,b.x), std::max(a.y,b.y)}; }
int2 min(const int2 & a, const int2 & b) { return {std::min(a.x,b.x), std::min(a.y,b.y)}; }

struct Screen
{
    Glyph glyphs[CONSOLE_WIDTH * CONSOLE_HEIGHT];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutChar(const int2 & coord, int character, Color attribute) { PutChar(coord.x, coord.y, character, attribute); }
    void PutChar(int x, int y, int character, Color attribute)
    {
        if(x < 0 || y < 0 || x >= CONSOLE_WIDTH || y >= CONSOLE_HEIGHT) return;
        glyphs[y * CONSOLE_WIDTH + x] = {attribute, character};
    }
};

int GameMain()
{
    SetTitle("Roguelike Experiments");

    const int2 directions[] = {{-1,1}, {0,1}, {1,1}, {-1,0}, {0,0}, {1,0}, {-1,-1}, {0,-1}, {1,-1}};
    int2 playerPos = {5,5};
    while(true)
    {
        Screen screen;
        for(int x=2; x<78; ++x)
        {
            screen.PutChar(x, 2, '#', Color::DkGray);
            screen.PutChar(x, 22, '#', Color::DkGray);
        }
        for(int y=2; y<23; ++y)
        {
            screen.PutChar(2, y, '#', Color::DkGray);
            screen.PutChar(77, y, '#', Color::DkGray);
        }
        for(int i=1; i<16; ++i)
        {
            screen.PutChar(2+i, 3, '%', (Color)i);
        }
        screen.PutChar(playerPos, '@', Color::Yellow);
        WriteOutput(screen.glyphs, playerPos.x, playerPos.y);

        switch(int ch = ReadInput())
        {
        case 'Q': 
            return 0;
        case '1': case '2': case '3':
        case '4': case '5': case '6':
        case '7': case '8': case '9':
            playerPos += directions[ch-'1'];
            break;
        }
        playerPos = max(playerPos, {0,0});
        playerPos = min(playerPos, {CONSOLE_WIDTH-1,CONSOLE_HEIGHT-1});
    }
}