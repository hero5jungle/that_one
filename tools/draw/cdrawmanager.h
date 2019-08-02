#pragma once

#include "../../sdk/sdk.h"
#include "DrawUtils.h"
namespace DrawManager {
	void Initialize();
	void Reload();
	void DrawString( int x, int y, Color color, std::wstring text, HFont font = gFonts.anon, bool center = false );
	void DrawString( int x, int y, Color color, std::string text, HFont font = gFonts.anon, bool center = false );
	void DrawString( int x, int y, Color color, const char* text, HFont font = gFonts.anon, bool center = false );
	void DrawString( int x, int y, Color color, const wchar_t* text, HFont font = gFonts.anon, bool center = false );

	constexpr byte GetESPHeight() {
		return 14;
	};

	void DrawLine( int x0, int y0, int x1, int y1, Color color );
	void DrawLineEx( int x, int y, int w, int h, Color color );

	void DrawRect( int x, int y, int w, int h, Color color );
	void OutlineRect( int x, int y, int w, int h, Color color );

	void DrawCircle( float x, float y, float r, float s, Color color );
	void DrawCircle( float x, float y, float r, float start, float end, float s, Color color );
	void Rotating_Dot_Circle( float x, float y, float r, float s, Color color );


	bool WorldToScreen( Vector& vOrigin, Vector& vScreen );

	extern unsigned long m_Font;
}
