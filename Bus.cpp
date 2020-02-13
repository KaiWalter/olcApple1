#include "Bus.h"
#include "Rom.h"
#include "MC6821.h"


Bus::Bus()
{
	cpu = std::make_shared<olc6502>();
	pia = std::make_shared<MC6821>();

	// Clear RAM contents, just in case :P
	for (auto& i : ram) i = 0x00;

	// load the cartridges & set Reset Vector
#ifdef TESTROM
	auto rom = std::make_shared<Rom>("6502_functional_test.bin", 0x0000);

	for (uintmax_t addr = 0x0000; addr <= 0xFFFF; addr++)
	{
		uint8_t data = 0x00;
		rom->cpuRead(addr, data);
		ram[addr] = data;
	}

	ram[0xFFFC] = 0x00;
	ram[0xFFFD] = 0x04;

#else
	roms.push_back(std::make_shared<Rom>("Apple1_HexMonitor.rom", 0xFF00));
	roms.push_back(std::make_shared<Rom>("Apple1_basic.rom", 0xE000));

	ram[0xFFFC] = 0x00;
	ram[0xFFFD] = 0xFF;
#endif

	// Connect CPU to communication bus
	cpu->ConnectBus(this);
}


Bus::~Bus()
{
}

void Bus::reset()
{
	cpu->reset();
	nSystemClockCounter = 0;
}

void Bus::clock()
{
	cpu->clock();
	nSystemClockCounter++;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
#ifdef TESTROM
#else
	for (auto r : roms)
	{
		if (r->cpuWrite(addr, data))
			return;
	}

	if (addr >= 0xD010 && addr <= 0xD01F)
	{
		pia->cpuWrite(addr, data);
	}
	else
#endif
		if (addr >= 0x0000 && addr <= 0xFFFF)
		{
			ram[addr] = data;
		}
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;

#ifdef TESTROM
#else
	for (auto r : roms)
	{
		if (r->cpuRead(addr, data))
			return data;
	}

	if (addr >= 0xD010 && addr <= 0xD01F)
	{
		data = pia->cpuRead(addr);
	}
	else
#endif
		if (addr >= 0x0000 && addr <= 0xFFFF)
		{
			data = ram[addr];
		}

	return data;
}

uint16_t Bus::RomLow()
{
	uint16_t result = 0xFFFF;

	for (auto r : roms)
	{
		if (r->Low() < result)
			result = r->Low();
	}

	return result;
}

uint16_t Bus::RomHigh()
{
	uint16_t result = 0x0000;

	for (auto r : roms)
	{
		if (r->High() > result)
			result = r->High();
	}

	return result;
}
