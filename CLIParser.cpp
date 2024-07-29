#include <exception>
#include <iostream>
#include <sstream>
#include <array>
#include <algorithm>

#include "CLIParser.hpp"

#define up std::unique_ptr

static std::unordered_map<std::string, std::string> boundFlags;

static const bool errBool { false };
static const int errInt { 0 };
static const float errFloat { 0 };
static const std::string errString { };
static const std::vector<std::string> errVecStr { };
static const std::vector<int> errVecInt { };
static const std::vector<float> errVecFloat { };

namespace CLIParser
{
    static const std::array<FlagType, 2> numTypes { 
        FlagType::Int,
            FlagType::Float
    };

    static const std::array<FlagType, 3> listType { 
        FlagType::IntList,
            FlagType::FloatList,
            FlagType::StringList
    };

    Flags::Flags(const std::unordered_map<std::string, ReturnPtr>& flagsToSet, const std::unordered_map<std::string, FlagType>& flagTypesToSet, const std::string& flagPrefix)
        : _flags(flagsToSet), _flagTypes(flagTypesToSet), _prefix(flagPrefix) 
    {
    }

    const bool& Flags::GetBool(std::string flagName)
    {
        flagName.insert(0, _prefix);

        if (!_flags.contains(flagName) || _flags.at(flagName).boolVal == nullptr)
            return errBool;

        if (_flagTypes.at(flagName) != FlagType::Bool)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not a bool\n";
            return errBool;
        }
        
