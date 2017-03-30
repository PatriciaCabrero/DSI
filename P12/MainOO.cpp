#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <WindowsX.h>

#include "basewin.h"

enum CLOCKMODE{ RUN, STOP };
enum MODE{ SelectMode, DrawMode, DragMode};

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
////////////////////////////////////DPI CLASS

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

///////////////////////////////////DPI END CLASS

class MainWindow : public BaseWindow<MainWindow>
{
	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;//Para la elipse
	ID2D1SolidColorBrush    *pStroke;//Para la flecha
	D2D1_ELLIPSE            ellipse;
	MODE					mode;
	CLOCKMODE				clockmode;
	SYSTEMTIME				time; //Variable que solo se pide en el modo run.
	
	//Crear elipses
	D2D1_ELLIPSE			ellipseMouse;
	D2D1_COLOR_F			colorellipse = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f);
	D2D1_POINT_2F           ptMouse;
	ID2D1SolidColorBrush    *pBrushM;//Para la elipse

	//CURSOR
	HCURSOR					cursorH,cursorC, cursorA;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void	DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth);
	void    OnPaint();
	void    Resize();
	void	RenderClock();
	void	OnKeyDown(WPARAM wparam);
	void	RenderElipseMouse();
	BOOL	HitTest(D2D1_ELLIPSE ellipse, float x, float y);
	//Control del raton
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp();
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void	OnRButtonDown();

public:

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
	}

	PCWSTR  ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};



BOOL MainWindow::HitTest(D2D1_ELLIPSE ellipse, float x, float y){
	const float a = ellipse.radiusX;
	const float b = ellipse.radiusY;
	const float x1 = x - ellipse.point.x;
	const float y1 = y - ellipse.point.y;
	const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
	return d <= 1.0f;
}
// Recalculate drawing layout when the size of the window changes.

void MainWindow::DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth)
{
	pRenderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(fAngle, ellipse.point)
		);

	// endPoint defines one end of the hand.
	D2D_POINT_2F endPoint = D2D1::Point2F(
		ellipse.point.x,
		ellipse.point.y - (ellipse.radiusY * fHandLength)
		);

	// Draw a line from the center of the ellipse to endPoint.
	pRenderTarget->DrawLine(
		ellipse.point, endPoint, pStroke, fStrokeWidth);
}

