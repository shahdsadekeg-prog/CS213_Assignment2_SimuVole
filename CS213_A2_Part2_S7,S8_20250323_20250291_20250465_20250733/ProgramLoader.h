#pragma once

#include <string>
#include "Memory.h"

using namespace std;

class ProgramLoader {
public:
    ProgramLoader();
    bool loadProgramFromFile( string filename, int startAddress, Memory& memory);

private:
    bool isValidHexInstruction(string& instruction);
    std::string cleanLine(string& line);
};