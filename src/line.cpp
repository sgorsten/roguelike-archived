#include "common.h"
#include "roguelike.h"

std::vector<int2> EvaluateBresenhamLine(int2 a, bool includeA, int2 b, bool includeB)
{
    int mainDelta = abs(b.x - a.x), sideDelta = abs(b.y - a.y);
    int2 mainStep = {a.x < b.x ? 1 : -1, 0}, sideStep = {0, a.y < b.y ? 1 : -1};
    if(abs(b.y - a.y) > abs(b.x - a.x))
    {
        std::swap(mainDelta, sideDelta);
        std::swap(mainStep, sideStep);
    }

    auto point = a;
    int error = mainDelta / 2;
    std::vector<int2> points;
    for(int i=0; i<=mainDelta; ++i)
    {
        if((includeA || i!=0) && (includeB || i!=mainDelta))
        {
            points.push_back(point);
        }
        point += mainStep;
        error -= sideDelta;
        if(error < 0)
        {
            point += sideStep;
            error += mainDelta;
        }
    }
    return move(points);
}