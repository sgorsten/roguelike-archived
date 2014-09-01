#include "map.h"
#include <sstream>

Map CreateMap(int seed)
{
    std::mt19937 engine(seed);

    const std::uniform_int_distribution<int> roomWidth(3,5);
    const std::uniform_int_distribution<int> roomHeight(2,4);

    int2 places = int2(MAP_WIDTH-2, MAP_HEIGHT-2) / 2; // We can place rooms at even coordinates which do not touch either boundary
    std::vector<Rect> rooms;

    for(int i=0; i<1000 && rooms.size() < 8; ++i)
    {
        int2 size = {roomWidth(engine), roomHeight(engine)};
        int2 place = {
            std::uniform_int_distribution<int>(0, places.x - size.x)(engine),
            std::uniform_int_distribution<int>(0, places.y - size.y)(engine)
        };
        Rect room = {place, place+size};
        bool good = true;
        for(auto & other : rooms)
        {
            if(room.Intersects(other))
            {
                good = false;
            }
        }
        if(good) rooms.push_back(room);
    }

    Map map;
    map.Fill({{0,0}, {MAP_WIDTH,MAP_HEIGHT}}, 2); // Fill with solid wall
    for(auto & room : rooms)
    {
        map.Fill({room.a*2+int2(1,1), room.b*2}, 1); // Carve rooms
    }
    return map;
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