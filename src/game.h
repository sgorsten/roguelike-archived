#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "actor.h"

class MessageBuffer
{
public:
    std::string message;
};

class Game
{
public:
    bool quit = false;
    MessageBuffer messages;
    Map map;
    std::vector<Actor> actors;

    bool TryMove(Actor & actor, Direction direction)
    {
        auto dest = actor.position + direction;
        auto & destTile = map.GetTile(dest);
        if(!destTile.walkable)
        {
            messages.message = "You bump into a "+destTile.label+".";
            return false;
        }
        for(auto & other : actors)
        {
            if(other.position == dest)
            {
                messages.message = "You bump into a "+other.name+".";
                return false;
            }
        }
        actor.position = dest;
        return true;
    }
};

#endif