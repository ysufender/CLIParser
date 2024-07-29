#pragma onceclipa

#include <unordered_map>
#include <string>
#include <vector>

namespace CLIParser 
{
    union ReturnPtr
    {
        bool* boolVal;
        int* intVal;
        float* floatVal;
        std::string* stringVal;
        std::vector<std::string>* stringList;
        std::vector<int>* intList;
        std::vector<float>* floatList;
    };

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
            const std::unordered_map<std::string, ReturnPtr>& _flags;
            const std::unordered_map<std::string, FlagType>& _flagTypes;
            const std::string& _prefix;
        
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
            Flags(const std::unordered_map<std::string, ReturnPtr>& flagsToSet, const std::unordered_map<std::string, FlagType>& flagTypesToSet, const std::string& flagPrefix);
    };

    class Parser
    {
        private:
            std::unordered_map<std::string, ReturnPtr> _resultFlags;
            std::unordered_map<std::string, FlagType> _flagsAndTypes;
            char** _cliEntries;
            int _entryCount;
            const std::string& _prefix;
            const std::string& _boundPrefix;

            bool _dead = false;
        
        public:
            Parser(char** programCli, int count, const std::string& prefix);
            Parser(char** programCli, int count, const std::string& prefix, const std::string& boundPrefix);
            Parser() = delete;
            Parser(Parser&) = delete;
            Parser(Parser&&) = delete;
            void AddFlag(std::string flagName, FlagType flagType);
            void BindFlag(std::string bindThis, std::string toThis);
            void RemoveFlag(std::string flagName);
            const Flags Parse();

        private:
            ReturnPtr CLIParamToObject(int& index);
            ReturnPtr HandleCliList(int& index);
            ReturnPtr HandleIntList(int& index);
            ReturnPtr HandleFloatList(int& index);
            ReturnPtr HandleStringList(int& index);
            ReturnPtr HandleCliNumber(int& index);
            ReturnPtr HandleCliString(int& index);
            ReturnPtr HandleCliBool(int& index);
            
            void Error(const std::vector<std::string>& messages, int line);
            void Error(const std::string& message, int line);
    };
}
