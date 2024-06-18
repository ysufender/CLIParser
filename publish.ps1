g++ -std=c++20 -static-libstdc++ -c CLIParser.cpp
ar rcs libCLIParser.a CLIParser.o

if (-not (Test-Path -Path build/Release)) { mkdir build/Release }
if (-not (Test-Path -Path test/)) { mkdir test }

Copy-Item -Force -Path libCLIParser.a -Destination test/
Copy-Item -Force -Path CLIParser.hpp -Destination test/
Move-Item -Force -Path libCLIParser.a -Destination build/Release/
Copy-Item -Force -Path CLIParser.hpp -Destination build/Release/
Remove-Item -Force -Path CLIParser.o
