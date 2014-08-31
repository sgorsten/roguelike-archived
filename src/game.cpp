#include "common.h"
#include "game.h"

void Game::Attack(Actor & aggressor, Direction direction)
{
    for(auto & target : actors)
    {
        if(target.isDead) continue;
        if(target.position == aggressor.position + direction)
        {
            Attack(aggressor, target);
            return;
        }
    }
    messages(aggressor, {"swing","swings"})("wildly at empty air.");
}

void Game::Attack(Actor & aggressor, Actor & target)
{
    messages(aggressor, aggressor.attack.verb).Object(target).Sentence();
    int damage = aggressor.attack.dice.Roll(engine);
    target.hitPoints -= damage;
    if(target.hitPoints < 1)
    {
        target.isDead = true;
        messages(target, {"die","dies"}).Sentence();
    }
}

void Game::Move(Actor & mover, Direction direction)
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
        if(other.isDead) continue;
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