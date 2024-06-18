#include "CLIParser.hpp"

// Try something

int main(int argc, char** args)
{	
	CLIParser parser { args, argc };
	//parser.AddFlag("b", FlagType::Bool);
	parser.AddFlag("sl", FlagType::StringList);
	//parser.AddFlag("il", FlagType::IntList);
	//parser.AddFlag("fl", FlagType::FloatList);
	Flags flags = parser.Parse();
	
	for (auto& it: flags.GetStringList("sl")) {
		std::cout << "sl: " << it << "\n";
	}

	//for (auto& it: flags.GetIntList<int>("il")) {
	//	std::cout << "il: " << it << "\n";
	//}

	//for (auto& it: flags.GetFloatList<float>("fl")) {
	//	std::cout << "fl: " << it << "\n";
	//}
	
	//if (flags.GetBool("b"))
	//	std::cout << "It's b'ing time";
	//else
	//	std::cout << "Nobe";
	
	return 1;
}
