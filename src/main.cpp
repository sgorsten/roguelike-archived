#include "roguelike.h"

struct Screen
{
    Glyph glyphs[CONSOLE_WIDTH * CONSOLE_HEIGHT];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutChar(const int2 & coord, int character, Color attribute)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= CONSOLE_WIDTH || coord.y >= CONSOLE_HEIGHT) return;
        glyphs[coord.y * CONSOLE_WIDTH + coord.x] = {attribute, character};
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
    int             tiles[CONSOLE_WIDTH * CONSOLE_HEIGHT];
    
                    Map()                                   { std::fill(std::begin(tiles), std::end(tiles), 0); }

    const int &     operator[](const int2 & coord) const    { return tiles[coord.y * CONSOLE_WIDTH + coord.x]; }
    const Tile &    GetTile(const int2 & coord) const       { return coord.x < 0 || coord.y < 0 || coord.x >= CONSOLE_WIDTH || coord.y >= CONSOLE_HEIGHT ? g_tiles[0] : g_tiles[(*this)[coord]]; }

    int &           operator[](const int2 & coord)          { return tiles[coord.y * CONSOLE_WIDTH + coord.x]; }
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
    map.Fill({{0,0}, {80,25}}, 2);
    map.Fill({{3,3}, {77,22}}, 1);

    const Direction directions[] = {Direction::SouthWest, Direction::South, Direction::SouthEast, Direction::West, Direction::None, Direction::East, Direction::NorthWest, Direction::North, Direction::NorthEast};
    int2 playerPos = {5,5};
    while(true)
    {
        Screen screen;
        for(int2 c={0,0}; c.y<25; ++c.y)
        {
            for(c.x=0; c.x<80; ++c.x)
            {
                const auto & tile = g_tiles[map[c]];
                screen.PutChar(c, tile.glyph.character, tile.glyph.color);
            }
        }
        for(int i=1; i<16; ++i)
        {
            screen.PutChar({2+i, 3}, '%', (Color)i);
        }
        screen.PutChar(playerPos, '@', Color::Yellow);
        WriteOutput(screen.glyphs, playerPos);

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
        playerPos = min(playerPos, {CONSOLE_WIDTH-1,CONSOLE_HEIGHT-1});
    }
}
