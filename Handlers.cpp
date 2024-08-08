#include <array>
#include <cstdlib>
#include <sstream>
#include <functional>
#include <unordered_set>

#include "CLIParser.hpp"

namespace Handlers
{
    using namespace CLIParser;

    const std::unordered_set<FlagType> numTypes { 
        FlagType::Int,
        FlagType::Float
    };

    extern const std::unordered_set<FlagType> listType { 
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
        std::string entry = cliEntries[index];
        
        if (!numTypes.contains(flagTypes->at(entry)))
            Error({"Type missmatch. Given flag ", cliEntries[index], " is not a number."}, __LINE__);

        bool isFloat = false;

        index++;
        std::string val { cliEntries[index] };
        index++; // responsibility of the function.

        if (flagTypes->at(entry) == FlagType::Float)
            return ReturnPtr { .floatVal = new float{std::stof(val)} };
        if (entry.starts_with("0x"))
            return ReturnPtr { .intVal = new int{std::stoi(val, 0, 16)} };

        return ReturnPtr { .intVal = new int{std::stoi(val)} };
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

        ReturnPtr rp;

        if (flagTypes->at(flagName) == FlagType::FloatList)
            rp = HandleFloatList(index);
        if (flagTypes->at(flagName) == FlagType::IntList)
            rp = HandleIntList(index);
        if (flagTypes->at(flagName) == FlagType::StringList)
            rp = HandleStringList(index);

        return rp;
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

        if (numTypes.contains(type))
            return std::move(HandleCliNumber(index));
        if (listType.contains(type))
            return std::move(HandleCliList(index));
        if (type == FlagType::String)
            return HandleCliString(index);
        if (type == FlagType::Bool)
            return HandleCliBool(index);

        Error({"Can't identify parameter type from entry ", cliEntries[index+1], " for ", cliEntries[index]}, __LINE__);

        return ReturnPtr{};
    }
}


