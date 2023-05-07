#pragma once
#include "pch.h"

LRESULT CALLBACK TrayWndProc(HWND, UINT, WPARAM, LPARAM);
void TrayAddIcon(HWND hwnd);
void TrayRemoveIcon(HWND hwnd);
