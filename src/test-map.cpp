#include "map.h"
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

    while(true)
    {
        std::ostringstream ss;
        ss << "Seed = " << seed;
        auto s = ss.str();

        Glyph screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {};
        for(int i=0; i<s.size(); ++i)
        {
            screen[0][i] = {Color::White, s[i]};
        }
        for(int y=0; y<MAP_HEIGHT; ++y)
        {
            for(int x=0; x<MAP_WIDTH; ++x)
            {
                screen[y+MAP_OFFSET_Y][x+MAP_OFFSET_X] = map.GetTile({x,y}).GetGlyph();
            }
        }
        WriteOutput(screen, {7,0});

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