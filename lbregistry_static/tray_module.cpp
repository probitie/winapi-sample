#include "pch.h"
#include "tray_module.h"
#include "registry_module.h"

static HINSTANCE hInst;
static NOTIFYICONDATA nid;

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