#include "HIDBOX.h"
#define INPUT_DEADZONE  ( 0.5f * FLOAT(0x7FFF) )

HIDBOX::HIDBOX()
{
}

HIDBOX::~HIDBOX()
{
}
//Xbox.dwResult

bool HIDBOX::leeMando(){
	//Xbox.OffState = Xbox.State;
	Xbox.dwResult = XInputGetState(0,&Xbox.State);
	if (Xbox.dwResult == ERROR_SUCCESS)return true;
	else return false;
}
void HIDBOX::escribeMando(){
	bool a;
	if (true) a = true;
	//Xbox.vibration.wLeftMotorSpeed = (WORD)
}
void HIDBOX::calibra(){
	bool a;
	if (true) a = true;
}
void HIDBOX::mando2HID(){
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
		fThumbLX = (float)Xbox.State.Gamepad.sThumbLX / (float)MAXBYTE;
		fThumbLY = (float)Xbox.State.Gamepad.sThumbLY / (float)MAXBYTE;

	//Joystick derecho
		fThumbRX = (float)Xbox.State.Gamepad.sThumbRX / (float)MAXBYTE;
		fThumbRY = (float)Xbox.State.Gamepad.sThumbRY / (float)MAXBYTE;

}