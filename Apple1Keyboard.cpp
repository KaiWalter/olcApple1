#include "Apple1Keyboard.h"

Apple1Keyboard::Apple1Keyboard(std::shared_ptr<MC6821> pia, std::shared_ptr<olc::PixelGameEngine> olc) :
	pia{ pia }, olc{ olc }
{
	// map keys
	mapKeys = MapOLCKeyToAppleKey();
	mapShiftedKeys = MapOLCShiftedKeyToAppleKey();
}

Apple1Keyboard::~Apple1Keyboard()
{
}

void Apple1Keyboard::ProcessKey()
{
	uint8_t keyPressed = 0;

	if (olc->GetKey(olc::SHIFT).bHeld)
	{
		// check for shifted keys pressed
		for (const auto& k : mapShiftedKeys)
		{
			if (olc->GetKey(k.first).bPressed)
			{
				keyPressed = k.second;
				break;
			}
		}
	}
	else
	{
		// check for regular keys pressed
		for (const auto& k : mapKeys)
		{
			if (olc->GetKey(k.first).bPressed)
			{
				keyPressed = k.second;
				break;
			}
		}
	}

	if (keyPressed)
	{
		if (keyPressed > 0 && keyPressed < 0x60)
		{
			pia->setCA1(SignalProcessing::Signal::Fall); // bring keyboard strobe to low to force active transition
			pia->setInputA(keyPressed | 0x80); // bit 7 is constantly set (+5V)
			pia->setCA1(SignalProcessing::Signal::Rise); // send only pulse
			pia->setCA1(SignalProcessing::Signal::Fall); // 20 micro secs are not worth emulating
		}
	}
}


std::map<olc::Key, uint8_t> Apple1Keyboard::MapOLCKeyToAppleKey()
{
	std::map<olc::Key, uint8_t> mapKey;

	mapKey[olc::Key::BACK] = 0x08;
	mapKey[olc::Key::ENTER] = 0x0D;
	mapKey[olc::Key::SPACE] = 0x20;
	mapKey[olc::Key::NP_MUL] = 0x2A;
	mapKey[olc::Key::NP_ADD] = 0x2B;
	mapKey[olc::Key::PLUS] = 0x2B;
	mapKey[olc::Key::NP_SUB] = 0x2D;
	mapKey[olc::Key::MINUS] = 0x2D;
	mapKey[olc::Key::NP_DECIMAL] = 0x2E;
	mapKey[olc::Key::NP_DIV] = 0x2F;

	mapKey[olc::Key::PERIOD] = 0x2E;
	mapKey[olc::Key::COMMA] = 0x2C;

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

std::map<olc::Key, uint8_t> Apple1Keyboard::MapOLCShiftedKeyToAppleKey()
{
	std::map<olc::Key, uint8_t> mapShiftedKey;

	// de-DE mapping
	mapShiftedKey[olc::Key::K0] = 0x3D;
	mapShiftedKey[olc::Key::K1] = 0x21;
	mapShiftedKey[olc::Key::K2] = 0x22;
	mapShiftedKey[olc::Key::K4] = 0x24;
	mapShiftedKey[olc::Key::K5] = 0x25;
	mapShiftedKey[olc::Key::K6] = 0x26;
	mapShiftedKey[olc::Key::K7] = 0x2F;
	mapShiftedKey[olc::Key::K8] = 0x28;
	mapShiftedKey[olc::Key::K9] = 0x29;

	mapShiftedKey[olc::Key::PLUS] = 0x2A;
	mapShiftedKey[olc::Key::PERIOD] = 0x3A;
	mapShiftedKey[olc::Key::COMMA] = 0x3B;
	mapShiftedKey[olc::Key::MINUS] = 0x5F;

	return mapShiftedKey;
}
