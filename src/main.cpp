#include "platform.h"

#include <algorithm>

int GameMain(Platform & platform)
{
    platform.SetCaption("Roguelike Experiments");

    Platform::Color colors[] = {
        {0x00,0x00,0x00}, {0x20,0x20,0xc0}, {0x20,0xc0,0x20}, {0x20,0xc0,0xc0}, {0xc0,0x20,0x20}, {0xc0,0x20,0xc0}, {0xc0,0xc0,0x20}, {0xc0,0xc0,0xc0},
        {0x40,0x40,0x40}, {0x40,0x40,0xff}, {0x40,0xff,0x40}, {0x40,0xff,0xff}, {0xff,0x40,0x40}, {0xff,0x40,0xff}, {0xff,0xff,0x40}, {0xff,0xff,0xff}
    };
    platform.SetPalette(colors);

    int x=5, y=5;
    while(true)
    {
        Platform::Cell cells[Platform::WIDTH * Platform::HEIGHT] = {};
        cells[y*Platform::WIDTH+x] = {'@', 14};
        platform.ShowScreen(cells);

        platform.SetCursor(x, y);

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
        x = std::max(x, 0);
        y = std::max(y, 0);
        x = std::min(x, Platform::WIDTH-1);
        y = std::min(y, Platform::HEIGHT-1);
    }
}