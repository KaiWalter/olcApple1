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
		auto nSize = std::filesystem::file_size(sFileName);
		if (nSize > 0xffff)
			nSize = 0xffff;

		// if 64k ROM skip to offset 
		if (nSize == 0xffff)
		{
			nSize -= iOffset;
			ifs.seekg(iOffset);
		}

		// if ROM flows into the vector table, reduce
		if ((nSize + iOffset) > 0xFFFA)
		{
			nSize -= 6;
		}

		vMemory.resize(nSize);
		ifs.read((char*)vMemory.data(), vMemory.size());

		this->iOffset = iOffset;
		this->iSize = nSize;
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
	if (addr >= iOffset)
	{
		uint32_t mapped_addr = addr - iOffset;
		if (mapped_addr >= 0 and mapped_addr < iSize)
		{
			data = vMemory[mapped_addr];
			return true;
		}
	}

	return false;
}

bool Rom::cpuWrite(uint16_t addr, uint8_t data)
{
	if (addr >= iOffset)
	{
		uint32_t mapped_addr = addr - iOffset;
		if (mapped_addr >= 0 and mapped_addr < iSize)
		{
			vMemory[mapped_addr] = data;
			return true;
		}
	}

	return false;
}

uint16_t Rom::Low()
{
	return iOffset;
}

uint16_t Rom::High()
{
	return iOffset + iSize - 1;
}
