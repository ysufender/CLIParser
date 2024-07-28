#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>

#include "CLIParser.hpp"

static std::unordered_map<std::string, std::string> boundFlags;

static const bool errBool { false };
static const int errInt { 0 };
static const float errFloat { 0 };
static const std::string errString { };
static const std::vector<std::string> errVecStr { };
static const std::vector<int> errVecInt { };
static const std::vector<float> errVecFloat { };

static const std::array<FlagType, 2> numTypes { 
    FlagType::Int,
    FlagType::Float
};

static const std::array<FlagType, 3> listType { 
    FlagType::IntList,
    FlagType::FloatList,
    FlagType::StringList
};

Flags::Flags(std::unordered_map<std::string, void*> flagsToSet, std::unordered_map<std::string, FlagType> flagTypesToSet)
    : flags(flagsToSet), flagTypes(flagTypesToSet) 
{
}

const bool& Flags::GetBool(std::string flagName)
{
	flagName.insert(0, 1, '-');

	if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errBool;

	if (flagTypes.at(flagName) != FlagType::Bool)
	{
        std::cerr << "Type missmatch. Given flag " << flagName << " is not a bool\n";
        return errBool;
	}
	
	return *static_cast<bool*>(flags.at(flagName));
}

const int& Flags::GetInt(std::string flagName)
{
	flagName.insert(0, 1, '-');

	if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errInt;

	if (flagTypes.at(flagName) != FlagType::Int)
	{
        std::cerr << "Type missmatch. Given flag " << flagName << " is not an int\n";
        return errInt;
    }
		
    return *static_cast<int*>(flags.at(flagName));
}

const float& Flags::GetFloat(std::string flagName)
{
	flagName.insert(0, 1, '-');
 
	if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errFloat;

	if (flagTypes.at(flagName) != FlagType::Float)
		return errFloat;
	
	return *static_cast<float*>(flags.at(flagName));
}

const std::string& Flags::GetString(std::string flagName)
{
	flagName.insert(0, 1, '-');
    
    if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errString;

    if (flagTypes.at(flagName) != FlagType::String)
    {
        std::cerr << "Type missmatch. Given flag " << flagName << " is not a string\n";
        return errString;
    }
	
   	return *static_cast<std::string*>(flags.at(flagName));
}

const std::vector<std::string>& Flags::GetStringList(std::string flagName)
{
	flagName.insert(0, 1, '-');

    if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errVecStr;
    	
    if (flagTypes.at(flagName) != FlagType::StringList)
    {
        std::cerr << "Type missmatch. Given flag " << flagName << " is not a string list\n";
        return errVecStr;
    }
		
    return *static_cast<std::vector<std::string>*>(flags.at(flagName));
}

const std::vector<int>& Flags::GetIntList(std::string flagName)
{
	flagName.insert(0, 1, '-');

    if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errVecInt;

    if (flagTypes.at(flagName) != FlagType::IntList)
	{
        std::cerr << "Type missmatch. Given flag " << flagName << " is not an int list\n";
        return errVecInt;
   	}
		
   	return *static_cast<std::vector<int>*>(flags.at(flagName));
}

const std::vector<float>& Flags::GetFloatList(std::string flagName)
{
	flagName.insert(0, 1, '-');

    if (!flags.contains(flagName) || flags.at(flagName) == nullptr)
        return errVecFloat;

    if (flagTypes.at(flagName) != FlagType::FloatList)
    {
        std::cerr << "Type missmatch. Given flag " << flagName << " is not a float list\n";
        return errVecFloat;
    }
    
    return *static_cast<std::vector<float>*>(flags.at(flagName));
}

Flags::~Flags()
{
    for (auto& [name, pointer] : flags)
    {
        const FlagType& type { flagTypes.at(name) }; 

        switch (type) {
            case FlagType::Bool:
                delete static_cast<bool*>(pointer);
                break;
            case FlagType::Int:
                delete static_cast<int*>(pointer);
                break;
            case FlagType::Float:
                delete static_cast<float*>(pointer);
                break;
            case FlagType::String:
                delete static_cast<std::string*>(pointer);
                break;
            case FlagType::StringList:
                delete static_cast<std::vector<std::string>*>(pointer);
                break;
            case FlagType::IntList:
                delete static_cast<std::vector<int>*>(pointer);
                break;
            case FlagType::FloatList:
                delete static_cast<std::vector<float>*>(pointer);
                break;
        }
    }
}


//
// CLIParser Implementation
//
CLIParser::CLIParser(char** cliInputsOfTheCaller, int count)
{
	cliEntries = cliInputsOfTheCaller;
	entryCount = count;
}

void CLIParser::AddFlag(std::string flagName, FlagType flagType)
{
	flagName.insert(0, 1, '-');

	if (flagsAndTypes.contains(flagName))
		return;

	flagsAndTypes[flagName] = flagType;
    resultFlags[flagName] = nullptr;
}

void CLIParser::BindFlag(std::string flagName, std::string bindTo)
{
    flagName.insert(0, 1, '-');
    bindTo.insert(0, 1, '-');

    if (boundFlags.contains(flagName))
            return;

    boundFlags[flagName] = bindTo;
}

void CLIParser::RemoveFlag(std::string flagName)
{
	flagName.insert(0, 1, '-');

	if (!flagsAndTypes.contains(flagName))
		return;
	
	flagsAndTypes.erase(flagName);
    resultFlags.erase(flagName);
}

