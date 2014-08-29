#include "platform.h"

#include <thread>

int GameMain(Platform & platform)
{
    Color colors[] = {
        {0x00,0x00,0x00}, {0x20,0x20,0xc0}, {0x20,0xc0,0x20}, {0x20,0xc0,0xc0}, {0xc0,0x20,0x20}, {0xc0,0x20,0xc0}, {0xc0,0xc0,0x20}, {0xc0,0xc0,0xc0},
        {0x40,0x40,0x40}, {0x40,0x40,0xff}, {0x40,0xff,0x40}, {0x40,0xff,0xff}, {0xff,0x40,0x40}, {0xff,0x40,0xff}, {0xff,0xff,0x40}, {0xff,0xff,0xff}
    };
    platform.SetPalette(colors);

    int x=5, y=5;
    while(true)
    {
        Screen screen = {"Roguelike Experiments", {}, x, y};
        screen.PutChar(x, y, '@', 14);
        platform.ShowScreen(screen);

        switch(platform.GetChar())
        {
        case 'Q': return 0;
        case '7': --x; --y; break;
        case '8':      --y; break;
        case '9': ++x; --y; break;
        case '4': --x;      break;
        case '5':           break;
        case '6': ++x;      break;
        case '1': --x; ++y; break;
        case '2':      ++y; break;
        case '3': ++x; ++y; break;
        }
    }
}