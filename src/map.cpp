#include "common.h"
#include "map.h"

const Tile Tile::tiles[] = {
    {{Color::Black, ' '}, false, "void"},
    {{Color::Gray, '.'}, true, "dirt floor"},
    {{Color::DkGray, '#'}, false, "wall"}
};

static bool CheckLineOfSight(const Map & map, const int2 & viewer, const int2 & target, bool includeTarget)
{
    for(auto point : EvaluateBresenhamLine(viewer, false, target, includeTarget))
    {
        if(!map.GetTile(point).walkable) // For now, assume unwalkable tiles block visibility
        {
            return false;
        }
    }
    return true;
}

bool Map::HasLineOfSight(const int2 & viewer, const int2 & target) const
{
    return CheckLineOfSight(*this, viewer, target, false) 
        || CheckLineOfSight(*this, viewer, target + int2(1,0), true)
        || CheckLineOfSight(*this, viewer, target + int2(0,1), true)
        || CheckLineOfSight(*this, viewer, target - int2(1,0), true)
        || CheckLineOfSight(*this, viewer, target - int2(0,1), true);
}