#include "controls.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../gui/menu.h"
#include "icons.h"
static TextureHolder arrow_t, arrowside_t, arrowdown_t;
static TextureHolder colorpicker, brightness, picker_t;

#define LTEXT Color(220)
#define BTEXT Color(150)
#define HTEXT Color(90, 160, 225)
#define CTEXT Color(180)
#define BACKGR Color(65, 65, 72)
inline void ClampFl( float& fl ) {
	if( fl > 1 ) {
		fl = 1;
	} else if( fl < 0 ) {
		fl = 0;
	}
}
Color hsv2rgb( float hue, float saturation, float brightness, int alpha = 255 ) {
	while( hue >= 1 ) {
		hue -= 1;
	}

	while( hue <= 0 ) {
		hue += 1;
	}

	ClampFl( saturation );
	ClampFl( brightness );
	float h = hue == 1.0f ? 0 : hue * 6.0f;
	float f = h - (int)h;
	float p = brightness * ( 1.0f - saturation );
	float q = brightness * ( 1.0f - saturation * f );
	float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

	if( h < 1 ) {
		return Color(
			(unsigned char)( brightness * 255 ),
			(unsigned char)( t * 255 ),
			(unsigned char)( p * 255 ),
			alpha
		);
	} else if( h < 2 ) {
		return Color(
			(unsigned char)( q * 255 ),
			(unsigned char)( brightness * 255 ),
			(unsigned char)( p * 255 ),
			alpha
		);
	} else if( h < 3 ) {
		return Color(
			(unsigned char)( p * 255 ),
			(unsigned char)( brightness * 255 ),
			(unsigned char)( t * 255 ),
			alpha
		);
	} else if( h < 4 ) {
		return Color(
			(unsigned char)( p * 255 ),
			(unsigned char)( q * 255 ),
			(unsigned char)( brightness * 255 ),
			alpha
		);
	} else if( h < 5 ) {
		return Color(
			(unsigned char)( t * 255 ),
			(unsigned char)( p * 255 ),
			(unsigned char)( brightness * 255 ),
			alpha
		);
	} else {
		return Color(
			(unsigned char)( brightness * 255 ),
			(unsigned char)( p * 255 ),
			(unsigned char)( q * 255 ),
			alpha
		);
	}
}
// - To do: Make this part of CDraw::Initialize instead, for the sake of simplicity
void InitTextures() {
	arrow_t = TextureHolder( arrow, 8, 8 );
	arrowside_t = TextureHolder( arrowside, 8, 8 );
	arrowdown_t = TextureHolder( arrowdown, 8, 8 );
	// Hue and saturation gradient
	byte* coloures = new byte[128 * 128 * 4];
	int i = 0;

	for( int h = 0; h < 128; h++ ) {
		for( int w = 0; w < 128; w++ ) {
			Color hsv = hsv2rgb( float( w ) / 128, float( 128 - h ) / 128.f, 1 );
			coloures[i] = hsv[0], coloures[i + 1] = hsv[1], coloures[i + 2] = hsv[2], coloures[i + 3] = hsv[3];
			i += 4;
		}
	}

	i = 0;
	// Value gradient
	byte* value = new byte[32 * 128 * 4];

	for( int h = 0; h < 128; h++ ) {
		for( int w = 0; w < 32; w++ ) {
			Color clr( ( ( 128 - h ) / 128.f ) * 255.f, w > 15 ? 0 : 255 );
			value[i] = clr[0], value[i + 1] = clr[1], value[i + 2] = clr[2], value[i + 3] = clr[3];
			i += 4;
		}
	}

	colorpicker = TextureHolder( coloures, 128, 128 );
	brightness = TextureHolder( value, 32, 128 );
	picker_t = TextureHolder( picker, 8, 8 );
}

// ===== BaseControl =====

