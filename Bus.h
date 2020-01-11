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

	std::shared_ptr<Rom> rom;

	std::array<uint8_t, 64 * 1024> ram;


public: // Bus Read & Write
	void write(uint16_t addr, uint8_t data);
	uint8_t read(uint16_t addr, bool bReadOnly = false);

private:
	// A count of how many clocks have passed
	uint32_t nSystemClockCounter = 0;

public: // System Interface
	// Connects a Rom object to the internal buses
	void insertRom(const std::shared_ptr<Rom>& rom);
	// Resets the system
	void reset();
	// Clocks the system - a single whole systme tick
	void clock();

};

