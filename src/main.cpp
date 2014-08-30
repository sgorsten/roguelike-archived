#include "platform.h"

struct Screen
{
    Glyph glyphs[CONSOLE_WIDTH * CONSOLE_HEIGHT];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutChar(const int2 & coord, int character, Color attribute)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= CONSOLE_WIDTH || coord.y >= CONSOLE_HEIGHT) return;
        glyphs[coord.y * CONSOLE_WIDTH + coord.x] = {attribute, character};
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
            screen.PutChar({x, 2}, '#', Color::DkGray);
            screen.PutChar({x, 22}, '#', Color::DkGray);
        }
        for(int y=2; y<23; ++y)
        {
            screen.PutChar({2, y}, '#', Color::DkGray);
            screen.PutChar({77, y}, '#', Color::DkGray);
        }
        for(int i=1; i<16; ++i)
        {
            screen.PutChar({2+i, 3}, '%', (Color)i);
        }
        screen.PutChar(playerPos, '@', Color::Yellow);
        WriteOutput(screen.glyphs, playerPos);

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