#include "common.h"
#include "game.h"
#include "player.h"

void Game::SpawnPlayer(const Race & race, const int2 & position)
{
    playerBrain = std::make_shared<PlayerBrain>(messages);
    Spawn(race, position, playerBrain);
}

void Game::Spawn(const Race & race, const int2 & position, std::shared_ptr<Brain> brain)
{
    Actor actor;
    actor.race = &race;
    actor.gender = race.isGendered ? (Dice{1,2,0}.Roll(engine) == 1 ? Gender::Female : Gender::Male) : Gender::Neuter;
    actor.hitPoints = race.hitPoints;
    actor.position = position;
    actor.brain = move(brain);
    actor.delay = 0;
    actor.isDead = false;
    actors.push_back(actor);
}

void Game::Tick()
{
    // Select an actor to act during this tick
    Actor * tickActor = nullptr;
    for(auto & actor : actors)
    {
        if(!tickActor || actor.delay < tickActor->delay)
        {
            tickActor = &actor;
        }
    }
    assert(tickActor);

    // Allow the actor to act
    auto action = tickActor->Think(*this);
    tickActor->delay += action.GetCost();
    action.Execute(*this, *tickActor);

    // Erase dead actors
    for(auto it = begin(actors); it != end(actors); )
    {
        if(it->isDead)
        {
            if(it->brain == playerBrain) quit = true; // NOTE: Should handle this more gracefully
            it = actors.erase(it);
        }
        else ++it;
    }
}

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
    messages(aggressor, aggressor.race->unarmedAttack.verb).Object(target).Sentence();
    int damage = aggressor.race->unarmedAttack.dice.Roll(engine);
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