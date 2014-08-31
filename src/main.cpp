#include "common.h"
#include "game.h"

struct Screen
{
    Glyph glyphs[SCREEN_HEIGHT][SCREEN_WIDTH];

    Screen() { Clear(); }
    void Clear() { memset(glyphs, 0, sizeof(glyphs)); }
    void PutChar(const int2 & coord, int character, Color attribute)
    {
        if(coord.x < 0 || coord.y < 0 || coord.x >= SCREEN_WIDTH || coord.y >= SCREEN_HEIGHT) return;
        glyphs[coord.y][coord.x] = {attribute, character};
    }
};

struct PlayerBrain : public Brain
{
    MessageBuffer & messages;

    Game & game;

    PlayerBrain(MessageBuffer & messages, Game & game) : messages(messages), game(game) {}

    Action Think(const Actor & actor, const Perception & perception) override
    {
        Screen screen;

        // Show message
        int2 cursor={0,0};
        for(auto ch : messages.message)
        {
            screen.PutChar(cursor, ch, Color::White);
            ++cursor.x;
        }
        messages.message.clear();

        // Show map
        int2 mapOffset = {MAP_OFFSET_X, MAP_OFFSET_Y};
        for(int2 c={0,0}; c.y<MAP_HEIGHT; ++c.y)
        {
            for(c.x=0; c.x<MAP_WIDTH; ++c.x)
            {
                const auto & tile = perception.GetVisibleTile(c);
                screen.PutChar(c+mapOffset, tile.glyph.character, tile.glyph.color);
            }
        }

        // Show actors
        for(auto other : perception.GetVisibleActors())
        {
            screen.PutChar(other->position + mapOffset, other->glyph.character, other->glyph.color);
        }

        // Show self        
        screen.PutChar(actor.position + mapOffset, actor.glyph.character, actor.glyph.color);
        WriteOutput(screen.glyphs, actor.position + mapOffset);

        // Make decision
        while(true)
        {
            switch(ReadInput())
            {
            case 'Q': return Action::MakeQuit();
            case '1': return Action::MakeMove(Direction::SouthWest); break;
            case '2': return Action::MakeMove(Direction::South); break;
            case '3': return Action::MakeMove(Direction::SouthEast); break;
            case '4': return Action::MakeMove(Direction::West); break;
            case '5': return Action::MakeRest(); break;
            case '6': return Action::MakeMove(Direction::East); break;
            case '7': return Action::MakeMove(Direction::NorthWest); break;
            case '8': return Action::MakeMove(Direction::North); break;
            case '9': return Action::MakeMove(Direction::NorthEast); break;
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

    game.actors = {
        {{Color::Yellow, '@'}, "player", 8, {5,5}},
        {{Color::Green, 'o'}, "orc", 5, {8,12}},
        {{Color::Red, 'D'}, "red dragon", 10, {20,8}}
    };
    Actor * player = &game.actors[0];
    player->brain = std::make_shared<PlayerBrain>(game.messages, game);
    
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
