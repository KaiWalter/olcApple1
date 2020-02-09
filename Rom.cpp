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
	this->nOffset = 0;
	this->nSize = 0;

	// try to read ROM file
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		auto nSize = std::filesystem::file_size(sFileName);

		vMemory.resize(nSize);
		ifs.read((char*)vMemory.data(), vMemory.size());

		this->nOffset = iOffset;
		this->nSize = nSize;
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
	if (addr >= nOffset)
	{
		uint32_t mapped_addr = addr - nOffset;
		if (mapped_addr >= 0 and mapped_addr < nSize)
		{
			data = vMemory[mapped_addr];
			return true;
		}
	}

	return false;
}

bool Rom::cpuWrite(uint16_t addr, uint8_t data)
{
	if (addr >= nOffset)
	{
		uint32_t mapped_addr = addr - nOffset;
		if (mapped_addr >= 0 and mapped_addr < nSize)
		{
			vMemory[mapped_addr] = data;
			return true;
		}
	}

	return false;
}

uint16_t Rom::Low()
{
	return nOffset;
}

uint16_t Rom::High()
{
	return nOffset + nSize - 1;
}
