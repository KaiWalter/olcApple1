#pragma once
#include <cstdint>
#include <memory>

#include "olcPixelGameEngine.h"

class MC6821
{
public:
	MC6821();
	~MC6821();

public:
	// Communications with Main Bus
	uint8_t cpuRead(uint16_t addr, bool rdonly = false);
	void    cpuWrite(uint16_t addr, uint8_t  data);

};