        return *_flags.at(flagName).boolVal;
    }

    const int& Flags::GetInt(std::string flagName)
    {
        flagName.insert(0, _prefix);

        if (!_flags.contains(flagName) || _flags.at(flagName).intVal == nullptr)
            return errInt;

        if (_flagTypes.at(flagName) != FlagType::Int)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not an int\n";
            return errInt;
        }
            
        return *_flags.at(flagName).intVal;
    }

    const float& Flags::GetFloat(std::string flagName)
    {
        flagName.insert(0, _prefix);
     
        if (!_flags.contains(flagName) || _flags.at(flagName).floatVal == nullptr)
            return errFloat;

        if (_flagTypes.at(flagName) != FlagType::Float)
            return errFloat;
        
        return *_flags.at(flagName).floatVal;
    }

    const std::string& Flags::GetString(std::string flagName)
    {
        flagName.insert(0, _prefix);
        
        if (!_flags.contains(flagName) || _flags.at(flagName).stringVal == nullptr)
            return errString;

        if (_flagTypes.at(flagName) != FlagType::String)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not a string\n";
            return errString;
        }
        
        return *_flags.at(flagName).stringVal;
    }

    const std::vector<std::string>& Flags::GetStringList(std::string flagName)
    {
        flagName.insert(0, _prefix);

        if (!_flags.contains(flagName) || _flags.at(flagName).stringList == nullptr)
            return errVecStr;
            
        if (_flagTypes.at(flagName) != FlagType::StringList)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not a string list\n";
            return errVecStr;
        }
            
        return *_flags.at(flagName).stringList;
    }

    const std::vector<int>& Flags::GetIntList(std::string flagName)
    {
        flagName.insert(0, _prefix);

        if (!_flags.contains(flagName) || _flags.at(flagName).intList == nullptr)
            return errVecInt;

        if (_flagTypes.at(flagName) != FlagType::IntList)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not an int list\n";
            return errVecInt;
        }
            
        return *_flags.at(flagName).intList;
    }

    const std::vector<float>& Flags::GetFloatList(std::string flagName)
    {
        flagName.insert(0, _prefix);

        if (!_flags.contains(flagName) || _flags.at(flagName).floatList == nullptr)
            return errVecFloat;

        if (_flagTypes.at(flagName) != FlagType::FloatList)
        {
            std::cerr << "Type missmatch. Given flag " << flagName << " is not a float list\n";
            return errVecFloat;
        }
        
        return *_flags.at(flagName).floatList;
    }

    //
    // CLIParser Implementation
    //
    Parser::Parser(char** programCli, int count, const std::string& prefix) 
        : _cliEntries(programCli), _entryCount(count), _prefix(prefix), _boundPrefix(prefix) 
    {
    }

    Parser::Parser(char** programCli, int count, const std::string& prefix, const std::string& boundPrefix)
        : _cliEntries(programCli), _entryCount(count), _prefix(prefix), _boundPrefix(boundPrefix)
    {
    }

    void Parser::AddFlag(std::string flagName, FlagType flagType)
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        flagName.insert(0, _prefix);

        if (_flagsAndTypes.contains(flagName))
            return;

        _flagsAndTypes[flagName] = flagType;
        _resultFlags[flagName].intVal = nullptr;
    }

    void Parser::BindFlag(std::string bindThis, std::string toThis)
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        bindThis.insert(0, _boundPrefix);
        toThis.insert(0, _prefix);

        if (boundFlags.contains(bindThis))
                return;

        boundFlags[bindThis] = toThis;
    }

    void Parser::RemoveFlag(std::string flagName)
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        flagName.insert(0, _prefix);

        if (!_flagsAndTypes.contains(flagName))
            return;
        
        _flagsAndTypes.erase(flagName);
        _resultFlags.erase(flagName);
    }

    const Flags Parser::Parse()
    {
        if (_dead)
            Error("You can't use the CLIParser after parsing the flags and returning.\n", __LINE__);

        for (int index = 1; index < _entryCount;)
        {
            const std::string& flag { _cliEntries[index] };

            if (boundFlags.contains(flag))
            {
                _flagsAndTypes[flag] = _flagsAndTypes[boundFlags[flag]];
                _resultFlags[boundFlags[flag]] = CLIParamToObject(index);
                _flagsAndTypes.erase(flag);

                continue;
            }

            if (!_resultFlags.contains(flag))
                Error({"Given flag ", flag, " has not been registered."}, __LINE__);
                
            _resultFlags[flag] = CLIParamToObject(index);
        }

        _dead = true;
        return Flags{std::move(_resultFlags), std::move(_flagsAndTypes), std::move(_prefix)};
    }

    ReturnPtr Parser::CLIParamToObject(int& index)
    {
        const std::string& flag { _cliEntries[index] };
        const FlagType type { _flagsAndTypes.at(flag) };

        if (std::find(numTypes.begin(), numTypes.end(), type) != numTypes.end())
            return std::move(HandleCliNumber(index));
        if (std::find(listType.begin(), listType.end(), type) != listType.end())
            return std::move(HandleCliList(index));
        if (type == FlagType::String)
            return HandleCliString(index);
        if (type == FlagType::Bool)
            return HandleCliBool(index);

        Error({"Can't identify parameter type from entry ", _cliEntries[index+1], " for ", _cliEntries[index]}, __LINE__);

        return ReturnPtr{};
    }

    ReturnPtr Parser::HandleCliList(int& index)
    {
        std::string flagName = _cliEntries[index];

        if (std::find(listType.begin(), listType.end(), _flagsAndTypes.at(flagName)) == listType.end())
            Error({"Type missmatch. Given flag ", flagName, " is not a list."}, __LINE__);

        if (_flagsAndTypes.at(flagName) == FlagType::FloatList)
            return HandleFloatList(index);
        if (_flagsAndTypes.at(flagName) == FlagType::IntList)
            return HandleIntList(index);
        if (_flagsAndTypes.at(flagName) == FlagType::StringList)
            return HandleStringList(index);

        return ReturnPtr {};
    }

    ReturnPtr Parser::HandleIntList(int& index)
    {
        std::vector<int>* resultVec = new std::vector<int>{};
        index++;

        for (; index < _entryCount; index++)
        {
            const std::string_view entry { _cliEntries[index] };

            if (entry.starts_with(_prefix) || entry.starts_with(_boundPrefix))
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

    ReturnPtr Parser::HandleFloatList(int& index)
    {
        std::vector<float>* resultVec = new std::vector<float>{};
        index++;

        for (; index < _entryCount; index++)
        {
            const std::string_view entry { _cliEntries[index] };

            if (entry.starts_with(_prefix) || entry.starts_with(_boundPrefix))
                break;

            try { resultVec->emplace_back(std::stof(entry.data())); }
            catch (const std::exception& e)
            {
                Error({"Can't convert ", entry.data(), " to a float"}, __LINE__);
            }
        }

        return ReturnPtr{ .floatList = resultVec };
    }

    ReturnPtr Parser::HandleStringList(int& index)
    {
        std::vector<std::string>* resultVec = new std::vector<std::string>{};
        index++;

        for (; index < _entryCount; index++)
        {
            const std::string_view entry { _cliEntries[index] };

            if (entry.starts_with(_prefix) || entry.starts_with(_boundPrefix))
                break;

            resultVec->emplace_back(entry.data());
        }

        return ReturnPtr{ .stringList = resultVec };
    }

    ReturnPtr Parser::HandleCliNumber(int& index)
    {
        if (std::find(numTypes.begin(), numTypes.end(), _flagsAndTypes.at(_cliEntries[index])) == numTypes.end())
            Error({"Type missmatch. Given flag ", _cliEntries[index], " is not a number."}, __LINE__);
        
        bool isFloat = false;

        index++;
        std::string entry = _cliEntries[index];
        index++; // responsibility of the function.
        
        if (entry.find_first_of('.') != std::string::npos)
            return ReturnPtr { .floatVal = new float{std::stof(entry)} };
        if (entry.starts_with("0x"))
            return ReturnPtr { .intVal = new int{std::stoi(entry, 0, 16)} };

        return ReturnPtr { .intVal = new int{std::stoi(entry)} };
    }

    ReturnPtr Parser::HandleCliString(int& index)
    {	
        if (_flagsAndTypes.at(_cliEntries[index]) != FlagType::String)
            Error({"Type missmatch. Given flag ", _cliEntries[index], " is not a string."}, __LINE__);
        
        // I'm sorry. Please don't kill m)e
        return ReturnPtr { .stringVal = new std::string{_cliEntries[(++index)++]} };
    }

    ReturnPtr Parser::HandleCliBool(int& index)
    {
        index++;
        return ReturnPtr { .boolVal = new bool{true} };
    }

    void Parser::Error(const std::vector<std::string>& messages, int line)
    {
        std::stringstream ss;
        for (const auto& msg : messages)
            ss << msg;
        std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << ss.str() << '\n';
        exit(1);
    }

    void Parser::Error(const std::string& message, int line)
    {
        std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << message << '\n';
        exit(1);
    }
}
