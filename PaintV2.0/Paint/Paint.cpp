// Paint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Paint.h"
#include <windowsx.h>
#include <Windows.h>
#include <fstream>
#include <commdlg.h>

#define MAX_LOADSTRING 100
#define ID_TIMER1	123123
// Global Variables:
WCHAR buffer[1024];
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT p1, p2, p2_ancien;
HBITMAP hBitmap;
BOOL enTrainDessin;
int currentPenWidth = 0;
HPEN myPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
int currentPenColor = 0;	// black
DWORD penColor = 0;
DWORD brushColor = 0;
DWORD shapeColor = 0;
int typeBrush = 0;
CHOOSECOLOR cc;
CHOOSEFONT cf;
static LOGFONT lf;
HFONT hfont, hfontPrev;
DWORD rgbPrev;
static COLORREF acrCustClr[16]; // array of custom colors
static DWORD rgbCurrent;        // initial color selection
int tempPWidth = currentPenWidth;
DWORD tempPColor = penColor;
DWORD tempBColor = brushColor;
DWORD tempSColor = shapeColor;
int tempTypeBrush = 0;
HDC hdc;
HWND hWnd;

int mode; //mode de dessin==> 0 pour ligne, 1 pour libre
		  // Forward declarations of functions included in this code module:

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK CustomizePen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Text(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID				FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawCircleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawEllipseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawArcProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID DrawLine(HDC hdc);
VOID FreeDraw(HDC hdc);
VOID PenBrushDemo(HDC hdc);
VOID DrawCircle(HDC hdc);
VOID DrawRectangle(HDC hdc);
VOID DrawEllipse(HDC hdc);
VOID DrawSquare(HDC hdc);
VOID DrawArc(HDC hdc);
VOID ChangePenColor(HDC hdc);
void CaptureScreen(HWND window, const char* filename);
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
	wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PAINT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU,
		0, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	// TODO: Add any drawing code that uses hdc here...
	hdc = BeginPaint(hWnd, &ps);
	
	




	switch (mode)
	{
	case 0:
		FreeDrawProc(hWnd, message, wParam, lParam);
		break;
	case 1:
		DrawLineProc(hWnd, message, wParam, lParam);
		break;
	case 2:
		DrawArcProc(hWnd, message, wParam, lParam);
		break;
	case 3:
		DrawRectangleProc(hWnd, message, wParam, lParam);
		break;
	case 4:
		DrawSquareProc(hWnd, message, wParam, lParam);
		break;
	case 5:
		DrawCircleProc(hWnd, message, wParam, lParam);
		break;
	case 6:
		DrawEllipseProc(hWnd, message, wParam, lParam);
		break;

	}
	switch (message)
	{
	case WM_CREATE:
	{
		p1.x = p1.y = 0;
		p2 = p1;
		mode = 1;
		enTrainDessin = FALSE;

		//Tạo bitmap
		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		//Tạo ra một bitmap tương thích với DC màn hình
		hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

		//Tô background cho bitmap
		//Tạo memory dc để tương tác với Bitmap
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(memDC, &rect, hBrush);
		DeleteObject(hBrush);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hdc);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_FORMAT_PEN:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_PEN), hWnd, CustomizePen);
			break;
		case ID_FORMAT_TEXT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TEXT), hWnd, Text);
			break;
		case ID_EXIT:
			if (MessageBox(hWnd, _T("Are you sure to close?"), _T("Confirm close"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{
				DestroyWindow(hWnd);
			}
			break;
		case ID_FILE_SAVE:
		{
			PBITMAPINFO PBI = CreateBitmapInfoStruct(hWnd, hBitmap);
			CreateBMPFile(hWnd, _T("bitmap.bmp"), PBI, hBitmap, hdc);
		}
		break;

		case ID_STYLE_FREE:
			mode = 0;
			break;
		case ID_STYLE_LINE:
			mode = 1;
			break;
		case ID_STYLE_CURVE:
			mode = 2;
			break;
		case ID_SHAPE_RECTANGLE:
			mode = 3;
			break;
		case ID_SHAPE_SQUARE:
			mode = 4;
			break;
		case ID_SHAPE_CIRCLE:
			mode = 5;
			break;
		case ID_SHAPE_ELLIPSE:
			mode = 6;
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_RBUTTONDOWN:
		SetTimer(hWnd, ID_TIMER1, 1000, NULL);

		break;
	case WM_RBUTTONDBLCLK:
		KillTimer(hWnd, ID_TIMER1);
		//effacer l'ecran
		SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)GetDC(hWnd), (LPARAM) nullptr);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case ID_TIMER1:
			KillTimer(hWnd, ID_TIMER1);
			mode = (mode + 1) % 7;
			MessageBox(hWnd, _T("Mode de dessin change"), _T("Notifcation"), MB_OK);
			break;
		}
	case WM_PAINT:
	{

		//Vẽ bitmap ra màn hình trước
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		RECT rect;
		GetClientRect(hWnd, &rect);
		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

		switch (mode)
		{
		case 0:
			//Vẽ tự do thì ta vẽ luôn trên màn hình và bitmap
			FreeDraw(memDC);
			FreeDraw(hdc);
			break;
		case 1:
			//Vẽ đường thẳng thì ta chỉ vẽ lên màn hình
			DrawLine(hdc);
			break;
		case 2:
			DrawArc(hdc);
			break;
		case 3:
			DrawRectangle(hdc);
			break;
		case 4:
			DrawSquare(hdc);
			break;
		case 5:
			DrawCircle(hdc);
			break;
		case 6:
			DrawEllipse(hdc);
			break;
		}
		ChangePenColor(hdc);
		DeleteObject(memDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID DrawArc(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		int length = abs(p1.x - p2.x) *0.5;
		Arc(hdc, (2 * p1.x - p2.x - length), p1.y, p2.x + length, p2.y + length, p1.x, p1.y, p2.x, p2.y);
	}
}

VOID DrawRectangle(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
	}
}

