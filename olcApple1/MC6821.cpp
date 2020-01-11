#include "MC6821.h"

MC6821::MC6821()
{

}

MC6821::~MC6821()
{

}

uint8_t MC6821::cpuRead(uint16_t addr, bool rdonly)
{
	uint16_t reg = addr & 0x03;

	return 0;
}

void MC6821::cpuWrite(uint16_t addr, uint8_t data)
{
	uint16_t reg = addr & 0x03;

}

