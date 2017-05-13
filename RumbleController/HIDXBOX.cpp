#include "HIDXBOX.h"
#define INPUT_DEADZONE  ( 0.5f * FLOAT(0x7FFF) )

HIDXBOX::HIDXBOX()
{
}

HIDXBOX::~HIDXBOX()
{
}
//Xbox.dwResult

bool HIDXBOX::leeMando(){
	//Xbox.OffState = Xbox.State;
	Xbox.dwResult = XInputGetState(0,&Xbox.State);
	if (Xbox.dwResult == ERROR_SUCCESS)return true;
	else return false;
}
void HIDXBOX::escribeMando(){
	Xbox.vibration.wLeftMotorSpeed = fLeftMotor* 65535;
	Xbox.vibration.wRightMotorSpeed = fRightMotor* 65535;
	XInputSetState(0, &Xbox.vibration);
}
void HIDXBOX::calibra(){
	bool a;
	if (true) a = true;
}
void HIDXBOX::mando2HID(){
	wbuttons = Xbox.State.Gamepad.wButtons;
	//Trigger derecho e izquierdo
	fLeftTrigger = (float) Xbox.State.Gamepad.bLeftTrigger / (float)MAXBYTE;
	fRightTrigger = (float)Xbox.State.Gamepad.bRightTrigger / (float)MAXBYTE;

//	if (g_bDeadZoneOn)
//	{
		// Zero value if thumbsticks are within the dead zone 
		if ((Xbox.State.Gamepad.sThumbLX < INPUT_DEADZONE &&
			Xbox.State.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
			(Xbox.State.Gamepad.sThumbLY < INPUT_DEADZONE &&
			Xbox.State.Gamepad.sThumbLY > -INPUT_DEADZONE))
		{
			Xbox.State.Gamepad.sThumbLX = 0;
			Xbox.State.Gamepad.sThumbLY = 0;
		}

		if ((Xbox.State.Gamepad.sThumbRX < INPUT_DEADZONE &&
			Xbox.State.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
			(Xbox.State.Gamepad.sThumbRY < INPUT_DEADZONE &&
			Xbox.State.Gamepad.sThumbRY > -INPUT_DEADZONE))
		{
			Xbox.State.Gamepad.sThumbRX = 0;
			Xbox.State.Gamepad.sThumbRY = 0;
		}
//	}

	//Joystick izquierdo
		fThumbLX = ((float)Xbox.State.Gamepad.sThumbLX / (float)MAXBYTE)/100;
		fThumbLY = ((float)Xbox.State.Gamepad.sThumbLY / (float)MAXBYTE) / 100;

	//Joystick derecho
		fThumbRX = ((float)Xbox.State.Gamepad.sThumbRX / (float)MAXBYTE) / 100;
		fThumbRY = ((float)Xbox.State.Gamepad.sThumbRY / (float)MAXBYTE) / 100;

}