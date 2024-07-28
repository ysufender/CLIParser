#pragma onceclipa

#include <cctype>
#include <unordered_map>
#include <string>
#include <vector>

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
	private:
		std::unordered_map<std::string, void*> flags;
		std::unordered_map<std::string, FlagType> flagTypes;
	
	public:
		const bool& GetBool(std::string flagName);
		const int& GetInt(std::string flagName);
		const float& GetFloat(std::string flagName);
		const std::string& GetString(std::string flagName);
		const std::vector<std::string>& GetStringList(std::string flagName);
		const std::vector<int>& GetIntList(std::string flagName);
		const std::vector<float>& GetFloatList(std::string flagName);

        Flags() = delete;
        Flags(Flags&) = delete;
		Flags(std::unordered_map<std::string, void*> flagsToSet, std::unordered_map<std::string, FlagType> flagTypesToSet);
        ~Flags();
};

class CLIParser
{
	private:
		std::unordered_map<std::string, void*> resultFlags;
		std::unordered_map<std::string, FlagType> flagsAndTypes;
		char** cliEntries;
		int entryCount;
	
	public:
		CLIParser(char** cliInputsOfTheCaller, int count);
		void AddFlag(std::string flagName, FlagType flagType);
        void BindFlag(std::string flagName, std::string bindTo);
		void RemoveFlag(std::string flagName);
		const Flags Parse();

	private:
		void HandleFlagEntry(int index);
		void* CLIParamToObject(int index);
		void* HandleCliList(int index);
		std::vector<int>* HandleIntList(int index);
		std::vector<float>* HandleFloatList(int index);
		std::vector<std::string>* HandleStringList(int index);
		void* HandleCliNumber(int index);
		std::string* HandleCliString(int index);
		bool* HandleCliBool(int index);
        
        void Error(const std::vector<std::string>& messages, int line);
        void Error(const std::string& message, int line);
};
