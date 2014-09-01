#ifndef MAP_H
#define MAP_H

#include "roguelike.h"

enum
{
    TILE_VOID,
    TILE_FLOOR,
    TILE_WALL,
    TILE_HIDDEN_DOOR,
    TILE_CLOSED_DOOR,
    TILE_OPEN_DOOR,
    NUM_TILES
};

class Tile
{
    struct Type
    {
        Glyph glyph;
        bool walkable;
        const char * label;
    };
    static const Type types[NUM_TILES];

    int             type;
public:
                    Tile() : type() {}
                    Tile(int type) : type(type) {}
    
    bool            operator == (Tile tile) const { return type == tile.type; }
    bool            operator != (Tile tile) const { return type != tile.type; }

    bool            IsVoid() const { return type == 0; }
    Glyph           GetGlyph() const { return types[type].glyph; }
    bool            IsWalkable() const { return types[type].walkable; }
    const char *    GetLabel() const { return types[type].label; }
};



struct Map
{
    Tile            tiles[MAP_HEIGHT][MAP_WIDTH];
    
                    Map()                                   {}

    const Tile &    operator[](const int2 & coord) const    { return tiles[coord.y][coord.x]; }
    Tile            GetTile(const int2 & coord) const       { return coord.x < 0 || coord.y < 0 || coord.x >= MAP_WIDTH || coord.y >= MAP_HEIGHT ? Tile(0) : (*this)[coord]; }
    bool            HasLineOfSight(const int2 & viewer, const int2 & target) const;

    Tile &          operator[](const int2 & coord)          { return tiles[coord.y][coord.x]; }
    void            Fill(const Rect & r, Tile tile)         { for(auto c=r.a; c.y<r.b.y; ++c.y) for(c.x=r.a.x; c.x<r.b.x; ++c.x) (*this)[c] = tile; }
};

Map GenerateRandomMap(std::mt19937 & engine);

#endif