VOID DrawSquare(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		int temp_left = min(p1.x, p2.x);
		int temp_top = min(p1.y, p2.y);
		int temp_right = max(p1.x, p2.x);
		int temp_bot = max(p1.y, p2.y);

		int length = min(abs(p2.x - p1.x), abs(p2.y - p1.y));

		if (p2.x < p1.x)
			temp_left = temp_right - length;
		else
			temp_right = temp_left + length;

		if (p2.y < p1.y)
			temp_top = temp_bot - length;
		else
			temp_bot = temp_top + length;


		Rectangle(hdc, temp_left, temp_top, temp_right, temp_bot);
	}
}

VOID DrawCircle(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		int temp_left = min(p1.x, p2.x);
		int temp_top = min(p1.y, p2.y);
		int temp_right = max(p1.x, p2.x);
		int temp_bot = max(p1.y, p2.y);

		int length = min(abs(p2.x - p1.x), abs(p2.y - p1.y));
		if (p2.x < p1.x)
			temp_left = temp_right - length;
		else
			temp_right = temp_left + length;

		if (p2.y < p1.y)
			temp_top = temp_bot - length;
		else
			temp_bot = temp_top + length;


		Ellipse(hdc, temp_left, temp_top, temp_right, temp_bot);
	}
}

VOID DrawEllipse(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		Ellipse(hdc, p1.x, p1.y, p2.x, p2.y);
	}
}

VOID DrawLine(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
	}
}

VOID FreeDraw(HDC hdc)
{
	if (enTrainDessin == TRUE)
	{
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		LineTo(hdc, p2.x, p2.y);
		p1 = p2;
	}
}

VOID DrawRectangleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		DrawRectangle(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}

VOID DrawSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		SelectObject(memDC, GetStockObject(NULL_BRUSH));
		DrawSquare(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}

VOID DrawEllipseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		DrawEllipse(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}

VOID DrawCircleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		DrawCircle(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}

VOID DrawArcProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		ChangePenColor(hdc);
		DrawArc(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);

		enTrainDessin = FALSE;
		break;
	}
}



VOID DrawLineProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2_ancien = p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{
			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:

		//Tiến hành cho vẽ lên bitmap
		HDC hdc = GetDC(hWnd);
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, hBitmap);
		ChangePenColor(hdc);
		DrawLine(memDC);
		DeleteObject(memDC);
		ReleaseDC(hWnd, hdc);
		enTrainDessin = FALSE;
		break;
	}
}

