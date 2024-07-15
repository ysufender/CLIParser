#include <iostream>

#include "CLIParser.hpp"

// Try something

int main(int argc, char** args)
{	
    //for (int i = 0; i < argc; i++)
    //std::cout << '\n' << args[i];

	CLIParser parser { args, argc };
	//parser.AddFlag("b", FlagType::Bool);
	parser.AddFlag("stringList", FlagType::StringList);
    parser.BindFlag("sl", "stringList");
	parser.AddFlag("il", FlagType::IntList);
	parser.AddFlag("fl", FlagType::FloatList);
	Flags flags = parser.Parse();

    auto& list = flags.GetStringList("stringList");

	for (auto& it: list) {
	std::cout << "stringList: \"" << it << "\"\n";
	}
    std::cout << "\n\n";
	for (auto& it: flags.GetIntList("il")) {
	std::cout << "il: " << it << "\n";
	}
    std::cout << "\n\n";
	for (auto& it: flags.GetFloatList("fl")) {
	std::cout << "fl: " << it << "\n";
	}
	
	//if (flags.GetBool("b"))
	//	std::cout << "It's b'ing time";
	//else
	//	std::cout << "Nobe";
	
	return 1;
}
