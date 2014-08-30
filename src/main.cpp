#include "roguelike.h"

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutChar(const int2 & coord, int character, Color attribute)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = {attribute, character};
    }
};

struct Tile
{
    Glyph glyph;
    bool walkable;
};

const Tile g_tiles[] = {
    {{Color::Black, ' '}, false},
    {{Color::Gray, '.'}, true},
    {{Color::DkGray, '#'}, false}
};

struct Map
{
    int             tiles[MAP_HEIGHT][MAP_WIDTH];
    
                    Map()                                   { memset(tiles, 0, sizeof(tiles)); }

    const int &     operator[](const int2 & coord) const    { return tiles[coord.y][coord.x]; }
    const Tile &    GetTile(const int2 & coord) const       { return coord.x < 0 || coord.y < 0 || coord.x >= MAP_WIDTH || coord.y >= MAP_HEIGHT ? g_tiles[0] : g_tiles[(*this)[coord]]; }

    int &           operator[](const int2 & coord)          { return tiles[coord.y][coord.x]; }
    void            Fill(const Rect & r, int tile)          { for(auto c=r.a; c.y<r.b.y; ++c.y) for(c.x=r.a.x; c.x<r.b.x; ++c.x) (*this)[c] = tile; }
};

bool TryMove(const Map & map, int2 & position, Direction direction)
{
    auto dest = position + direction;
    if(!g_tiles[map[dest]].walkable) return false;
    position = dest;
    return true;
}

int GameMain()
{
    SetTitle("Roguelike Experiments");

    Map map;
    map.Fill({{0,0}, {MAP_WIDTH,MAP_HEIGHT}}, 2);
    map.Fill({{3,3}, {MAP_WIDTH-3,MAP_HEIGHT-3}}, 1);

    const Direction directions[] = {Direction::SouthWest, Direction::South, Direction::SouthEast, Direction::West, Direction::None, Direction::East, Direction::NorthWest, Direction::North, Direction::NorthEast};
    int2 playerPos = {5,5};
    while(true)
    {
        Screen screen;
        int2 mapOffset = {MAP_OFFSET_X, MAP_OFFSET_Y};
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                const auto & tile = g_tiles[map[c]];
                screen.PutChar(c+mapOffset, tile.glyph.character, tile.glyph.color);
            }
        }
        for(int i=1; i<16; ++i)
        {
            screen.PutChar(int2{2+i, 3}+mapOffset, '%', (Color)i);
        }
        screen.PutChar(playerPos+mapOffset, '@', Color::Yellow);
        WriteOutput(screen.glyphs, playerPos+mapOffset);

        switch(int ch = ReadInput())
        {
        case 'Q': 
            return 0;
        case '1': case '2': case '3':
        case '4': case '5': case '6':
        case '7': case '8': case '9':
            TryMove(map, playerPos, directions[ch-'1']);
            break;
        }
        playerPos = max(playerPos, {0,0});
        playerPos = min(playerPos, {SCREEN_WIDTH-1,SCREEN_HEIGHT-1});
    }
}