void BaseControl::RunControl( int Index ) {
	if( type == e_control::null ) {
		return;
	}

	bool mouse = gMenu.mouseOver( x, y, GetWidth(), GetHeight() );

	if( gMenu.GetFocus() == Index && !( flags & noinput ) && mouse ) {
		HandleInput();
	}

	if( !( flags & nodraw ) ) {
		Draw( mouse );
	}
}

// ===== TabGroup =====

int TabGroup::Draw( bool mouseOver ) {
	h = GetHeight();
	int ty = y;

	for( auto& tab : tabs ) {
		//if (tab->visible != nullptr && !*tab->visible)
		//continue;
		tab->SetWidth( w );
		tab->SetPos( x, ty );
		tab->enabled = tab == active;
		tab->HandleInput();
		tab->Draw( gMenu.mouseOver( x, ty, w, GetTabHeight ) );
		ty += GetTabHeight + spacing;
	}

	return h;
}

void TabGroup::HandleInput() {
	h = GetHeight();

	if( !gMenu.mouseOver( x, y, w, h ) ) {
		return;
	}

	int ty = y;

	for( auto& tab : tabs ) {
		if( gMenu.mouseOver( x, ty, w, GetTabHeight ) && gMenu.mb == e_mb::lclick ) {
			active = tab;
			break;
		}

		ty += GetTabHeight + spacing;
	}
}

// ===== Tab =====

int Tab::Draw( bool mouseOver ) {
	Color clr = enabled ? LTEXT : BTEXT;

	if( mouseOver ) {
		DrawManager::DrawRect( x, y, w, h, Color( 20 ) );
	}

	DrawManager::DrawString( x + 20, y + 2, clr, name, gFonts.calibri_light );
	return h;
}

// ===== Groupbox =====

int Groupbox::Draw( bool mouseOver ) {
	// Initializing our height variable
	GetHeight();
	int strw, strh;
	Int::Surface->GetTextSize( gFonts.verdana_bold, ToWstring( name ).c_str(), strw, strh );
	y += ( strh / 2 );
	DrawManager::DrawLine( x, y, x, y + h, Color( 80 ) );
	DrawManager::DrawLine( x + w, y, x + w, y + h, Color( 80 ) );
	DrawManager::DrawLine( x, y + h, x + w, y + h, Color( 80 ) );
	DrawManager::DrawString( x + ( w / 2 ) - ( strw / 2 ), y - ( strh / 2 ), Color( 110 ), name, gFonts.verdana_bold );
	DrawManager::DrawLine( x, y, x + ( w / 2 ) - ( strw / 2 ) - 5, y, Color( 80 ) );
	DrawManager::DrawLine( x + ( w / 2 ) + ( strw / 2 ) + 5, y, x + w, y, Color( 80 ) );
	int cx = x + SPACING, cy = y + SPACING;

	for( auto& i : children ) {
		i->SetPos( cx, cy );
		i->SetWidth( w - ( SPACING * 2 ) );
		bool over = gMenu.mouseOver( cx, cy, i->GetWidth(), i->GetHeight() );
		bool getInput = !( i->flags & noinput ) && over && !gMenu.IsDialogOpen();
		i->Draw( getInput );
		cy += i->GetHeight() + SPACING;
	}

	y -= ( strh / 2 );
	return h;
}

void Groupbox::HandleInput() {
	int strw, strh;
	Int::Surface->GetTextSize( gFonts.verdana_bold, ToWstring( name ).c_str(), strw, strh );
	int cx = x + SPACING, cy = y + SPACING + ( strh / 2 );

	for( auto& i : children ) {
		i->SetPos( cx, cy );
		i->SetWidth( w - ( SPACING * 2 ) );
		bool over = gMenu.mouseOver( cx, cy, i->GetWidth(), i->GetHeight() );
		bool getInput = !( i->flags & noinput ) && over && !gMenu.IsDialogOpen();

		if( getInput ) {
			i->HandleInput();
		}

		cy += i->GetHeight() + SPACING;
	}
}

