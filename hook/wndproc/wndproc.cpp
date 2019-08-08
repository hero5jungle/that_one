#include "wndproc.h"
#include "../../menu/gui/menu.h"
#include "../dx9/dx9.h"

WNDPROC windowProc;

LRESULT __stdcall Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch( uMsg ) {
		case WM_MOUSEWHEEL:
			if( (int)wParam < 0 ) {
				gMenu.mw = e_mw::up;
			} else {
				gMenu.mw = e_mw::down;
			}

			break;

		case WM_KEYDOWN:
			if( wParam > 255 ) {
				break;
			}

			gMenu.keys[wParam] = true, gMenu.last_key = wParam, gMenu.key = wParam;
			break;

		case WM_KEYUP:
			if( wParam > 255 ) {
				break;
			}

			gMenu.keys[wParam] = false;

			if( gMenu.last_key == wParam ) {
				gMenu.last_key = NULL;
			}

		default:
			break;
	}

	return CallWindowProcW( windowProc, hWnd, uMsg, wParam, lParam );
}