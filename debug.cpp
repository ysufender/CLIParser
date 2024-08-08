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

    parser.AddFlag<FlagType::Bool>("b", "Just a bool", true);
    parser.AddFlag<FlagType::Int>("i", "A regular int", 42);
    parser.AddFlag<FlagType::Float>("f", "Your friendly float",87);
    parser.AddFlag<FlagType::String>("s", "Some string", "Testing Things");
    parser.AddFlag<FlagType::StringList>("sl", "Cool. Why do they call this a vector?", { "Hello", "World" });
    parser.AddFlag<FlagType::IntList>("il", "INTEGER LIST", { 5, 4, 32 });
    parser.AddFlag<FlagType::FloatList>("fl", "FLOATSSS",{ 4.152f, 87.785f });

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

void Test3(int argc, char** args)
{
    Parser parser { args, argc, "--", "-" };

    parser.AddFlag<FlagType::String>("string1", "Some String Value");
    parser.AddFlag<FlagType::String>("string2", "Some Other String Value");
    parser.AddFlag<FlagType::String>("string3", "Defaulted string value", "Default Value");
    parser.AddFlag<FlagType::String>("string4", "Binded String Value");

    parser.BindFlag("s4", "string4");

    Flags flags { parser.Parse() };

    std::cout << "Debug CLI Usage:\n\tCLIParser <..flags..>\n";
    std::cout << flags.GetHelpText() << '\n';
}

int main(int argc, char** args)
{	
    //Test1(argc, args);
    //std::cout << "\n\n";
    //Test2(argc, args);
    //std::cout << "\n\n";
    Test3(argc, args);

	return 1;
}
