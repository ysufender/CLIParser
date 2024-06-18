#include <cctype>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#define vector std::vector
#define dict std::unordered_map
#define string std::string

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
		dict<string, void*> flags;
		dict<string, FlagType> flagTypes;
	
	public:
		void SetUp(dict<string, void*> flagsToSet, dict<string, FlagType> flagTypesToSet);
		bool GetFlagBool(string flagName);
		int GetFlagInt(string flagName);
		float GetFlagFloat(string flagName);
		string GetFlagString(string flagName);
		template<typename T>
		vector<T> GetFlagList(string flagName);
};

class CLIParser : Flags
{
	private:
		dict<string, void*> resultFlags;
		dict<string, FlagType> flagsAndTypes;
		char** cliEntries;
		int entryCount;
	
	public:
		CLIParser(char** cliInputsOfTheCaller, int count);
		void AddFlag(string flagName, FlagType flagType);
		void RemoveFlag(string flagName);
		Flags Parse();

	private:
		void HandleFlagEntry(int index);
		void* CLIParamToObject(int index);
		void* HandleCliList(int index);
		vector<int>* HandleIntList(int index);
		vector<float>* HandleFloatList(int index);
		vector<string>* HandleStringList(int index);
		void* HandleCliNumber(int index);
		string* HandleCliString(int index);
		bool* HandleCliBool(int index);
};