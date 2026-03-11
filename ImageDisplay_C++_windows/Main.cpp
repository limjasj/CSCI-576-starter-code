//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include <iostream>
#include <vector>
#define CRT_SECURE_NO_WARNINGS

#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage;						// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text
extern bool showBlocks = false;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//struct Block
//{
//	int x;
//	int y;
//	int size;
//
//	std::vector<std::vector<std::vector<int>>> qCoeff;
//
//	Block(int x0, int y0, int N)
//	{
//		x = x0;
//		y = y0;
//		size = N;
//
//		qCoeff.resize(3, std::vector<std::vector<int>>(N, std::vector<int>(N)));
//	}
//};
//std::vector<Block*> allBlocks;

// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// The rest of command line argument is truncated.
	// If you want to use it, please modify the code.
	if (lpCmdLine[0] == 0) {
		wprintf(L"No command line argument.");
		return -1;
	}

	//read  parameters	
	int cnt = 0;
	int startCnt = 0;
	char* scale = "";
	char* quantization = "";
	char* mode = "";
	char* extra = "";
	char* quantizer = "";
	char* bitsPerPixel = "";

	bool isCompression = true;

	if (!isCompression)
	{
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0) {
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The first parameter was: %s \n", lpCmdLine);

		cnt++;
		startCnt = cnt;
		scale = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The second parameter was: %s \n", &lpCmdLine[startCnt]);

		cnt++;
		startCnt = cnt;
		quantization = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The third parameter was: %s \n", &lpCmdLine[startCnt]);

		cnt++;
		startCnt = cnt;
		mode = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The fourth parameter was: %s \n", &lpCmdLine[startCnt]);

		cnt++;
		startCnt = cnt;
		extra = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		if (cnt == startCnt) extra = 0;
		lpCmdLine[cnt] = 0;
		printf("The fifth parameter was: %s \n", &lpCmdLine[startCnt]);
	}

	if (isCompression)
	{
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0) {
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The first parameter was: %s \n", lpCmdLine);

		cnt++;
		startCnt = cnt;
		mode = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The mode is: %s \n", &lpCmdLine[startCnt]);

		cnt++;
		startCnt = cnt;
		quantizer = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The quantizer is: %s \n", &lpCmdLine[startCnt]);

		cnt++;
		startCnt = cnt;
		bitsPerPixel = &lpCmdLine[startCnt];
		while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0)
		{
			cnt++;
		}
		lpCmdLine[cnt] = 0;
		printf("The bitsPerPixel is: %s \n", &lpCmdLine[startCnt]);
	}

	// Set up the images
	// Modify the height and width values here to read and display an image with
  	// different dimensions. 
	int w = 512;
	int h = 512;
	inImage.setWidth(w);
	inImage.setHeight(h);

	inImage.setImagePath(lpCmdLine);
	inImage.ReadImage();

	inImage.Compress(mode, quantizer, bitsPerPixel);


	//inImage.Quantize(scale, quantization, mode, extra);
	//inImage.Modify(scale, quantization, mode, extra);
	//inImage.WriteImage();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				char text[1000];
				strcpy(text, "The original image is shown as follows. \n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nUpdate program with your code to modify input image. \n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();

				SetDIBitsToDevice(hdc,
								  0,100,inImage.getWidth(),inImage.getHeight(),
								  0,0,0,inImage.getHeight(),
								  inImage.getImageData(),&bmi,DIB_RGB_COLORS);

				if (showBlocks)
				{
					

					int blockSize = 8; // for M=1

					HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
					HPEN oldPen = (HPEN)SelectObject(hdc, pen);
					HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

					int width = inImage.getWidth();
					int height = inImage.getHeight();

					int offsetY = 100; // image starts at y=100 in your code

					for (Block* b : inImage.compressor->getAllBlocks())
					{
						Rectangle(hdc,
							b->x,
							offsetY + b->y,
							b->x + b->size,
							offsetY + b->y + b->size);
					}

					/*for (int x = 0; x <= width; x += blockSize)
					{
						MoveToEx(hdc, x, offsetY, NULL);
						LineTo(hdc, x, offsetY + height);
					}

					for (int y = 0; y <= height; y += blockSize)
					{
						MoveToEx(hdc, 0, offsetY + y, NULL);
						LineTo(hdc, width, offsetY + y);
					}*/

					SelectObject(hdc, oldPen);
					DeleteObject(pen);
				}
							   
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wParam == 'B' || wParam == 'b')
			{
				showBlocks = !showBlocks;   // toggle on/off
				InvalidateRect(hWnd, NULL, false); // redraw window
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


