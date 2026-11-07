#include <iostream>
#include "CPU.h"
#include "Memory.h"
#include <iomanip>
#include <cmath>
#include <sstream>

using namespace std;

double decodeVoleFloat(int byteVal) {
    if (byteVal == 0) return 0.0;

    int sign = (byteVal >> 7) & 1;
    int exp = (byteVal >> 4) & 0x07;
    int mantissa = byteVal & 0x0F;

    double val = (mantissa / 16.0) * pow(2.0, exp - 4);
    return (sign == 1) ? -val : val;
}

int encodeVoleFloat(double val) {
    if (val == 0.0) return 0x00;

    int sign = (val < 0) ? 1 : 0;
    double absVal = fabs(val);

    int exp = 4;
    double mantissa = absVal;

    while (mantissa >= 1.0 && exp < 7) {
        mantissa /= 2.0;
        exp++;
    }
    while (mantissa < 0.5 && exp > 0) {
        mantissa *= 2.0;
        exp--;
    }
    int intMantissa = round(mantissa * 16.0);

    if (intMantissa >= 16) {
        intMantissa /= 2;
        exp++;
    }

    if (exp > 7) exp = 7;
    if (exp < 0) { exp = 0; intMantissa = 0; }

    return (sign << 7) | ((exp & 0x07) << 4) | (intMantissa & 0x0F);
}

CPU::CPU() : registers(16, 0), programcounter(0), instructionregister("0000"), isHalted(false) {}

void CPU::setregister(int index, int value) {
    if (index >= 0 && index < registers.size()) {
        registers[index] = value;
    }
}

int CPU::getregister(int index) const {
    if (index >= 0 && index < registers.size()) {
        return registers[index];
    }
    return 0;
}

void CPU::setpc(int address) {
    programcounter = address;
}

int CPU::getpc() const {
    return programcounter;
}

string CPU::getIR() const {
    return instructionregister;
}

bool CPU::fetch(Memory& memory) {
    if (isHalted || programcounter + 1 >= 256) return false;

    // استبدال getinstructionat بقراءة البايتين المباشرة من الذاكرة بـ read
    unsigned char b1 = memory.read(static_cast<unsigned short>(programcounter));
    unsigned char b2 = memory.read(static_cast<unsigned short>(programcounter + 1));

    stringstream ss;
    ss << hex << uppercase << setfill('0') << setw(2) << (int)b1 << setw(2) << (int)b2;
    string fetchedHexadec = ss.str();

    if (fetchedHexadec.length() == 4) {
        instructionregister = fetchedHexadec;
        programcounter += 2;
        return true;
    }
    return false;
}

void CPU::decodeandexecute(Memory& memory) {
    cout << "Executing IR: " << instructionregister << endl;
    char opcode = instructionregister[0];
    char regR = instructionregister[1];
    string addressOrVal = instructionregister.substr(2, 2);

    // Extract required indices and numeric values
    int rIndex = stoi(string(1, regR), nullptr, 16);
    int XY = stoi(addressOrVal, nullptr, 16);
    int sIndex = stoi(string(1, instructionregister[2]), nullptr, 16);
    int tIndex = stoi(string(1, instructionregister[3]), nullptr, 16);

    switch (opcode) {
    case '1':
        // استبدال memory.getValue بـ memory.read
        registers[rIndex] = memory.read(static_cast<unsigned short>(XY));
        break;

    case '2':
        registers[rIndex] = XY;
        break;

    case '3':
        // استبدال memory.setValue بـ memory.write
        memory.write(static_cast<unsigned short>(XY), static_cast<unsigned char>(registers[rIndex]));
        break;

    case '4':
        registers[tIndex] = registers[sIndex];
        break;

    case '5':
        registers[rIndex] = (registers[sIndex] + registers[tIndex]) & 0xFF;
        break;

    case '6': {
        double val1 = decodeVoleFloat(registers[sIndex]);
        double val2 = decodeVoleFloat(registers[tIndex]);
        double sum = val1 + val2;
        registers[rIndex] = encodeVoleFloat(sum);
        break;
    }

    case '7':
        registers[rIndex] = (registers[sIndex] | registers[tIndex]);
        break;

    case '8':
        registers[rIndex] = (registers[sIndex] & registers[tIndex]);
        break;

    case '9':
        registers[rIndex] = (registers[sIndex] ^ registers[tIndex]);
        break;

    case 'A':
    case 'a': {
        int steps = XY & 0x0F;
        int value = registers[rIndex] & 0xFF;
        steps = steps % 8;
        if (steps > 0) {
            registers[rIndex] = ((value >> steps) | (value << (8 - steps))) & 0xFF;
        }
        break;
    }

    case 'B':
    case 'b':
        if (registers[rIndex] == registers[0]) {
            programcounter = XY;
        }
        break;

    case 'C':
    case 'c':
        cout << "Halt instruction executed. Stopping CPU." << endl;
        isHalted = true;
        break;

    default:
        cout << "ERROR: Invalid instruction rejected (" << instructionregister << ")." << endl;
        isHalted = true;
        break;
    }
}

void CPU::run(Memory& memory) {
    cout << "Starting run at once" << endl;
    isHalted = false;

    while (!isHalted && fetch(memory)) {
        decodeandexecute(memory);
    }
}

void CPU::stepbystep(Memory& memory) {
    cout << "Starting step by step execution" << endl;
    isHalted = false;

    if (!isHalted && fetch(memory)) {
        decodeandexecute(memory);
        cout << "current state is PC: " << programcounter << " | IR: " << instructionregister << endl;
    }
}

void CPU::print() const {
    cout << "PC: " << programcounter << " | IR: " << instructionregister << endl;
    cout << "Registers (R0-RF): ";
    for (int i = 0; i < 16; i++) {
        cout << "R" << hex << uppercase << i << ":" << registers[i] << " ";
    }
    cout << dec << endl;
}