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

// include GDI+
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

// to block access to cv and atomic image load status

// TODO выбрать либо мьютекс либо евент
// TODO добавить свой логгер (logger.h)

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> imageDataReady = false;



int width1{}, height1{}, channels1{};
unsigned char *data1{};
int width2{}, height2{}, channels2{};
unsigned char *data2{};
Bitmap* bmp1;
Bitmap* bmp2;






/* TODO
 * проблема - если в потоке открыть изображение с stb_image - оно корректно отображается на экране
 * если переслать изображение по сети и сравнить память с загруженым с stb_image - все норм
 * однако если попробовать переданое по сети нарисовать - оно не рисуется полностью,
 * оставляя серое пятно на оставшемся его пространстве
 *
 *
 *
 * еще надо для каждой части лаб создать приложение - тест
 *
 * еще, либо Condition Variable либо Мьютекс!
 *
 * добавить CreateThread into receiveimage
 *
 *
 *
 *  проверить правильно ли умножать width*height*channels
 *
 * или попробовать передавать битмап
 *
 * или вообще весь файл(костыль!)
 *
 * или попробовать повызывать функции отдельно без потоков и посмотреть правильно ли результирующее изображение
 *
 * или попробовать с бмп(по сути уже написал про это)
 *
 */





void ImageReceiverThread(HWND hWindow)
{
    if(!imageDataReady.load())
    {
        
        //data1 = receive_image(LB_PORT, &width1, &height1, &channels1);
        //imageDataReady.store(true);
        // redraw window to display image (WM_PAINT)

        // testing result - data1 is identical to data2
    	//for testing
            // 2 lines for debug incorrect image display
        //imageDataReady.store(true);

        

        //TODO попробовать скопировать это дело
        //unsigned char* data_copy_in_thread = new unsigned char[width1 * height1 * channels1];
        //std::memcpy(data_copy_in_thread, data1, width1 * height1 * channels1);
        //для теста - вместо получения по сети загрузить с файла
        //delete[] data1;

        //data1 = data_copy_in_thread;

        //был data2 внизу//
        //data2 = stbi_load("D:\\files\\testimg.jpg", &width2, &height2, &channels2, 0);
        // assert loaded and received images are equal
        //LB_ASSERT(memcmp(data1, data2, width2 * height2) == 0, "sending by localhost failed - resulting image is not equal to the origin");


        //RedrawWindow(hWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    }
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
    bool bImgStarted = false;
    std::thread img1Thread;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // start image thread
        if (!bImgStarted)
        {
            img1Thread = std::thread(ImageReceiverThread, msg.hwnd);
            bImgStarted = true;
        }

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
    case WM_PAINT: // it is called once on the startup as I see
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // wait until we receive an obeme.jpg
            
            // TODO write Image class and rename snake case to camel case in lbdll
            // TODO and use namespace | LB is a reserved name already for macro btw
            // TODO also incapsulate host/port settings 
            //process_image(data1, width1, height1, channels1,
            //   [](auto &r, auto &g, auto &b){})
            if (true)//imageDataReady.load())
            {
              
                
                // change image to black rectangle for testing
                //process_image(data1, width1, height1, channels1,
                //    [](auto& r, auto& g, auto& b) {r = 0; g = 0; b = 0; });
                //unsigned char* data1_copy = new unsigned char[width1 * height1 * channels1];
                //std::memcpy(data1_copy, data1, width1 * height1 * channels1);
                data1 = receive_image(LB_PORT, &width1, &height1, &channels1);

                data2 = stbi_load("D:\\files\\testimg.jpg", &width2, &height2, &channels2, 0);
				// assert loaded and received images are equal
				LB_ASSERT(memcmp(data1, data2, width1 * height1 * channels1) == 0, "sending by localhost failed - resulting image is not equal to the origin");


            	bmp1 = new Bitmap(width1, height1, width1 * channels1, PixelFormat32bppRGB, (BYTE*)data1);
            	//bmp2 = new Bitmap(width2, height2, width2 * channels2, PixelFormat32bppRGB, (BYTE*)data2);

                Gdiplus::Graphics graphics(hdc);

                // Отрисуйте изображения на окне
                graphics.DrawImage(bmp1, 0, 0);
                //delete[] data1_copy;
                //graphics.DrawImage(bmp2, width1, 0);

                // this code is for test if image receiving worked
                //int val = count_colored_pixels(data1, width1, height1, channels1, 0, 0, 0);
                //std::wstringstream out{};
                //out << "result is " << val;
                //MessageBox(hWnd, out.str().c_str(), L"колво точек", MB_OK);

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
