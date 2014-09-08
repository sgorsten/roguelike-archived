#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <sstream>

enum IndefArticle { ANoun, AnNoun };
enum Gender { Neuter, Female, Male, Plural };

struct CommonNoun
{
    const char * noun[4];

    CommonNoun(const char * singular, const char * plural)
    {
        noun[Neuter] = noun[Female] = noun[Male] = singular;
        noun[Plural] = plural;
    }

    CommonNoun(const char * female, const char * male, const char * plural)
    {
        noun[Neuter] = noun[Male] = male;
        noun[Female] = female;
        noun[Plural] = plural;
    }

    CommonNoun(const char * neuter, const char * female, const char * male, const char * plural)
    {
        noun[Neuter] = neuter;
        noun[Female] = female;
        noun[Male] = male;
        noun[Plural] = plural;
    }
};

struct Noun
{
    int uniqueId;
    bool isDefinite;
    Gender gender;
    std::vector<const char *> prefixes;
    CommonNoun noun;
    std::vector<const char *> suffixes;

    void AddGenderPrefix()
    {
        if(gender == Neuter || gender == Plural) return; // No need for a prefix
        if(noun.noun[Female] != noun.noun[Male]) return; // Gender already determined in base noun
        prefixes.push_back(gender == Female ? "female" : "male");
    }
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

    void AddNoun(const Noun & noun, bool isDefinite, bool isSubject)
    {
        if(noun.uniqueId == listenerId)
        {
            AddWord("you");
            return;
        }
        
        if(noun.uniqueId == lastNouns[noun.gender])
        {
            const char * subjectPronouns[] = {"it","she","he","they"};
            const char * objectPronouns[] = {"it","her","him","them"};
            AddWord((isSubject ? subjectPronouns : objectPronouns)[noun.gender]);
            return;
        }
         
        lastNouns[noun.gender] = noun.uniqueId;

        if(isDefinite) AddWord("the");
        else if(noun.gender == Plural) AddWord("some");
        else
        {
            auto firstWord = noun.noun.noun[noun.gender];
            if(!noun.prefixes.empty()) firstWord = noun.prefixes.front();
            auto first = tolower(firstWord[0]);
            if(first == 'a' || first == 'e' || first == 'i' || first == 'o' || first == 'u') AddWord("an");
            else AddWord("a");
        }
        for(auto & prefix : noun.prefixes) AddWord(prefix);
        AddWord(noun.noun.noun[noun.gender]);
        for(auto & suffix : noun.suffixes) AddWord(suffix);
    }

    void AddSubject(const Noun & noun, bool isDefinite)
    {
        AddNoun(noun, isDefinite, true);
    }

    void AddObject(const Noun & noun, bool isDefinite)
    {
        AddNoun(noun, isDefinite, false);
    }

    void AddSubjectVerbPhrase(const Noun & subject, const Verb & verb)
    {
        AddSubject(subject, false);
        AddWord(subject.uniqueId == listenerId || subject.gender == Plural ? verb.secondPerson : verb.thirdPerson);
    }

    void EndSentence()
    {
        ss << '.';
        capitalize = insertSpace = true;
    }
};

typedef std::function<void(WordStream &)> Sentence;

Sentence MakeSentence(const Noun & subject, const Verb & transitiveVerb, const Noun & object)
{
    return [=](WordStream & words)
    {
        words.AddSubjectVerbPhrase(subject, transitiveVerb);
        words.AddObject(object, false);
        words.EndSentence();        
    };
}

Sentence MakeSentence(const Noun & subject, const Verb & intransitiveVerb)
{
    return [=](WordStream & words)
    {
        words.AddSubjectVerbPhrase(subject, intransitiveVerb);
        words.EndSentence();        
    };
}

#include <iostream>

template<int N> void RenderParagraph(const Sentence (&sentences)[N], const Noun & listener)
{
    WordStream words(listener.uniqueId);
    for(auto & s : sentences) s(words);
    std::cout << "   " << words.ss.str() << '\n' << std::endl;    
}

int main(int argc, char * argv[])
{
    const CommonNoun hero {"heroine","hero","heroes"};
    const CommonNoun bandit {"bandit","bandits"};
    const CommonNoun sword {"sword","swords"};
    const CommonNoun villager {"villager","villagers"};
    
    const Noun theElf {1, true, Female, {"elf"}, hero, {}};
    Noun anOrc {2, false, Male, {}, bandit, {}};
    anOrc.AddGenderPrefix();
    anOrc.prefixes.push_back("orc");
    const Noun aSword {3, false, Neuter, {"gleaming"}, sword, {}};
    const Noun someVillagers {4, false, Plural, {}, villager, {}};
    const Verb draw {"draw","draws"};
    const Verb attack {"attack","attacks"};
    const Verb die {"die","dies"};
    const Verb cheer {"cheer","cheers"};
    const Verb spot {"spot","spots"};

    const Sentence sentences[] = {
        MakeSentence(someVillagers,spot,anOrc),
        MakeSentence(theElf,draw,aSword),
        MakeSentence(theElf,attack,anOrc),
        MakeSentence(anOrc,die),
        MakeSentence(someVillagers,cheer)        
    };

    RenderParagraph(sentences, theElf);
    RenderParagraph(sentences, anOrc);
    RenderParagraph(sentences, someVillagers);
    RenderParagraph(sentences, aSword);

    return 0;
}