int Groupbox::GetHeight() {
	h = SPACING;

	for( auto& i : children ) {
		if( i->visible && !*i->visible ) {
			continue;
		}

		h += i->GetHeight() + SPACING;
	}

	int strw, strh;
	Int::Surface->GetTextSize( gFonts.verdana_bold, ToWstring( name ).c_str(), strw, strh );
	return h + ( strh / 2 );
}

// ===== Checkbox =====

int Checkbox::Draw( bool mouseOver ) {
	if( value == -1 ) {
		DrawManager::DrawString( x, y - 1, CTEXT, name, gFonts.verdana_bold );
		return h;
	}

	if( value ) {
		DrawManager::DrawRect( x + 2, y + 2, h - 4, h - 4, Color( 35, 135, 230 ) );
	}

	DrawManager::OutlineRect( x, y, h, h, Color( 100 ) );
	DrawManager::DrawString( x + 18, y - 1, mouseOver ? HTEXT : CTEXT, name, gFonts.verdana_bold );
	return h;
}

void Checkbox::HandleInput() {
	if( value != -1 && ( gMenu.mb == e_mb::lclick || gMenu.mb == e_mb::rclick ) ) {
		value = !value;
	}
}

bool Checkbox::QuickReturn( bool Value, int X, int Y, int W ) {
	int Width = W == -1 ? w : W;
	bool hovered = gMenu.mouseOver( X, Y, Width, h );
	value = Value, x = X, y = Y, w = Width;
	Draw( hovered );

	if( hovered ) {
		HandleInput();
	}

	return value;
}

// ===== DrawPanel =====

int DrawPanel::Draw( bool mouseOver ) {
	// Draw a control panel, then draw our stuff within it
	int topbar = gMenu.style->ControlPanel( x, y, w, h, name );
	draw( x, y + topbar, w, h - topbar );
	return h;
}

// ===== Slider =====

int Slider::Draw( bool mouseOver ) {
	int nw = w - 30;
	Color clr = mouseOver ? HTEXT : CTEXT;
	DrawManager::DrawString( x, y, clr, name, gFonts.verdana_bold );
	DrawManager::DrawRect( x, y + 17, nw, 4, BACKGR );
	int percent = nw * ( value - min ) / ( max - min );

	if( mouseOver ) {
		DrawManager::DrawRect( x, y + 17, percent, 4, HTEXT );
		arrow_t.Draw( x + percent - 3, y + 17, CTEXT );
	} else {
		DrawManager::DrawRect( x, y + 17, percent, 4, Color( 85, 85, 95 ) );
	}

	DrawManager::DrawString( x + nw + 3, y + 9, Color( 125, 125, 130 ), std::to_wstring( value ), gFonts.verdana_bold );
	return h;
}

void Slider::HandleInput() {
	if( gMenu.mb == e_mb::lclick || gMenu.mw == e_mw::up ) {
		value -= step;
	} else if( gMenu.mb == e_mb::rclick || gMenu.mw == e_mw::down ) {
		value += step;
	}

	if( value > max ) {
		value = min;
	} else if( value < min ) {
		value = max;
	}
}

// ===== Listbox =====
#define TEXTH 15
int Listbox::Draw( bool mouseOver ) {
	Color clr = mouseOver ? HTEXT : CTEXT;
	DrawManager::DrawString( x, y, clr, name, gFonts.verdana_bold );
	gMenu.style->DialogButton( x, y + TEXTH, w, list[value].c_str(), mouseOver );
	arrowdown_t.Draw( x + w - 10, y + TEXTH + 6, Color( 125, 125, 140 ) );
	return h;
}

void DrawListbox( void* data, size_t Index );
static Dialog listDlg( DrawListbox );

void Listbox::HandleInput() {
	if( value > list.size() ) {
		value = list.size() - 1;
	}

	if( gMenu.mb != e_mb::lclick ) {
		return;
	}

	listDlg.data = this;
	gMenu.OpenDialog( listDlg );
}

