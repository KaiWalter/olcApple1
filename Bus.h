#pragma once
#include <cstdint>
#include <array>

#include "olc6502.h"
#include "MC6821.h"
#include "Rom.h"

class Bus
{
public:
	Bus();
	~Bus();

public: // Devices on bus
	olc6502 cpu;
	MC6821 pia;

	std::list<std::shared_ptr<Rom>> roms;

	std::array<uint8_t, 64 * 1024> ram;


public: // Bus Read & Write
	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);
	uint16_t RomLow();
	uint16_t RomHigh();

private:
	// A count of how many clocks have passed
	uint32_t nSystemClockCounter = 0;

public: // System Interface
	// Resets the system
	void reset();
	// Clocks the system - a single whole systme tick
	void clock();

};

