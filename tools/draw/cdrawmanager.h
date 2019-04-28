#pragma once

#include "../../sdk/sdk.h"
#include "DrawUtils.h"
namespace DrawManager {
  void Initialize();
  void Reload();
  void DrawString( int x, int y, Color clrColor, const wchar_t *pszText );
  void DrawString( int x, int y, Color clrColor, const char *pszText, ... );
  byte GetESPHeight( );
  void DrawLine( int x0, int y0, int x1, int y1, Color clrColor );
  void DrawLineEx( int x, int y, int w, int h, Color clrColor );
  void DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius );
  void DrawRect( int x, int y, int w, int h, Color clrColor );
  void DrawCircle( float x, float y, float r, float s, Color color );
  void DrawCircle( float x, float y, float r, float start, float end, float s, Color color );
  void Rotating_Dot_Circle( float x, float y, float r, float s, Color color );
  void OutlineRect( int x, int y, int w, int h, Color clrColor );
  bool WorldToScreen( Vector &vOrigin, Vector &vScreen );
  void DrawString( int x, int y, Color color, string text, HFont font, bool center = false );
  void DrawString( int x, int y, Color color, const char *pszText, HFont font, bool center = false );
  void DrawBox( Vector vOrigin, Color color, int box_width, int radius );
  extern unsigned long m_Font;
}
