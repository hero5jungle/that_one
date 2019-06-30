#include "panels.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../hack/esp/esp.h"
#include "../../menu/gui/menu.h"
#include "../../sdk/cmat/cmat.h"
#include "../../tools/signature/csignature.h"

CScreenSize gScreen;
unsigned int FocusOverlayPanel = 0;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce ) {
  try {
    const char *panel_name = gInts.Panels->GetName( vguiPanel );
    
    if( !strcmp( "HudScope", panel_name ) && gCvars.sniper_noscope.value ) {
      return;
    }
    
    gHooks.PaintTraverse.get_original()( pPanels, vguiPanel, forceRepaint, allowForce );
    
    if( !FocusOverlayPanel ) {
      if( strstr( panel_name, "FocusOverlayPanel" ) ) {
        FocusOverlayPanel = vguiPanel;
        Intro();
      }
    }
    
    if( FocusOverlayPanel == vguiPanel ) {
      if( gInts.Engine->IsDrawingLoadingImage() ) {
        return;
      }
      
      gInts.Panels->SetTopmostPopup( vguiPanel, true );
      //resolution change fix
      CScreenSize newSize;
      gInts.Engine->GetScreenSize( newSize.Width, newSize.Height );
      
      if( newSize.Width != gScreen.Width || newSize.Height != gScreen.Height ) {
        DrawManager::Reload();
      }
      
      //esp
      if( gInts.Engine->IsInGame() ) {
        CBaseEntity *pLocal = gInts.EntList->GetClientEntity( me );
        
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
      gInts.Panels->SetMouseInputEnabled( vguiPanel, gMenu.enabled );
      gMenu.EndInput();
    }
  } catch( ... ) {
    Fatal( "Failed PaintTraverse" );
  }
}

void Intro() {
    DrawManager::Initialize();
    gMenu.CreateGUI();
    gNetVars.Initialize();
    Materials::Initialize();
    InitTextures();
    gInts.Engine->ClientCmd_Unrestricted( "toggleconsole" );
    gInts.cvar->ConsoleColorPrintf( Colors::Yellow, "that_one Injected\n" );
}