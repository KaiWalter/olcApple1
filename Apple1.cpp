#include <iostream>
#include <sstream>

#include "Bus.h"
#include "Rom.h"
#include "olc6502.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

/*

Reference material:
http://www.myapplecomputer.net/apple-1-specs.html
http://www.applefritter.com/book/export/html/22

*/


class Apple1 : public olc::PixelGameEngine
{
public:
	Apple1() { sAppName = "Apple 1 Emulator"; }

	Bus a1bus;
	std::shared_ptr<Rom> rom;
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(a1bus.read(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", a1bus.cpu.status & olc6502::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", a1bus.cpu.status & olc6502::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", a1bus.cpu.status & olc6502::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", a1bus.cpu.status & olc6502::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", a1bus.cpu.status & olc6502::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", a1bus.cpu.status & olc6502::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", a1bus.cpu.status & olc6502::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", a1bus.cpu.status & olc6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(a1bus.cpu.pc, 4));
		DrawString(x, y + 20, "A: $" + hex(a1bus.cpu.a, 2) + "  [" + std::to_string(a1bus.cpu.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(a1bus.cpu.x, 2) + "  [" + std::to_string(a1bus.cpu.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(a1bus.cpu.y, 2) + "  [" + std::to_string(a1bus.cpu.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(a1bus.cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(a1bus.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(a1bus.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		// Load the cartridge
		rom = std::make_shared<Rom>("Apple1_HexMonitor.rom", 0xFF00);
		if (!rom->ImageValid())
			return false;

		a1bus.insertRom(rom);

		// Set Reset Vector
		a1bus.ram[0xFFFC] = 0x00;
		a1bus.ram[0xFFFD] = 0xFF;

		// Extract dissassembly
		mapAsm = a1bus.cpu.disassemble(0xF000, 0xFFFF);

		// Reset
		a1bus.cpu.reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::BLACK);

		if (GetKey(olc::Key::SPACE).bPressed)
		{
			do
			{
				a1bus.cpu.clock();
			} while (!a1bus.cpu.complete());
		}

		if (GetKey(olc::Key::R).bPressed)
			a1bus.cpu.reset();

		if (GetKey(olc::Key::I).bPressed)
			a1bus.cpu.irq();

		if (GetKey(olc::Key::N).bPressed)
			a1bus.cpu.nmi();

		DrawRect(0, 0, 40 * 8, 24 * 8);
		DrawCpu(40 *8 + 10, 2);
		DrawCode(40 * 8 + 10, 72, 26);

		DrawString(10, 370, "SPACE = Step Instruction    R = RESET    I = IRQ    N = NMI");

		return true;
	}
};

/*
	Initialize and startup the actual emulation
*/
int main()
{
	auto *demo = new Apple1();

	demo->Construct(680, 480, 2, 2);

	demo->Start();

	delete demo;

	return 0;
}