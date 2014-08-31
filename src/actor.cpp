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
        game.TryMove(actor, direction);
    });
}

const Tile & Perception::GetVisibleTile(const int2 & coord) const
{ 
    if(dist2(self.position, coord) > self.sightRange * self.sightRange) return g_tiles[0]; // Can't see past sight range
    // TODO: Line-of-sight check
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
        if(dist2(position, other.position) > sightRange * sightRange) continue;                
        visibleActors.push_back(&other);
    }
    return brain->Think(*this, Perception(*this, game.map, visibleActors));   
}