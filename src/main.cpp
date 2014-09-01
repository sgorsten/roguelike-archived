#include "game.h"

struct NpcBrain : public Brain
{
    std::mt19937 engine;

    Action Think(const Actor & actor, const Perception & perception) override
    {
        std::uniform_int_distribution<int> distribution(0,15);
        switch(distribution(engine))
        {
        case 0: return Action::MakeMove(Direction::SouthWest);
        case 1: return Action::MakeMove(Direction::South);
        case 2: return Action::MakeMove(Direction::SouthEast);
        case 3: return Action::MakeMove(Direction::West);
        case 4: return Action::MakeMove(Direction::East);
        case 5: return Action::MakeMove(Direction::NorthWest);
        case 6: return Action::MakeMove(Direction::North);
        case 7: return Action::MakeMove(Direction::NorthEast);
        default: return Action::MakeRest(); 
        }
    }
};

#include <chrono>

int GameMain()
{
    SetTitle("Roguelike Experiments");

    Game game;
    game.engine = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
    game.map = GenerateRandomMap(game.engine);

    game.SpawnPlayer(Race::races[0], game.SelectRandomLocation());
    auto npcBrain = std::make_shared<NpcBrain>();
    game.Spawn(Race::races[1], game.SelectRandomLocation(), npcBrain);
    game.Spawn(Race::races[2], game.SelectRandomLocation(), npcBrain);
    game.Spawn(Race::races[3], game.SelectRandomLocation(), npcBrain);

    game.messages.self = &game.actors[0];
    
    while(true)
    {
        game.Tick();
        if(game.quit) return 0;
    }
}
