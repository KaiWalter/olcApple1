#include <iostream>
#include <sstream>

#include "Bus.h"
#include "Rom.h"
#include "olc6502.h"
#include "Apple1Terminal.h"
#include "Apple1Keyboard.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


/*

Reference material:
http://www.myapplecomputer.net/apple-1-specs.html
http://www.applefritter.com/book/export/html/22
Apple 1 HEXROM DISASSEMBLY: https://gist.github.com/robey/1bb6a99cd19e95c81979b1828ad70612
*/


class Apple1 : public olc::PixelGameEngine
{
public:
	Bus* a1bus;
	Apple1Terminal* a1term;
	Apple1Keyboard* a1kbd;

private:
	std::map<uint16_t, std::string> mapAsm;
	bool runEmulator = true;

public:
	Apple1()
	{
		sAppName = "Apple 1 Emulator";

		a1bus = new Bus();
		a1term = new Apple1Terminal(&a1bus->pia);
		a1kbd = new Apple1Keyboard(&a1bus->pia, this);

		// extract dissassembly
		mapAsm = a1bus->cpu.disassemble(0xE000, 0xFFFF);
	}

private:
	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	std::string bin(uint8_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 1)
			s[i] = "01"[n & 0x1];
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
				sOffset += " " + hex(a1bus->read(nAddr, true), 2);
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
		DrawString(x + 64, y, "N", a1bus->cpu.status & olc6502::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", a1bus->cpu.status & olc6502::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", a1bus->cpu.status & olc6502::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", a1bus->cpu.status & olc6502::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", a1bus->cpu.status & olc6502::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", a1bus->cpu.status & olc6502::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", a1bus->cpu.status & olc6502::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", a1bus->cpu.status & olc6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(a1bus->cpu.pc, 4));
		DrawString(x, y + 20, "A: $" + hex(a1bus->cpu.a, 2) + "  [" + std::to_string(a1bus->cpu.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(a1bus->cpu.x, 2) + "  [" + std::to_string(a1bus->cpu.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(a1bus->cpu.y, 2) + "  [" + std::to_string(a1bus->cpu.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(a1bus->cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(a1bus->cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a == mapAsm.end())
					break;
				else
					DrawString(x, nLineY, (*it_a).second);
			}
		}

		it_a = mapAsm.find(a1bus->cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.begin())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a == mapAsm.begin())
					break;
				else
					DrawString(x, nLineY, (*it_a).second);
			}
		}
	}

	bool OnUserCreate()
	{
		SystemReset();

		return true;
	}


	void SystemReset()
	{
		// Reset
		a1bus->cpu.reset();

		a1term->ClearScreen();
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		// process cpu cycle
		if (runEmulator)
		{
			do
			{
				a1bus->cpu.clock();
			} while (!a1bus->cpu.complete());
		}

		// check for emulator keys pressed
		if (GetKey(olc::Key::F2).bPressed)
		{
			do
			{
				a1bus->cpu.clock();
			} while (!a1bus->cpu.complete());
		}
		else if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			SystemReset();
		}
		else if (GetKey(olc::Key::F5).bPressed)
		{
			runEmulator = !runEmulator;
		}
		else
		{
			// check for Apple1 Keyboard
			a1kbd->ProcessKey();
		}

		DrawCpu(40 * 8 + 10, 2);
		DrawCode(40 * 8 + 10, 72, 26);

		DrawString(10, 370, "ESC = RESET  F5 = run or single step  F2 = step");

		DrawSprite(0, 72, a1term->getScreenSprite());

		return true;
	}
};

/*
	Initialize and startup the actual emulation
*/
int main()
{
	auto* demo = new Apple1();

	demo->Construct(600, 400, 2, 2);

	demo->Start();

	delete demo;

	return 0;
}