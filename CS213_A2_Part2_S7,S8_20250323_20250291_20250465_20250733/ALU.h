#pragma once
#include <string>
#include <vector>
#include <cstdint>
using namespace std;
struct Decoder {
	int opcode;
	int R;
	int S;
	int T;
	int xy;
	bool isValid;
};
class ALU {
public:
	static Decoder decode(const string& instraction);
	static void execute(const Decoder& d, vector<uint8_t>& registers, int &pc);
	static bool isValidHexInstruction(const string& instruction);
}; 
