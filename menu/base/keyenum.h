#pragma once
#include <Windows.h>
#include <string>
using namespace std;

class CKey {
	public:
	inline bool AlphaChar( char c ) {
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
	}

	inline bool AcceptedKey( byte c ) {
		if( AlphaChar( c ) ) {
			return true;
		}

		switch( c ) {
			case VK_ESCAPE:
			case VK_TAB:
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_SPACE:
			case VK_LBUTTON:
			case VK_RBUTTON:
			case VK_MBUTTON:
				return true;
		}

		return false;
	}

	wstring KeyToString( byte c ) {
		if( !AcceptedKey( c ) ) {
			return to_wstring( c );
		} else if( AlphaChar( c ) ) {
			return wstring( 1, c );
		}

		switch( c ) {
			case VK_ESCAPE:
				return L"Escape";

			case VK_TAB:
				return L"Tab";

			case VK_CONTROL:
				return L"Control";

			case VK_SHIFT:
				return L"Shift";

			case VK_SPACE:
				return L"Spacebar";

			case VK_LBUTTON:
				return L"Left mouse";

			case VK_RBUTTON:
				return L"Right mouse";

			case VK_MBUTTON:
				return L"Middle mouse";
		}

		return L"";
	}
};
extern CKey gKey;