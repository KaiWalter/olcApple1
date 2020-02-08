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
	Bus a1bus;
	MC6821 pia;
	Apple1Terminal* a1term;
	Apple1Keyboard* a1kbd;

private:
	std::shared_ptr<Rom> rom;
	std::map<uint16_t, std::string> mapAsm;
	std::map<olc::Key, uint8_t> mapKeys;

public:
	Apple1()
	{
		sAppName = "Apple 1 Emulator";

		// load the cartridge
		rom = std::make_shared<Rom>("Apple1_HexMonitor.rom", 0xFF00);
		if (!rom->ImageValid())
			return;

		a1bus.insertRom(rom);

		// configure PIA
		a1term = new Apple1Terminal(&a1bus.pia);
		a1kbd = new Apple1Keyboard();

		// set Reset Vector
		a1bus.ram[0xFFFC] = 0x00;
		a1bus.ram[0xFFFD] = 0xFF;

		// map keys
		mapKeys = MapOLCKeyToAppleKey();
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
				if (++it_a == mapAsm.end())
					break;
				else
					DrawString(x, nLineY, (*it_a).second);
			}
		}

		it_a = mapAsm.find(a1bus.cpu.pc);
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
		if (!rom)
			return false;

		// extract dissassembly
		mapAsm = a1bus.cpu.disassemble(0xF000, 0xFFFF);

		SystemReset();

		return true;
	}


	std::map<olc::Key, uint8_t> MapOLCKeyToAppleKey()
	{
		std::map<olc::Key, uint8_t> mapKey;

		mapKey[olc::Key::BACK] = 0x08;
		mapKey[olc::Key::ENTER] = 0x0D;
		mapKey[olc::Key::SPACE] = 0x20;
		mapKey[olc::Key::NP_MUL] = 0x2A;
		mapKey[olc::Key::NP_ADD] = 0x2B;
		mapKey[olc::Key::NP_SUB] = 0x2D;
		mapKey[olc::Key::NP_DECIMAL] = 0x2E;
		mapKey[olc::Key::NP_DIV] = 0x2F;

		mapKey[olc::Key::PERIOD] = 0x2E;

		mapKey[olc::Key::K0] = 0x30;
		mapKey[olc::Key::K1] = 0x31;
		mapKey[olc::Key::K2] = 0x32;
		mapKey[olc::Key::K3] = 0x33;
		mapKey[olc::Key::K4] = 0x34;
		mapKey[olc::Key::K5] = 0x35;
		mapKey[olc::Key::K6] = 0x36;
		mapKey[olc::Key::K7] = 0x37;
		mapKey[olc::Key::K8] = 0x38;
		mapKey[olc::Key::K9] = 0x39;

		mapKey[olc::Key::NP0] = 0x30;
		mapKey[olc::Key::NP1] = 0x31;
		mapKey[olc::Key::NP2] = 0x32;
		mapKey[olc::Key::NP3] = 0x33;
		mapKey[olc::Key::NP4] = 0x34;
		mapKey[olc::Key::NP5] = 0x35;
		mapKey[olc::Key::NP6] = 0x36;
		mapKey[olc::Key::NP7] = 0x37;
		mapKey[olc::Key::NP8] = 0x38;
		mapKey[olc::Key::NP9] = 0x39;

		mapKey[olc::Key::A] = 0x41;
		mapKey[olc::Key::B] = 0x42;
		mapKey[olc::Key::C] = 0x43;
		mapKey[olc::Key::D] = 0x44;
		mapKey[olc::Key::E] = 0x45;
		mapKey[olc::Key::F] = 0x46;
		mapKey[olc::Key::G] = 0x47;
		mapKey[olc::Key::H] = 0x48;
		mapKey[olc::Key::I] = 0x49;
		mapKey[olc::Key::J] = 0x4A;
		mapKey[olc::Key::K] = 0x4B;
		mapKey[olc::Key::L] = 0x4C;
		mapKey[olc::Key::M] = 0x4D;
		mapKey[olc::Key::N] = 0x4E;
		mapKey[olc::Key::O] = 0x4F;
		mapKey[olc::Key::P] = 0x50;
		mapKey[olc::Key::Q] = 0x51;
		mapKey[olc::Key::R] = 0x52;
		mapKey[olc::Key::S] = 0x53;
		mapKey[olc::Key::T] = 0x54;
		mapKey[olc::Key::U] = 0x55;
		mapKey[olc::Key::V] = 0x56;
		mapKey[olc::Key::W] = 0x57;
		mapKey[olc::Key::X] = 0x58;
		mapKey[olc::Key::Y] = 0x59;
		mapKey[olc::Key::Z] = 0x5A;

		return mapKey;
	}

	void SystemReset()
	{
		// Reset
		a1bus.cpu.reset();

		a1term->ClearScreen();
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::BLACK);

		do
		{
			a1bus.cpu.clock();
		} while (!a1bus.cpu.complete());

		// check for emulator keys pressed
		if (GetKey(olc::Key::F5).bPressed)
		{
			SystemReset();
		}

		// check for regular keys pressed
		for (const auto& k : mapKeys)
		{
			if (GetKey(k.first).bPressed)
			{
				if (k.second > 0 && k.second < 0x60)
				{
					a1bus.pia.setCA1(SignalProcessing::Signal::Fall); // bring keyboard strobe to low to force active transition
					a1bus.pia.setInputA(k.second | 0x80); // bit 7 is constantly set (+5V)
					a1bus.pia.setCA1(SignalProcessing::Signal::Rise); // send only pulse
					a1bus.pia.setCA1(SignalProcessing::Signal::Fall); // 20 micro secs are not worth emulating
					break;
				}

			}
		}

		DrawCpu(40 * 8 + 10, 2);
		DrawCode(40 * 8 + 10, 72, 26);

		DrawString(10, 370, "F5 = RESET");

		DrawSprite(0, 0, a1term->getScreenSprite());

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