#include "cdrawmanager.h"
#include "../util/Util.h"
#include "../../hook/panel/panels.h"

namespace DrawManager {
  unsigned long m_Font;
#define ESP_HEIGHT 14
  
  void Initialize() {
    if( gInts.Surface == nullptr )
      return;
      
    gFonts.Initialize();
    gTex.Initialize();
    Reload();
  }
  
  void Reload() {
    if( gInts.Surface == nullptr )
      return;
      
    gInts.Engine->GetScreenSize( gScreenSize.iScreenWidth, gScreenSize.iScreenHeight );
    gFonts.Reload();
    gTex.Reload();
  }
  
  
  void DrawString( int x, int y, Color color, string text, HFont font, bool center ) {
    DrawString( x, y, color, text.c_str(), font, center );
  }
  
  void DrawString( int x, int y, Color color, const char *pszText, HFont font, bool center ) {
    if( pszText == nullptr )
      return;
      
    va_list va_alist = nullptr;
    int Wide = 0, Tall = 0;
    char szBuffer[256] = { '\0' };
    wchar_t szString[128] = { '\0' };
    va_start( va_alist, pszText );
    vsprintf_s( szBuffer, pszText, va_alist );
    va_end( va_alist );
    MultiByteToWideChar( CP_UTF8, 0, szBuffer, -1, szString, 128 );
    
    if( center ) {
      gInts.Surface->GetTextSize( font, szString, Wide, Tall );
      x -= Wide / 2;
    }
    
    gInts.Surface->DrawSetTextPos( x, y );
    gInts.Surface->DrawSetTextFont( font );
    gInts.Surface->DrawSetTextColor( color[0], color[1], color[2], color[3] );
    gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
  }
  
  void DrawBox( Vector vOrigin, Color color, int box_width, int radius ) {
    Vector vScreen;
    
    if( !WorldToScreen( vOrigin, vScreen ) )
      return;
      
    int radius2 = radius << 1;
    OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, 0x000000FF );
    OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), 0x000000FF );
    DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, color );
    DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width, color );
    DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2, color );
    DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, color );
  }
  
  void DrawString( int x, int y, Color clrColor, const wchar_t *pszText ) {
    if( pszText == nullptr )
      return;
      
    gInts.Surface->DrawSetTextPos( x, y );
    gInts.Surface->DrawSetTextFont( m_Font );
    gInts.Surface->DrawSetTextColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawPrintText( pszText, wcslen( pszText ) );
  }
  
  void DrawString( int x, int y, Color clrColor, const char *pszText, ... ) {
    if( pszText == nullptr )
      return;
      
    va_list va_alist;
    char szBuffer[1024] = { '\0' };
    wchar_t szString[1024] = { '\0' };
    va_start( va_alist, pszText );
    vsprintf_s( szBuffer, pszText, va_alist );
    va_end( va_alist );
    wsprintfW( szString, L"%S", szBuffer );
    gInts.Surface->DrawSetTextPos( x, y );
    gInts.Surface->DrawSetTextFont( m_Font );
    gInts.Surface->DrawSetTextColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
  }
  
  byte GetESPHeight() {
    return ESP_HEIGHT;
  }
  
  void DrawLine( int x, int y, int x1, int y1, Color clrColor ) {
    gInts.Surface->DrawSetColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawLine( x, y, x1, y1 );
  }
  
  void DrawLineEx( int x, int y, int w, int h, Color clrColor ) {
    gInts.Surface->DrawSetColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawLine( x, y, x + w, y + h );
  }
  
  void DrawRect( int x, int y, int w, int h, Color clrColor ) {
    gInts.Surface->DrawSetColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawFilledRect( x, y, x + w, y + h );
  }
  
  void OutlineRect( int x, int y, int w, int h, Color clrColor ) {
    gInts.Surface->DrawSetColor( clrColor[0], clrColor[1], clrColor[2], clrColor[3] );
    gInts.Surface->DrawOutlinedRect( x, y, x + w, y + h );
  }
  
  void DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius ) {
    Vector vScreen;
    
    if( !WorldToScreen( vOrigin, vScreen ) )
      return;
      
    int radius2 = radius << 1;
    OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, Colors::Black );
    OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), Colors::Black );
    DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, Color( r, g, b, alpha ) );
    DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width, Color( r, g, b, alpha ) );
    DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2, Color( r, g, b, alpha ) );
    DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, Color( r, g, b, alpha ) );
  }
  
  bool WorldToScreen( Vector &vOrigin, Vector &vScreen ) {
    const matrix3x4 &worldToScreen = gInts.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix
    float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
    vScreen.z = 0; //Screen doesn't have a 3rd dimension.
    
    if( w > 0.001 ) { //If the object is within view.
      float fl1DBw = 1 / w; //Divide 1 by the angle.
      vScreen.x = ( gScreenSize.iScreenWidth / 2 ) + ( 0.5 * ( ( worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3] ) * fl1DBw ) * gScreenSize.iScreenWidth + 0.5 ); //Get the X dimension and push it in to the Vector.
      vScreen.y = ( gScreenSize.iScreenHeight / 2 ) - ( 0.5 * ( ( worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3] ) * fl1DBw ) * gScreenSize.iScreenHeight + 0.5 ); //Get the Y dimension and push it in to the Vector.
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
  
  //(0,2pi),(0,-2pi),(-2pi,0),(0,2pi)
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
    //start is left, end goes anti clockwise
    bool flip = false;
    
    for( float a = timer; a < timer + ( 3.141 * 2 ); a += Step ) {
      float xt = r * cos( a ) + x;
      float yt = r * sin( a ) + y;
      DrawRect( xt - 1, yt - 1, 3, 3, color );
      
      if( flip ) {
        float xl = r * cos( a + Step ) + x;
        float yl = r * sin( a + Step ) + y;
        DrawLine( xt, yt, xl, yl, color );
      }
      
      flip = !flip;
    }
  }
  
}