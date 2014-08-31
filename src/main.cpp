#include "common.h"
#include "roguelike.h"

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

struct Tile
{
    Glyph glyph;
    bool walkable;
    std::string label;
};

const Tile g_tiles[] = {
    {{Color::Black, ' '}, false, "void"},
    {{Color::Gray, '.'}, true, "dirt floor"},
    {{Color::DkGray, '#'}, false, "wall"}
};

struct Map;
struct Actor;

class Game;

struct Action
{
    virtual ~Action() {}
    virtual void Execute(Game & game, Actor & actor) = 0;
};

std::unique_ptr<Action> MakeRestAction();

struct Brain
{
    virtual std::unique_ptr<Action> Think(const Actor & actor, const Map & map, const std::vector<Actor> & others) = 0;
};

struct Actor
{
    Glyph glyph;
    std::string name;
    int2 position;
    std::shared_ptr<Brain> brain;

    std::unique_ptr<Action> Think(const Map & map, const std::vector<Actor> & others) const
    {
        if(brain) return brain->Think(*this, map, others);
        else return MakeRestAction();
    }
};

struct Map
{
    int             tiles[MAP_HEIGHT][MAP_WIDTH];
    
                    Map()                                   { memset(tiles, 0, sizeof(tiles)); }

    const int &     operator[](const int2 & coord) const    { return tiles[coord.y][coord.x]; }
    const Tile &    GetTile(const int2 & coord) const       { return coord.x < 0 || coord.y < 0 || coord.x >= MAP_WIDTH || coord.y >= MAP_HEIGHT ? g_tiles[0] : g_tiles[(*this)[coord]]; }

    int &           operator[](const int2 & coord)          { return tiles[coord.y][coord.x]; }
    void            Fill(const Rect & r, int tile)          { for(auto c=r.a; c.y<r.b.y; ++c.y) for(c.x=r.a.x; c.x<r.b.x; ++c.x) (*this)[c] = tile; }
};

class MessageBuffer
{
public:
    std::string message;
};

class Game
{
public:
    bool quit = false;
    MessageBuffer messages;
    Map map;
    std::vector<Actor> actors;

    bool TryMove(Actor & actor, Direction direction)
    {
        auto dest = actor.position + direction;
        auto & destTile = g_tiles[map[dest]];
        if(!destTile.walkable)
        {
            messages.message = "You bump into a "+destTile.label+".";
            return false;
        }
        for(auto & other : actors)
        {
            if(other.position == dest)
            {
                messages.message = "You bump into a "+other.name+".";
                return false;
            }
        }
        actor.position = dest;
        return true;
    }
};

struct QuitAction : public Action
{
    void Execute(Game & game, Actor & actor) override
    {
        game.quit = true;
    }    
};

struct RestAction : public Action
{
    void Execute(Game & game, Actor & actor) override {}    
};
std::unique_ptr<Action> MakeRestAction() { return std::make_unique<RestAction>(); }

struct MoveAction : public Action
{
    Direction direction;
    MoveAction(Direction direction) : direction(direction) {}
    void Execute(Game & game, Actor & actor) override
    {
        game.TryMove(actor, direction);
    }
};

struct PlayerBrain : public Brain
{
    MessageBuffer & messages;

    Game & game;

    PlayerBrain(MessageBuffer & messages, Game & game) : messages(messages), game(game) {}

    std::unique_ptr<Action> Think(const Actor & actor, const Map & map, const std::vector<Actor> & others) override
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
                const auto & tile = g_tiles[map[c]];
                screen.PutChar(c+mapOffset, tile.glyph.character, tile.glyph.color);
            }
        }

        // Show actors
        for(auto & actor : others)
        {
            screen.PutChar(actor.position+mapOffset, actor.glyph.character, actor.glyph.color);
        }
        WriteOutput(screen.glyphs, actor.position+mapOffset);

        // Make decision
        while(true)
        {
            const Direction directions[] = {
                Direction::SouthWest, Direction::South, Direction::SouthEast, 
                Direction::West, Direction::None, Direction::East, 
                Direction::NorthWest, Direction::North, Direction::NorthEast
            };
            switch(int ch = ReadInput())
            {
            case 'Q': 
                throw std::runtime_error("Quitting time!"); // TODO: Obviously not this
            case '1': return std::make_unique<MoveAction>(Direction::SouthWest); break;
            case '2': return std::make_unique<MoveAction>(Direction::South); break;
            case '3': return std::make_unique<MoveAction>(Direction::SouthEast); break;
            case '4': return std::make_unique<MoveAction>(Direction::West); break;
            case '5': break; // Rest
            case '6': return std::make_unique<MoveAction>(Direction::East); break;
            case '7': return std::make_unique<MoveAction>(Direction::NorthWest); break;
            case '8': return std::make_unique<MoveAction>(Direction::North); break;
            case '9': return std::make_unique<MoveAction>(Direction::NorthEast); break;
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
        {{Color::Yellow, '@'}, "player", {5,5}},
        {{Color::Green, 'o'}, "orc", {8,12}},
        {{Color::Red, 'D'}, "red dragon", {20,8}}
    };
    Actor * player = &game.actors[0];
    player->brain = std::make_shared<PlayerBrain>(game.messages, game);
    
    while(true)
    {
        for(auto & actor : game.actors)
        {
            auto action = actor.Think(game.map, game.actors);
            action->Execute(game, actor);
        }
    }
}
