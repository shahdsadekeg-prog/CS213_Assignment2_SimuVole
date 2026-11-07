#include "ProgramLoader.h"
#include <fstream>
#include <cctype>

using namespace std;

ProgramLoader::ProgramLoader() {
}
bool ProgramLoader::loadProgramFromFile(string filename, int startAddress, Memory& memory) {
    if (startAddress < 0 || startAddress >= 256 || startAddress % 2 != 0) {
        return false;
    }

    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line, cleaned;
    while (getline(file, line)) {
        cleaned = cleanLine(line);

        if (cleaned.empty()) {
            continue;
        }
        if (!isValidHexInstruction(cleaned)) {
            file.close();
            return false;
        }

        string B1str = cleaned.substr(0, 2);
        string B2str = cleaned.substr(2, 2);

        int B1 = stoi(B1str, nullptr, 16);
        int B2 = stoi(B2str, nullptr, 16);

        memory.write(startAddress, static_cast<unsigned char>(B1));
        memory.write(startAddress + 1, static_cast<unsigned char>(B2));

        startAddress += 2;
    }

    file.close();
    return true;
}

bool ProgramLoader::isValidHexInstruction(string& instruction) {
    if (instruction.length() != 4) {
        return false;
    }
    for (char c : instruction) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}

string ProgramLoader::cleanLine( string& line) {
    string str = line;
    size_t commentPos = str.find(';');

    if (commentPos != string::npos) {
        str = str.substr(0, commentPos);
    }

    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(" \t\r\n");
    str = str.substr(start, end - start + 1);

    return str;
}