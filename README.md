# CLIParser

You need to pass cool things to the CLI but not too cool (that makes it casual I think)? It supports integers, floats, strings, bools and a couple of lists.

## Installation

Simply clone the repository by

`git clone https://github.com/The2ndSlimShady/CLIParser.git`

then you can either use

`./build.ps1`

and create an executable named `CLIParser.exe`

or you can do

`./publish.ps1`

and create the static library `libCLIParser.a`

## Adding/Removing Flags

To use the CLIParser, we first use the `CLIParser` class.

```
#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc};
	parser.AddFlag("i", FlagType::Int);
	parser.AddFlag("f", FlagType::Float);
	parser.AddFlag("s", FlagType::String);
	parser.AddFlag("b", FlagType::Bool);
	parser.AddFlag("il", FlagType::IntList);
	parser.AddFlag("fl", FlagType::FloatList);
	parser.AddFlag("sl", FlagType::StringList);
}
```

Voila! Now it's time to parse the command line and return our flags.

```
#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc};
	parser.AddFlag("i", FlagType::Int);
	parser.AddFlag("f", FlagType::Float);
	parser.AddFlag("s", FlagType::String);
	parser.AddFlag("b", FlagType::Bool);
	parser.AddFlag("il", FlagType::IntList);
	parser.AddFlag("fl", FlagType::FloatList);
	parser.AddFlag("sl", FlagType::StringList);

	Flags flags = parser.Parse();
	
	int i = flags.GetInt("i");
	float f = flags.GetFloat("f");
	string s = flags.GetString("s");
	bool b = flags.GetBool("b");
	vector<int> = flags.GetIntList("il");
	vector<float> = flags.GetFloatList("fl");
	vector<string> = flags.GetStringList("sl");
}
```
Yeah that's all to it.

> Note: There is a define or an alias statement (I can't remember which) for std::string. That's why I'm using string directly. That might be a bad idea though. Maybe I'll change it later.
> 
> Another Note: There is also an alias for std::vector -> vector
>
>Another Another Note: And also for std::unordered_map -> dict

## Passing Conventions

As for primitives:

`exe -b -str1 string -str2 "string 2" -str3 'string 3' -i 69 -f 42`

As for lists:

`exe -floatL [5.4, 358, 6.5] -strL [str, "List", 'for you'] -intL [5, 4]`

Be aware that the square brackets must be attached to the first and last element. Also every element has to have the comma symbol right next to them with a space away from the previous one. 

So `[4,4,3]`, `[ List ]`, `[5,4 , 3]` will cause you trouble.
 
