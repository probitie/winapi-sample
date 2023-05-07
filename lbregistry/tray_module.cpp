#include "pch.h"
#include "tray_module.h"
#include "registry_module.h"

static HINSTANCE hInst;
static NOTIFYICONDATA nid;

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_APP && lParam == WM_LBUTTONUP)
    {
        std::wstringstream msg{};
        uint32_t result{};

        const char* key = "key";

        // it works

        // test write to registry
        //WriteUnsignedIntToRegistry(key, 11023);

        //test delete
        //DeleteFromRegistry(key);

        msg << "Key: " << key << "; ";
    	if(ReadUnsignedIntFromRegistry("key", result))
        {
            msg << "Found a value: " << result;
        }
    	else
        {
            msg << "Error: no value with such a key";
        }

        MessageBox(hWnd, msg.str().c_str(), L"Tray Icon", MB_OK);

    }
    return 0;
}

void TrayAddIcon(HWND hWnd)
{
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE
        | NIF_TIP;
    nid.uCallbackMessage = WM_APP;
    nid.hIcon = LoadIcon(hInst, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Registry Tray App");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void TrayRemoveIcon(HWND hWnd)
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}