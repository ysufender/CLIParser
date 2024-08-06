#include <exception>
#include <iostream>
#include <sstream>
#include <array>
#include <algorithm>
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
    using namespace CLIParser;

    const std::array<FlagType, 2> numTypes { 
        FlagType::Int,
            FlagType::Float
    };

    const std::array<FlagType, 3> listType { 
        FlagType::IntList,
            FlagType::FloatList,
            FlagType::StringList
    };

    char** cliEntries;
    int entryCount;
    std::string_view prefix;
    std::string_view boundPrefix;
    std::unordered_map<std::string, FlagType>* flagTypes;

    void Error(const std::vector<std::string>& messages, int line)
    {
        std::stringstream ss;
        for (const auto& msg : messages)
            ss << msg;
        std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << ss.str() << '\n';
        exit(1);
    }

    void Error(const std::string& message, int line)
    {
        std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << message << '\n';
        exit(1);
    }


    ReturnPtr HandleIntList(int& index)
    {
        std::vector<int>* resultVec = new std::vector<int>{};
        index++;

        for (; index < entryCount; index++)
        {
            const std::string_view entry { cliEntries[index] };

            if (entry.starts_with(prefix) || entry.starts_with(boundPrefix))
                break;

            try 
            { 
                int base { 10 };

                if (entry.starts_with("0x"))
                    base = 16;

                resultVec->emplace_back(std::stoi(entry.data(), 0, base)); 
            }
            catch (const std::exception& e)
            {
                Error({"Can't convert ", entry.data(), " to an integer."}, __LINE__);
            }
        }

        return ReturnPtr{ .intList = resultVec };
    }

    ReturnPtr HandleFloatList(int& index)
    {
        std::vector<float>* resultVec = new std::vector<float>{};
        index++;

        for (; index < entryCount; index++)
        {
            const std::string_view entry { cliEntries[index] };

            if (entry.starts_with(prefix) || entry.starts_with(boundPrefix))
                break;

            try { resultVec->emplace_back(std::stof(entry.data())); }
            catch (const std::exception& e)
            {
                Error({"Can't convert ", entry.data(), " to a float"}, __LINE__);
            }
        }

        return ReturnPtr{ .floatList = resultVec };
    }

    ReturnPtr HandleStringList(int& index)
    {
        std::vector<std::string>* resultVec = new std::vector<std::string>{};
        index++;

        for (; index < entryCount; index++)
        {
            const std::string_view entry { cliEntries[index] };

            if (entry.starts_with(prefix) || entry.starts_with(boundPrefix))
                break;

            resultVec->emplace_back(entry.data());
        }

        return ReturnPtr{ .stringList = resultVec };
    }

    ReturnPtr HandleCliNumber(int& index)
    {
        if (std::find(numTypes.begin(), numTypes.end(), flagTypes->at(cliEntries[index])) == numTypes.end())
            Error({"Type missmatch. Given flag ", cliEntries[index], " is not a number."}, __LINE__);

        bool isFloat = false;

        index++;
        std::string entry = cliEntries[index];
        index++; // responsibility of the function.

        if (entry.find_first_of('.') != std::string::npos)
            return ReturnPtr { .floatVal = new float{std::stof(entry)} };
        if (entry.starts_with("0x"))
            return ReturnPtr { .intVal = new int{std::stoi(entry, 0, 16)} };

        return ReturnPtr { .intVal = new int{std::stoi(entry)} };
    }

    ReturnPtr HandleCliString(int& index)
    {	
        if (flagTypes->at(cliEntries[index]) != FlagType::String)
            Error({"Type missmatch. Given flag ", cliEntries[index], " is not a string."}, __LINE__);

        // I'm sorry. Please don't kill m)e
        return ReturnPtr { .stringVal = new std::string{cliEntries[(++index)++]} };
    }

    ReturnPtr HandleCliList(int& index)
    {
        std::string flagName = cliEntries[index];

        if (std::find(listType.begin(), listType.end(), flagTypes->at(flagName)) == listType.end())
            Error({"Type missmatch. Given flag ", flagName, " is not a list."}, __LINE__);

        if (flagTypes->at(flagName) == FlagType::FloatList)
            return HandleFloatList(index);
        if (flagTypes->at(flagName) == FlagType::IntList)
            return HandleIntList(index);
        if (flagTypes->at(flagName) == FlagType::StringList)
            return HandleStringList(index);

        return ReturnPtr {};
    }

    ReturnPtr HandleCliBool(int& index)
    {
        index++;
        return ReturnPtr { .boolVal = new bool{true} };
    }

    ReturnPtr CLIParamToObject(int& index)
    {
        const std::string& flag { cliEntries[index] };
        const FlagType type { flagTypes->at(flag) };

        if (std::find(numTypes.begin(), numTypes.end(), type) != numTypes.end())
            return std::move(HandleCliNumber(index));
        if (std::find(listType.begin(), listType.end(), type) != listType.end())
            return std::move(HandleCliList(index));
        if (type == FlagType::String)
            return HandleCliString(index);
        if (type == FlagType::Bool)
            return HandleCliBool(index);

        Error({"Can't identify parameter type from entry ", cliEntries[index+1], " for ", cliEntries[index]}, __LINE__);

        return ReturnPtr{};
    }
}

namespace CLIParser
{
    using namespace Handlers;

    Flags::Flags(const std::unordered_map<std::string, ReturnPtr>& flagsToSet, const std::unordered_map<std::string, FlagType>& flagTypesToSet, const std::string_view prefix)
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
        cliEntries = programCli;
        entryCount = count;
        prefix = _prefix;
        boundPrefix = _boundPrefix;
        flagTypes = &_flagsAndTypes;
    }

    Parser::Parser(char** programCli, int count, std::string_view prefix, std::string&& boundPrefix)
        : _prefix(prefix), _boundPrefix(boundPrefix)
    {
        cliEntries = programCli;
        entryCount = count;
        prefix = _prefix;
        boundPrefix = _boundPrefix;
        flagTypes = &_flagsAndTypes;
    }

    void Parser::BindFlag(std::string&& bindThis, std::string&& toThis)
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        bindThis.insert(0, _boundPrefix);
        toThis.insert(0, _prefix);

        if (boundFlags.contains(bindThis))
                return;

        boundFlags[bindThis] = toThis;
    }

    const Flags Parser::Parse()
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        for (int index = 1; index < entryCount;)
        {
            const std::string_view flag { cliEntries[index] };

            if (boundFlags.contains(flag.data()))
            {
                _flagsAndTypes[flag.data()] = _flagsAndTypes[boundFlags[flag.data()]];
                _resultFlags[boundFlags[flag.data()]] = CLIParamToObject(index);
                _flagsAndTypes.erase(flag.data());

                continue;
            }

            if (!_resultFlags.contains(flag.data()))
                Error({"Given flag ", flag.data(), " has not been registered."}, __LINE__);
                
            _resultFlags[flag.data()] = CLIParamToObject(index);
        }

        _dead = true;
        return Flags{std::move(_resultFlags), std::move(_flagsAndTypes), _prefix};
    }
}
