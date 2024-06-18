g++ -std=c++20 -static-libstdc++ -DDEBUG -o CLIParser.exe CLIParser.cpp

if (-not (Test-Path -Path build/)) { mkdir build }
if (-not (Test-Path -Path build/Debug)) { mkdir build/Debug }

Move-Item -Force -Path CLIParser.exe -Destination build/Debug/
Copy-Item -Force -Path CLIParser.hpp -Destination build/Debug/
