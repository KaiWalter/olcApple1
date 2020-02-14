#pragma once
#include "MC6821.h"
#include "olcPixelGameEngine.h"

class Apple1Keyboard
{
public:
	Apple1Keyboard(std::shared_ptr<MC6821> pia, std::shared_ptr<olc::PixelGameEngine> olc);
	~Apple1Keyboard();

	void ProcessKey();

private:
	std::map<olc::Key, uint8_t> mapKeys;
	std::map<olc::Key, uint8_t> mapShiftedKeys;
	std::map<olc::Key, uint8_t> MapOLCKeyToAppleKey();
	std::map<olc::Key, uint8_t> MapOLCShiftedKeyToAppleKey();
	std::shared_ptr<MC6821> pia;
	std::shared_ptr<olc::PixelGameEngine> olc;
};

