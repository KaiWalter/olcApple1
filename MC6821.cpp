
#include "MC6821.h"

/*
This is a more or less signal accurate emulation of the MC6821 as used in the Apple1.
I use bCRx_Bit bool flags instead of a bit structure to allow for a better understanding
of the signal flow.
*/

MC6821::MC6821()
{
	nIRA = 0xFF;
	nIRB = 0;

	nCA1 = nCA2 = SignalProcessing::Signal::Rise;

	nCRA = nCRB = nORA = nORB = 0;

	nDDRA = nDDRB = 0;
	nDDRA_neg = nDDRB_neg = 0xFF;

	fSendInterrupt = NULL;
	fSendOutputA = fSendOutputB = NULL;

	updateControlRegisters();
}

MC6821::~MC6821()
{

}

void MC6821::updateControlRegisters()
{
	// section A -----------------------------------------
	bCRA_Bit0_EnableIRQA1 = (nCRA & 0x01) == 0x01;
	bCRA_Bit1_CA1_PositiveTrans = (nCRA & 0x02) == 0x02;
	bCRA_Bit2_WritePort = (nCRA & 0x04) == 0x04;
	bCRA_Bit5_OutputMode = (nCRA & 0x20) == 0x20;

	bCRA_Bit3_EnableIRQA2 = false;
	bCRA_Bit3_PulseOutput = false;
	bCRA_Bit3_CA2_set_high = false;
	bCRA_Bit4_CA2_PositiveTrans = false;
	bCRA_Bit4_ManualOutput = false;

	if (bCRA_Bit5_OutputMode)
	{
		bCRA_Bit4_ManualOutput = (nCRA & 0x10) == 0x10;
		if (bCRA_Bit4_ManualOutput)
		{
			bCRA_Bit3_CA2_set_high = (nCRA & 0x08) == 0x08;
			nCA2 = bCRA_Bit3_CA2_set_high ? Signal::Rise : Signal::Fall;
		}
		else
			bCRA_Bit3_PulseOutput = (nCRA & 0x08) == 0x08;
	}
	else
	{
		bCRA_Bit3_EnableIRQA2 = (nCRA & 0x08) == 0x08;
		bCRA_Bit4_CA2_PositiveTrans = (nCRA & 0x10) == 0x10;
	}

	// section B -----------------------------------------
	bCRB_Bit0_EnableIRQB1 = (nCRB & 0x01) == 0x01;
	bCRB_Bit1_CB1_PositiveTrans = (nCRB & 0x02) == 0x02;
	bCRB_Bit2_WritePort = (nCRB & 0x04) == 0x04;
	bCRB_Bit5_OutputMode = (nCRB & 0x20) == 0x20;

	bCRB_Bit3_EnableIRQB2 = false;
	bCRB_Bit3_PulseOutput = false;
	bCRB_Bit3_CB2_set_high = false;
	bCRB_Bit4_CB2_PositiveTrans = false;
	bCRB_Bit4_ManualOutput = false;

	if (bCRB_Bit5_OutputMode)
	{
		bCRB_Bit4_ManualOutput = (nCRB & 0x10) == 0x10;
		if (bCRB_Bit4_ManualOutput)
		{
			bCRB_Bit3_CB2_set_high = (nCRB & 0x08) == 0x08;
			nCB2 = bCRB_Bit3_CB2_set_high ? Signal::Rise : Signal::Fall;
		}
		else
			bCRB_Bit3_PulseOutput = (nCRB & 0x08) == 0x08;
	}
	else
	{
		bCRB_Bit3_EnableIRQB2 = (nCRB & 0x08) == 0x08;
		bCRB_Bit4_CB2_PositiveTrans = (nCRB & 0x10) == 0x10;
	}
}

void MC6821::updateIRQ()
{
	if (fSendInterrupt &&
		(
		(bCRA_Bit0_EnableIRQA1 && (nCRA & 0x80) == 0x80) ||
			(bCRA_Bit3_EnableIRQA2 && (nCRA & 0x40) == 0x40) ||
			(bCRB_Bit0_EnableIRQB1 && (nCRB & 0x80) == 0x80) ||
			(bCRB_Bit3_EnableIRQB2 && (nCRB & 0x40) == 0x40)
			)
		)
		fSendInterrupt(InterruptSignal::IRQ);
}