void DrawListbox( void* data, size_t Index ) {
	BaseControl* control = (BaseControl*)data;

	if( control == nullptr || control->type != e_control::listbox ) {
		return gMenu.CloseDialog( Index );
	}

	Listbox* listbox = (Listbox*)control;
	int x = listbox->x, y = listbox->y + listbox->GetHeight(), w = listbox->GetWidth(), h = listbox->list.size() * 16 + 20;

	if( gMenu.mb == e_mb::lclick && !gMenu.mouseOver( x, y - listbox->GetHeight(), w, h + listbox->GetHeight() ) ) {
		return gMenu.CloseDialog( Index );
	}

	gMenu.style->Dialog( x, y, w, h );
	x += 10, y += 8, w -= 20, h -= 20;

	for( size_t i = 0; i < listbox->list.size(); i++ ) {
		if( gMenu.mouseOver( x, y, w, 15 ) ) {
			if( gMenu.mb == e_mb::lclick ) {
				listbox->value = i;
				return gMenu.CloseDialog( Index );
			}

			DrawManager::DrawRect( x, y, w, 16, Color( 44, 44, 55 ) );
		}

		DrawManager::DrawString( x, y + 1, CTEXT, listbox->list[i], gFonts.verdana_bold );
		y += 16;
	}
}
int ColorPicker::Draw( bool mouseOver ) {
	DrawManager::DrawString( x, y, mouseOver ? HTEXT : CTEXT, name, gFonts.verdana_bold );
	DrawManager::DrawRect( x, y + 17, w, 5, get_color() );

	if( !mouseOver ) {
		DrawManager::DrawRect( x, y + 17, w, 5, Color( 0, 150 ) );
	}

	return h;
}

void DrawColorBox( void* data, size_t Index );
static Dialog colorDlg( DrawColorBox );

void ColorPicker::HandleInput() {
	if( gMenu.mb != e_mb::lclick ) {
		return;
	}

	colorDlg.x = x, colorDlg.y = y;
	colorDlg.data = this;
	gMenu.OpenDialog( colorDlg );
}

Color ColorPicker::get_color() {
	if( rainbow ) {
		float red = sin( 3.141 / 2 * Int::globals->curtime + 2 ) * 127 + 128;
		float green = sin( 3.141 / 2 * Int::globals->curtime + 0 ) * 127 + 128;
		float blue = sin( 3.141 / 2 * Int::globals->curtime + 4 ) * 127 + 128;
		return Color( red, green, blue );
	} else {
		return color;
	}
}

