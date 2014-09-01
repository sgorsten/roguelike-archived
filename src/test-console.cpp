#include "roguelike.h"

#include <sstream>

int GameMain()
{
    SetTitle("Console test");

    Glyph screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {};
    for(int y=0; y<16; ++y)
    {
        for(int x=0; x<16; ++x)
        {
            screen[y+2][x+2] = {Color::Gray, y*16+x};
        }

        screen[y+2][20] = {(Color)y, 'X'};
    }

    int2 cursor;
    while(true)
    {
        int code = cursor.y*16+cursor.x;
        std::ostringstream ss;
        ss << '\'' << (char)(code) << "' = 0x" << std::hex << code << "  ";
        auto s = ss.str();
        for(int i=0; i<s.size(); ++i)
            screen[2][20+i] = {Color::Gray, s[i]};
        for(int i=1; i<16; ++i)
            screen[2+i][20] = screen[2+i][21] = screen[2+i][22] = {(Color)i, code};
        WriteOutput(screen, cursor+int2(2,2));

        switch(ReadInput())
        {
        case 'Q': return 0;
        case '1': cursor += Direction::SouthWest; break;
        case '2': cursor += Direction::South; break;
        case '3': cursor += Direction::SouthEast; break;
        case '4': cursor += Direction::West; break;
        case '6': cursor += Direction::East; break;
        case '7': cursor += Direction::NorthWest; break;
        case '8': cursor += Direction::North; break;
        case '9': cursor += Direction::NorthEast; break;
        }
        cursor.x = (cursor.x + 16) % 16;
        cursor.y = (cursor.y + 16) % 16;
    }
}