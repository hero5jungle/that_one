#include "cdrawmanager.h"
#include "../../hook/panel/panels.h"

namespace DrawManager {
	unsigned long m_Font;

	void Initialize() {
		if( gInts.Surface == nullptr ) {
			return;
		}

		gFonts.Initialize();
		gTex.Initialize();
		Reload();
	}

	void Reload() {
		if( gInts.Surface == nullptr ) {
			return;
		}

		gInts.Engine->GetScreenSize( gScreen.Width, gScreen.Height );
		gFonts.Reload();
		gTex.Reload();
	}

	void DrawString( int x, int y, Color color, std::wstring text, HFont font, bool center ) {
		DrawString( x, y, color, text.c_str(), font, center );
	}

	void DrawString( int x, int y, Color color, std::string text, HFont font, bool center ) {
		DrawString( x, y, color, text.c_str(), font, center );
	}

	void DrawString( int x, int y, Color color, const char* text, HFont font, bool center ) {
		DrawString( x, y, color, ToWstring( text ).c_str(), font, center );
	}

	void DrawString( int x, int y, Color Color, const wchar_t* text, HFont font, bool center ) {
		if( text == nullptr ) {
			return;
		}

		if( center ) {
			int Wide = 0, Tall = 0;
			gInts.Surface->GetTextSize( font, text, Wide, Tall );
			x -= Wide / 2;
		}

		gInts.Surface->DrawSetTextPos( x, y );
		gInts.Surface->DrawSetTextFont( font );
		gInts.Surface->DrawSetTextColor( Color[0], Color[1], Color[2], Color[3] );
		gInts.Surface->DrawPrintText( text, wcslen( text ) );
	}

	void DrawLine( int x, int y, int x1, int y1, Color color ) {
		gInts.Surface->DrawSetColor( color[0], color[1], color[2], color[3] );
		gInts.Surface->DrawLine( x, y, x1, y1 );
	}

	void DrawLineEx( int x, int y, int w, int h, Color color ) {
		gInts.Surface->DrawSetColor( color[0], color[1], color[2], color[3] );
		gInts.Surface->DrawLine( x, y, x + w, y + h );
	}

	void DrawRect( int x, int y, int w, int h, Color color ) {
		gInts.Surface->DrawSetColor( color[0], color[1], color[2], color[3] );
		gInts.Surface->DrawFilledRect( x, y, x + w, y + h );
	}

	void OutlineRect( int x, int y, int w, int h, Color color ) {
		gInts.Surface->DrawSetColor( color[0], color[1], color[2], color[3] );
		gInts.Surface->DrawOutlinedRect( x, y, x + w, y + h );
	}

	bool WorldToScreen( Vector& vOrigin, Vector& vScreen ) {
		const matrix3x4& worldToScreen = gInts.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix
		float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
		vScreen.z = 0; //Screen doesn't have a 3rd dimension.

		if( w > 0.001 ) { //If the object is within view.
			float fl1DBw = 1 / w; //Divide 1 by the angle.
			vScreen.x = ( gScreen.Width / 2 ) + ( 0.5 * ( ( worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3] ) * fl1DBw ) * gScreen.Width + 0.5 ); //Get the X dimension and push it in to the Vector.
			vScreen.y = ( gScreen.Height / 2 ) - ( 0.5 * ( ( worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3] ) * fl1DBw ) * gScreen.Height + 0.5 ); //Get the Y dimension and push it in to the Vector.
			return true;
		}

		return false;
	}

	void DrawCircle( float x, float y, float r, float s, Color color ) {
		float Step = 3.141 * 2.0 / s;

		for( float a = 0; a < ( 3.141 * 2.0 ); a += Step ) {
			float x1 = r * cos( a ) + x;
			float y1 = r * sin( a ) + y;
			float x2 = r * cos( a + Step ) + x;
			float y2 = r * sin( a + Step ) + y;
			DrawLine( x1, y1, x2, y2, color );
		}
	}

	void DrawCircle( float x, float y, float r, float start, float end, float s, Color color ) {
		float Step = 3.141 * 2.0 / s;

		//start is left, end goes anti clockwise
		for( float a = start; a < end; a += Step ) {
			float x1 = r * cos( a ) + x;
			float y1 = r * sin( a ) + y;
			float x2 = r * cos( a + Step ) + x;
			float y2 = r * sin( a + Step ) + y;
			DrawLine( x1, y1, x2, y2, color );
		}
	}

	void Rotating_Dot_Circle( float x, float y, float r, float s, Color color ) {
		float Step = 3.141 / s;
		float timer = gInts.globals->curtime;

		for( float a = timer; a < timer + ( 3.141 * 2 ); a += Step ) {
			float xt = r * cos( a ) + x;
			float yt = r * sin( a ) + y;
			DrawRect( xt - 1, yt - 1, 3, 3, color );
		}
	}

}