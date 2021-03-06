#include "common.h"
#include "message.h"
#include "actor.h"

MessageBuffer::MessageBuffer() : self() 
{
    Clear(); 
}

void MessageBuffer::Clear()
{
    message.clear();
    memset(lastActors, 0, sizeof(lastActors));
    start = true;
}

void MessageBuffer::Sentence()
{
    message += ". ";
    start = true;
}

MessageBuffer & MessageBuffer::operator()(const char * text)
{
    assert(text && *text);
    if(start)
    {
        message += toupper(*text);
        ++text;
    }
    else message += ' ';
    message += text;
    start = false;
    return *this;
}

MessageBuffer & MessageBuffer::Subject(const Actor & actor)
{
    if(&actor == self)
    {
        return (*this)("you");
    }

    if(&actor == lastActors[(int)actor.gender])
    {
        switch(actor.gender)
        {
        case Gender::Neuter: return (*this)("it");
        case Gender::Female: return (*this)("she");
        case Gender::Male: return (*this)("he");
        }
    }

    lastActors[(int)actor.gender] = &actor;    
    return (*this)(actor.race->noun.indefArticle)(actor.race->noun.singular);
}

MessageBuffer & MessageBuffer::Object(const Actor & actor)
{
    if(&actor == self)
    {
        return (*this)("you");
    }

    if(&actor == lastActors[(int)actor.gender])
    {
        switch(actor.gender)
        {
        case Gender::Neuter: return (*this)("it");
        case Gender::Female: return (*this)("her");
        case Gender::Male: return (*this)("him");
        }
    }
    lastActors[(int)actor.gender] = &actor;

    // If the actor is of the opposite gender as the player, mention the gender explicitly
    if(actor.gender != Gender::Neuter && actor.gender != self->gender)
    {
        return (*this)(actor.gender == Gender::Male ? "a male" : "a female")(actor.race->noun.singular);
    }

    return (*this)(actor.race->noun.indefArticle)(actor.race->noun.singular);
}   

MessageBuffer & MessageBuffer::operator()(const Actor & actor, const Verb & verb)
{
    return Subject(actor)(&actor == self ? verb.secondPerson : verb.thirdPerson);
}