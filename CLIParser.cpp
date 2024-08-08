#include <array>
#include <sstream>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_set>

#include "CLIParser.hpp"

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
    extern std::string_view prefix;
    extern std::string_view boundPrefix;
    extern std::unordered_map<std::string, CLIParser::FlagType>* flagTypes;
    extern const std::unordered_set<CLIParser::FlagType> listType;

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
        const std::string_view prefix,
        const std::string& description
    ) : _description(description)
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
        Handlers::prefix = _prefix;
        Handlers::boundPrefix = _boundPrefix;
    }

    Parser::Parser(char** programCli, int count, std::string_view prefix, std::string&& boundPrefix)
        : _prefix(prefix), _boundPrefix(boundPrefix)
    {
        Handlers::cliEntries = programCli;
        Handlers::entryCount = count;
        Handlers::flagTypes = &_flagsAndTypes;
        Handlers::prefix = _prefix;
        Handlers::boundPrefix = _boundPrefix;
    }

    void Parser::BindFlag(std::string&& bindThis, std::string&& toThis)
    {
        if (_dead)
            Handlers::Error("You can't use the CLIParser after parsing the flags and returning.", __LINE__);

        bindThis.insert(0, _boundPrefix);
        toThis.insert(0, _prefix);

        if (!_resultFlags.contains(toThis))
            Handlers::Error("You can't bind '"+bindThis+"' to the nonexistent flag '"+toThis+"'.", __LINE__);

        if (_boundFlags.contains(bindThis))
                return;

        _boundFlags[bindThis] = toThis;

        auto index { std::find_if(_flagDescriptions.begin(), _flagDescriptions.end(), [&toThis](const FlagDescription& fd){return fd.flag==toThis;}) - _flagDescriptions.begin() };
        _flagDescriptions.at(index).bound = bindThis;
    }

    const Flags Parser::Parse()
    {
        if (_dead)
            Handlers::Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        for (int index = 1; index < Handlers::entryCount;)
        {
            const std::string& flag { Handlers::cliEntries[index] };

            if (_boundFlags.contains(flag))
            {
                _flagsAndTypes[flag.data()] = _flagsAndTypes[_boundFlags[flag.data()]];
                _resultFlags[_boundFlags[flag.data()]] = Handlers::CLIParamToObject(index);
                _flagsAndTypes.erase(flag.data());

                continue;
            }

            if (!_resultFlags.contains(flag))
                Handlers::Error({"Given flag ", flag, " has not been registered."}, __LINE__);
                
            _resultFlags[flag] = Handlers::CLIParamToObject(index);
        }

        _dead = true;
        return Flags{_resultFlags, _flagsAndTypes, _prefix, GetHelpText()};
    }

    void Parser::Seperator()
    {
        std::stringstream ss;
        ss << '%' << _flagDescriptions.size();
        _flagDescriptions.emplace_back(ss.str(), "", "");
    }

    const std::string Parser::GetHelpText() const
    {
        std::stringstream ss;

        ss << "\nAvailable Flags:";

        for (const auto& desc : _flagDescriptions)
        {
            if (desc.flag.starts_with("%"))
            {
                ss << '\n';
                continue;
            }

            ss << "\n\t" << desc.flag 
               << ' ' << (Handlers::listType.contains(_flagsAndTypes.at(desc.flag)) ? "<..params..>" : (_flagsAndTypes.at(desc.flag) == FlagType::Bool ? "" : "<value>"))
               << (desc.bound.empty() ?  "" : ", ") 
               << desc.bound << " : " << desc.description;
        }

        return ss.str();
    }
}
