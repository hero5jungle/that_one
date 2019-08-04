#pragma once
#include "../headers/vector.h"
typedef unsigned char byte;
struct Color {
	byte rgba[4];
	Color() {
		rgba[0] = 0;
		rgba[1] = 0;
		rgba[2] = 0;
		rgba[3] = 255;
	}
	Color( byte r, byte g, byte b, byte a = 255 ) {
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
	}
	Color( byte b, byte a = 255 ) {
		rgba[0] = b;
		rgba[1] = b;
		rgba[2] = b;
		rgba[3] = a;
	}
	byte operator[]( int i ) {
		return rgba[i];
	}
	bool operator==( Color clr ) {
		return ( rgba[0] == clr[0] ) &&
			( rgba[1] == clr[1] ) &&
			( rgba[2] == clr[2] ) &&
			( rgba[3] == clr[3] );
	}
	bool operator!=( Color clr ) {
		return !( operator==( clr ) );
	}
	int to_int() {
		return int( ( rgba[0] ) << 24 |
			( rgba[1] ) << 16 |
			( rgba[2] ) << 8 |
			( rgba[3] ) );
	}
	int to_dx9() {
		return int( ( rgba[3] ) << 24 |
			( rgba[0] ) << 16 |
			( rgba[1] ) << 8 |
			( rgba[2] ) );
	}
	void from_int( int input ) {
		rgba[0] = ( input >> 24 ) & 0xFF;
		rgba[1] = ( input >> 16 ) & 0xFF;
		rgba[2] = ( input >> 8 ) & 0xFF;
		rgba[3] = input & 0xFF;
	}
	Vector rgb() {
		return { rgba[0] / 255.0f, rgba[1] / 255.0f, rgba[2] / 255.0f };
	}
};
namespace Colors {
	const Color White = Color( 255, 255, 255, 255 );
	const Color Black = Color( 0, 0, 0, 255 );
	const Color Red = Color( 255, 0, 0, 255 );
	const Color Orange = Color( 255, 165, 0, 255 );
	const Color Yellow = Color( 255, 255, 0, 255 );
	const Color Green = Color( 0, 255, 0, 255 );
	const Color Blue = Color( 0, 0, 255, 255 );
	const Color Indigo = Color( 75, 0, 130, 255 );
	const Color Violet = Color( 128, 66, 244, 255 );
	const Color Pink = Color( 255, 192, 203, 255 );
}