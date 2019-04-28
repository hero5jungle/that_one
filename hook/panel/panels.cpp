#include "panels.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../hack/esp/esp.h"
#include "../../menu/gui/menu.h"
#include "../../sdk/cmat/cmat.h"
#include "../../tools/log/log.h"
#include "../../tools/signature/csignature.h"
#include <cstdio>
CScreenSize gScreenSize;

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce ) {
  try {
    const char *szName = gInts.Panels->GetName( vguiPanel );
    
    if( !strcmp( "HudScope", szName ) && gCvars.Noscope.value ) {
      return;
    }
    
    VMTManager &hook = VMTManager::GetHook( pPanels ); //Get a pointer to the instance of your VMTManager with the function GetHook.
    hook.GetMethod<void( __thiscall * )( PVOID, unsigned int, bool, bool )>( 41 )( pPanels, vguiPanel, forceRepaint, allowForce ); //Call the original.
    static unsigned int FocusOverlay;
    
    if( !FocusOverlay ) {
      if( strcmp( szName, "FocusOverlayPanel" ) == 0 ) {
        FocusOverlay = vguiPanel;
        Intro();
      }
    } else
      gInts.Panels->SetTopmostPopup( FocusOverlay, true );
      
    if( FocusOverlay != vguiPanel || gInts.Engine->IsDrawingLoadingImage() )
      return;
      
    CScreenSize newSize;
    gInts.Engine->GetScreenSize( newSize.iScreenWidth, newSize.iScreenHeight );
    
    if( newSize.iScreenWidth != gScreenSize.iScreenWidth || newSize.iScreenHeight != gScreenSize.iScreenHeight )
      DrawManager::Reload();
      
    CBaseEntity *pLocal = gInts.EntList->GetClientEntity( me );
    
    if( pLocal ) {
      if( !pLocal->IsDormant() )
        if( pLocal->GetLifeState() == LIFE_ALIVE )
          ESP::Run( pLocal );
    }
    
    // ========== Update your input FIRST to enable usage throughout your program ========== //
    gMenu.GetInput();
    // - Other cheat code here that requires input can go here
    // ========== Menu code must be called AFTER everything else draws ========== //
    gMenu.Draw();
    gInts.Panels->SetMouseInputEnabled( vguiPanel, gMenu.enabled );
    // ========== I may or may not remove this and find a better method. Call after you're done with input ========== //
    gMenu.EndInput();
  } catch( ... ) {
    Log::Fatal( "Failed PaintTraverse" );
  }
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !! Do NOT forget to initialize your menu, fonts, textures, etc.  !!
// !! This is required for the menu to work                         !!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Intro( ) {
  try {
    DrawManager::Initialize(); //Initalize the drawing class.
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
      
      Log::Fatal( "\r\nAn update changed the config order\r\nfind your old config renamed as that_one.backup" );
    }
    
    gNetVars.Initialize();
    Materials::Initialize();
    InitTextures();
    gInts.Engine->ClientCmd_Unrestricted( "toggleconsole" );
    gInts.cvar->ConsoleColorPrintf( Colors::Yellow, "that_one Injected\n" );
    Log::Msg( "Injection Successful" ); //If the module got here without crashing, it is good day.
  } catch( ... ) {
    Log::Fatal( "Failed Intro" );
  }
}