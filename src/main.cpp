#include "common.h"
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

int GameMain()
{
    SetTitle("Roguelike Experiments");

    Game game;
    game.map.Fill({{0,0}, {MAP_WIDTH,MAP_HEIGHT}}, 2);
    game.map.Fill({{3,3}, {MAP_WIDTH-3,MAP_HEIGHT-3}}, 1);
    game.map.Fill({{18,3}, {19,10}}, 2);

    game.SpawnPlayer(Race::races[0], {5,5});
    auto npcBrain = std::make_shared<NpcBrain>();
    game.Spawn(Race::races[1], {8,11}, npcBrain);
    game.Spawn(Race::races[2], {8,12}, npcBrain);
    game.Spawn(Race::races[3], {20,8}, npcBrain);

    game.messages.self = &game.actors[0];
    
    while(true)
    {
        game.Tick();
        if(game.quit) return 0;
    }
}