void DrawColorBox( void* data, size_t Index ) {
	BaseControl* control = (BaseControl*)data;

	if( control == nullptr || control->type != e_control::colorpicker ) {
		return gMenu.CloseDialog( Index );
	}

	if( gMenu.mb == e_mb::lclick && !gMenu.mouseOver( colorDlg.x, colorDlg.y, 175, 200 ) ) {
		return gMenu.CloseDialog( Index );
	}

	gMenu.style->Dialog( colorDlg.x, colorDlg.y, 175, 200 );
	int X = colorDlg.x + 2, Y = colorDlg.y + 2;// , W = 171, H = 181;
	static int ccursorx = 50, ccursory = 50;
	static int vcursory = 0;
	static bool mColorbox = false, mValue = false;

	if( gMenu.mb == e_mb::lclick && gMenu.mouseOver( X + 5, Y + 30, colorpicker.GetWidth(), colorpicker.GetHeight() ) ) {
		mColorbox = true;
	} else if( gMenu.mb != e_mb::ldown ) {
		mColorbox = false;
	}

	if( gMenu.mb == e_mb::lclick && gMenu.mouseOver( X + colorpicker.GetWidth() + 10, Y + 30, 15, brightness.GetHeight() ) ) {
		mValue = true;
	} else if( gMenu.mb != e_mb::ldown ) {
		mValue = false;
	}

	if( mColorbox ) {
		ccursorx = gMenu.mouse.x - X - 5, ccursory = gMenu.mouse.y - Y - 30;

		if( ccursorx < 0 ) {
			ccursorx = 0;
		} else if( ccursorx > colorpicker.GetWidth() ) {
			ccursorx = colorpicker.GetWidth();
		}

		if( ccursory < 0 ) {
			ccursory = 0;
		} else if( ccursory > colorpicker.GetHeight() - 1 ) {
			ccursory = colorpicker.GetHeight() - 1;
		}
	} else if( mValue ) {
		vcursory = gMenu.mouse.y - Y - 30;

		if( vcursory < 0 ) {
			vcursory = 0;
		} else if( vcursory > brightness.GetHeight() - 1 ) {
			vcursory = brightness.GetHeight() - 1;
		}
	}

	ColorPicker* color = (ColorPicker*)control;
	static Checkbox checkbox( "Default" );
	color->bDef = checkbox.QuickReturn( color->bDef, X + 5, Y + 30 + colorpicker.GetHeight() + 5 );
	static Checkbox rainbow( "Rainbow" );
	color->rainbow = rainbow.QuickReturn( color->rainbow, X + 5, Y + 30 + colorpicker.GetHeight() + 25 );

	if( !color->bDef )
		color->color = hsv2rgb(
		float( ccursorx ) / colorpicker.GetWidth(),
		1.f - ( float( ccursory ) / colorpicker.GetHeight() ),
		1.f - ( float( vcursory ) / brightness.GetHeight() ) );
	else {
		color->color = color->cDef;
	}

	DrawManager::OutlineRect( X + 4, Y + 29, colorpicker.GetWidth() + 2, colorpicker.GetHeight() + 2, Color( 58, 58, 70 ) );
	colorpicker.Draw( X + 5, Y + 30, Color( 255 - ( float( vcursory ) / brightness.GetHeight() ) * 255 ) );
	DrawManager::OutlineRect( X + colorpicker.GetWidth() + 9, Y + 29, 16 + 2, brightness.GetHeight() + 2, Color( 58, 58, 70 ) );
	brightness.Draw( X + colorpicker.GetWidth() + 10, Y + 30 );
	DrawManager::OutlineRect( X + 4, Y + 4, colorpicker.GetWidth() + 23, 22, Color( 58, 58, 70 ) );
	DrawManager::DrawRect( X + 5, Y + 5, colorpicker.GetWidth() + 21, 20, color->get_color() );
	picker_t.Draw( X + 5 + ccursorx - 4, Y + 30 + ccursory - 4 );
	arrowside_t.Draw( X + colorpicker.GetWidth() + 30, Y + 30 + vcursory );
}
// ===== KeyBind =====

int KeyBind::Draw( bool mouseOver ) {
	Color clr = mouseOver ? HTEXT : CTEXT;
	DrawManager::DrawString( x, y, clr, name, gFonts.verdana_bold );
	DrawManager::DrawRect( x, y + TEXTH, w, TEXTH, Color( 42, 42, 48 ) );
	DrawManager::OutlineRect( x, y + TEXTH, w, TEXTH, Color( 60 ) );
	DrawManager::DrawString( x + 3, y + TEXTH, mouseOver ? clr : Color( 125, 125, 130 ), gKey.KeyToString( key ), gFonts.verdana_bold );
	DrawManager::DrawRect( x + w - 10, y + TEXTH + 5, 6, 1, Color( 125, 125, 140 ) );
	DrawManager::DrawRect( x + w - 10, y + TEXTH + 7, 6, 1, Color( 125, 125, 140 ) );
	DrawManager::DrawRect( x + w - 10, y + TEXTH + 9, 6, 1, Color( 125, 125, 140 ) );
	return h;
}

void DrawKeyBindBox( void* data, size_t Index );
static Dialog keyDlg( DrawKeyBindBox );

