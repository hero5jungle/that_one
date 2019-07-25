#include "drawutils.h"
#include "../../sdk/sdk.h"

CFonts gFonts;
CTextures gTex;


void CFonts::Initialize() {
	anon = gInts.Surface->CreateFont();
	verdana_bold = gInts.Surface->CreateFont();
	calibri = gInts.Surface->CreateFont();
	calibri_light = gInts.Surface->CreateFont();
	calibri_light_small = gInts.Surface->CreateFont();
}
void CFonts::Reload() {
	gInts.Surface->SetFontGlyphSet( gFonts.anon, "Anonymous Pro", ESP_HEIGHT, 500, 0, 0, FONTFLAG_OUTLINE );
	gInts.Surface->SetFontGlyphSet( gFonts.verdana_bold, "Verdana", 14, 800, 0, 0, FONTFLAG_ANTIALIAS );
	gInts.Surface->SetFontGlyphSet( gFonts.calibri, "Calibri", 24, 500, 0, 0, FONTFLAG_ANTIALIAS );
	gInts.Surface->SetFontGlyphSet( gFonts.calibri_light, "Calibri Light", 24, 250, 0, 0, FONTFLAG_ANTIALIAS );
	gInts.Surface->SetFontGlyphSet( gFonts.calibri_light_small, "Calibri Light", 18, 250, 0, 0, FONTFLAG_ANTIALIAS );
}

void CTextures::Initialize() {
	dark_gray = gInts.Surface->CreateNewTextureID();
}
void CTextures::Reload() {
	static const byte dark_gray_t[4] = { 50, 50, 50, 255 };
	gInts.Surface->DrawSetTextureRGBA( dark_gray, dark_gray_t, 1, 1 );
}


TextureHolder::TextureHolder( const byte* pRawRGBAData, int W, int H ) {
	m_iTexture = gInts.Surface->CreateNewTextureID( true );

	if( !m_iTexture ) {
		return;
	}

	gInts.Surface->DrawSetTextureRGBA( m_iTexture, pRawRGBAData, W, H );
	rawData = pRawRGBAData;
	m_iW = W, m_iH = H;
	m_bValid = true;
}
bool TextureHolder::Draw( int x, int y, Color clr, float scale ) {
	if( !gInts.Surface->IsTextureIDValid( m_iTexture ) ) {
		return false;
	}

	gInts.Surface->DrawSetColor( clr[0], clr[1], clr[2], clr[3] );
	gInts.Surface->DrawSetTexture( m_iTexture );
	gInts.Surface->DrawTexturedRect( x, y, x + (m_iW * scale), y + (m_iH * scale) );
	return true;
}

