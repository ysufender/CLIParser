#pragma once

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
            std::unordered_map<std::string, ReturnPtr> _flags;
            std::unordered_map<std::string, FlagType> _flagTypes;

        
        public:
            const bool& GetBool(std::string flagName) const;
            const int& GetInt(std::string flagName) const;
            const float& GetFloat(std::string flagName) const;
            const std::string& GetString(std::string flagName) const;
            const std::vector<std::string>& GetStringList(std::string flagName) const;
            const std::vector<int>& GetIntList(std::string flagName) const;
            const std::vector<float>& GetFloatList(std::string flagName) const;

            Flags() = delete;
            Flags(Flags&) = delete;
            Flags(const std::unordered_map<std::string, ReturnPtr>& flagsToSet, const std::unordered_map<std::string, FlagType>& flagTypesToSet, const std::string_view prefix);

            ~Flags();
    };

    class Parser
    {
        private:
            std::unordered_map<std::string, ReturnPtr> _resultFlags;
            std::unordered_map<std::string, FlagType> _flagsAndTypes;
            const std::string_view _prefix;
            const std::string_view _boundPrefix;

            bool _dead = false;
        
        public:
            Parser(char** programCli, int count, std::string_view prefix);
            Parser(char** programCli, int count, std::string_view prefix, std::string&& boundPrefix);
            Parser() = delete;
            Parser(Parser&) = delete;
            Parser(Parser&&) = delete;
            void AddFlag(std::string&& flagName, FlagType flagType);
            void BindFlag(std::string&& bindThis, std::string&& toThis);
            void RemoveFlag(std::string&& flagName);
            const Flags Parse();

        private:
            void* operator new(size_t) = delete;
            void* operator new(size_t, void*) = delete;
            void* operator new[](size_t) = delete;
            void* operator new[](size_t, void*) = delete;
    };
}
