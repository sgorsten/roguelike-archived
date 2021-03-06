#include "roguelike.h"
#include "screen.h"

#include <sstream>

int GameMain()
{
    SetTitle("Console test");

    Color colors[] = {Color::Black,Color::Blue,Color::Green,Color::Cyan,Color::Red,Color::Magenta,Color::Brown,Color::Gray,Color::DkGray,Color::LtBlue,Color::LtGreen,Color::LtCyan,Color::LtRed,Color::LtMagenta,Color::Yellow,Color::White};

    Screen screen;
    for(int y=0; y<16; ++y)
    {
        for(int x=0; x<16; ++x)
        {
            screen.PutGlyph({x+2,y+2}, {Color::Gray, y*16+x});
        }
        screen.PutGlyph({20,y+2}, {colors[y], 'X'});
    }

    int2 cursor;
    while(true)
    {
        int code = cursor.y*16+cursor.x;
        std::ostringstream ss;
        ss << '\'' << (char)(code) << "' = 0x" << std::hex << code << "  ";
        screen.PutString({20,2}, Color::Gray, ss.str());
        for(int i=1; i<16; ++i)
        {
            float hue = (i-1)*360.0f/15;
            for(int j=0; j<15; ++j)
            {
                screen.PutGlyph({20+j,2+i}, {Color::FromHSV(hue, 1.0f, (float)(j+1)/15), (char)code});
            }
        }
            
        WriteOutput(screen.glyphs, cursor+int2(2,2));

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