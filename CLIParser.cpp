#include "CLIParser.hpp"

void Flags::SetUp(dict<string, void*> flagsToSet, dict<string, FlagType> flagTypesToSet)
{
	flags = flagsToSet;
	flagTypes = flagTypesToSet;

	//for (auto& it: flagTypes) {
		//std::cout << it.first << "\n";
		//std::cout << it.first << ": " << int(it.second) << "\n";
	//}
}

bool Flags::GetBool(string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Bool)
	{
	    // Handle Error
	    return false;
	}
	
	if (flags[flagName] == nullptr)
		return false;
	
	return *static_cast<bool*>(flags[flagName]);
}

int Flags::GetInt(string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Int)
	{
		// Handle Error
        	return 0;
    	}
		
		
	if (flags[flagName] == nullptr)
		return 0;
    
    	return *static_cast<int*>(flags[flagName]);
}

float Flags::GetFloat(string flagName)
{
	flagName.insert(0, 1, '-');
	if (flagTypes[flagName] != FlagType::Float)
	{
		// Handle Error
		return 0.0;
	}
	
	if (flags[flagName] == nullptr)
		return 0.0;

	return *static_cast<float*>(flags[flagName]);
}

string Flags::GetString(string flagName)
{
	flagName.insert(0, 1, '-');
    	if (flagTypes[flagName] != FlagType::String)
    	{
        	// Handle Error
        	return "";
    	}
		
		
	if (flags[flagName] == nullptr)
		return string("");
    
    	return *static_cast<string*>(flags[flagName]);
}

vector<string> Flags::GetStringList(string flagName)
{
	flagName.insert(0, 1, '-');
    	if (flagTypes[flagName] != FlagType::StringList)
    	{
        	// Handle Error
        	return vector<string>();
    	}
		
		
	if (flags[flagName] == nullptr)
		return vector<string>();
    
    	return *static_cast<vector<string>*>(flags[flagName]);
}

vector<int> Flags::GetIntList(string flagName)
{
	flagName.insert(0, 1, '-');
    	if (flagTypes[flagName] != FlagType::IntList)
    	{
        	// Handle Error
        	return vector<int>();
    	}
		
		
	if (flags[flagName] == nullptr)
		return vector<int>();
    
    	return *static_cast<vector<int>*>(flags[flagName]);
}

vector<float> Flags::GetFloatList(string flagName)
{
	flagName.insert(0, 1, '-');
    	if (flagTypes[flagName] != FlagType::FloatList)
    	{
        	// Handle Error
        	return vector<float>();
    	}
		
		
	if (flags[flagName] == nullptr)
		return vector<float>();
    
    	return *static_cast<vector<float>*>(flags[flagName]);
}

CLIParser::CLIParser(char** cliInputsOfTheCaller, int count)
{
	cliEntries = cliInputsOfTheCaller;
	entryCount = count;
}

void CLIParser::AddFlag(string flagName, FlagType flagType)
{
	flagName.insert(0, 1, '-');

	if (flagsAndTypes.contains(flagName))
		return;
	
	flagsAndTypes[flagName] = flagType;
}

void CLIParser::RemoveFlag(string flagName)
{
	flagName.insert(0, 1, '-');

	if (!flagsAndTypes.contains(flagName))
		return;
	
	flagsAndTypes.erase(flagName);
}

Flags CLIParser::Parse()
{
	//for (auto& i : flagsAndTypes)
	//	std::cout << i.first << ": " << int(i.second) << "\n";	

	for (int index = 1; index < entryCount; index++)
	{
		string entry = cliEntries[index];
		
		if (entry[0] == '-')
			HandleFlagEntry(index);
	}

	Flags toReturn;
	toReturn.SetUp(resultFlags, flagsAndTypes);
	return toReturn;
}

void CLIParser::HandleFlagEntry(int index)
{
	if (resultFlags.contains(cliEntries[index]))
		return;

	resultFlags[cliEntries[index]] = CLIParamToObject(index);;
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
	
	std::cout << "An Error Occured\n";
	return nullptr;
}

void* CLIParser::HandleCliList(int index)
{
	string flagName = cliEntries[index];

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

vector<int>* CLIParser::HandleIntList(int index)
{
	vector<int>* resultVec = new vector<int>();

	std::stringstream ss;

	index++;
	string entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stoi(ss.str()));
			ss.str(string());
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

vector<float>* CLIParser::HandleFloatList(int index)
{
	vector<float>* resultVec = new vector<float>();

	std::stringstream ss;

	index++;
	string entry = cliEntries[index];
	entry.erase(0, 1);
	
	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();
			ss << entry;
			resultVec->push_back(std::stof(ss.str()));
			ss.str(string());
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

vector<string>* CLIParser::HandleStringList(int index)
{
	vector<string>* resultVec = new vector<string>();

	std::stringstream ss;

	index++;
	string entry = cliEntries[index];
	entry.erase(0, 1);

	while (entry.back() != ']')
	{
		if (entry.back() == ',')
		{
			entry.pop_back();	
			ss << entry;
			resultVec->push_back(ss.str());
			ss.str(string());
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
	string entry = cliEntries[index];

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

string* CLIParser::HandleCliString(int index)
{	
	if (flagsAndTypes[cliEntries[index]] != FlagType::String)
	{
		// Handle Error
		return nullptr;
	}

	return new string(cliEntries[index+1]);
}

bool* CLIParser::HandleCliBool(int index)
{
	return new bool(true);
}