const Flags CLIParser::Parse()
{
    for (int index = 1; index < entryCount; index++)
    {
		std::string entry = cliEntries[index];
		
		if (entry[0] == '-')
			HandleFlagEntry(index);
	}

	return Flags{resultFlags, flagsAndTypes};
}

void CLIParser::HandleFlagEntry(int index)
{
    const std::string& flag { cliEntries[index] };


    if (boundFlags.contains(flag))
    {
        flagsAndTypes[flag] = flagsAndTypes[boundFlags[flag]];
        resultFlags[boundFlags[flag]] = CLIParamToObject(index);
        flagsAndTypes.erase(flag);

        return;
    }

    if (!resultFlags.contains(flag))
        Error({"Given flag ", flag, " has not been registered."}, __LINE__);
        
    resultFlags[flag] = CLIParamToObject(index);
}

void* CLIParser::CLIParamToObject(int index)
{
    const std::string& flag { cliEntries[index] };
    const FlagType type { flagsAndTypes.at(flag) };

    if (std::find(numTypes.begin(), numTypes.end(), type) != numTypes.end())
        return HandleCliNumber(index);
    if (std::find(listType.begin(), listType.end(), type) != listType.end())
        return HandleCliList(index);
    if (type == FlagType::String)
        return HandleCliString(index);
    if (type == FlagType::Bool)
        return HandleCliBool(index);

    Error({"Can't identify parameter type from entry ", cliEntries[index+1], " for ", cliEntries[index]}, __LINE__);
	return nullptr;
}

void* CLIParser::HandleCliList(int index)
{
	std::string flagName = cliEntries[index];

    if (std::find(listType.begin(), listType.end(), flagsAndTypes.at(flagName)) == listType.end())
    {
        Error({"Type missmatch. Given flag ", flagName, " is not a list."}, __LINE__);
        return nullptr;
    }

	if (flagsAndTypes.at(flagName) == FlagType::FloatList)
		return HandleFloatList(index);
	if (flagsAndTypes.at(flagName) == FlagType::IntList)
		return HandleIntList(index);
	if (flagsAndTypes.at(flagName) == FlagType::StringList)
		return HandleStringList(index);

	return nullptr;
}

std::vector<int>* CLIParser::HandleIntList(int index)
{
	std::vector<int>* resultVec = new std::vector<int>();
    index++;

    for (; index < entryCount; index++)
    {
        const std::string_view entry { cliEntries[index] };

        if (entry.starts_with('-'))
            break;

        try { resultVec->emplace_back(std::stoi(entry.data())); }
        catch (const std::exception& e)
        {
            Error("You probably wrote one of the integer values wrong.", __LINE__);
        }
    }

	return resultVec;
}

std::vector<float>* CLIParser::HandleFloatList(int index)
{
	std::vector<float>* resultVec = new std::vector<float>();
    index++;

    for (; index < entryCount; index++)
    {
        const std::string_view entry { cliEntries[index] };

        if (entry.starts_with('-'))
            break;

        try { resultVec->emplace_back(std::stof(entry.data())); }
        catch (const std::exception& e)
        {
            Error("Probably you made a misspell somewhere writing a float value.", __LINE__);
        }
    }

	return resultVec;
}

std::vector<std::string>* CLIParser::HandleStringList(int index)
{
	std::vector<std::string>* resultVec = new std::vector<std::string>();
    index++;

    for (; index < entryCount; index++)
    {
        const std::string_view entry { cliEntries[index] };

        if (entry.starts_with('-'))
            break;

        resultVec->emplace_back(entry.data());
    }

	return resultVec;
}

void* CLIParser::HandleCliNumber(int index)
{
	if (flagsAndTypes[cliEntries[index]] != FlagType::Int && 
		flagsAndTypes[cliEntries[index]] != FlagType::Float)
	{
        Error({"Type missmatch. Given flag ", cliEntries[index], " is not a number."}, __LINE__);
		return nullptr;
	}
	
	bool isFloat = false;

	std::stringstream ss;

	index++;
	std::string entry = cliEntries[index];

	while (entry.front() != '-')
	{
		for (int i = 0; i < entry.length(); i++)
		{
			if (entry[i] == '.')
				isFloat = true;
		}

		ss << entry;
		index++;
		entry = cliEntries[index];
	}
	
	if (ss.str().empty())
	{
        Error({"Can't construct number from, well, nothing. ", "(After ", cliEntries[index-1], ")"}, __LINE__);
		return nullptr;
	}
	
	if (isFloat)
		return new float(std::stof(ss.str()));

	return new int(std::stoi(ss.str()));
}

std::string* CLIParser::HandleCliString(int index)
{	
	if (flagsAndTypes[cliEntries[index]] != FlagType::String)
	{
        Error({"Type missmatch. Given flag ", cliEntries[index], " is not a string."}, __LINE__);
		return nullptr;
	}

	return new std::string(cliEntries[index+1]);
}

bool* CLIParser::HandleCliBool(int index)
{
	return new bool(true);
}

void CLIParser::Error(const std::vector<std::string>& messages, int line)
{
    std::stringstream ss;
    for (const auto& msg : messages)
        ss << msg;
    std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << ss.str() << '\n';
    exit(1);
}

void CLIParser::Error(const std::string& message, int line)
{
    std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.cpp:" << line << ") >>> " << message << '\n';
    exit(1);
}
