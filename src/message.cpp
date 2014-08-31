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
    lastActor = nullptr;
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

    if(&actor == lastActor)
    {
        switch(actor.gender)
        {
        case Gender::Neuter: return (*this)("it");
        case Gender::Female: return (*this)("she");
        case Gender::Male: return (*this)("he");
        }
    }

    lastActor = &actor;    
    return (*this)(actor.noun.indefArticle)(actor.noun.singular);
}

MessageBuffer & MessageBuffer::Object(const Actor & actor)
{
    if(&actor == self)
    {
        return (*this)("you");
    }

    if(&actor == lastActor)
    {
        switch(actor.gender)
        {
        case Gender::Neuter: return (*this)("it");
        case Gender::Female: return (*this)("her");
        case Gender::Male: return (*this)("him");
        }
    }

    lastActor = &actor;    
    return (*this)(actor.noun.indefArticle)(actor.noun.singular);
}   

MessageBuffer & MessageBuffer::operator()(const Actor & actor, const Verb & verb)
{
    return Subject(actor)(&actor == self ? verb.secondPerson : verb.thirdPerson);
}