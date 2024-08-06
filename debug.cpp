#include <ios>
#include <iostream>

#include "CLIParser.hpp"

// Try something
    
using namespace CLIParser;

void PrintForeach(const auto& container)
{
    std::cout << '[';
    for (const auto& val : container)
        std::cout << val << ',';
    std::cout << "]\n";
}

void AddF(Parser& parser)
{
    parser.AddFlag<FlagType::Bool>("b");
    parser.AddFlag<FlagType::Int>("i");
    parser.AddFlag<FlagType::Float>("f");
    parser.AddFlag<FlagType::String>("s");
    parser.AddFlag<FlagType::StringList>("sl");
    parser.AddFlag<FlagType::IntList>("il");
    parser.AddFlag<FlagType::FloatList>("fl");
}

void Test1(int argc, char** args)
{
    Parser parser { args, argc, "--", "-" };
    AddF(parser);
    Flags flags = parser.Parse();

    std::cout << "TEST 1\n";
    std::cout << std::boolalpha << flags.GetFlag<FlagType::Bool>("b") << std::noboolalpha << '\n';
    std::cout << flags.GetFlag<FlagType::Int>("i") << '\n';
    std::cout << flags.GetFlag<FlagType::Float>("f") << '\n';
    std::cout << flags.GetFlag<FlagType::String>("s") << '\n';
    PrintForeach(flags.GetFlag<FlagType::StringList>("sl"));
    PrintForeach(flags.GetFlag<FlagType::IntList>("il"));
    PrintForeach(flags.GetFlag<FlagType::FloatList>("fl"));
}

void Test2(int argc, char** args)
{
    Parser parser { args, argc, "--", "-" };
    AddF(parser);
    Flags flags = parser.Parse();

    std::cout << "TEST 2\n";
    std::cout << std::boolalpha << flags.GetFlag<FlagType::Bool>("b") << std::noboolalpha << '\n';
    std::cout << flags.GetFlag<FlagType::Int>("i") << '\n';
    std::cout << flags.GetFlag<FlagType::Float>("f") << '\n';
    std::cout << flags.GetFlag<FlagType::String>("s") << '\n';
    PrintForeach(flags.GetFlag<FlagType::StringList>("sl"));
    PrintForeach(flags.GetFlag<FlagType::IntList>("il"));
    PrintForeach(flags.GetFlag<FlagType::FloatList>("fl"));
}

int main(int argc, char** args)
{	
    //Test1(argc, args);
    //std::cout << "\n\n";
    //Test2(argc, args);
    //std::cout << "\n\n";

	return 1;
}
