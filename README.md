# CLIParser

You need to pass cool things to the CLI but not too cool (that makes it casual I think)? It supports integers, floats, strings, bools and a couple of lists.


## Installation

Simply clone the repository by

`git clone https://github.com/The2ndSlimShady/CLIParser.git`

then you can either use

`./build.ps1` or `./build.sh`

and create an executable named `CLIParser.exe` at `build/Debug` to try it out by using `debug.cpp`

or you can do

`./publish.ps1` or `./publish.sh`

and create the static library `libCLIParser.a`.

Or you can just go and do `cmake blah blah blah -DCMAKE_BUILD_TYPE=DebugTest` to include `debug.cpp` and create an executable,
Or yet again `cmake blah blah blah -DCMAKE_BUILD_TYPE=whateveryouwantking` to just compile it to a static lib.


## Adding/Removing Flags

To use the CLIParser, we use the `CLIParser`. It's arguments are arguments, argument count and prefix used in cli.

```cpp
#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc, "--"};
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

```cpp
#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc, "--"};
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
	std::string s = flags.GetString("s");
	bool b = flags.GetBool("b");
	std::vector<int> = flags.GetIntList("il");
	std::vector<float> = flags.GetFloatList("fl");
	std::vector<string> = flags.GetStringList("sl");
}

```

Yeah that's all to it.


## Binding Flags

Sometimes we might want to use both `-h` and `--help` for help flag. But we want them both to have the same value depending on usage. So what do we do? Yeah we bind them.
Binding is a one way operation. You can bind a flag to another but can't (you can actually but I wouldn't suggest doing so) bind the binder one to the binded. Yeah.

```cpp
#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc, "--"};
	parser.AddFlag("help", FlagType::Bool);
	parser.BindFlag("h", "help");
}
```

Now, we binded the `--h` flag to `--help`. So whenever we do `exe --h`, the value of `--help` will be true.

```cpp
#include <iostream>

#include "CLIParser.hpp"

int main(int argc, char** args)
{
	CLIParser parser {args, argc, "--"};
	parser.AddFlag("help", FlagType::Bool);
	parser.BindFlag("h", "help");

	Flags flags = parser.Parse();
	bool help = flags.GetBool("help");

	std::cout <<  "This shall print one if help is needed: " << help << '\n';
}
```

Fun Fact: If you try to `flags.GetBool("h")` you'll always get `false`. That's because `h` is binded to `help` so actually there is no flag named `h`. That's all thanks.

Another Fun Fact: You can change the prefix used for binded flags. `CLIParser` has another constructor that takes an argument named `boundPrefix`.

```cpp

#include "CLIParser.hpp"

int main(int argc, char** args)
{
    CLIParser parser {args, argc, "--", "-"};   // now the bound flags will use `-` as prefix
    parser.AddFlag("help", FlagType::Bool);
    parser.BindFLag("h", "help");

    Flags flags = parser.Parse();
    bool help = flags.GetBool("help");
	std::cout <<  "This shall print one if help is needed: " << help << '\n';
}
```

Now the `-h` is bound to `--help` instead of `--h`. Pretty neat huh? (Just say yes)

  
## Passing Conventions

As for primitives:

`exe -b -str1 string -str2 "string 2" -str3 'string 3' -i 69 -f 42`

~As for lists:~

~`exe -floatL [5.4, 358, 6.5] -strL [str, "List", 'for you'] -intL [5, 4]`~

~Be aware that the square brackets must be attached to the first and last element. Also every element has to have the comma symbol right next to them with a space away from the next one.
So `[4,4,3]`, `[ List ]`, `[5,4 , 3]` will cause you trouble.~

Why did I make the lists like that in the first place? I don't even know myself. Now you can just do:

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
