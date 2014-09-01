#include "common.h"
#include "actor.h"
#include "game.h"

Action Action::Quit()
{
    return Action(0, [](Game & game, Actor &)
    {
        game.quit = true;
    });
}

Action Action::Rest()
{
    return Action(100, [](Game & game, Actor & actor)
    {
        // TODO: Replenish stamina / heal, etc.
    });
}

Action Action::Move(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & mover)
    {
        Verb bump = {"bump","bumps"};

        auto dest = mover.position + direction;
        auto destTile = game.map.GetTile(dest);

        // Discover secret door
        if(destTile == TILE_HIDDEN_DOOR)
        {
            game.messages(mover, {"discover","discovers"})("a")(destTile.GetLabel()).Sentence();
            game.map[dest] = TILE_FLOOR;
            return;
        }

        if(!destTile.IsWalkable())
        {
            game.messages(mover, bump)("into a")(destTile.GetLabel()).Sentence();
            return;
        }

        if(auto other = game.GetActor(dest))
        {
            game.messages(mover, bump)("into").Object(*other).Sentence();
            return;
        }
        mover.position = dest;
    });
}

Action Action::Open(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & actor)
    {
        auto target = actor.position + direction;
        if(game.map.GetTile(target) == TILE_CLOSED_DOOR)
        {
            game.messages(actor, {"open","opens"})("the door").Sentence();
            game.map[target] = TILE_OPEN_DOOR;
            return;
        }
    });
}

Action Action::Close(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & actor)
    {
        auto target = actor.position + direction;
        if(game.map.GetTile(target) == TILE_OPEN_DOOR)
        {
            game.messages(actor, {"close","closes"})("the door").Sentence();
            game.map[target] = TILE_CLOSED_DOOR;
            return;
        }
    });
}

Action Action::Attack(Direction direction)
{
    return Action(100, [direction](Game & game, Actor & actor)
    {
        if(auto target = game.GetActor(actor.position + direction))
        {
            game.messages(actor, actor.race->unarmedAttack.verb).Object(*target).Sentence();
            int damage = actor.race->unarmedAttack.dice.Roll(game.engine);
            target->hitPoints -= damage;
            if(target->hitPoints < 1)
            {
                target->isDead = true;
                game.messages(*target, {"die","dies"}).Sentence();
            }
        }
        else
        {
            game.messages(actor, {"swing","swings"})("wildly at empty air.");
        }
    });
}

Tile Perception::GetVisibleTile(const int2 & coord) const
{ 
    if(self.position.Dist2(coord) > self.race->sightRange * self.race->sightRange) return Tile(); // Can't see past sight range
    if(!map.HasLineOfSight(self.position, coord)) return Tile(); // Tile is occluded
    return map.GetTile(coord);
}

Action Actor::Think(const Game & game) const
{
    // If actor has no brain, always rest
    if(!brain)
    {
        return Action::Rest();
    }

    // Otherwise, let the brain make a decision based on a perception
    std::vector<const Actor *> visibleActors;
    for(auto & other : game.actors)
    {
        if(&other == this) continue;
        if(other.isDead) continue;
        if(position.Dist2(other.position) > race->sightRange * race->sightRange) continue;
        if(!game.map.HasLineOfSight(position, other.position)) continue;         
        visibleActors.push_back(&other);
    }
    return brain->Think(*this, Perception(*this, game.map, visibleActors));   
}