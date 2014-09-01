#include "map.h"
#include "screen.h"
#include <sstream>

Map CreateMap(int seed)
{
    return GenerateRandomMap(std::mt19937(seed));
}

int GameMain()
{
    SetTitle("Map test");

    int seed = 1;
    Map map = CreateMap(seed);

    Screen screen;

    while(true)
    {
        screen.Clear();

        std::ostringstream ss;
        ss << "Seed = " << seed;
        screen.PutString({0,0}, Color::White, ss.str());

        for(int y=0; y<MAP_HEIGHT; ++y)
        {
            for(int x=0; x<MAP_WIDTH; ++x)
            {
                screen.PutGlyph({x+MAP_OFFSET_X,y+MAP_OFFSET_Y}, map.GetTile({x,y}).GetGlyph());
            }
        }
        WriteOutput(screen.glyphs, {7,0});

        switch(ReadInput())
        {
        case 'Q': return 0;
        case '4': map = CreateMap(--seed); break;
        case '6': map = CreateMap(++seed); break;
        case '2': map = CreateMap(seed -= 10); break;
        case '8': map = CreateMap(seed += 10); break;
        }
    }
}