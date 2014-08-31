#include "common.h"
#include "race.h"

const Race Race::races[] = {
    {{Color::Green, 'h'}, {"a","human","humans"}, true, 8, {{"hit","hits"},{2,6,3}}, 100},
    {{Color::Brown, 'r'}, {"a","rat","rats"}, false, 5, {{"bite","bites"},{1,4,0}}, 20},
    {{Color::Green, 'o'}, {"an","orc","orcs"}, true, 5, {{"swing at","swings at"},{1,6,1}}, 30},
    {{Color::Red, 'D'}, {"a","red dragon","red dragons"}, true, 10, {{"breathe fire at","breathes fire at"},{3,6,5}}, 200}
};