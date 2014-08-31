#include "common.h"
#include "map.h"

const Tile Tile::tiles[] = {
    {{Color::Black, ' '}, false, "void"},
    {{Color::Gray, '.'}, true, "dirt floor"},
    {{Color::DkGray, '#'}, false, "wall"}
};

bool Map::HasLineOfSight(const int2 & viewer, const int2 & target) const
{
    for(auto point : EvaluateBresenhamLine(viewer, false, target, false))
    {
        if(!GetTile(point).walkable) // For now, assume unwalkable tiles block visibility
        {
            return false;
        }
    }
    return true;
}