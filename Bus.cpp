#include "Bus.h"
#include "Rom.h"
#include "MC6821.h"


Bus::Bus()
{
	// load the cartridges
	roms.push_back(std::make_shared<Rom>("Apple1_HexMonitor.rom", 0xFF00));
	roms.push_back(std::make_shared<Rom>("Apple1_basic.rom", 0xE000));

	// set Reset Vector
	ram[0xFFFC] = 0x00;
	ram[0xFFFD] = 0xFF;

	// Connect CPU to communication bus
	cpu.ConnectBus(this);

	// Clear RAM contents, just in case :P
	for (auto &i : ram) i = 0x00;
}


Bus::~Bus()
{
}

void Bus::reset()
{
	cpu.reset();
	nSystemClockCounter = 0;
}

void Bus::clock()
{
	cpu.clock();

	nSystemClockCounter++;
}

void Bus::write(uint16_t addr, uint8_t data)
{
	for (auto r : roms)
	{
		if(r->cpuWrite(addr, data))
			return;
	}

	if (addr >= 0xD010 && addr <= 0xD001F)
	{
		pia.cpuWrite(addr, data);
	}
	else if (addr >= 0x0000 && addr <= 0xFFFF)
	{
		ram[addr] = data;
	}
}

uint8_t Bus::read(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;

	for (auto r : roms)
	{
		if (r->cpuRead(addr, data))
			return data;
	}

	if (addr >= 0xD010 && addr <= 0xD001F)
	{
		data = pia.cpuRead(addr);
	}
	else if (addr >= 0x0000 && addr <= 0xFFFF)
	{
		data = ram[addr];
	}

	return data;
}
