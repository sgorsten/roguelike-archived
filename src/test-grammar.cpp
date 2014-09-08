#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <sstream>

enum Gender { Neuter, Female, Male, Plural };

struct Noun
{
    int uniqueId;
    std::string name;
    Gender gender;
};

struct Verb
{
    const char * secondPerson;
    const char * thirdPerson;
};

struct WordStream
{
    int listenerId = 0;
    std::ostringstream ss;
    bool capitalize = true;
    bool insertSpace = false;
    int lastNouns[4];

    WordStream(int listenerId) : listenerId(listenerId) { memset(lastNouns, 0, sizeof(lastNouns)); }

    void AddWord(const char * word)
    {
        if(insertSpace)
        {
            ss << ' ';
            insertSpace = false;
        }

        if(capitalize)
        {
            ss << (char)toupper(*word);
            ++word;
            capitalize = false;
        }

        ss << word;
        insertSpace = true;
    }

    void AddSubject(const Noun & noun)
    {
        if(noun.uniqueId == listenerId)
        {
            AddWord("you");
            return;
        }
        
        if(noun.uniqueId == lastNouns[noun.gender])
        {
            const char * subjectPronouns[] = {"it","she","he","they"};
            AddWord(subjectPronouns[noun.gender]);
            return;
        }
         
        lastNouns[noun.gender] = noun.uniqueId;
        AddWord(noun.name.c_str());
    }

    void AddObject(const Noun & noun)
    {
        if(noun.uniqueId == listenerId)
        {
            AddWord("you");
            return;
        }
        
        if(noun.uniqueId == lastNouns[noun.gender])
        {
            const char * objectPronouns[] = {"it","her","him","them"};
            AddWord(objectPronouns[noun.gender]);
            return;
        }
         
        lastNouns[noun.gender] = noun.uniqueId;
        AddWord(noun.name.c_str());
    }

    void AddSubjectVerbPhrase(const Noun & subject, const Verb & verb)
    {
        AddSubject(subject);
        AddWord(subject.uniqueId == listenerId || subject.gender == Plural ? verb.secondPerson : verb.thirdPerson);
    }

    void EndSentence()
    {
        ss << '.';
        capitalize = insertSpace = true;
    }
};

class Sentence
{
    std::function<void(WordStream &)> write;
public:
    Sentence() {}

    Sentence(const Noun & subject, const Verb & transitiveVerb, const Noun & object) : write([=](WordStream & words)
    {
        words.AddSubjectVerbPhrase(subject, transitiveVerb);
        words.AddObject(object);
        words.EndSentence();        
    }) {}

    Sentence(const Noun & subject, const Verb & intransitiveVerb) : write([=](WordStream & words)
    {
        words.AddSubjectVerbPhrase(subject, intransitiveVerb);
        words.EndSentence();        
    }) {}

    void Write(WordStream & words) const { write(words); }
};

#include <iostream>

template<int N> void RenderParagraph(const Sentence (&sentences)[N], const Noun & listener)
{
    WordStream words(listener.uniqueId);
    for(auto & s : sentences) s.Write(words);
    std::cout << "   " << words.ss.str() << '\n' << std::endl;    
}

int main(int argc, char * argv[])
{
    const Noun elf {1,"the elven hero",Female};
    const Noun orc {2,"the orc bandit",Male};
    const Noun sword {3,"a sword",Neuter};
    const Noun villagers {4,"the villagers",Plural};
    const Verb draw {"draw","draws"};
    const Verb attack {"attack","attacks"};
    const Verb die {"die","dies"};
    const Verb cheer {"cheer","cheers"};
    const Verb spot {"spot","spots"};

    const Sentence sentences[] = {
        {villagers,spot,orc},
        {elf,draw,sword},
        {elf,attack,orc},
        {orc,die},
        {villagers,cheer}        
    };

    RenderParagraph(sentences, elf);
    RenderParagraph(sentences, orc);
    RenderParagraph(sentences, villagers);
    RenderParagraph(sentences, sword);

    return 0;
}