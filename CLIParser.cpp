#include <iostream>
#include <sstream>

#include "CLIParser.hpp"

static std::unordered_map<std::string, std::string> boundFlags;

static const bool errBool { false };
static const int errInt { 0 };
static const float errFloat { 0 };
static const std::string errString { };
static const std::vector<std::string> errVecStr { };
static const std::vector<int> errVecInt { };
static const std::vector<float> errVecFloat { };

void Flags::SetUp(std::unordered_map<std::string, void*> flagsToSet, std::unordered_map<std::string, FlagType> flagTypesToSet)
{
	flags = flagsToSet;
	flagTypes = flagTypesToSet;
}

const bool& Flags::GetBool(std::string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Bool)
	{
	    // Handle Error
        return errBool;
	}
	
	if (flags[flagName] == nullptr)
        return errBool;
	
	return *static_cast<bool*>(flags[flagName]);
}

const int& Flags::GetInt(std::string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Int)
	{
		// Handle Error
        return errInt;
    }
		
		
	if (flags[flagName] == nullptr)
		return errInt;
    
    return *static_cast<int*>(flags[flagName]);
}

const float& Flags::GetFloat(std::string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Float)
	{
		// Handle Error
		return errFloat;
	}
	
	if (flags[flagName] == nullptr)
        return errFloat;

	return *static_cast<float*>(flags[flagName]);
}

const std::string& Flags::GetString(std::string flagName)
{
	flagName.insert(0, 1, '-');
    if (flagTypes[flagName] != FlagType::String)
    {
        // Handle Error
        return errString;
    }
		
		
	if (flags[flagName] == nullptr)
        return errString;
    
   	return *static_cast<std::string*>(flags[flagName]);
}

const std::vector<std::string>& Flags::GetStringList(std::string flagName)
{
	flagName.insert(0, 1, '-');
    	
    if (flagTypes[flagName] != FlagType::StringList)
    {
       	// Handle Error
        return errVecStr;
    }
		
	if (flags[flagName] == nullptr)
        return errVecStr;
    
    return *static_cast<std::vector<std::string>*>(flags[flagName]);
}

const std::vector<int>& Flags::GetIntList(std::string flagName)
{
	flagName.insert(0, 1, '-');

    if (flagTypes[flagName] != FlagType::IntList)
	{
       	// Handle Error
        return errVecInt;
   	}
		
	if (flags[flagName] == nullptr)
        return errVecInt;
    
   	return *static_cast<std::vector<int>*>(flags[flagName]);
}

const std::vector<float>& Flags::GetFloatList(std::string flagName)
{
	flagName.insert(0, 1, '-');

    if (flagTypes[flagName] != FlagType::FloatList)
    {
       	// Handle Error
        return errVecFloat;
    }
		
		
	if (flags[flagName] == nullptr)
        return errVecFloat;
    
    return *static_cast<std::vector<float>*>(flags[flagName]);
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
}

const Flags CLIParser::Parse()
{
	//for (auto& i : flagsAndTypes)
	//	std::cout << i.first << ": " << int(i.second) << "\n";	

	for (int index = 1; index < entryCount; index++)
	{
		std::string entry = cliEntries[index];
		
		if (entry[0] == '-')
			HandleFlagEntry(index);
	}

	Flags toReturn;
	toReturn.SetUp(resultFlags, flagsAndTypes);

	return toReturn;
}

void CLIParser::HandleFlagEntry(int index)
{
    std::string flag { cliEntries[index] };

	if (resultFlags.contains(cliEntries[index]))
		return;

    if (boundFlags.contains(flag))
    {
        AddFlag("sl", FlagType::StringList);
        resultFlags[boundFlags[flag]] = CLIParamToObject(index);
        RemoveFlag("sl");
    }
    else
        resultFlags[flag] = CLIParamToObject(index);
}

void* CLIParser::CLIParamToObject(int index)
{
	if (index+1 == entryCount)
		return HandleCliBool(index);

	char toIdentify = cliEntries[index+1][0];

	if (toIdentify == '[')
		return HandleCliList(index);
	if (isdigit(toIdentify))
		return HandleCliNumber(index);
	if (std::isalpha(toIdentify))
		return HandleCliString(index);
	if (cliEntries[index+1][0] == '-')
		return HandleCliBool(index);
	
	std::cout << "An Error Occured During the CLI Parsing Process\n";
	return nullptr;
}

void* CLIParser::HandleCliList(int index)
{
	std::string flagName = cliEntries[index];

    	if (flagsAndTypes[flagName] != FlagType::FloatList &&
		flagsAndTypes[flagName] != FlagType::IntList &&
		flagsAndTypes[flagName] != FlagType::StringList)
    	{
        	// Handle Error
        	return nullptr;
    	}

	
	//if (isdigit(cliEntries[index+1][1]))
	//	return HandleIntList(index);
	//return HandleStringList(index);
	
	if (flagsAndTypes[flagName] == FlagType::FloatList)
		return HandleFloatList(index);
	if (flagsAndTypes[flagName] == FlagType::IntList)
		return HandleIntList(index);
	if (flagsAndTypes[flagName] == FlagType::StringList)
		return HandleStringList(index);

	// Handle Error
	
	return nullptr;
}

std::vector<int>* CLIParser::HandleIntList(int index)
{
	std::vector<int>* resultVec = new std::vector<int>();

	std::stringstream ss;

	index++;
	std::string entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stoi(ss.str()));
			ss.str(std::string());
			continue;
		}

		index++;
		entry = cliEntries[index];
	}

	entry.pop_back();
	ss << entry;
	
	if (ss.str().empty())
		return resultVec;

	resultVec->push_back(std::stoi(ss.str()));

	return resultVec;
}

std::vector<float>* CLIParser::HandleFloatList(int index)
{
	std::vector<float>* resultVec = new std::vector<float>();

	std::stringstream ss;

	index++;
	std::string entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stof(ss.str()));
			ss.str(std::string());
			continue;
		}

		index++;
		entry = cliEntries[index];
	}

	entry.pop_back();
	ss << entry;
	
	if (ss.str().empty())
		return resultVec;

	resultVec->push_back(std::stof(ss.str()));

	return resultVec;
}

std::vector<std::string>* CLIParser::HandleStringList(int index)
{
	std::vector<std::string>* resultVec = new std::vector<std::string>();

	std::stringstream ss;

	index++;
	std::string entry = cliEntries[index];
	entry.erase(0, 1);

	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();	
			ss << entry;
			resultVec->push_back(ss.str());
			ss.str(std::string());
			continue;
		}

		index++;
		entry = cliEntries[index];
	}

	entry.pop_back();
	ss << entry;
	
	if (ss.str().empty())
		return resultVec;

	resultVec->push_back(ss.str());

	return resultVec;
}

void* CLIParser::HandleCliNumber(int index)
{
	if (flagsAndTypes[cliEntries[index]] != FlagType::Int && 
		flagsAndTypes[cliEntries[index]] != FlagType::Float)
	{
		// Handle Error
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
		// Handle Error
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
		// Handle Error
		return nullptr;
	}

	return new std::string(cliEntries[index+1]);
}

bool* CLIParser::HandleCliBool(int index)
{
	return new bool(true);
}
