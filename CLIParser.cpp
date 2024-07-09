#include <iostream>
#include <sstream>

#include "CLIParser.hpp"

static dict_t<string_t, string_t> boundFlags;

static const bool errBool { false };
static const int errInt { 0 };
static const float errFloat { 0 };
static const string_t errString { " " };
static const vector_t<string_t> errVecStr { };
static const vector_t<int> errVecInt { };
static const vector_t<float> errVecFloat { };

void Flags::SetUp(dict_t<string_t, void*> flagsToSet, dict_t<string_t, FlagType> flagTypesToSet)
{
	flags = flagsToSet;
	flagTypes = flagTypesToSet;
}

const bool& Flags::GetBool(string_t flagName)
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

const int& Flags::GetInt(string_t flagName)
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

const float& Flags::GetFloat(string_t flagName)
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

const string_t& Flags::GetString(string_t flagName)
{
	flagName.insert(0, 1, '-');
    if (flagTypes[flagName] != FlagType::String)
    {
        // Handle Error
        return errString;
    }
		
		
	if (flags[flagName] == nullptr)
        return errString;
    
   	return *static_cast<string_t*>(flags[flagName]);
}

const vector_t<string_t>& Flags::GetStringList(string_t flagName)
{
	flagName.insert(0, 1, '-');
    	
    if (flagTypes[flagName] != FlagType::StringList)
    {
       	// Handle Error
        return errVecStr;
    }
		
	if (flags[flagName] == nullptr)
        return errVecStr;
    
    return *static_cast<vector_t<string_t>*>(flags[flagName]);
}

const vector_t<int>& Flags::GetIntList(string_t flagName)
{
	flagName.insert(0, 1, '-');

    if (flagTypes[flagName] != FlagType::IntList)
	{
       	// Handle Error
        return errVecInt;
   	}
		
	if (flags[flagName] == nullptr)
        return errVecInt;
    
   	return *static_cast<vector_t<int>*>(flags[flagName]);
}

const vector_t<float>& Flags::GetFloatList(string_t flagName)
{
	flagName.insert(0, 1, '-');

    if (flagTypes[flagName] != FlagType::FloatList)
    {
       	// Handle Error
        return errVecFloat;
    }
		
		
	if (flags[flagName] == nullptr)
        return errVecFloat;
    
    return *static_cast<vector_t<float>*>(flags[flagName]);
}

CLIParser::CLIParser(char** cliInputsOfTheCaller, int count)
{
	cliEntries = cliInputsOfTheCaller;
	entryCount = count;
}

void CLIParser::AddFlag(string_t flagName, FlagType flagType)
{
	flagName.insert(0, 1, '-');

	if (flagsAndTypes.contains(flagName))
		return;
	
	flagsAndTypes[flagName] = flagType;
}

void CLIParser::BindFlag(string_t flagName, string_t bindTo)
{
    flagName.insert(0, 1, '-');
    bindTo.insert(0, 1, '-');

    if (boundFlags.contains(flagName))
            return;

    boundFlags[flagName] = bindTo;
}

void CLIParser::RemoveFlag(string_t flagName)
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
		string_t entry = cliEntries[index];
		
		if (entry[0] == '-')
			HandleFlagEntry(index);
	}

	Flags toReturn;
	toReturn.SetUp(resultFlags, flagsAndTypes);
	return toReturn;
}

void CLIParser::HandleFlagEntry(int index)
{
    string_t flag { cliEntries[index] };

	if (resultFlags.contains(cliEntries[index]))
		return;

    if (boundFlags.contains(flag))
    {
        std::cout << "Flag " << flag << " is bound to " << boundFlags[flag] << '\n';
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
	string_t flagName = cliEntries[index];

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

vector_t<int>* CLIParser::HandleIntList(int index)
{
	vector_t<int>* resultVec = new vector_t<int>();

	std::stringstream ss;

	index++;
	string_t entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stoi(ss.str()));
			ss.str(string_t());
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

vector_t<float>* CLIParser::HandleFloatList(int index)
{
	vector_t<float>* resultVec = new vector_t<float>();

	std::stringstream ss;

	index++;
	string_t entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stof(ss.str()));
			ss.str(string_t());
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

vector_t<string_t>* CLIParser::HandleStringList(int index)
{
	vector_t<string_t>* resultVec = new vector_t<string_t>();

	std::stringstream ss;

	index++;
	string_t entry = cliEntries[index];
	entry.erase(0, 1);

	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();	
			ss << entry;
			resultVec->push_back(ss.str());
			ss.str(string_t());
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
	string_t entry = cliEntries[index];

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

string_t* CLIParser::HandleCliString(int index)
{	
	if (flagsAndTypes[cliEntries[index]] != FlagType::String)
	{
		// Handle Error
		return nullptr;
	}

	return new string_t(cliEntries[index+1]);
}

bool* CLIParser::HandleCliBool(int index)
{
	return new bool(true);
}


