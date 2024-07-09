#pragma once

#include <cctype>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#define vector_t std::vector
#define dict_t std::unordered_map
#define string_t std::string

enum class FlagType
{
	Int = 0,
	Float = 1,
	String = 2,
	IntList = 3,
	FloatList = 4,
	StringList = 5,
	Bool = 6 
};

class Flags 
{
	friend class CLIParser;

	private:
		dict_t<string_t, void*> flags;
		dict_t<string_t, FlagType> flagTypes;
	
	public:
		void SetUp(dict_t<string_t, void*> flagsToSet, dict_t<string_t, FlagType> flagTypesToSet);
		bool GetBool(string_t flagName);
		int GetInt(string_t flagName);
		float GetFloat(string_t flagName);
		string_t GetString(string_t flagName);
		vector_t<string_t> GetStringList(string_t flagName);
		vector_t<int> GetIntList(string_t flagName);
		vector_t<float> GetFloatList(string_t flagName);
};

class CLIParser : Flags
{
	private:
		dict_t<string_t, void*> resultFlags;
		dict_t<string_t, FlagType> flagsAndTypes;
		char** cliEntries;
		int entryCount;
	
	public:
		CLIParser(char** cliInputsOfTheCaller, int count);
		void AddFlag(string_t flagName, FlagType flagType);
		void RemoveFlag(string_t flagName);
		Flags Parse();

	private:
		void HandleFlagEntry(int index);
		void* CLIParamToObject(int index);
		void* HandleCliList(int index);
		vector_t<int>* HandleIntList(int index);
		vector_t<float>* HandleFloatList(int index);
		vector_t<string_t>* HandleStringList(int index);
		void* HandleCliNumber(int index);
		string_t* HandleCliString(int index);
		bool* HandleCliBool(int index);
};