VOID FreeDrawProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT windowRect;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		p1.x = GET_X_LPARAM(lParam);
		p1.y = GET_Y_LPARAM(lParam);
		p2 = p1;
		enTrainDessin = TRUE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam&MK_LBUTTON)
		{

			p2.x = GET_X_LPARAM(lParam);
			p2.y = GET_Y_LPARAM(lParam);
			GetWindowRect(hWnd, &windowRect);
			InvalidateRect(hWnd, &windowRect, FALSE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_LBUTTONUP:
		enTrainDessin = FALSE;
		break;
	}
}

VOID ChangePenColor(HDC hdc)
{
	HPEN hPen = CreatePen(PS_SOLID, currentPenWidth, penColor);
	HBRUSH hBrush;
	switch (typeBrush)		// cai dat brush va pen
	{
	case 0:
		hBrush = CreateSolidBrush(shapeColor);
	case 1:
		hBrush = CreateHatchBrush(100,shapeColor);
	default:
		hBrush = CreateSolidBrush(shapeColor);
		break;
	}
							
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
}
void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
								// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL);


	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, (NULL));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);


	// Close the .BMP file.  
	CloseHandle(hf);


	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}


PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);


	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}


INT_PTR CALLBACK CustomizePen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND handleText = GetDlgItem(hDlg,IDC_TEXTSIZE);
	HDC btnPenColor = GetDC(GetDlgItem(hDlg, IDC_PENCOLOR));
	HWND btnBrushColor = GetDlgItem(hDlg, IDC_BRUSHCOLOR);
	HWND btnShapeColor = GetDlgItem(hDlg, IDC_SHAPECOLOR);


	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	switch (message)
	{
	case WM_INITDIALOG:
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_SMALL:
			tempPWidth = 5;
			SetWindowText(handleText, _T("Size: 5"));
			break;
		case IDC_MEDIUM:
			tempPWidth = 10;
			SetWindowText(handleText, _T("Size: 10"));
			break;
		case IDC_BIG:
			tempPWidth = 15;
			SetWindowText(handleText, _T("Size: 15"));
			break;
			
		case IDC_PENCOLOR:
			if (ChooseColor(&cc) == TRUE)
			{
				tempPColor = cc.rgbResult;
			}
			break;
		case IDC_BRUSHCOLOR:
			if (ChooseColor(&cc) == TRUE)
			{
				tempBColor = cc.rgbResult;
			}
			break;
		case IDC_SHAPECOLOR:
			if (ChooseColor(&cc) == TRUE)
			{
				tempSColor = cc.rgbResult;
			}
			break;
		case IDC_HATCHBRUSH:
			tempTypeBrush = 1;
			break;
		case IDC_SOLIDBRUSH:
			tempTypeBrush = 0;
			break;
		case IDC_PATTERNBRUSH:
			tempTypeBrush = 2;
			break;
		case ID_ADD:
			typeBrush = tempTypeBrush;
			currentPenWidth = tempPWidth;
			penColor = tempPColor;
			brushColor = tempBColor;
			shapeColor = tempSColor;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;	
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			break;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Text(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND textHandle = GetDlgItem(hDlg, IDC_EDIT1);
	HDC mainhdc = GetDC(hWnd);
	HDC subhdc = CreateCompatibleDC(mainhdc);
	RECT textbox;
	
	int len = 0;
	// Initialize CHOOSEFONT
	/*ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof (cf);
	cf.hwndOwner = hDlg;
	cf.lpLogFont = &lf;
	cf.rgbColors = rgbCurrent;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
*/
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_FONT:
			/*if (ChooseFont(&cf) == TRUE)
			{

				hfont = CreateFontIndirect(cf.lpLogFont);
				hfontPrev=(HFONT)SelectObject(hdc, hfont);
				rgbCurrent = cf.rgbColors;	
				rgbPrev = SetTextColor(hdc, rgbCurrent);
				
			}*/
			break;
			
		case IDOK:
			GetWindowText(textHandle, buffer, 1024);
			len=wcslen(buffer);
			//SetRect(&textbox, 100, 50, 184, 84);
			//SelectObject(subhdc, hBitmap);
			TextOut(mainhdc, 50, 50, buffer, len);
			TextOut(subhdc, 50, 50, buffer, len);
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