void KeyBind::HandleInput() {
	if( gMenu.mb != e_mb::lclick ) {
		return;
	}

	keyDlg.x = x, keyDlg.y = y;
	keyDlg.h = 100;
	keyDlg.data = this;
	gMenu.OpenDialog( keyDlg );
}
bool KeyBind::UpdateKeys() {
	if( !gMenu.last_key || !gKey.AcceptedKey( gMenu.last_key ) ) {
		return false;
	}

	if( gMenu.last_key == VK_ESCAPE ) {
		return true;
	}

	key = gMenu.last_key;
	return true;
}

bool KeyBind::KeyDown() {
	switch( mode ) {
		case e_kbmode::disabled:
			return false;

		case e_kbmode::always:
			return true;

		case e_kbmode::hold:
			return gMenu.keys[key];

		case e_kbmode::toggle:
			if( !gMenu.keys[key] ) {
				keyHeld = false;
			}

			if( !keyHeld && gMenu.keys[key] ) {
				toggled = !toggled;
				keyHeld = true;
			}

			return toggled;
	}

	return false;
}

void DrawKeyBindBox( void* data, size_t Index ) {
	BaseControl* control = (BaseControl*)data;

	if( control == nullptr || control->type != e_control::keybind ) {
		return gMenu.CloseDialog( Index );
	}

	static bool bWait = false;

	if( gMenu.mb == e_mb::lclick && !bWait && !gMenu.mouseOver( keyDlg.x, keyDlg.y, keyDlg.w, keyDlg.h ) ) {
		return gMenu.CloseDialog( Index );
	}

	KeyBind* kb = (KeyBind*)control;
	gMenu.style->Dialog( keyDlg.x, keyDlg.y, keyDlg.w, keyDlg.h );
	int x = keyDlg.x + 2, y = keyDlg.y + 2, w = keyDlg.w - 4, h = keyDlg.h - 4;
	DrawManager::DrawString( x + 5, y + 5, Color( 100 ), kb->name, gFonts.verdana_bold );
	y += 21, h -= 21;
	bool hover_setkey = gMenu.mouseOver( x + 6, y + 6, w - 12, 18 );

	if( !bWait && hover_setkey && gMenu.mb == e_mb::lclick ) {
		bWait = true;
	}

	if( bWait ) {
		bWait = !kb->UpdateKeys();
	}

	DrawManager::OutlineRect( x + 6, y + 6, w - 12, 18, Color( 80 ) );
	DrawManager::DrawRect( x + 7, y + 7, w - 14, 16, Color( hover_setkey ? 65 : 40 ) );
	DrawManager::DrawString( x + 9, y + 8, Color( 200 ), bWait ? L"Select a key (or Escape)" : gKey.KeyToString( kb->key ), gFonts.verdana_bold );
	static Listbox list( "Setting", { "Disabled", "Always on", "Hold key", "Toggle with key" } );
	list.SetPos( x + 6, y + 30 ), list.SetWidth( w - 12 );
	static bool list_open = false;

	if( gMenu.GetFocus() == Index && list_open ) {
		kb->mode = (e_kbmode)list.value, list_open = false;
	}

	list_open = gMenu.GetFocus() != Index;
	list.value = (int)kb->mode;
	list.RunControl( Index );
}
int Functionbox::Draw( bool mouseOver ) {
	if( mouseOver ) {
		DrawManager::DrawRect( x + 2, y + 2, h - 4, h - 4, Color( 35, 135, 230 ) );
	}

	DrawManager::OutlineRect( x, y, h, h, Color( 100 ) );
	DrawManager::DrawString( x + 32, y + 7, mouseOver ? HTEXT : CTEXT, name, gFonts.verdana_bold );
	return h;
}

void Functionbox::HandleInput() {
	if( gMenu.mb == e_mb::lclick || gMenu.mb == e_mb::rclick ) {
		func();
	}
}