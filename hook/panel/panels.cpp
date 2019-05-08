#include "panels.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../hack/esp/esp.h"
#include "../../menu/gui/menu.h"
#include "../../sdk/cmat/cmat.h"
#include "../../tools/signature/csignature.h"
CScreenSize gScreenSize;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce ) {
  try {
    const char *panel_name = gInts.Panels->GetName( vguiPanel );
    
    if( !strcmp( "HudScope", panel_name ) && gCvars.sniper_noscope.value ) {
      return;
    }
    
    gHooks.PaintTraverse.get_original()( pPanels, vguiPanel, forceRepaint, allowForce );
    static unsigned int FocusOverlayPanel = 0;
    
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
      gInts.Engine->GetScreenSize( newSize.iScreenWidth, newSize.iScreenHeight );
      
      if( newSize.iScreenWidth != gScreenSize.iScreenWidth || newSize.iScreenHeight != gScreenSize.iScreenHeight ) {
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
  try {
    DrawManager::Initialize();
    gMenu.CreateGUI();
    
    try {
      if( checkExists( "that_one.json" ) ) {
        LoadFromJson();
      } else {
        SaveToJson();
      }
    } catch( ... ) {
      if( rename( "that_one.json", "that_one.backup" ) ) { //non zero on fail
        remove( "that_one.backup" );
        rename( "that_one.json", "that_one.backup" );
      }
      
      SaveToJson();
      MessageBoxA( nullptr, "An update changed the config\r\nfind your old config renamed as that_one.backup", "FATAL ERROR", MB_ICONERROR | MB_TOPMOST );
    }
    
    gNetVars.Initialize();
    Materials::Initialize();
    InitTextures();
    gInts.Engine->ClientCmd_Unrestricted( "toggleconsole" );
    gInts.cvar->ConsoleColorPrintf( Colors::Yellow, "that_one Injected\n" );
  } catch( ... ) {
    Fatal( "Failed Intro" );
  }
}