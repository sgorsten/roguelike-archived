#include "map.h"
#include <sstream>

std::vector<std::pair<int2,Direction>> EnumerateDoorCandidates(const Rect & room, const Rect & other)
{
    std::vector<std::pair<int2,Direction>> candidates;
    if(other.a.x > room.b.x) for(int y=room.a.y; y<room.b.y; y+=2) candidates.push_back({{room.b.x-1, y}, Direction::East});
    if(other.b.x < room.a.x) for(int y=room.a.y; y<room.b.y; y+=2) candidates.push_back({{room.a.x, y}, Direction::West});
    if(other.a.y > room.b.y) for(int x=room.a.x; x<room.b.x; x+=2) candidates.push_back({{x, room.b.y-1}, Direction::South});
    if(other.b.y < room.a.y) for(int x=room.a.x; x<room.b.x; x+=2) candidates.push_back({{x, room.a.y}, Direction::North});
    return candidates;
}

std::vector<int2> EnumerateIntersections(const Map & map)
{
    std::vector<int2> intersections;
    for(int2 c={1,1}; c.y<MAP_HEIGHT; c.y+=2)
    {
        for(c.x=1; c.x<MAP_WIDTH; c.x+=2)
        {
            if(!map.GetTile(c).IsWalkable()) continue;
            if(map.GetTile(c+int2(-1,-1)).IsWalkable()) continue;
            if(map.GetTile(c+int2(+1,-1)).IsWalkable()) continue;
            if(map.GetTile(c+int2(+1,+1)).IsWalkable()) continue;
            if(map.GetTile(c+int2(-1,+1)).IsWalkable()) continue;
            int pathCount = 0;
            if(map.GetTile(c+int2(-1,0)).IsWalkable()) ++pathCount;
            if(map.GetTile(c+int2(+1,0)).IsWalkable()) ++pathCount;
            if(map.GetTile(c+int2(0,-1)).IsWalkable()) ++pathCount;
            if(map.GetTile(c+int2(0,+1)).IsWalkable()) ++pathCount;
            if(pathCount >= 3) intersections.push_back(c);
        }
    }
    return intersections;
}

void CarveTunnel(Map & map, std::mt19937 & engine, const Rect & roomA, const Rect & roomB)
{
    auto doorsA = EnumerateDoorCandidates(roomA, roomB);
    auto doorsB = EnumerateDoorCandidates(roomB, roomA);
    auto doorA = doorsA[std::uniform_int_distribution<int>(0,doorsA.size()-1)(engine)];
    auto doorB = doorsB[std::uniform_int_distribution<int>(0,doorsB.size()-1)(engine)];
    auto pointA = doorA.first + int2(doorA.second) * 2;
    auto pointB = doorB.first + int2(doorB.second) * 2;
    auto delta = pointB - pointA, absDelta = delta.Abs() / 2;
    int2 stepMain = {delta.x > 0 ? 1 : -1, 0};
    int2 stepSide = {0, delta.y > 0 ? 1 : -1};
    if(absDelta.y > absDelta.x)
    {
        std::swap(absDelta.x, absDelta.y);
        std::swap(stepMain, stepSide);
    }
    int turn = std::uniform_int_distribution<int>(0, absDelta.x)(engine);
    auto point = pointA;
    map[point] = 1;
    for(int i=0; i<turn; ++i)
    {
        point += stepMain;
        map[point] = 1;
        point += stepMain;
        map[point] = 1;
    }
    for(int i=0; i<absDelta.y; ++i)
    {
        point += stepSide;
        map[point] = 1;
        point += stepSide;
        map[point] = 1;
    }
    for(int i=turn; i<absDelta.x; ++i)
    {
        point += stepMain;
        map[point] = 1;
        point += stepMain;
        map[point] = 1;
    }
    map[doorA.first + doorA.second] = 1; // TODO: Door tiles
    map[doorB.first + doorB.second] = 1; // TODO: Door tiles
}
Map CreateMap(int seed)
{
    std::mt19937 engine(seed);

    const std::uniform_int_distribution<int> roomWidth(3,5);
    const std::uniform_int_distribution<int> roomHeight(2,4);
    const int2 places = int2(MAP_WIDTH-2, MAP_HEIGHT-2) / 2; // We can place rooms at even coordinates which do not touch either boundary

    // Place a bunch of rooms
    std::vector<Rect> rooms;
    for(int i=0; i<1000 && rooms.size() < 8; ++i)
    {
        int2 size = {roomWidth(engine), roomHeight(engine)};
        int2 place = {
            std::uniform_int_distribution<int>(0, places.x - size.x)(engine),
            std::uniform_int_distribution<int>(0, places.y - size.y)(engine)
        };
        Rect room = {place*2 + int2(1,1), (place+size)*2};
        Rect expandedRoom = {room.a-int2(2,2), room.b+int2(2,2)};
        bool good = true;
        for(auto & other : rooms)
        {
            if(expandedRoom.Intersects(other))
            {
                good = false;
            }
        }
        if(good) rooms.push_back(room);
    }

    // Fill with solid wall
    Map map;
    map.Fill({{0,0}, {MAP_WIDTH,MAP_HEIGHT}}, 2); 

    // Carve rooms
    for(auto & room : rooms) map.Fill(room, 1);

    // Carve tunnels
    for(int i=1; i<rooms.size(); ++i)
    {
        int j = std::uniform_int_distribution<int>(0, i-1)(engine);
        CarveTunnel(map, engine, rooms[i], rooms[j]);
    }

    // Find intersections, and place some hidden doors
    for(auto point : EnumerateIntersections(map))
    {
        if(std::uniform_real_distribution<float>()(engine) < 0.2f)
        {
            static const Direction directions[] = {Direction::North, Direction::East, Direction::South, Direction::West};
            while(true)
            {
                auto dir = directions[std::uniform_int_distribution<int>(0,3)(engine)];
                if(map[point+dir] != 2)
                {
                    map[point+dir] = 3;

                    auto lastPoint = point+dir;
                    auto curPoint = lastPoint+dir;
                    while(true)
                    {
                        int neighbors = 0;
                        for(int i=0; i<4; ++i) if(map.GetTile(curPoint + directions[i]).IsWalkable() && curPoint + directions[i] != lastPoint) ++neighbors;
                        if(neighbors >= 2)
                        {
                            map[lastPoint] = 3;
                            break;
                        }
                        for(int i=0; i<4; ++i)
                        {
                            auto newPoint = curPoint + directions[i];
                            if(map.GetTile(newPoint).IsWalkable() && newPoint != lastPoint)
                            {
                                lastPoint = curPoint;
                                curPoint = newPoint;
                                break;
                            }
                        }
                    }

                    break;
                }
            }
        }
    }

    // Place doors on rooms
    for(auto & room : rooms)
    {
        for(int x=room.a.x; x<room.b.x; x+=2)
        {
            if(map.GetTile({x,room.a.y-1}).IsWalkable()) map[{x,room.a.y-1}] = 4;
            if(map.GetTile({x,room.b.y}).IsWalkable()) map[{x,room.b.y}] = 4;
        }

        for(int y=room.a.y; y<room.b.y; y+=2)
        {
            if(map.GetTile({room.a.x-1,y}).IsWalkable()) map[{room.a.x-1,y}] = 4;
            if(map.GetTile({room.b.x,y}).IsWalkable()) map[{room.b.x,y}] = 4;
        }
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