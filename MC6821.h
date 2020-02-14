#pragma once
#include <cstdint>
#include <memory>

#include "olcPixelGameEngine.h"


namespace SignalProcessing
{
	enum Signal : uint8_t
	{
		Fall = 0,
		Rise = 1
	};

	enum InterruptSignal : uint8_t
	{
		NoSignal = 0,
		IRQ = 1,
		NMI = 2,
		BRK = 4
	};
}

using namespace SignalProcessing;

class MC6821
{
public:
	MC6821();
	~MC6821();

protected:
	uint8_t nORA; // Output register A
	uint8_t nIRA; // Input register A
	uint8_t nDDRA; // data direction register A                 (Output=1, Input=0)
	uint8_t nDDRA_neg; // negative data direction register A    (Output=0, Input=1)
	Signal nCA1; // control line A1
	Signal nCA2; // control line A2

	uint8_t nCRA; // control register A
	bool bCRA_Bit0_EnableIRQA1;
	bool bCRA_Bit1_CA1_PositiveTrans;
	bool bCRA_Bit2_WritePort;
	bool bCRA_Bit3_EnableIRQA2;
	bool bCRA_Bit3_PulseOutput;
	bool bCRA_Bit3_CA2_set_high;
	bool bCRA_Bit4_CA2_PositiveTrans;
	bool bCRA_Bit4_ManualOutput;
	bool bCRA_Bit5_OutputMode;

	uint8_t nORB; // Output register B
	uint8_t nIRB; // Input register B 
	uint8_t nDDRB; // data direction register B                 (Output=1, Input=0)
	uint8_t nDDRB_neg; // negative data direction register B    (Output=0, Input=1)
	Signal nCB1; // control line B1
	Signal nCB2; // control line B2

	uint8_t nCRB; // control register B
	bool bCRB_Bit0_EnableIRQB1;
	bool bCRB_Bit1_CB1_PositiveTrans;
	bool bCRB_Bit2_WritePort;
	bool bCRB_Bit3_EnableIRQB2;
	bool bCRB_Bit3_PulseOutput;
	bool bCRB_Bit3_CB2_set_high;
	bool bCRB_Bit4_CB2_PositiveTrans;
	bool bCRB_Bit4_ManualOutput;
	bool bCRB_Bit5_OutputMode;

	std::function<void(uint8_t)> fSendOutputA;
	std::function<void(uint8_t)> fSendOutputB;
	std::function<void(SignalProcessing::InterruptSignal)> fSendInterrupt;

public:
	// Communications with Main Bus
	uint8_t cpuRead(uint16_t addr, bool rdonly = false);
	void    cpuWrite(uint16_t addr, uint8_t  data);

	void setInputA(uint8_t b);
	void setInputB(uint8_t b);

	void setOutputAHandler(std::function<void(uint8_t)> h);
	void setOutputBHandler(std::function<void(uint8_t)> h);
	void setInterruptHandler(std::function<void(SignalProcessing::InterruptSignal)> h);

	void setCA1(Signal b);
	Signal getCA1();
	void setCA2(Signal b);
	Signal getCA2();

	void setCB1(Signal b);
	Signal getCB1();
	void setCB2(Signal b);
	Signal getCB2();

private:
	void updateControlRegisters();
	void updateIRQ();

};

