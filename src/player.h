#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

#include <sstream>

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }
    void Clear() { Clear({{0,0},{SCREEN_WIDTH,SCREEN_HEIGHT}}); }
    void Clear(const Rect & rect)
    {
        for(int2 c=rect.a; c.y<rect.b.y; ++c.y)
        {
            for(c.x=rect.a.x; c.x<rect.b.x; ++c.x)
            {
                PutGlyph(c, {});
            }
        }
    }
    void PutGlyph(const int2 & coord, Glyph glyph)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = glyph;
    }
};

struct PlayerBrain : public Brain
{
    MessageBuffer & messages;
    Tile knownTiles[MAP_HEIGHT][MAP_WIDTH];
    Screen screen;

    PlayerBrain(MessageBuffer & messages) : messages(messages) { memset(knownTiles, 0, sizeof(knownTiles)); }

    void Remember(const Perception & perception)
    {
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                auto tile = perception.GetVisibleTile(c);
                if(!tile.IsVoid()) knownTiles[c.y][c.x] = tile;
            }
        }
    }

    Tile GetTile(const int2 & coord) const { return knownTiles[coord.y][coord.x]; }

    Action MoveOrAttack(const Actor & actor, const Perception & perception, Direction direction)
    {
        for(auto other : perception.GetVisibleActors())
        {
            if(other->position == actor.position + direction)
            {
                // TODO: Confirm that other actor is an enemy
                return Action::Attack(direction);
            }
        }
        if(GetTile(actor.position + direction) == TILE_CLOSED_DOOR) return Action::Open(direction); // TODO: Maybe prompt for confirmation?
        return Action::Move(direction);
    }

    Direction PromptDirection(const Actor & actor, const char * prompt)
    {
        screen.Clear({{0,0},{MAP_WIDTH,3}});
        std::ostringstream ss;
        ss << prompt << " (12346789, or Z to cancel)?";
        auto s = ss.str();
        for(int i=0; i<s.size(); ++i)
            screen.PutGlyph({i,0}, {Color::White,s[i]});

        int2 mapOffset = {MAP_OFFSET_X, MAP_OFFSET_Y};
        WriteOutput(screen.glyphs, actor.position + mapOffset);

        while(true)
        {
            switch(ReadInput())
            {
            case 'z': case 'Z': return Direction::None;
            case '1': return Direction::SouthWest; 
            case '2': return Direction::South; 
            case '3': return Direction::SouthEast; 
            case '4': return Direction::West; 
            case '6': return Direction::East; 
            case '7': return Direction::NorthWest; 
            case '8': return Direction::North; 
            case '9': return Direction::NorthEast;
            }
        }
    }

    Action Think(const Actor & actor, const Perception & perception) override
    {
        messages.self = &actor;

        Remember(perception);

        screen.Clear();

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
                auto tile = knownTiles[c.y][c.x];
                screen.PutGlyph(c + mapOffset, tile.GetGlyph());
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
            case 'Q': return Action::Quit();
            case '1': return MoveOrAttack(actor, perception, Direction::SouthWest);
            case '2': return MoveOrAttack(actor, perception, Direction::South);
            case '3': return MoveOrAttack(actor, perception, Direction::SouthEast);
            case '4': return MoveOrAttack(actor, perception, Direction::West);
            case '5': return Action::Rest(); break;
            case '6': return MoveOrAttack(actor, perception, Direction::East); 
            case '7': return MoveOrAttack(actor, perception, Direction::NorthWest);
            case '8': return MoveOrAttack(actor, perception, Direction::North);
            case '9': return MoveOrAttack(actor, perception, Direction::NorthEast);
            case 'o': {
                auto direction = PromptDirection(actor, "Open which door");
                if(direction != Direction::None) return Action::Open(direction);
            } break;
            case 'c': {
                auto direction = PromptDirection(actor, "Close which door");
                if(direction != Direction::None) return Action::Close(direction);
            } break;
            }
        }
    }
};

#endif