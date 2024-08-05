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

void Test1(int argc, char** args)
{
    Parser parser { args, argc, "--", "-" };

    parser.AddFlag<FlagType::Bool>("b");
    parser.AddFlag<FlagType::Int>("i");
    parser.AddFlag<FlagType::Float>("f");
    parser.AddFlag<FlagType::String>("s");
    parser.AddFlag<FlagType::StringList>("sl");
    parser.AddFlag<FlagType::IntList>("il");
    parser.AddFlag<FlagType::FloatList>("fl");

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

    parser.AddFlag<FlagType::Bool>("b", true);
    parser.AddFlag<FlagType::Int>("i", 42);
    parser.AddFlag<FlagType::Float>("f", 0.48);
    parser.AddFlag<FlagType::String>("s", "Testing Things");
    parser.AddFlag<FlagType::StringList>("sl", { "Hello", "World" });
    parser.AddFlag<FlagType::IntList>("il", { 5, 4, 32 });
    parser.AddFlag<FlagType::FloatList>("fl", { 4.152, 87.785 });

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
    Test1(argc, args);
    std::cout << "\n\n";
    Test2(argc, args);

	return 1;
}
