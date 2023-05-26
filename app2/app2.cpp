// app2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "app2.h"

// for receiving images in thread
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <sstream>
#include "../lbdll/lbdll.h"
#include "../stb_image_lib/stb_image.h"
#include "../lbregistry_static/registry_module.h"

// include GDI+
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

int width1{}, height1{}, channels1{};
unsigned char *data1{};
int width2{}, height2{}, channels2{};
unsigned char *data2{};
Bitmap* bmp1;
Bitmap* bmp2;


void ImageReceiverThread1(HWND hWindow)
{
    
    data1 = receive_image(LB_PORT, &width1, &height1, &channels1);

    // TODO move to app1 this and from the second thread
    process_image(data1, width1, height1, channels1,
        [](uint8_t& r, uint8_t& g, uint8_t& b)
        {
            r += 10;
            g += 10;
            b += 10;
        }
    );

    const int white_count = count_colored_pixels(data1, width1, height1, channels1, 255, 255, 255);
    WriteUnsignedIntToRegistry(LB_REG_KEY1, white_count);

	// redraw window to display image (WM_PAINT)
    RedrawWindow(hWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

}

void ImageReceiverThread2(HWND hWindow)
{

    data2 = receive_image(LB_PORT, &width2, &height2, &channels2);
    process_image(data2, width2, height2, channels2,
        [](uint8_t& r, uint8_t& g, uint8_t& b)
        {
            r *= 1.2;
            g *= 1.2;
            b *= 1.2;
        }
    );

    const int black_count = count_colored_pixels(data2, width2, height2, channels2, 0, 0, 0);
    WriteUnsignedIntToRegistry(LB_REG_KEY2, black_count);

    // redraw window to display image (WM_PAINT)
    RedrawWindow(hWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

}
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APP2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APP2));

    MSG msg;

    // start image receiving thread
    std::thread img1Thread;

    // this block of code is before the loop in purpose of
    // reducing branching/ assignment operations
	// TODO it would be suitable to use __builtings_expect(isThreadStarted, 0)

    GetMessage(&msg, nullptr, 0, 0);

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


	img1Thread = std::thread(ImageReceiverThread1, msg.hwnd);
    // img2Thread = std::thread(ImageReceiverThread2, msg.hwnd);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    delete bmp1;
    delete bmp2;
    delete[] data1;
    delete[] data2;
    img1Thread.join();
    GdiplusShutdown(gdiplusToken); // GDI+ Shut up()

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APP2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT: // it is called once on the startup or after redraw/resize event
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            Gdiplus::Graphics graphics(hdc);

            if (data1)
            {
                bmp1 = new Bitmap(width1, height1, width1 * channels1, PixelFormat32bppRGB, (BYTE*)data1);
                graphics.DrawImage(bmp1, 0, 0);
            }
            if (data2)
            {
                bmp2 = new Bitmap(width2, height2, width2 * channels2, PixelFormat32bppRGB, (BYTE*)data2);
            	graphics.DrawImage(bmp2, width1, 0);
            }

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
