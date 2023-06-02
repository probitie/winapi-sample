// app1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "app1.h"

// include GDI+
#include <objidl.h>
#include <gdiplus.h>

#include <sstream>

#include "../lbdll/lbdll.h"
#include "../stb_image_lib/stb_image.h"
#include "../lbregistry_static/registry_module.h"
#include "../lbregistry_static/tray_module.h"




static bool hook_ctrlPressed = false;
static bool hook_altPressed = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);


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

using namespace Gdiplus;

int width1, height1, channels1;
unsigned char* data1, *data1_to_send;
int width2, height2, channels2;
unsigned char* data2, *data2_to_send;

// Создание GDI+ Bitmap объектов для каждого изображения
Bitmap* bmp1;
Bitmap* bmp2;

HWND ghWnd = nullptr; // will be initialized with first message in WinMain

void swapRB(unsigned char* data, int width, int height) {
    for (int i = 0; i < width * height * 4; i += 4) {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // Инициализация GDI+

    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Загрузка изображений с помощью stb_image
    // Так же предполагается что ищ находится в папке билда
    data1 = stbi_load("D:\\files\\testimg.jpg", &width1, &height1, &channels1, 4);
    swapRB(data1, width1, height1);

	int data1_size = width1 * height1 * channels1;
	data1_to_send = new unsigned char[data1_size];
    memcpy_s(data1_to_send, data1_size, data1, data1_size);
    process_image(data1_to_send, width1, height1, channels1,
        [](uint8_t& r, uint8_t& g, uint8_t& b)
        {
            r += 10;
            g += 10;
            b += 10;
        }
    );


	data2 = stbi_load("D:\\files\\testimg.jpg", &width2, &height2, &channels2, 4);
    swapRB(data2, width2, height2);

	int data2_size = width2 * height2 * channels2;
    data2_to_send = new unsigned char[data2_size];
    memcpy_s(data2_to_send, data2_size, data2, data2_size);

	process_image(data2_to_send, width2, height2, channels2,
        [](uint8_t& r, uint8_t& g, uint8_t& b)
        {
            r *= 1.2;
            g *= 1.2;
            b *= 1.2;
        }
    );


    LB_ASSERT(data1 != nullptr && data2 != nullptr, "can't load image")


        
    // Создание GDI+ Bitmap объектов для каждого изображения
        bmp1 = new Bitmap(width1, height1, width1 * 4, PixelFormat32bppARGB, (BYTE*)data1);

	bmp2 = new Bitmap(width2, height2, width2 * 4, PixelFormat32bppARGB, (BYTE*)data2);
    

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APP1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APP1));

    MSG msg;

    // this block of code is before the loop in purpose of
    // reducing branching/ assignment operations
    // as here ghWnd is initialized
    GetMessage(&msg, nullptr, 0, 0);
    ghWnd = msg.hwnd;
    
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    UnhookWindowsHookEx(hHook);

    delete bmp1;
    delete bmp2;
    stbi_image_free(data1);
    stbi_image_free(data2);

    delete data1_to_send;
    delete data2_to_send;

    Gdiplus::GdiplusShutdown(gdiplusToken);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APP1);
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
    //TrayWndProc(hWnd, message, wParam, lParam); // i need it only if it handles clicks or whatever

    switch (message)
    {
    case WM_CREATE:
	    {
        TrayAddIcon(hWnd);
        HWND hwndButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"push",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            20,         // x position 
            height1 + 30,         // y position 
            100,        // Button width
            100,        // Button height
            hWnd,     // Parent window
            (HMENU)ID_LB_PUSH_BUTTON, // Unique ID for the button
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
	    }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_LB_PUSH_BUTTON:
                if (wmEvent == BN_CLICKED)
                {
                    // Todo make them async
                    send_image(LB_HOST, LB_PORT1, data1_to_send, width1, height1, channels1);
                    send_image(LB_HOST, LB_PORT2, data2_to_send, width2, height2, channels2);
                    MessageBox(hWnd, L"Button clicked, sending image!", L"Info", MB_OK);

                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Создайте объект Graphics
            Gdiplus::Graphics graphics(hdc);

            // Отрисуйте изображения на окне
            graphics.DrawImage(bmp1, 0, 0);
            graphics.DrawImage(bmp2, width1, 0);


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

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        switch (wParam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (hook_ctrlPressed && p->vkCode == 'Y') {
                std::wstringstream msg;
                uint32_t val;

                msg << "key 1: " << LB_REG_KEY1 << "; ";

                if (ReadUnsignedIntFromRegistry(LB_REG_KEY1, val))
                {
                    msg << "value: " << val;
                }
                else
                {
                    msg << "Error: there is no value";
                }

                msg << "\nkey 2: " << LB_REG_KEY2 << "; ";

                if (ReadUnsignedIntFromRegistry(LB_REG_KEY2, val))
                {
                    msg << "value: " << val;
                }
                else
                {
                    msg << "Error: there is no value";
                }

                MessageBox(ghWnd, msg.str().c_str(), L"Registry", MB_OK);

                hook_ctrlPressed = false;
            }
            else if (hook_altPressed && p->vkCode == 'R') {
                if (DeleteFromRegistry(LB_REG_KEY1) && DeleteFromRegistry(LB_REG_KEY2))
                    MessageBox(ghWnd, L"Deleted successfully", L"Registry", MB_OK);

                else
                    MessageBox(ghWnd, L"Can not delete the values, they might be already deleted",
                        L"Registry", MB_OK);


                hook_altPressed = false;
            }

            else if (p->vkCode == VK_LCONTROL) hook_ctrlPressed = true;
            else if (p->vkCode == VK_LMENU) hook_altPressed = true;

            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (p->vkCode == VK_CONTROL) hook_ctrlPressed = false;
            else if (p->vkCode == VK_MENU) hook_altPressed = false;
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


