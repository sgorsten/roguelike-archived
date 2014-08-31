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

    void Attack(Actor & aggressor, Direction direction)
    {
        for(auto & target : actors)
        {
            if(target.position == aggressor.position + direction)
            {
                Attack(aggressor, target);
                return;
            }
        }
        messages(aggressor, {"swing","swings"})("wildly at empty air.");
    }

    void Attack(Actor & aggressor, Actor & target)
    {
        messages(aggressor, {"swing","swings"})("at").Object(target)("and")(aggressor, {"hit","hits"}).Object(target).Sentence();
    }

    void Move(Actor & mover, Direction direction)
    {
        Verb bump = {"bump","bumps"};

        auto dest = mover.position + direction;
        auto & destTile = map.GetTile(dest);
        if(!destTile.walkable)
        {
            messages(mover, bump)("into a")(destTile.label).Sentence();
            return;
        }
        for(auto & other : actors)
        {
            if(other.position == dest)
            {
                messages(mover, bump)("into").Object(other).Sentence();
                messages(other, {"glare","glares"})("at").Object(mover).Sentence();
                messages(mover, {"punch","punches"}).Object(other).Sentence();
                return;
            }
        }
        mover.position = dest;
        return;
    }
};

#endif