#include "panels.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../hack/esp/esp.h"
#include "../../menu/gui/menu.h"
#include "../../sdk/cmat/cmat.h"
#include "../../tools/signature/csignature.h"
#include <cstdio>
CScreenSize gScreenSize;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce ) {
  try {
    const char *szName = gInts.Panels->GetName( vguiPanel );
    
    if( !strcmp( "HudScope", szName ) && gCvars.Noscope.value ) {
      return;
    }
    
    VMTManager &hook = VMTManager::GetHook( pPanels );
    hook.GetMethod<void( __thiscall * )( PVOID, unsigned int, bool, bool )>( 41 )( pPanels, vguiPanel, forceRepaint, allowForce );
    static unsigned int FocusOverlay;
    
    if( !FocusOverlay ) {
      if( strcmp( szName, "FocusOverlayPanel" ) == 0 ) {
        FocusOverlay = vguiPanel;
        Intro();
      }
    } else {
      gInts.Panels->SetTopmostPopup( FocusOverlay, true );
    }
    
    if( FocusOverlay != vguiPanel || gInts.Engine->IsDrawingLoadingImage() ) {
      return;
    }
    
    CScreenSize newSize;
    gInts.Engine->GetScreenSize( newSize.iScreenWidth, newSize.iScreenHeight );
    
    if( newSize.iScreenWidth != gScreenSize.iScreenWidth || newSize.iScreenHeight != gScreenSize.iScreenHeight ) {
      DrawManager::Reload();
    }
    
    CBaseEntity *pLocal = gInts.EntList->GetClientEntity( me );
    
    if( pLocal ) {
      if( !pLocal->IsDormant() )
        if( pLocal->GetLifeState() == LIFE_ALIVE ) {
          ESP::Run( pLocal );
        }
    }
    
    gMenu.GetInput();
    gMenu.Draw();
    gInts.Panels->SetMouseInputEnabled( vguiPanel, gMenu.enabled );
    gMenu.EndInput();
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
      
      Fatal( "An update changed the config order\r\nfind your old config renamed as that_one.backup" );
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