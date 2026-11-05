#ifndef MEMORY_H
#define MEMORY_H
#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Memory
{
private:
	vector<unsigned char> cells;
public:
	Memory();
	unsigned char read(unsigned short address) const;
	void write(unsigned short address, unsigned char value);
	void clear();

	bool loadFromFile(const string& filename, unsigned short startAddress);
	void displayMemoryMatrix() const;
	void displayScreenOutput() const;
	bool modifyCellManually(unsigned short address, unsigned char newValue);
};
#endif
