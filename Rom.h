#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

class Rom
{
public:
	Rom(const std::string& sFileName, uint16_t iOffset);
	~Rom();

public:
	bool ImageValid();

	bool cpuRead(uint16_t addr, uint8_t& data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	uint16_t Low();
	uint16_t High();

private:
	bool bImageValid = false;

	std::vector<uint8_t> vMemory;
	uint16_t iOffset;
	uint16_t iSize;
};