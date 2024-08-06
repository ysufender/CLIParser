#include <array>
#include <string>
#include <string_view>

#include "CLIParser.hpp"

static std::unordered_map<std::string, std::string> boundFlags;

static const bool errBool { false };
static const int errInt { 0 };
static const float errFloat { 0 };
static const std::string errString { };
static const std::vector<std::string> errVecStr { };
static const std::vector<int> errVecInt { };
static const std::vector<float> errVecFloat { };

namespace Handlers
{
    extern char** cliEntries;
    extern int entryCount;
    extern std::unordered_map<std::string, CLIParser::FlagType>* flagTypes;

    void Error(const std::vector<std::string>& messages, int line);
    void Error(const std::string& message, int line);
    CLIParser::ReturnPtr CLIParamToObject(int& index);
}

namespace CLIParser
{
    //
    // Flags Implementation
    //
    Flags::Flags(
        const std::unordered_map<std::string, ReturnPtr>& flagsToSet,
        const std::unordered_map<std::string, FlagType>& flagTypesToSet,
        const std::string_view prefix
    )
    {
        for (const auto& [flag, returnPtr] : flagsToSet)
        {
            std::string newFlag { flag };
            newFlag.erase(0, prefix.size());

            _flags[newFlag] = returnPtr;
            _flagTypes[newFlag] = flagTypesToSet.at(flag);
        }
    }

    Flags::~Flags()
    {
        using FT = CLIParser::FlagType;

        for (const auto& [f, p] : _flags)
        {
            FT t { _flagTypes.at(f) };

            if(t == FT::Int)
                delete p.intVal;
            if(t == FT::Bool)
                delete p.boolVal;
            if(t == FT::Float)
                delete p.floatVal;
            if(t == FT::String)
                delete p.stringVal;
            if(t == FT::IntList)
                delete p.intList;
            if(t == FT::FloatList)
                delete p.floatList;
            if(t == FT::StringList)
                delete p.stringList;
        }
    }


    //
    // CLIParser Implementation
    //
    Parser::Parser(char** programCli, int count, std::string_view prefix)
        : _prefix(prefix), _boundPrefix(prefix)
    {
        Handlers::cliEntries = programCli;
        Handlers::entryCount = count;
        Handlers::flagTypes = &_flagsAndTypes;
    }

    Parser::Parser(char** programCli, int count, std::string_view prefix, std::string&& boundPrefix)
        : _prefix(prefix), _boundPrefix(boundPrefix)
    {
        Handlers::cliEntries = programCli;
        Handlers::entryCount = count;
        Handlers::flagTypes = &_flagsAndTypes;
    }

    void Parser::BindFlag(std::string&& bindThis, std::string&& toThis)
    {
        if (_dead)
            Handlers::Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        bindThis.insert(0, _boundPrefix);
        toThis.insert(0, _prefix);

        if (boundFlags.contains(bindThis))
                return;

        boundFlags[bindThis] = toThis;
    }

    const Flags Parser::Parse()
    {
        if (_dead)
            Handlers::Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        for (int index = 1; index < Handlers::entryCount;)
        {
            const std::string_view flag { Handlers::cliEntries[index] };

            if (boundFlags.contains(flag.data()))
            {
                _flagsAndTypes[flag.data()] = _flagsAndTypes[boundFlags[flag.data()]];
                _resultFlags[boundFlags[flag.data()]] = Handlers::CLIParamToObject(index);
                _flagsAndTypes.erase(flag.data());

                continue;
            }

            if (!_resultFlags.contains(flag.data()))
                Handlers::Error({"Given flag ", flag.data(), " has not been registered."}, __LINE__);
                
            _resultFlags[flag.data()] = Handlers::CLIParamToObject(index);
        }

        _dead = true;
        return Flags{std::move(_resultFlags), std::move(_flagsAndTypes), _prefix};
    }
}
