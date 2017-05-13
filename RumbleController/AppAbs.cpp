#include "HIDXBOX.h"

#define TARGET_XBOX360
#ifdef TARGET_XBOX360
	HIDXBOX Control;
#elif defined TARGET_PS3
	HIDPS Control;
#elif defined (TARGET_WII)
	HIDWII Control;
#endif

#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <XInput.h>
#include <basetsd.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include "resource.h"



//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void RenderFrame();
RECT RVentana = { 0,0,1024,600 };


//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STATE
{
    XINPUT_STATE lastState;
    XINPUT_STATE state;
    DWORD dwResult;
    bool bLockVibration;
    XINPUT_VIBRATION vibration;
};


WCHAR g_szMessage[4][1024] = {0};
HWND g_hWnd;
bool    g_bDeadZoneOn = true;
SYSTEMTIME time;

void generaEventos(){
	POINT pt, pt0;
	GetCursorPos(&pt);
	HWND hWnd = GetActiveWindow();
	
	//JOYL2RATON
	if (Control.TLX() != 0 | Control.TLY() != 0){
		pt.x += Control.TLX()*10;
		pt.y -= Control.TLY()*10;
		SetCursorPos(pt.x, pt.y);
	}

	GetWindowRect(GetDesktopWindow(), &RVentana);

	if (Control.TRX() != 0 || Control.TRY() != 0) {
		pt.x += Control.TRXf() * 10;
		pt.y -= Control.TRYf() * 10;
		SetCursorPos(pt.x, pt.y);
	}
	
	/*if (Control.TRX() != 0 | Control.TRY() != 0) {
		pt.x = ((RVentana.right / 2 + (RVentana.right / 2 * (Control.TRXf()*Control.velX()))));// *Control.velX()));// +RVentana.right / 2;
		pt.y -= Control.TRYf() * 10;
		//pt.y =-( RVentana.bottom / 2 + (RVentana.right / 2 * Control.TRYf())*Control.velY());
		SetCursorPos(pt.x, pt.y);
	}
	*/
	//BOTONES
	if (Control.BD(XINPUT_GAMEPAD_X))
		mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, NULL);
	if (Control.BU(XINPUT_GAMEPAD_X))
		mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);


	if (Control.BD(XINPUT_GAMEPAD_B) )
		mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, NULL);
	if (Control.BU( XINPUT_GAMEPAD_B) )
		mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);

	
	if (Control.BD( XINPUT_GAMEPAD_Y)) 
		keybd_event(VK_VOLUME_UP, 0xAF, 0, 0);
	if (Control.BD(XINPUT_GAMEPAD_Y))
		keybd_event(VK_VOLUME_UP, 0xAF, KEYEVENTF_KEYUP, 0);

	if (Control.BD(XINPUT_GAMEPAD_A)) 
		keybd_event(VK_VOLUME_DOWN, 0xAE,  0, 0);
	if (Control.BD(XINPUT_GAMEPAD_A))
		keybd_event(VK_VOLUME_DOWN, 0xAE, KEYEVENTF_KEYUP, 0);

	//FLECHAS
	if (Control.BD(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x25, 0, 0);
	if (Control.BD(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x25, KEYEVENTF_KEYUP, 0);

	if (Control.BD(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x26, 0, 0);
	if (Control.BD(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x26, KEYEVENTF_KEYUP, 0);

	if (Control.BD(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x27, 0, 0);
	if (Control.BD(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x27, KEYEVENTF_KEYUP, 0);

	if (Control.BD(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x28, 0, 0);
	if (Control.BD(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x28, KEYEVENTF_KEYUP, 0);
	
	//TRIGGERS
	if (Control.LT()!=0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, (DWORD) (35*Control.LT()), NULL);
	if (Control.RT() != 0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, -(DWORD)(35 * Control.RT()), NULL);

	//Shoulder
	if (Control.BD(XINPUT_GAMEPAD_LEFT_SHOULDER) )
		keybd_event(VK_BROWSER_BACK, 0x46, 0 , 0);
	if (Control.BU(XINPUT_GAMEPAD_LEFT_SHOULDER))
		keybd_event(VK_BROWSER_BACK, 0x46, KEYEVENTF_KEYUP, 0);

	if (Control.BD( XINPUT_GAMEPAD_RIGHT_SHOULDER))
		keybd_event(VK_BROWSER_FORWARD, 0xA7, 0, 0);
	if (Control.BU( XINPUT_GAMEPAD_RIGHT_SHOULDER))
		keybd_event(VK_BROWSER_FORWARD, 0xA7, KEYEVENTF_KEYUP, 0);

	
	//Josticks pulsados
	if (Control.BD(XINPUT_GAMEPAD_RIGHT_THUMB))
		keybd_event(VK_VOLUME_UP, 0xAF,  0, 0);
	if (Control.BU(XINPUT_GAMEPAD_RIGHT_THUMB))
		keybd_event(VK_VOLUME_UP, 0xAF, KEYEVENTF_KEYUP, 0);

	if (Control.BD(XINPUT_GAMEPAD_LEFT_THUMB))
		keybd_event(VK_VOLUME_DOWN, 0xAE,  0, 0);
	if (Control.BU(XINPUT_GAMEPAD_LEFT_THUMB))
		keybd_event(VK_VOLUME_DOWN, 0xAE, KEYEVENTF_KEYUP, 0);
	
	//BUSCO TECLAS ABAJO
	
	if (Control.BD(XINPUT_GAMEPAD_RIGHT_THUMB))
		keybd_event(VK_VOLUME_UP, 0xAF, KEYEVENTF_EXTENDEDKEY | 0, 0);
	if (Control.BU(XINPUT_GAMEPAD_RIGHT_THUMB))
		keybd_event(VK_VOLUME_UP, 0xAF, KEYEVENTF_EXTENDEDKEY | 0, 0);

	if (Control.BD(XINPUT_GAMEPAD_LEFT_THUMB))
		keybd_event(VK_VOLUME_DOWN, 0xAE, KEYEVENTF_EXTENDEDKEY | 0, 0);
	if (Control.BU(XINPUT_GAMEPAD_LEFT_THUMB))
		keybd_event(VK_VOLUME_DOWN, 0xAE, KEYEVENTF_EXTENDEDKEY | 0, 0);

	//HOME	
	if (Control.BD(XINPUT_GAMEPAD_START)|| Control.BD(XINPUT_GAMEPAD_START))
		keybd_event(VK_HOME, 0x24, KEYEVENTF_EXTENDEDKEY | 0, 0);

	

	//ESCAPE
	if (Control.BD(XINPUT_GAMEPAD_BACK))
		PostQuitMessage(0);

}

VOID CALLBACK MyTimerProc() // current system time 
{
	//UpdateControllerState();
	Control.actualiza();
	generaEventos();
}

int APIENTRY wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, int )
{
	Control.calibra();
   
	//Create el timer
	UINT_PTR timer = SetTimer(g_hWnd, 0, 10, (TIMERPROC)MyTimerProc); // timer callback
    
	// Enter the message loop
    MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    // Clean up 
	KillTimer(NULL, timer);

    return 0;
}

//-----------------------------------------------------------------------------
void RenderFrame()
{

}



//-----------------------------------------------------------------------------
// Window message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_ACTIVATEAPP:
        {
            if( wParam == TRUE )
            {
                // App is now active, so re-enable XInput
                XInputEnable( true );
            }
            else
            {
                // App is now inactive, so disable XInput to prevent
                // user input from effecting application and to 
                // disable rumble. 
               // XInputEnable( false );
            }
            break;
        }

      /*  case WM_PAINT:
        {
            // Paint some simple explanation text
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint( hWnd, &ps );
            SetBkColor( hDC, 0xFF0000 );
            SetTextColor( hDC, 0xFFFFFF );
            RECT rect;
            GetClientRect( hWnd, &rect );

            rect.top = 20;
            rect.left = 20;
            DrawText( hDC,
                      L"Use the controller's left/right trigger to adjust the speed of the left/right rumble motor.\n"
                      L"Press any controller button to lock or unlock at the current rumble speed.\n",
                      -1, &rect, 0 );

            for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
            {
                rect.top = i * 80 + 90;
                rect.left = 20;
                DrawText( hDC, g_szMessage[i], -1, &rect, 0 );
            }

            EndPaint( hWnd, &ps );
            return 0;
        }*/
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE) PostQuitMessage(0);
			break;
		}
        case WM_DESTROY:
        {
            PostQuitMessage( 0 );
            break;
        }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



