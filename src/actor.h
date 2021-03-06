#ifndef ACTOR_H
#define ACTOR_H

#include "roguelike.h"
#include "race.h"

class Game;
struct Map;
class Tile;
struct Actor;

class Action
{
    int cost;
    std::function<void(Game &, Actor & actor)> effect;

    Action(int cost, std::function<void(Game &, Actor & actor)> effect) : cost(cost), effect(effect) {}
public:
    int GetCost() const { return cost; }
    void Execute(Game & game, Actor & actor) { effect(game, actor); }

    static Action Quit();
    static Action Rest();
    static Action Move(Direction direction);
    static Action Attack(Direction direction);
    static Action Open(Direction direction);
    static Action Close(Direction direction);
};

class Perception
{
    const Actor & self;
    const Map & map;
    const std::vector<const Actor *> & actors;
public:
    Perception(const Actor & self, const Map & map, const std::vector<const Actor *> & actors) : self(self), map(map), actors(actors) {}

    Tile GetVisibleTile(const int2 & coord) const;
    const std::vector<const Actor *> & GetVisibleActors() const { return actors; }
};

struct Brain
{
    virtual Action Think(const Actor & actor, const Perception & perception) = 0;
};

struct Actor
{
    const Race * race;
    Gender gender;
    int hitPoints;
    int2 position;
    std::shared_ptr<Brain> brain;
    int delay = 0;
    bool isDead = false;

    Action Think(const Game & game) const;
};

#endif