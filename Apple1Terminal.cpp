#include "Apple1Terminal.h"

/*
This implementation of the terminal is not (yet) signal or shift register compliant.
Reference material:
https://www.sbprojects.net/projects/apple1/terminal.php
https://www.sbprojects.net/projects/apple1/a-one-terminal.php
*/


Apple1Terminal::Apple1Terminal(std::shared_ptr<MC6821> pia)
{
	// load character ROMs
	LoadCharacterRom("Apple1_charmap.rom", cCharacterRom, false);
	LoadCharacterRom("Apple1_charmap.rom", cCharacterRomInverted, true);

	// wire up with PIA
	pia->setOutputBHandler([&](uint8_t dsp) {
		ReceiveOutput(dsp);
	});
}

Apple1Terminal::~Apple1Terminal()
{
}

void Apple1Terminal::ClearScreen()
{
	// Clear Screen
	for (int y = 0; y <= sprScreen.height; y++)
		for (int x = 0; x <= sprScreen.width; x++)
			sprScreen.SetPixel(x, y, olc::BLACK);

	for (auto& c : cScreenBuffer)
		c = ' ';

	nCursorY = nCursorX = 0;
}

void Apple1Terminal::ProcessOutput()
{
	if (displayQueue.empty())
		return;

	uint8_t dsp = displayQueue.front();

	// make lower case key upper
	if (dsp >= 0x61 && dsp <= 0x7A)
		dsp &= 0x5F;

	// clear old cursor
	RenderCharacter(nCursorX, nCursorY, cCharacterRom[cScreenBuffer[nCursorY * nCols + nCursorX]]);

	// display new character
	switch (dsp)
	{
	case 0x0D:
		nCursorX = 0;
		nCursorY++;
		break;
	default:
		if (dsp >= 0x20 && dsp <= 0x5F)
		{
			cScreenBuffer[nCursorY * nCols + nCursorX] = dsp;

			RenderCharacter(nCursorX, nCursorY, cCharacterRom[dsp]);

			nCursorX++;
		}
		break;
	}

	// check cursor position
	if (nCursorX == nCols)
	{
		nCursorX = 0;
		nCursorY++;
	}
	if (nCursorY == nRows)
	{
		// scroll up
		for (int y = 0; y < nRows - 1; y++)
			for (int x = 0; x < nCols; x++)
			{
				cScreenBuffer[y * nCols + x] = cScreenBuffer[(y + 1) * nCols + x];
				RenderCharacter(x, y, cCharacterRom[cScreenBuffer[y * nCols + x]]);
			}

		int y = (nRows - 1);
		for (int x = 0; x < nCols; x++)
		{
			cScreenBuffer[y * nCols + x] = ' ';
			RenderCharacter(x, y, cCharacterRom[cScreenBuffer[y * nCols + x]]);
		}

		nCursorY--;
	}

	// draw new cursor
	RenderCharacter(nCursorX, nCursorY, cCharacterRomInverted[cScreenBuffer[nCursorY * nCols + nCursorX]]);

	displayQueue.pop();
}

olc::Sprite* Apple1Terminal::getScreenSprite()
{
	return &sprScreen;
}

void Apple1Terminal::ReceiveOutput(uint8_t dsp)
{
	displayQueue.push(dsp);
}

void Apple1Terminal::LoadCharacterRom(const std::string& sFileName, uint8_t(&rom)[256][8], bool bInvert)
{
	std::ifstream ifs;
	std::vector<uint8_t> vMemory;

	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		vMemory.resize(std::filesystem::file_size(sFileName));
		ifs.read((char*)vMemory.data(), vMemory.size());

		ifs.close();
	}

	// feed into character map
	// flip/reverse bits from right-to-left to left-to-right
	uint8_t nCharIndex = 0;
	uint8_t nLineIndex = 0;

	for (int c = 0; c < vMemory.size(); c++)
	{
		uint8_t fromMask = 0x80;
		uint8_t toMask = 0x01;
		uint8_t bNew = 0;
		for (int bit = 0; bit < 8; bit++)
		{
			if ((vMemory[c] & fromMask) == fromMask) bNew |= toMask;
			fromMask >>= 1;
			toMask <<= 1;
		}

		if (bInvert)
			rom[nCharIndex][nLineIndex] = ~bNew;
		else
			rom[nCharIndex][nLineIndex] = bNew;

		nLineIndex++;
		if (nLineIndex == 8)
		{
			nLineIndex = 0;
			nCharIndex++;
		}
	}
}

void Apple1Terminal::RenderCharacter(uint8_t x, uint8_t y, uint8_t c[])
{
	int32_t scanline = y * nCharHeight;
	int32_t linepos = x * nCharWidth;

	for (int r = 0; r < nCharHeight; r++)
	{
		uint8_t mask = c[r];
		for (int c = nCharWidth; c >= 0; c--, mask >>= 1)
		{
			if ((mask & 1) == 1)
			{
				sprScreen.SetPixel(linepos + c, scanline + r, olc::DARK_GREEN);
			}
			else
			{
				sprScreen.SetPixel(linepos + c, scanline + r, olc::BLACK);
			}
		}
	}
}

