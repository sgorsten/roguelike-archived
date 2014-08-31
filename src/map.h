#ifndef MAP_H
#define MAP_H

#include "roguelike.h"

struct Tile
{
    Glyph glyph;
    bool walkable;
    std::string label;
};

const Tile g_tiles[] = {
    {{Color::Black, ' '}, false, "void"},
    {{Color::Gray, '.'}, true, "dirt floor"},
    {{Color::DkGray, '#'}, false, "wall"}
};

struct Map
{
    int             tiles[MAP_HEIGHT][MAP_WIDTH];
    
                    Map()                                   { memset(tiles, 0, sizeof(tiles)); }

    const int &     operator[](const int2 & coord) const    { return tiles[coord.y][coord.x]; }
    const Tile &    GetTile(const int2 & coord) const       { return coord.x < 0 || coord.y < 0 || coord.x >= MAP_WIDTH || coord.y >= MAP_HEIGHT ? g_tiles[0] : g_tiles[(*this)[coord]]; }
    bool            HasLineOfSight(const int2 & viewer, const int2 & target) const;

    int &           operator[](const int2 & coord)          { return tiles[coord.y][coord.x]; }
    void            Fill(const Rect & r, int tile)          { for(auto c=r.a; c.y<r.b.y; ++c.y) for(c.x=r.a.x; c.x<r.b.x; ++c.x) (*this)[c] = tile; }
};

#endif