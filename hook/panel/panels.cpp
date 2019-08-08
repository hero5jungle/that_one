#include "panels.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../hack/esp/esp.h"
#include "../../menu/gui/menu.h"
#include "../../sdk/cmat/cmat.h"

CScreenSize gScreen;
unsigned int FocusOverlayPanel = 0;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce ) {
	try {
		const char* panel_name = Int::Panels->GetName( vguiPanel );

		if( !strcmp( "HudScope", panel_name ) && Global.sniper_noscope.value ) {
			return;
		}

		Hook::PaintTraverse.get_original()( pPanels, vguiPanel, forceRepaint, allowForce );

		if( !FocusOverlayPanel ) {
			if( strstr( panel_name, "FocusOverlayPanel" ) ) {
				FocusOverlayPanel = vguiPanel;
				Intro();
			}
		}

		if( FocusOverlayPanel == vguiPanel ) {
			if( Int::Engine->IsDrawingLoadingImage() ) {
				return;
			}

			Int::Panels->SetTopmostPopup( vguiPanel, true );
			//resolution change fix
			CScreenSize newSize;
			Int::Engine->GetScreenSize( newSize.Width, newSize.Height );

			if( newSize.Width != gScreen.Width || newSize.Height != gScreen.Height ) {
				DrawManager::Reload();
			}

			//esp
			if( Int::Engine->IsInGame() ) {
				CBaseEntity* pLocal = Int::EntityList->GetClientEntity( me );

				if( pLocal ) {
					if( !pLocal->IsDormant() )
						if( pLocal->GetLifeState() == LIFE_ALIVE ) {
							ESP::Run( pLocal );
						}
				}
			}

			//menu
			gMenu.GetInput();
			gMenu.Draw();
			Int::Panels->SetMouseInputEnabled( vguiPanel, gMenu.enabled );
			gMenu.EndInput();
		}
	} catch( ... ) {
		Fatal( "Failed PaintTraverse" );
	}
}

void Intro() {
	DrawManager::Initialize();
	gMenu.CreateGUI();
	Global.Sv_cheat.value = false;
	gNetVars.Initialize();
	Materials::Initialize();
	InitTextures();
	Int::Engine->ClientCmd_Unrestricted( "toggleconsole" );
	Int::cvar->ConsoleColorPrintf( Colors::Yellow, "that_one Injected\n" );
}