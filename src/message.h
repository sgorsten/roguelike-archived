#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"
#include "roguelike.h"

enum class Gender { Neuter, Female, Male };

struct Noun
{
    const char * indefArticle;
    const char * singular;
    const char * plural;
};

struct Verb
{
    const char * secondPerson;
    const char * thirdPerson;
};

struct Actor;
class MessageBuffer
{
public:
    const Actor * self;
    const Actor * lastActor;
    std::string message;
    bool start;

    MessageBuffer();

    void Clear();
    void Sentence();

    MessageBuffer & operator()(const char * text);
    MessageBuffer & Subject(const Actor & actor); // Emit a subject phrase
    MessageBuffer & Object(const Actor & actor); // Emit an object phrase
    MessageBuffer & operator()(const Actor & actor, const Verb & verb); // Emit a subject-verb phrase
};

#endif