uint8_t MC6821::cpuRead(uint16_t addr, bool rdonly)
{
	uint16_t reg = addr & 0x03;

	uint8_t data = 0;

	switch (reg)
	{
	case 0: // PA

		nCRA &= 0x3F;  // IRQ flags implicitly cleared by a read

		// mix input and output
		data |= nORA & nDDRA;
		data |= nIRA & nDDRA_neg;

		break;

	case 1: // CRA
		data = nCRA;
		break;

	case 2: // PB

		nCRB &= 0x3F; // IRQ flags implicitly cleared by a read

		// mix input and output
		data |= nORB & nDDRB;
		data |= nIRB & nDDRB_neg;

		break;

	case 3: // CRB
		data = nCRB;
		break;
	}

	return data;
}

void MC6821::cpuWrite(uint16_t addr, uint8_t data)
{
	uint16_t reg = addr & 0x03;

	switch (reg)
	{
	case 0: // DDRA / PA
		if (bCRA_Bit2_WritePort)
		{
			nORA = data; // into output register A
			if (fSendOutputA)
			{
				// mix input and output
				uint8_t bOut = 0;
				bOut |= nORA & nDDRA;
				bOut |= nIRA & nDDRA_neg;
				fSendOutputA(bOut);
			}
		}
		else
		{
			nDDRA = data; // into data direction register A
			nDDRA_neg = (uint8_t)~data;
		}
		break;

	case 1: // CRA
		nCRA = (nCRA & 0xC0) | (data & 0x3F); // do not change IRQ flags
		updateControlRegisters();
		updateIRQ();
		break;

	case 2: // DDRB / PB
		if (bCRB_Bit2_WritePort)
		{
			nORB = data; // into output register B
			if (fSendOutputB)
			{
				// mix input and output
				uint8_t bOut = 0;
				bOut |= nORB & nDDRB;
				bOut |= nIRB & nDDRB_neg;
				fSendOutputB(bOut);

				if (bCRB_Bit5_OutputMode && !bCRB_Bit4_ManualOutput) // handshake on write mode
				{
					nCB2 = Signal::Fall;
					if (bCRB_Bit3_PulseOutput) nCB2 = Signal::Rise;
				}
			}
		}
		else
		{
			nDDRB = data; // into data direction register B
			nDDRB_neg = ~data;
		}
		break;

	case 3: // CRB
		nCRB = (nCRB & 0xC0) | (data & 0x3F); // do not change IRQ flags
		updateControlRegisters();
		updateIRQ();
		break;
	}
}

void MC6821::setInputA(uint8_t b)
{
	nIRA = b;
}

void MC6821::setInputB(uint8_t b)
{
	nIRB = b;

}

void MC6821::setOutputAHandler(std::function<void(uint8_t)> h)
{
	fSendOutputA = h;
}

void MC6821::setOutputBHandler(std::function<void(uint8_t)> h)
{
	fSendOutputB = h;
}

void MC6821::setInterruptHandler(std::function<void(SignalProcessing::InterruptSignal)> h)
{
	fSendInterrupt = h;
}

void MC6821::setCA1(Signal b)
{
	// flag interrupt 
	if (nCA1 != b && (bCRA_Bit1_CA1_PositiveTrans ? Signal::Rise : Signal::Fall) == b)
	{
		nCRA |= 0x80; // set bit 7 IRQA1
		updateIRQ();
		if (bCRA_Bit5_OutputMode && !bCRA_Bit4_ManualOutput && !bCRA_Bit3_PulseOutput) // handshake mode
			nCA2 = Signal::Rise;
	}
	nCA1 = b;
}

Signal MC6821::getCA1()
{
	return nCA1;
}

void MC6821::setCA2(Signal b)
{
	if (nCA2 != b && (bCRA_Bit4_CA2_PositiveTrans ? Signal::Rise : Signal::Fall) == b)
	{
		nCRA |= 0x40; // set bit 6 IRQA2
		updateIRQ();
	}
	nCA2 = b;
}

Signal MC6821::getCA2()
{
	return nCA2;
}

void MC6821::setCB1(Signal b)
{
	if (nCB1 != b && (bCRB_Bit1_CB1_PositiveTrans ? Signal::Rise : Signal::Fall) == b)
	{
		nCRB |= 0x80; // set bit 7 IRQB1
		updateIRQ();
		if (bCRB_Bit5_OutputMode && !bCRB_Bit4_ManualOutput && !bCRB_Bit3_PulseOutput) // handshake mode
			nCB2 = Signal::Rise;
	}
	nCB1 = b;
}

Signal MC6821::getCB1()
{
	return nCB1;
}

void MC6821::setCB2(Signal b)
{
	if (nCB2 != b && (bCRB_Bit4_CB2_PositiveTrans ? Signal::Rise : Signal::Fall) == b)
	{
		nCRB |= 0x40; // set bit 6 IRQB2
		updateIRQ();
	}
	nCB2 = b;
}

Signal MC6821::getCB2()
{
	return nCB2;
}
