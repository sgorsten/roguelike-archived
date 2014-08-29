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
    Platform::Cell cells[Platform::WIDTH * Platform::HEIGHT];

    Screen() { Clear(); }
    void Clear() { memset(cells, 0, sizeof(cells)); }
    void PutChar(const int2 & coord, int character, int attribute) { PutChar(coord.x, coord.y, character, attribute); }
    void PutChar(int x, int y, int character, int attribute)
    {
        if(x < 0 || y < 0 || x >= Platform::WIDTH || y >= Platform::HEIGHT) return;
        cells[y * Platform::WIDTH + x] = {character, attribute};
    }
};

int GameMain(Platform & platform)
{
    platform.SetCaption("Roguelike Experiments");

    const Platform::Color colors[] = {
        {0x00,0x00,0x00}, {0x20,0x20,0xc0}, {0x20,0xc0,0x20}, {0x20,0xc0,0xc0}, {0xc0,0x20,0x20}, {0xc0,0x20,0xc0}, {0xc0,0xc0,0x20}, {0xc0,0xc0,0xc0},
        {0x40,0x40,0x40}, {0x40,0x40,0xff}, {0x40,0xff,0x40}, {0x40,0xff,0xff}, {0xff,0x40,0x40}, {0xff,0x40,0xff}, {0xff,0xff,0x40}, {0xff,0xff,0xff}
    };
    platform.SetPalette(colors);

    const int2 directions[] = {{-1,1}, {0,1}, {1,1}, {-1,0}, {0,0}, {1,0}, {-1,-1}, {0,-1}, {1,-1}};
    int2 playerPos = {5,5};
    while(true)
    {
        Screen screen;
        for(int x=2; x<78; ++x)
        {
            screen.PutChar(x, 2, '#', 8);
            screen.PutChar(x, 22, '#', 8);
        }
        for(int y=2; y<23; ++y)
        {
            screen.PutChar(2, y, '#', 8);
            screen.PutChar(77, y, '#', 8);
        }
        screen.PutChar(playerPos, '@', 14);
        platform.ShowScreen(screen.cells);

        platform.SetCursor(playerPos.x, playerPos.y);

        switch(int ch = platform.GetChar())
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
        playerPos = min(playerPos, {Platform::WIDTH-1,Platform::HEIGHT-1});
    }
}