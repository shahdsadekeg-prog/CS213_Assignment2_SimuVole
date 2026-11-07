#ifndef CPU_H
#define CPU_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Memory;

class CPU {
private:
    vector<int> registers;
    int programcounter;
    string instructionregister;
    bool isHalted;

public:
    CPU();

    void setregister(int index, int value);
    int getregister(int index) const;
    void setpc(int address);
    int getpc() const;
    string getIR() const;


    bool fetch(Memory& memory);
    void decodeandexecute(Memory& memory);

    void run(Memory& memory);
    void stepbystep(Memory& memory);

    void print() const;
};

#endif