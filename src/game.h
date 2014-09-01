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
    std::mt19937 engine;
    std::shared_ptr<Brain> playerBrain;

    int2 SelectRandomLocation();
    void SpawnPlayer(const Race & race, const int2 & position);
    void Spawn(const Race & race, const int2 & position, std::shared_ptr<Brain> brain);

    void Tick();
    void Move(Actor & mover, Direction direction);
    void Attack(Actor & aggressor, Direction direction);
    void Attack(Actor & aggressor, Actor & target);
};

#endif