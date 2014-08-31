#include "common.h"
#include "game.h"

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
        messages.message.clear();

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
    game.map.Fill({{18,3}, {19,10}}, 2);

    game.actors = {
        {{Color::Yellow, '@'}, "player", 8, {5,5}},
        {{Color::Green, 'o'}, "orc", 5, {8,12}},
        {{Color::Red, 'D'}, "red dragon", 10, {20,8}}
    };
    Actor * player = &game.actors[0];
    player->brain = std::make_shared<PlayerBrain>(game.messages);
    
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
