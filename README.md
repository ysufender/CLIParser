# CLIParser

A simple CLI parser. Simple data types and simple operations are allowed, for simple usages.

## Installation

Simply clone the repository by

`git clone https://github.com/The2ndSlimShady/CLIParser.git`

then you can either use

`./build.ps1` or `./build.sh`

and create an executable named `CLIParser.exe` at `build/Debug` to try it out by using `debug.cpp`

or you can do

`./publish.ps1` or `./publish.sh`

and create the static library `libCLIParser.a`, which you can link to your projects and use.

Or you can just go and do `cmake blah blah blah -DCMAKE_BUILD_TYPE=DebugTest` to include `debug.cpp` and create an executable,
Or yet again `cmake blah blah blah -DCMAKE_BUILD_TYPE=whateveryouwantking` to just compile it to a static lib.


## Adding/Removing Flags

The `Parser` class does all the work for us. Simply pass the `args`, `argc`, and the prefix of your choice to the constructor, then use the `AddFlag` method.

```cpp
#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
	Parser parser {args, argc, "--"};
	parser.AddFlag<FlagType::Int>("i");
	parser.AddFlag<FlagType::Float>("f");
	parser.AddFlag<FlagType::String>("s");
	parser.AddFlag<FlagType::Bool>("b");
	parser.AddFlag<FlagType::IntList>("il");
	parser.AddFlag<FlagType::FloatList>("fl");
	parser.AddFlag<FlagType::StringList>("sl");
}
```

Voila! Now it's time to parse the command line and return our flags.

```cpp
#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
	Parser parser {args, argc, "--"};
	parser.AddFlag<FlagType::Int>("i");
	parser.AddFlag<FlagType::Float>("f");
	parser.AddFlag<FlagType::String>("s");
	parser.AddFlag<FlagType::Bool>("b");
	parser.AddFlag<FlagType::IntList>("il");
	parser.AddFlag<FlagType::FloatList>("fl");
	parser.AddFlag<FlagType::StringList>("sl");

	Flags flags = parser.Parse();
	int i = flags.GetFlag<FlagType::Int>("i");
	float f = flags.GetFlag<FlagType::Float>("f");
	std::string s = flags.GetFlag<FlagType::String>("s");
	bool b = flags.GetFlag<FlagType::Bool>("b");
	std::vector<int> = flags.GetFlag<FlagType::IntList>("il");
	std::vector<float> = flags.GetFlag<FlagType::FloatList>("fl");
	std::vector<string> = flags.GetFlag<FlagType::StringList>("sl");
}
```

Be aware that with this way, all the flags will be set to the default constructed values of their types. If you want to specify a default value, see the next header.


## Default Values

Specifying default values is easy, just add the value as another argument to `AddFlag` method. The template metaprogramming dark magic will handle the rest.

```cpp
#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
	Parser parser {args, argc, "--"};
	parser.AddFlag<FlagType::String>("someStr", "Yeah, defaults...");

    Flags flags = parser.Parse();

    std::cout << flags.GetFlag<FlagType::String>("someStr") << '\n';
}

// Outputs 'Yeah, defaults...' if not value is provided in CLI.
```


## Binding Flags

Sometimes we might want to use both `-h` and `--help` for help flag. But we want them both to have the same value depending on usage. So what do we do? Yeah we bind them.
Binding is a one way operation. You can bind a flag to another but can't (you can actually but I wouldn't suggest doing so) bind the binder one to the binded. Yeah.

```cpp
#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
	Parser parser {args, argc, "--"};
	parser.AddFlag<FlagType::Bool>("help");
	parser.BindFlag("h", "help");
}
```

Now, we binded the `--h` flag to `--help`. So whenever we do `exe --h`, the value of `--help` will be true.

```cpp
#include <iostream>

#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
	Parser parser {args, argc, "--"};
	parser.AddFlag<FlagType::Bool>("help");
	parser.BindFlag("h", "help");

	Flags flags = parser.Parse();
	bool help = flags.GetFlag<FlagType::Bool>("help");

	std::cout <<  "This shall print one if help is needed: " << help << '\n';
}
```

Fun Fact: If you try to `flags.GetFlag<FlagType::Bool>("h")`, you'll get a nice, warm error message slapped into your face. That's because `h` flag doesn't exist, it was a CLI alias for `help`. 

Another Fun Fact: You can change the prefix used for binded flags. `Parser` class has another constructor that takes an argument named `boundPrefix`.

```cpp

#include "CLIParser.hpp"

using namespace CLIParser;

int main(int argc, char** args)
{
    Parser parser {args, argc, "--", "-"};   // now the bound flags will use `-` as prefix
    parser.AddFlag<FlagType::Bool>("help");
    parser.BindFLag("h", "help");

    Flags flags = parser.Parse();
    bool help = flags.GetFlag<FlagType::Bool>("help");
	std::cout <<  "This shall print one if help is needed: " << help << '\n';
}
```

Now the `-h` is bound to `--help` instead of `--h`. Pretty neat huh? (Just say yes)

  
## Passing Conventions

As for primitives:

`exe -b -str1 string -str2 "string 2" -str3 'string 3' -i 69 -f 42`

And for lists:

`exe -floatL 5.4 .5 .4 7 360 -strL yup "this" is_a 'string list' -intL 15 20 88`

Fun Fact: I forgot that you can use hexadecimal with integers. Like:

`exe --int-list 0xFF 15 0x15 --straight-up-int 0x00`


## NFAQ (Not so Frequently Asked Questions)

~Q: Can I do "[ 5, ]" for an integer list?\
A: For God's sake NO!~

Q: You got rid of that stupid list syntax? \
A: I absolutely did.

Q: How are you able to think of a solution that is soooo ugly?\
A: Natural talent.

Q: Will you create more ugly things like this?\
A: Absolutely YES!

Q: Has anyone ever saw your  codes and gave their opinion?\
A: Absolutely NO!

Q: Why are you writing these docs and readmes that no one will read?\
A: One day, I was sleeping. But Jesus, what a sleep! Then a pigeon came and said "You shall write thy docs and readmes". That day I started to write them.

NQ: Please stop writing READMEs. \
A: Nah.

## TODO

Somehow I broke everything. I can't pass values to flags from CLI
