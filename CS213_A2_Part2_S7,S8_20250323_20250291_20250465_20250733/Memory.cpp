#include "Memory.h"
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

Memory::Memory() {
	cells.resize(256, 0x00);
}

void Memory::clear() {
	fill(cells.begin(), cells.end(), 0x00);
}

unsigned char Memory::read(unsigned short address)const {
	if (address >= 256)
	{
		cout << "Erorr: Address out of bounds!\n";
		return 0;
	}
	return cells[address];
}
void Memory::write(unsigned short address, unsigned char value) {

	if (address >= 256)
	{
		cout << "Erorr: Address out of bounds!\n";
		return;
	}
	cells[address] = value;

	if (address == 0xFF)

	{
		cout << "\n[Screen Output at 0xFF]: " << (int)value << "(Char: " << static_cast<char>(value) << "\n";
	}
}
bool Memory::modifyCellManually(unsigned short address, unsigned char newValue) {

	if (address >= 256)

	{
		cout << "Error: Invalid address for manual modification.\n";
		return false;
	}

	write(address, newValue);

	cout << "Success: Cell [0X" << hex << address << "]updated to 0x " << (int)newValue << ".\n";
	return true;

}
bool Memory::loadFromFile(const string& filename, unsigned short startAddress) {

	if (startAddress >= 256)

	{
		cout << "Error: Invalid start address.\n";
		return false;
	}

	ifstream file(filename);
	if (!file.is_open())

	{
		cout << "Error could not open file" << filename << "\n";
		return false;
	}
	string line;
	unsigned short currentAddress = startAddress;
	while (getline(file, line))

	{
		int start = 0;

		while (start < line.length() && (line[start] == ' ' || line[start] == '\t' || line[start] == '\r' || line[start] == '\n'))
		{
			start++;
		}
		if (start >= line.length() || line[start] == ';')

		{
			continue;
		}
		string instructionHex = line.substr(start, 4);

		if (instructionHex.length() < 4)

		{
			continue;

		}
		try
		{
			string byte1str = instructionHex.substr(0, 2);
			string byte2str = instructionHex.substr(2, 2);
			unsigned char byte1 = static_cast<unsigned char>(stoi(byte1str, nullptr, 16));
			unsigned char byte2 = static_cast<unsigned char>(stoi(byte2str, nullptr, 16));

			if (currentAddress + 1 >= 256)

			{

				cout << "Error: program exceeds memory size!\n";

				file.close();

				return false;

			}
			cells[currentAddress] = byte1;

			cells[currentAddress + 1] = byte2;

			currentAddress += 2;

		}
		catch (...)

		{
			cout << "Error Prasing instruction.\n";
		}
	}

	file.close();
	return true;
}