#include "drawutils.h"
#include "../../sdk/sdk.h"

CFonts gFonts;
CTextures gTex;


void CFonts::Initialize() {
	anon = Int::Surface->CreateFont();
	verdana_bold = Int::Surface->CreateFont();
	calibri = Int::Surface->CreateFont();
	calibri_light = Int::Surface->CreateFont();
	calibri_light_small = Int::Surface->CreateFont();
}
void CFonts::Reload() {
	Int::Surface->SetFontGlyphSet( gFonts.anon, "Anonymous Pro", ESP_HEIGHT, 500, 0, 0, FONTFLAG_OUTLINE );
	Int::Surface->SetFontGlyphSet( gFonts.verdana_bold, "Verdana", 14, 800, 0, 0, FONTFLAG_ANTIALIAS );
	Int::Surface->SetFontGlyphSet( gFonts.calibri, "Calibri", 24, 500, 0, 0, FONTFLAG_ANTIALIAS );
	Int::Surface->SetFontGlyphSet( gFonts.calibri_light, "Calibri Light", 24, 250, 0, 0, FONTFLAG_ANTIALIAS );
	Int::Surface->SetFontGlyphSet( gFonts.calibri_light_small, "Calibri Light", 18, 250, 0, 0, FONTFLAG_ANTIALIAS );
}

void CTextures::Initialize() {
	dark_gray = Int::Surface->CreateNewTextureID();
}
void CTextures::Reload() {
	static const byte dark_gray_t[4] = { 50, 50, 50, 255 };
	Int::Surface->DrawSetTextureRGBA( dark_gray, dark_gray_t, 1, 1 );
}


TextureHolder::TextureHolder( const byte* pRawRGBAData, int W, int H ) {
	m_iTexture = Int::Surface->CreateNewTextureID( true );

	if( !m_iTexture ) {
		return;
	}

	Int::Surface->DrawSetTextureRGBA( m_iTexture, pRawRGBAData, W, H );
	rawData = pRawRGBAData;
	m_iW = W, m_iH = H;
	m_bValid = true;
}
bool TextureHolder::Draw( int x, int y, Color clr, float scale ) {
	if( !Int::Surface->IsTextureIDValid( m_iTexture ) ) {
		return false;
	}

	Int::Surface->DrawSetColor( clr[0], clr[1], clr[2], clr[3] );
	Int::Surface->DrawSetTexture( m_iTexture );
	Int::Surface->DrawTexturedRect( x, y, x + ( m_iW * scale ), y + ( m_iH * scale ) );
	return true;
}

