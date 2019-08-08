#include "sdk/sdk.h"
#include "hook/client/client.h"
#include "hook/panel/panels.h"
#include "hook/dx9/dx9.h"
#include "hook/wndproc/wndproc.h"
#include "menu/gui/menu.h"
#include "hack/event/event.h"
#include "sdk/cmat/keyvalues.h"
#include <thread>

#define Interface(dll) (CreateInterfaceFn)(GetProcAddress(Signatures::GetModuleHandleSafe( dll ), "CreateInterface"))

DWORD WINAPI dwMainThread( LPVOID lpArguments ) {
	//Client
	CreateInterfaceFn ClientFactory = Interface( L"client.dll" );
	Int::Client = (CHLClient*)( ClientFactory( "VClient017", nullptr ) );
	Int::EntityList = (CEntList*)( ClientFactory( "VClientEntityList003", nullptr ) );
	Int::Prediction = (IPrediction*)( ClientFactory( "VClientPrediction001", nullptr ) );
	Int::GameMovement = (IGameMovement*)( ClientFactory( "GameMovement001", nullptr ) );
	XASSERT( Int::Client );
	XASSERT( Int::EntityList );
	XASSERT( Int::Prediction );
	XASSERT( Int::GameMovement );
	//Engine
	CreateInterfaceFn EngineFactory = Interface( L"engine.dll" );
	Int::Engine = (EngineClient*)( EngineFactory( "VEngineClient013", nullptr ) );
	Int::EngineTrace = (IEngineTrace*)( EngineFactory( "EngineTraceClient003", nullptr ) );
	Int::ModelInfo = (IVModelInfo*)( EngineFactory( "VModelInfoClient006", nullptr ) );
	Int::EventManager = (IGameEventManager2*)( EngineFactory( "GAMEEVENTSMANAGER002", nullptr ) );
	Int::RenderView = (CRenderView*)( EngineFactory( "VEngineRenderView014", nullptr ) );
	Int::MdlRender = (CModelRender*)( EngineFactory( "VEngineModel016", nullptr ) );
	XASSERT( Int::Engine );
	XASSERT( Int::EngineTrace );
	XASSERT( Int::ModelInfo );
	XASSERT( Int::EventManager );
	XASSERT( Int::RenderView );
	XASSERT( Int::MdlRender );
	//Surface
	CreateInterfaceFn VGUIFactory = Interface( L"vguimatsurface.dll" );
	Int::Surface = (ISurface*)( VGUIFactory( "VGUI_Surface030", nullptr ) );
	XASSERT( Int::Surface );
	//Cvar
	CreateInterfaceFn CvarFactory = Interface( L"vstdlib.dll" );
	Int::cvar = (ICvar*)( CvarFactory( "VEngineCvar004", nullptr ) );
	XASSERT( Int::cvar );
	//Materials
	CreateInterfaceFn MatSysFactory = Interface( L"materialsystem.dll" );
	Int::MatSystem = (CMaterialSystem*)( MatSysFactory( "VMaterialSystem081", nullptr ) );
	XASSERT( Int::MatSystem );
	CreateInterfaceFn VGUI2Factory = Interface( L"vgui2.dll" );
	Int::Panels = (IPanel*)( VGUI2Factory( "VGUI_Panel009", nullptr ) );
	XASSERT( Int::Panels );
	//globals
	Int::globals = *(CGlobals * *)( Signatures::GetEngineSignature( "A1 ? ? ? ? 8B 11 68" ) + 8 );
	XASSERT( Int::globals );
	//
	const DWORD ClientMode = Signatures::GetClientSignature( "8B 0D ? ? ? ? 8B 02 D9 05" );
	XASSERT( ClientMode );
	Int::ClientMode = **(ClientModeShared * **)( ClientMode + 2 );
	XASSERT( Int::ClientMode );
	//glow
	Int::GlowManager = *(CGlowObjectManager * *)( Signatures::GetClientSignature( "8B 0D ? ? ? ? A1 ? ? ? ? 56 8B 37" ) + 0x2 );
	XASSERT( Int::GlowManager );
	//dx9
	Int::DirectXDevice = **(DWORD **)( Signatures::GetDirectSignature( "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) + 0x1 );
	XASSERT( Int::DirectXDevice );
	// material stuff
	Keyvalues::GetOffsets();
	//
	Hook::PaintTraverse.setup( Int::Panels, gOffsets::PaintTraverse, &Hooked_PaintTraverse );
	//
	Hook::FrameStageNotifyThink.setup( Int::Client, gOffsets::FrameStageNotifyThink, &Hooked_FrameStageNotifyThink );
	//
	Hook::CreateMove.setup( Int::ClientMode, gOffsets::CreateMove, &Hooked_CreateMove );
	//
	Hook::DrawModelExecute.setup( Int::MdlRender, gOffsets::DrawModelExecute, &Hooked_DrawModelExecute );
	//
	Hook::dx9.setup((DWORD * *)Int::DirectXDevice);
	Hook::EndScene.setup(gOffsets::EndSceneOffset,&Hooked_EndScene);
	Hook::Reset.setup( gOffsets::ResetOffset, &Hooked_Reset );
	//
	gEvents.InitEvents();

	while( true ) {
		if( ( window = FindWindowW( L"Valve001", nullptr ) ) ) {
			break;
		}
	}

	windowProc = (WNDPROC)( SetWindowLongPtr( window, GWLP_WNDPROC, (LONG_PTR)( &Hooked_WndProc ) ) );

	return 0;
}

void WINAPI detach_loop( HMODULE hInstance ) {
	while( !detach ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
	}

	Int::Panels->SetMouseInputEnabled( FocusOverlayPanel, false );

	//wndproc undo
	SetWindowLongW( window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( windowProc ) );

	FreeLibraryAndExitThread( hInstance, 0 );
}


BOOL APIENTRY DllMain( HMODULE hInstance, DWORD dwReason, LPVOID lpReserved ) {
	if( dwReason == DLL_PROCESS_ATTACH ) {
		if( HANDLE handle = CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)( dwMainThread ), nullptr, 0, nullptr ) ) {
			CloseHandle( handle );
		}
		if( HANDLE handle = CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)( detach_loop ), hInstance, 0, nullptr ) ) {
			CloseHandle( handle );
		}
	}

	return TRUE;
}
