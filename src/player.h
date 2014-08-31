#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutGlyph(const int2 & coord, Glyph glyph)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = glyph;
    }
};

struct PlayerBrain : public Brain
{
    MessageBuffer & messages;
    int knownTiles[MAP_HEIGHT][MAP_WIDTH];

    PlayerBrain(MessageBuffer & messages) : messages(messages) { memset(knownTiles, 0, sizeof(knownTiles)); }

    void Remember(const Perception & perception)
    {
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                int tile = &perception.GetVisibleTile(c) - Tile::tiles;
                if(tile) knownTiles[c.y][c.x] = tile;
            }
        }
    }

    Action MoveOrAttack(const Actor & actor, const Perception & perception, Direction direction)
    {
        for(auto other : perception.GetVisibleActors())
        {
            if(other->position == actor.position + direction)
            {
                // TODO: Confirm that other actor is an enemy
                return Action::MakeAttack(direction);
            }
        }
        return Action::MakeMove(direction);
    }

    Action Think(const Actor & actor, const Perception & perception) override
    {
        messages.self = &actor;

        Remember(perception);

        Screen screen;

        // Show message
        int2 cursor={0,0};
        for(auto ch : messages.message)
        {
            screen.PutGlyph(cursor, {Color::White,ch});
            ++cursor.x;
        }
        messages.Clear();

        // Show map
        int2 mapOffset = {MAP_OFFSET_X, MAP_OFFSET_Y};
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                const auto & tile = Tile::tiles[knownTiles[c.y][c.x]];
                screen.PutGlyph(c + mapOffset, tile.glyph);
            }
        }

        // Show actors
        for(auto other : perception.GetVisibleActors())
        {
            screen.PutGlyph(other->position + mapOffset, other->race->glyph);
        }

        // Show self        
        screen.PutGlyph(actor.position + mapOffset, {Color::Yellow, '@'});
        WriteOutput(screen.glyphs, actor.position + mapOffset);

        // Make decision
        while(true)
        {
            switch(ReadInput())
            {
            case 'Q': return Action::MakeQuit();
            case '1': return MoveOrAttack(actor, perception, Direction::SouthWest);
            case '2': return MoveOrAttack(actor, perception, Direction::South);
            case '3': return MoveOrAttack(actor, perception, Direction::SouthEast);
            case '4': return MoveOrAttack(actor, perception, Direction::West);
            case '5': return Action::MakeRest(); break;
            case '6': return MoveOrAttack(actor, perception, Direction::East); 
            case '7': return MoveOrAttack(actor, perception, Direction::NorthWest);
            case '8': return MoveOrAttack(actor, perception, Direction::North);
            case '9': return MoveOrAttack(actor, perception, Direction::NorthEast);
            }
        }
    }
};

#endif