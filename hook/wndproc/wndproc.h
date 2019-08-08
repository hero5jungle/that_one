#pragma once
#include "../../sdk/sdk.h"

extern WNDPROC windowProc;

LRESULT __stdcall Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
