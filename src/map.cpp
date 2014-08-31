#include "common.h"
#include "map.h"

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