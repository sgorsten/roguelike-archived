#include "map.h"

int GameMain()
{
    SetTitle("Map test");

    Map map;
    map.Fill({{0,0}, {MAP_WIDTH,MAP_HEIGHT}}, 2);
    map.Fill({{3,3}, {MAP_WIDTH-3,MAP_HEIGHT-3}}, 1);
    map.Fill({{18,3}, {19,10}}, 2);

    while(true)
    {
        Glyph screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {};
        for(int y=0; y<MAP_HEIGHT; ++y)
        {
            for(int x=0; x<MAP_WIDTH; ++x)
            {
                screen[y+MAP_OFFSET_Y][x+MAP_OFFSET_X] = map.GetTile({x,y}).GetGlyph();
            }
        }
        WriteOutput(screen, {0,0});

        switch(ReadInput())
        {
        case 'Q': return 0;
        }
    }
}