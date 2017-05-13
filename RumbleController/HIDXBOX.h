#include <Windows.h>
#include <XInput.h>

#pragma once
#include "HID.h"
#define INPUT_DEADZONE  ( 0.5f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.


struct CONTROLER_STATE1
{
	XINPUT_STATE State;
	XINPUT_STATE OffState; // Estado para calibrar señales analogicas
	
	DWORD dwResult;
	XINPUT_VIBRATION vibration;
};
class HIDXBOX :
	public HID
{
public:
	HIDXBOX();
	~HIDXBOX();

	virtual bool leeMando();
	virtual void escribeMando();
	virtual void calibra();
	virtual void mando2HID();
private:
	CONTROLER_STATE1 Xbox;
};

