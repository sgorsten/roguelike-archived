#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "actor.h"

class Game
{
public:
    bool quit = false;
    MessageBuffer messages;
    Map map;
    std::vector<Actor> actors;

    bool TryMove(Actor & actor, Direction direction)
    {
        Verb bump = {"bump","bumps"};

        auto dest = actor.position + direction;
        auto & destTile = map.GetTile(dest);
        if(!destTile.walkable)
        {
            messages(actor, bump)("into a")(destTile.label).Sentence();
            return false;
        }
        for(auto & other : actors)
        {
            if(other.position == dest)
            {
                messages(actor, bump)("into").Object(other).Sentence();
                messages(other, {"glare","glares"})("at").Object(actor).Sentence();
                messages(actor, {"punch","punches"}).Object(other).Sentence();
                return false;
            }
        }
        actor.position = dest;
        return true;
    }
};

#endif