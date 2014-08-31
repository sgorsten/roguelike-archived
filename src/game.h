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

    void Move(Actor & mover, Direction direction);
    void Attack(Actor & aggressor, Direction direction);
    void Attack(Actor & aggressor, Actor & target);
};

#endif