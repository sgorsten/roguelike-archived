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

    enum InputMode { Seed, Look } mode = Seed;
    int2 cursor = {MAP_WIDTH/2, MAP_HEIGHT/2};
    Screen screen;

    while(true)
    {
        screen.Clear();

        std::ostringstream ss;
        ss << "(S)eed = " << seed;
        screen.PutString({0,0}, Color::White, ss.str());
        screen.PutString({0,1}, Color::White, "(L)ook");

        for(int y=0; y<MAP_HEIGHT; ++y)
        {
            for(int x=0; x<MAP_WIDTH; ++x)
            {
                for(int dir=0; dir<9; ++dir)
                {
                    if(map.GetTile(int2(x,y) + (Direction)dir).IsWalkable())
                    {
                        auto glyph = map.GetTile({x,y}).GetGlyph();
                        if(mode == Look && !map.HasLineOfSight(cursor, {x,y})) glyph.color = glyph.color * 0.1f;
                        screen.PutGlyph({x+MAP_OFFSET_X,y+MAP_OFFSET_Y}, glyph);
                        break;
                    }
                }                
            }
        }
        WriteOutput(screen.glyphs, mode == Seed ? int2(9,0) : cursor+int2(MAP_OFFSET_X,MAP_OFFSET_Y));

        switch(mode)
        {
        case Seed:
            switch(ReadInput())
            {
            case 'Q': return 0;
            case '4': map = CreateMap(--seed); break;
            case '6': map = CreateMap(++seed); break;
            case '2': map = CreateMap(seed -= 10); break;
            case '8': map = CreateMap(seed += 10); break;
            case 'l': mode = Look; break;
            }
            break;
        case Look:
            switch(ReadInput())
            {
            case 'Q': return 0;
            case '4': cursor += Direction::West; break;
            case '6': cursor += Direction::East; break;
            case '2': cursor += Direction::South; break;
            case '8': cursor += Direction::North; break;
            case 's': mode = Seed; break;
            case 'o': if(map.GetTile(cursor) == TILE_CLOSED_DOOR) map[cursor] = TILE_OPEN_DOOR; break;
            case 'c': if(map.GetTile(cursor) == TILE_OPEN_DOOR) map[cursor] = TILE_CLOSED_DOOR; break;
            }
            break; 
        }           
    }
}