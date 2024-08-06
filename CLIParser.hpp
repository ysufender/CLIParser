#pragma once

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

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

    template<typename T, FlagType F>
    concept flaggable = requires(){
        (F == FlagType::Bool && std::is_same_v<T, bool>);
        (F == FlagType::Int && std::is_same_v<T, int>);
        (F == FlagType::Float && std::is_same_v<T, float>);
        (F == FlagType::String && std::is_same_v<T, std::string>);
        (F == FlagType::StringList&& std::is_same_v<T, std::vector<std::string>>);
        (F == FlagType::IntList && std::is_same_v<T, std::vector<int>>);
        (F == FlagType::FloatList && std::is_same_v<T, std::vector<float>>);
    };

    // I'm so sorry...
    template<FlagType F>
    using determineVType = typename std::conditional< F == FlagType::Int, int,
        typename std::conditional<F == FlagType::Bool, bool,
            typename std::conditional<F == FlagType::Float, float,
                typename std::conditional<F == FlagType::String, std::string,
                    typename std::conditional<F == FlagType::IntList, std::vector<int>,
                        typename std::conditional<F == FlagType::FloatList, std::vector<float>, std::vector<std::string>>::type
                    >::type
                >::type
            >::type
        >::type
    >::type;


    class Flags 
    {
        private:
            std::unordered_map<std::string, ReturnPtr> _flags;
            std::unordered_map<std::string, FlagType> _flagTypes;

        public:
            Flags() = delete;
            Flags(Flags&) = delete;

            Flags(
                const std::unordered_map<std::string, ReturnPtr>& flagsToSet,
                const std::unordered_map<std::string, FlagType>& flagTypesToSet,
                const std::string_view prefix
            );
            ~Flags();

            template<FlagType F>
                requires flaggable<determineVType<F>, F>
            const determineVType<F>& GetFlag(const std::string& flagName) const
            {
                if(!_flags.contains(flagName))
                {
                    std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.hpp:" << __LINE__  << ") >>> Given flag " << flagName << " does not exist.\n";
                    exit(1);
                }

                if(_flagTypes.at(flagName) != F)
                {
                    std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.hpp:" << __LINE__  << ") >>> Type missmatch. Given flag " << flagName << " is not matching with flag's value.\n";
                    exit(1);
                }

                return *reinterpret_cast<determineVType<F>*>(_flags.at(flagName).intVal);
            }

        private:
            void* operator new(size_t) = delete;
            void* operator new(size_t, void*) = delete;
            void* operator new[](size_t) = delete;
            void* operator new[](size_t, void*) = delete;
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
            Parser() = delete;
            Parser(Parser&) = delete;
            Parser(Parser&&) = delete;

            Parser(char** programCli, int count, std::string_view prefix);
            Parser(
                char** programCli,
                int count,
                std::string_view prefix,
                std::string&& boundPrefix
            );

            void BindFlag(std::string&& bindThis, std::string&& toThis);
            const Flags Parse();

            template<FlagType F>
                requires flaggable<determineVType<F>, F>
            void AddFlag(std::string&& flagName, determineVType<F> defaultVal = determineVType<F>()) 
            {
                if (_dead)
                {
                    std::cerr << "[ERROR][CLIParser::Error](CLIParser/CLIParser.hpp:" << __LINE__  << ") >>> You can't use this Parser instance after parsing the flags and returning.\n";
                    exit(1);
                }

                flagName.insert(0, _prefix);

                if (_flagsAndTypes.contains(flagName))
                    return;

                _flagsAndTypes[flagName] = F;
                _resultFlags[flagName].intVal = reinterpret_cast<int*>(new determineVType<F>{defaultVal});
            }

        private:
            void* operator new(size_t) = delete;
            void* operator new(size_t, void*) = delete;
            void* operator new[](size_t) = delete;
            void* operator new[](size_t, void*) = delete;
    };
}
