#include "Rom.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>

Rom::Rom(const std::string& sFileName, uint16_t iOffset)
{
	std::ifstream ifs;
	
	// init
	this->bImageValid = false;
	this->iOffset = 0;
	this->iSize = 0;

	// try to read ROM file
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		this->iSize = std::filesystem::file_size(sFileName);
		vMemory.resize(this->iSize);
		ifs.read((char*)vMemory.data(), vMemory.size());

		this->iOffset = iOffset;
		this->bImageValid = true;

		ifs.close();
	}
}

Rom::~Rom()
{
}

bool Rom::ImageValid()
{
	return bImageValid;
}

bool Rom::cpuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = addr - iOffset;
	if (mapped_addr >= 0 and mapped_addr <= iSize)
	{
		data = vMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Rom::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = addr - iOffset;
	if (mapped_addr >= 0 and mapped_addr <= iSize)
	{
		vMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}