void MainWindow::RenderClock()
{
	pRenderTarget->FillEllipse(ellipse, pBrush);
	pRenderTarget->DrawEllipse(ellipse, pStroke);
	// Draw hands
	// 60 minutes = 30 degrees, 1 minute = 0.5 degree
	const float fHourAngle = (360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f);
	const float fMinuteAngle = (360.0f / 60) * (time.wMinute);
	const float fSecondAngle = (360.0f / 60) * (time.wSecond);

	DrawClockHand(0.6f, fHourAngle, 6);
	DrawClockHand(0.85f, fMinuteAngle, 4);
	DrawClockHand(0.95f, fSecondAngle, 2);

	// Restore the identity transformation.
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void MainWindow::RenderElipseMouse(){
	//DIBUJA LA ELIPSE DEL RATON
	pRenderTarget->FillEllipse(ellipseMouse, pBrushM);
	pRenderTarget->DrawEllipse(ellipseMouse, pStroke);
}

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 8;
		const float y = size.height / 8;
		const float radius = min(x, y);
		ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		mode = MODE::SelectMode;
		//CURSORES.
		cursorC = LoadCursor(0, IDC_CROSS);
		cursorH = LoadCursor(0, IDC_HAND);
		cursorA = LoadCursor(0, IDC_ARROW);
		SetCursor(cursorA);
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);
		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
			const D2D1_COLOR_F color1 = D2D1::ColorF(0.0f, 0.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color1, &pStroke);
			hr = pRenderTarget->CreateSolidColorBrush(colorellipse, &pBrushM);
			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
	KillTimer(m_hwnd, 0);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		RenderClock();
		RenderElipseMouse();
		
		hr = pRenderTarget->EndDraw();



		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"Circle", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		SetTimer(m_hwnd, // handle to main window
			0, // timer identifier
			1000, //1-second interval
			NULL); // timer callback
		clockmode = CLOCKMODE::RUN;
		GetLocalTime(&time);

        DPIScale::Initialize(pFactory);
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;

	case WM_TIMER: // process the 1-second timer
		if (clockmode == CLOCKMODE::RUN)
			GetLocalTime(&time);

		PostMessage(m_hwnd, WM_PAINT, NULL, NULL);
		return 0;

	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;
		break;

	case WM_LBUTTONDOWN:
		if (mode == MODE::SelectMode){
			if (HitTest(ellipseMouse, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))){
				mode = MODE::DragMode;
			}
			else
			{
				mode = MODE::DrawMode;
			}
			OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		}
		else if (mode == MODE::DragMode){
			ptMouse.x = GET_X_LPARAM(lParam);
			ptMouse.y = GET_Y_LPARAM(lParam);
		}
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;

	case WM_RBUTTONDOWN:
		OnRButtonDown();
		return 0;

	case WM_MOUSEMOVE:
		if (mode == MODE::SelectMode){
			if (HitTest(ellipseMouse, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))){
				SetCursor(LoadCursor(NULL, IDC_HAND));
				mode = MODE::DragMode;
			}
		}

		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	default:
		break;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::OnKeyDown(WPARAM wParam){

	switch (wParam)
	{
	case VK_ESCAPE:
		PostMessage(m_hwnd, WM_DESTROY, NULL, NULL);
		return;
	case VK_SPACE:
		if (clockmode == CLOCKMODE::RUN)
			clockmode = CLOCKMODE::STOP;
		else clockmode = CLOCKMODE::RUN;
	default:
		break;
	}
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	if (HitTest(ellipse, pixelX, pixelY)){
		mode == MODE::DragMode;
	}
	else{
		mode == MODE::DrawMode;
		SetCapture(m_hwnd);
		ellipseMouse.point = ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
		ellipseMouse.radiusX = ellipseMouse.radiusY = 1.0f;
		InvalidateRect(m_hwnd, NULL, FALSE);
		SetCursor(cursorC);
	}
	
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
	if (mode != MODE::DrawMode){
		if (HitTest(ellipseMouse, dips.x, dips.y)){
			SetCursor(cursorH);
		}
		else {
			mode = MODE::SelectMode;
			SetCursor(cursorA);

		}
	}
	if (flags & MK_LBUTTON)
	{
		
		const float width = (dips.x - ptMouse.x) / 2;
		const float height = (dips.y - ptMouse.y) / 2;
		const float x1 = ptMouse.x + width;
		const float y1 = ptMouse.y + height;

		if (mode == MODE::DrawMode){
			ellipseMouse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

			InvalidateRect(m_hwnd, NULL, FALSE);
			SetCursor(cursorC);
		}
		if (mode == MODE::DragMode)
		{
			ellipseMouse.point.x += (float)width*2;
			ellipseMouse.point.y += (float)height*2;
			ptMouse = dips;
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
	}
}


void MainWindow::OnLButtonUp()
{
	ReleaseCapture();
	mode = MODE::SelectMode;
}

void MainWindow::OnRButtonDown()
{
	CHOOSECOLOR cc; // common dialog box structure
	static COLORREF acrCustClr[16]; // array of custom colors
	static DWORD rgbCurrent; // initial color selection
	// Initialize CHOOSECOLOR

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	if (ChooseColor(&cc) == TRUE)
	{
		//En ​cc.rgbResult​ tenemos el color seleccionado
		//Utilizarlo para configurar nuestra brocha
		//Es necesario transformarlo al formato de color de D2D
		colorellipse.r = (float)GetRValue(cc.rgbResult)/255.0;
		colorellipse.g = (float)GetGValue(cc.rgbResult)/255.0;
		colorellipse.b = (float)GetBValue(cc.rgbResult)/255.0;
		pBrushM->SetColor(colorellipse);

	}
}
