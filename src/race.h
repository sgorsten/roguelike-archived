#ifndef RACE_H
#define RACE_H

#include "roguelike.h"
#include "message.h"

struct Dice
{
    int count;
    int sides;
    int bonus;

    int Roll(std::mt19937 & engine) const
    {
        int total = bonus;
        std::uniform_int_distribution<int> distribution(1,sides);
        for(int i=0; i<count; ++i) total += distribution(engine);
        return total;
    }
};

struct Attack
{
    Verb verb;
    Dice dice;
};

struct Race
{
    Glyph glyph;            // What do characters of this race normally look like (avatars will be overridden to @)
    Noun noun;              // How is an unnamed character of this race described?
    bool isGendered;        // If true, Female and Male genders will be available, if false, only Neuter will be available
    int sightRange;         // How many tiles can a character of this race normally see (0 for blind races)
    Attack unarmedAttack;   // An attack which is available when no weapons are equipped
    int hitPoints;          // How many hit points does a character of this race normally have

    static const Race races[4];
};

#endif