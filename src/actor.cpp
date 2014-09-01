#include "common.h"
#include "actor.h"
#include "game.h"

Action Action::MakeQuit()
{
    return Action(0, [](Game & game, Actor &)
    {
        game.quit = true;
    });
}

Action Action::MakeRest()
{
    return Action(100, [](Game & game, Actor & actor)
    {
        // TODO: Replenish stamina / heal, etc.
    });
}

Action Action::MakeMove(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & actor)
    {
        game.Move(actor, direction);
    });
}

Action Action::MakeAttack(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & actor)
    {
        game.Attack(actor, direction);
    });
}

Tile Perception::GetVisibleTile(const int2 & coord) const
{ 
    if(dist2(self.position, coord) > self.race->sightRange * self.race->sightRange) return Tile(); // Can't see past sight range
    if(!map.HasLineOfSight(self.position, coord)) return Tile(); // Tile is occluded
    return map.GetTile(coord);
}

Action Actor::Think(const Game & game) const
{
    // If actor has no brain, always rest
    if(!brain)
    {
        return Action::MakeRest();
    }

    // Otherwise, let the brain make a decision based on a perception
    std::vector<const Actor *> visibleActors;
    for(auto & other : game.actors)
    {
        if(&other == this) continue;
        if(other.isDead) continue;
        if(dist2(position, other.position) > race->sightRange * race->sightRange) continue;
        if(!game.map.HasLineOfSight(position, other.position)) continue;         
        visibleActors.push_back(&other);
    }
    return brain->Think(*this, Perception(*this, game.map, visibleActors));   
}