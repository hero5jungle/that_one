#include "sdk/sdk.h"
#include "hook/client/client.h"
#include "hook/panel/panels.h"
#include "menu/gui/menu.h"
#include <Windows.h>
#include "hack/event/event.h"
#include "tools/vmt/vmthooks.h"
#include "tools/signature/csignature.h"
#include "sdk/cmat/keyvalues.h"
CGlobalVariables gCvars;
CInterfaces gInts;
CHooks gHooks;

DWORD WINAPI dwMainThread( LPVOID lpArguments ) {
  if( gInts.Client == nullptr ) {
    //Client
    CreateInterface_t ClientFactory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "client.dll" ), "CreateInterface" ) );
    gInts.Client = ( CHLClient * )( ClientFactory( "VClient017", nullptr ) );
    gInts.EntList = ( CEntList * )( ClientFactory( "VClientEntityList003", nullptr ) );
    XASSERT( gInts.Client );
    XASSERT( gInts.EntList );
    //Engine
    CreateInterface_t EngineFactory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "engine.dll" ), "CreateInterface" ) );
    gInts.Engine = ( EngineClient * )( EngineFactory( "VEngineClient013", nullptr ) );
    gInts.EngineTrace = ( IEngineTrace * )( EngineFactory( "EngineTraceClient003", nullptr ) );
    gInts.ModelInfo = ( IVModelInfo * )( EngineFactory( "VModelInfoClient006", nullptr ) );
    gInts.EventManager = ( IGameEventManager2 * )( EngineFactory( "GAMEEVENTSMANAGER002", nullptr ) );
    gInts.RenderView = ( CRenderView * )( EngineFactory( "VEngineRenderView014", nullptr ) );
    gInts.MdlRender = ( CModelRender * )( EngineFactory( "VEngineModel016", nullptr ) );
    XASSERT( gInts.Engine );
    XASSERT( gInts.EngineTrace );
    XASSERT( gInts.ModelInfo );
    XASSERT( gInts.EventManager );
    XASSERT( gInts.RenderView );
    XASSERT( gInts.MdlRender );
    //Surface
    CreateInterface_t VGUIFactory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "vguimatsurface.dll" ), "CreateInterface" ) );
    gInts.Surface = ( ISurface * )( VGUIFactory( "VGUI_Surface030", nullptr ) );
    XASSERT( gInts.Surface );
    //Cvar
    CreateInterface_t CvarFactory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "vstdlib.dll" ), "CreateInterface" ) );
    gInts.cvar = ( ICvar * )( CvarFactory( "VEngineCvar004", nullptr ) );
    XASSERT( gInts.cvar );
    //Materials
    CreateInterface_t MatSysFactory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "materialsystem.dll" ), "CreateInterface" ) );
    gInts.MatSystem = ( CMaterialSystem * )( MatSysFactory( "VMaterialSystem081", nullptr ) );
    XASSERT( gInts.MatSystem );
    
    if( !gInts.Panels ) {
      CreateInterface_t VGUI2Factory = ( CreateInterfaceFn )( GetProcAddress( Signatures::GetModuleHandleSafe( "vgui2.dll" ), "CreateInterface" )
                                                            );
      gInts.Panels = ( IPanel * )( VGUI2Factory( "VGUI_Panel009", nullptr ) );
      XASSERT( gInts.Panels );
      
      if( gInts.Panels ) {
        gHooks.panelHook->Init( gInts.Panels );
        gHooks.panelHook->HookMethod( &Hooked_PaintTraverse, 41 );
        gHooks.panelHook->Rehook();
      }
    }
    
    //globals
    gInts.globals = *( CGlobals ** )( Signatures::GetEngineSignature( "A1 ? ? ? ? 8B 11 68" ) + 8 );
    XASSERT( gInts.globals );
    //
    const DWORD dwClientModeAddress = Signatures::GetClientSignature( "8B 0D ? ? ? ? 8B 02 D9 05" );
    XASSERT( dwClientModeAddress );
    //
    gInts.ClientMode = **( ClientModeShared ** * )( dwClientModeAddress + 2 );
    XASSERT( gInts.ClientMode );
    // material stuff
    Keyvalues::GetOffsets();
    //
    gHooks.FrameStageNotifyThink->Init( gInts.Client );
    gHooks.FrameStageNotifyThink->HookMethod( &FrameStageNotifyThink, 35 );
    gHooks.FrameStageNotifyThink->Rehook();
    //
    gHooks.CreateMove->Init( gInts.ClientMode );
    gHooks.CreateMove->HookMethod( &Hooked_CreateMove, 21 );
    gHooks.CreateMove->Rehook();
    //
    gHooks.DrawModelExucute->Init( gInts.MdlRender );
    gHooks.DrawModelExucute->HookMethod( &Hooked_DrawModelExecute, 19 );
    gHooks.DrawModelExucute->Rehook();
    //
    gEvents.InitEvents();
    HWND thisWindow;
    
    while( true ) {
      if( ( thisWindow = FindWindow( "Valve001", nullptr ) ) ) {
        break;
      }
    }
    
    if( thisWindow ) {
      gMenu.windowProc = ( WNDPROC )( SetWindowLongPtr( thisWindow, GWLP_WNDPROC, ( LONG_PTR )( &Hooked_WndProc ) ) );
    }
  }
  
  return 0;
}

BOOL APIENTRY DllMain( HMODULE hInstance, DWORD dwReason, LPVOID lpReserved ) {
  if( dwReason == DLL_PROCESS_ATTACH ) {
    DisableThreadLibraryCalls( hInstance );
    CreateThread( nullptr, 0, ( LPTHREAD_START_ROUTINE )( dwMainThread ), nullptr, 0, nullptr );
  }
  
  return true;
}


