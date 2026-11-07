#include "ALU.h"
#include <iostream>
#include <vector>
#include <cctype>
#include <cmath>
using namespace std;
bool ALU::isValidHexInstruction(const string& instruction) {
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
Decoder ALU::decode(const string& instruction) {
	Decoder decoder;
	if (!isValidHexInstruction(instruction)) {
		decoder.isValid = false;
		return decoder;
	}
	decoder.opcode = stoi(instruction.substr(0, 1), nullptr, 16);
	decoder.R = stoi(instruction.substr(1, 1), nullptr, 16);
	decoder.S = stoi(instruction.substr(2, 1), nullptr, 16);
	decoder.T = stoi(instruction.substr(3, 1), nullptr, 16);
	decoder.xy = stoi(instruction.substr(2, 2), nullptr, 16);
	decoder.isValid = true;
	return decoder;
}

void ALU::execute(const Decoder& dec, vector <uint8_t>& registers, int& pc) {
	if (!dec.isValid) {
		return;
	}
	switch (dec.opcode) {
		case 1: {
			// يتم التعامل مع قراءة الذاكرة في MainWindow أو CPU
			break;
		}
		case 2: {
			// Load Immediate: R[dec.R] = dec.xy
			registers[dec.R] = static_cast<uint8_t>(dec.xy);
			break;
		}
		case 3: {
			// Store in Memory
			break;
		}
		case 4: {
			// Move: R[dec.T] = R[dec.S]
			registers[dec.T] = registers[dec.S];
			break;
		}
		case 5: {
			int8_t val1 = static_cast<int8_t>(registers[dec.S]);
			int8_t val2 = static_cast<int8_t>(registers[dec.T]);
			int8_t sum = val1 + val2;
			registers[dec.R] = static_cast<uint8_t>(sum);
			break;
		}
		case 6: {
			auto toDouble = [](uint8_t val) -> double {

				int sign = (val >> 7) & 1;
				int exponent = (val >> 4) & 0x07;
				int mantissa = val & 0x0F;

				if (val == 0) return 0.0;
				double mantissaval = mantissa / 16.0;
				double value = mantissaval * pow(2, exponent - 4);
				return (sign == 1) ? -value : value;
				
			};

			auto toVoleFloat = [](double val) -> uint8_t {
				if (val == 0.0) return 0;
				uint8_t sign = (val < 0) ? 1 : 0;
				val = abs(val);
				int exp = 0;
				double mantissa = val;
				while (mantissa >= 1.0 && exp < 7) {
					mantissa /= 2.0;
					exp++;
				}
				while (mantissa < 0.5 && exp > -4) {
					mantissa *= 2.0;
					exp--;
				}
				int expbits = exp + 4;
				if (expbits < 0) expbits = 0;

				if (expbits > 7) expbits = 7;
				int mantbits = static_cast<int>(mantissa * 16.0 + 0.5);

				if (mantbits > 15) mantbits = 15;
					return (sign << 7) | ((expbits & 0x07) << 4) | (mantbits & 0x0F);
			};
			double v1 = toDouble(registers[dec.S]);
			double v2 = toDouble(registers[dec.T]);


			registers[dec.R] = toVoleFloat(v1 + v2);
			break;
		}	
		case 7: {
			registers[dec.R] = registers[dec.S] | registers[dec.T];
			break;
		}
		case 8: {
			registers[dec.R] = registers[dec.S] & registers[dec.T];
			break;
		}
		case 9: {
			registers[dec.R] = registers[dec.S] ^ registers[dec.T];
			break;
		}
		case 0xA: {
			uint8_t val = registers[dec.R];
			int steps = dec.T % 8;
			registers[dec.R] = (val >> steps) | (val << (8 - steps));
			break;
		}
		case 0xB: {
			if (registers[dec.R] == registers[0]) {
				pc = dec.xy;
			}
			break;
		}
	}
}