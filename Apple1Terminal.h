#pragma once
#include <queue>

#include "MC6821.h"

class Apple1Terminal
{
public:
	Apple1Terminal(std::shared_ptr<MC6821> pia);
	~Apple1Terminal();
	void ClearScreen();
	bool ProcessOutput();
	olc::Sprite* getScreenSprite();

	static uint16_t Width();
	static uint16_t Height();

private:
	const static uint8_t nRows = 24;
	const static uint8_t nCols = 40;
	const static uint8_t nCharHeight = 8;
	const static uint8_t nCharWidth = 8;
	uint8_t cScreenBuffer[nRows * nCols];
	uint8_t cCharacterRom[256][8];
	uint8_t cCharacterRomInverted[256][8];
	uint8_t nCursorY;
	uint8_t nCursorX;
	std::queue<uint8_t> displayQueue;

	olc::Sprite sprScreen = olc::Sprite(nCols * nCharWidth, nRows * nCharHeight);

	void ReceiveOutput(uint8_t dsp);
	void LoadCharacterRom(const std::string& sFileName, uint8_t(&rom)[256][8], bool bInvert = false);
	void RenderCharacter(uint8_t x, uint8_t y, uint8_t c[]);
};

