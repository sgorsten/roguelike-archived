#include "common.h"
#include "game.h"

#include <random>

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

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutGlyph(const int2 & coord, Glyph glyph)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = glyph;
    }
};

struct PlayerBrain : public Brain
{
    MessageBuffer & messages;
    int knownTiles[MAP_HEIGHT][MAP_WIDTH];

    PlayerBrain(MessageBuffer & messages) : messages(messages) { memset(knownTiles, 0, sizeof(knownTiles)); }

    void Remember(const Perception & perception)
    {
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                int tile = &perception.GetVisibleTile(c) - Tile::tiles;
                if(tile) knownTiles[c.y][c.x] = tile;
            }
        }
    }

    Action MoveOrAttack(const Actor & actor, const Perception & perception, Direction direction)
    {
        for(auto other : perception.GetVisibleActors())
        {
            if(other->position == actor.position + direction)
            {
                // TODO: Confirm that other actor is an enemy
                return Action::MakeAttack(direction);
            }
        }
        return Action::MakeMove(direction);
    }

    Action Think(const Actor & actor, const Perception & perception) override
    {
        Remember(perception);

        Screen screen;

        // Show message
        int2 cursor={0,0};
        for(auto ch : messages.message)
        {
            screen.PutGlyph(cursor, {Color::White,ch});
            ++cursor.x;
        }
        messages.Clear();

        // Show map
        int2 mapOffset = {MAP_OFFSET_X, MAP_OFFSET_Y};
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                const auto & tile = Tile::tiles[knownTiles[c.y][c.x]];
                screen.PutGlyph(c + mapOffset, tile.glyph);
            }
        }

        // Show actors
        for(auto other : perception.GetVisibleActors())
        {
            screen.PutGlyph(other->position + mapOffset, other->glyph);
        }

        // Show self        
        screen.PutGlyph(actor.position + mapOffset, actor.glyph);
        WriteOutput(screen.glyphs, actor.position + mapOffset);

        // Make decision
        while(true)
        {
            switch(ReadInput())
            {
            case 'Q': return Action::MakeQuit();
            case '1': return MoveOrAttack(actor, perception, Direction::SouthWest);
            case '2': return MoveOrAttack(actor, perception, Direction::South);
            case '3': return MoveOrAttack(actor, perception, Direction::SouthEast);
            case '4': return MoveOrAttack(actor, perception, Direction::West);
            case '5': return Action::MakeRest(); break;
            case '6': return MoveOrAttack(actor, perception, Direction::East); 
            case '7': return MoveOrAttack(actor, perception, Direction::NorthWest);
            case '8': return MoveOrAttack(actor, perception, Direction::North);
            case '9': return MoveOrAttack(actor, perception, Direction::NorthEast);
            }
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

    game.actors = {
        {{Color::Yellow, '@'}, {"a","player","players"}, Gender::Male, 8, {5,5}},
        {{Color::Brown, 'r'}, {"a","rat","rats"}, Gender::Neuter, 5, {8,12}},
        {{Color::Green, 'o'}, {"an","orc","orcs"}, Gender::Male, 5, {8,12}},
        {{Color::Red, 'D'}, {"a","red dragon","red dragons"}, Gender::Female, 10, {20,8}}
    };
    Actor * player = &game.actors[0];
    player->brain = std::make_shared<PlayerBrain>(game.messages);
    game.messages.self = player;

    game.actors[1].brain = game.actors[2].brain = std::make_shared<NpcBrain>();
    
    while(true)
    {
        for(auto & actor : game.actors)
        {
            auto action = actor.Think(game);
            action.Execute(game, actor);
            if(game.quit) return 0;
        }
    }
}
