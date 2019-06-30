#include "sdk/sdk.h"
#include "hook/client/client.h"
#include "hook/panel/panels.h"
#include "menu/gui/menu.h"
#include <Windows.h>
#include "hack/event/event.h"
#include "tools/vmt/vmthooks.h"
#include "tools/signature/csignature.h"
#include "sdk/cmat/keyvalues.h"
#include <thread>
#include <chrono>

CGlobalVariables gCvars;
CInterfaces gInts;
CHooks gHooks;
HWND window;

#define Interface(dll) (CreateInterfaceFn)(GetProcAddress(Signatures::GetModuleHandleSafe( dll ), "CreateInterface"))

DWORD WINAPI dwMainThread(LPVOID lpArguments) {
	//Client
	CreateInterfaceFn ClientFactory = Interface("client.dll");
	gInts.Client = (CHLClient*)(ClientFactory("VClient017", nullptr));
	gInts.EntList = (CEntList*)(ClientFactory("VClientEntityList003", nullptr));
	gInts.Prediction = (IPrediction*)(ClientFactory("VClientPrediction001", nullptr));
	gInts.GameMovement = (IGameMovement*)(ClientFactory("GameMovement001", nullptr));
	XASSERT(gInts.Client);
	XASSERT(gInts.EntList);
	XASSERT(gInts.Prediction);
	XASSERT(gInts.GameMovement);
	//Engine
	CreateInterfaceFn EngineFactory = Interface("engine.dll");
	gInts.Engine = (EngineClient*)(EngineFactory("VEngineClient013", nullptr));
	gInts.EngineTrace = (IEngineTrace*)(EngineFactory("EngineTraceClient003", nullptr));
	gInts.ModelInfo = (IVModelInfo*)(EngineFactory("VModelInfoClient006", nullptr));
	gInts.EventManager = (IGameEventManager2*)(EngineFactory("GAMEEVENTSMANAGER002", nullptr));
	gInts.RenderView = (CRenderView*)(EngineFactory("VEngineRenderView014", nullptr));
	gInts.MdlRender = (CModelRender*)(EngineFactory("VEngineModel016", nullptr));
	XASSERT(gInts.Engine);
	XASSERT(gInts.EngineTrace);
	XASSERT(gInts.ModelInfo);
	XASSERT(gInts.EventManager);
	XASSERT(gInts.RenderView);
	XASSERT(gInts.MdlRender);
	//Surface
	CreateInterfaceFn VGUIFactory = Interface("vguimatsurface.dll");
	gInts.Surface = (ISurface*)(VGUIFactory("VGUI_Surface030", nullptr));
	XASSERT(gInts.Surface);
	//Cvar
	CreateInterfaceFn CvarFactory = Interface("vstdlib.dll");
	gInts.cvar = (ICvar*)(CvarFactory("VEngineCvar004", nullptr));
	XASSERT(gInts.cvar);
	//Materials
	CreateInterfaceFn MatSysFactory = Interface("materialsystem.dll");
	gInts.MatSystem = (CMaterialSystem*)(MatSysFactory("VMaterialSystem081", nullptr));
	XASSERT(gInts.MatSystem);
	CreateInterfaceFn VGUI2Factory = Interface("vgui2.dll");
	gInts.Panels = (IPanel*)(VGUI2Factory("VGUI_Panel009", nullptr));
	XASSERT(gInts.Panels);
	//globals
	gInts.globals = *(CGlobals * *)(Signatures::GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);
	XASSERT(gInts.globals);
	//
	const DWORD ClientMode = Signatures::GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
	XASSERT(ClientMode);
	gInts.ClientMode = **(ClientModeShared * **)(ClientMode + 2);
	XASSERT(gInts.ClientMode);
	// material stuff
	Keyvalues::GetOffsets();
	//
  gHooks.PaintTraverse.setup(gInts.Panels, gOffsets::PaintTraverse, &Hooked_PaintTraverse);
  //
  gHooks.FrameStageNotifyThink.setup(gInts.Client, gOffsets::FrameStageNotifyThink, &Hooked_FrameStageNotifyThink);
	//
	gHooks.CreateMove.setup(gInts.ClientMode, gOffsets::CreateMove, &Hooked_CreateMove);
	//
	gHooks.DrawModelExecute.setup(gInts.MdlRender, gOffsets::DrawModelExecute, &Hooked_DrawModelExecute);
	//
	gEvents.InitEvents();

	while (true) {
		if ((window = FindWindow("Valve001", nullptr))) {
			break;
		}
	}

	if (window) {
		gMenu.windowProc = (WNDPROC)(SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)(&Hooked_WndProc)));
	}

	return 0;
}

void WINAPI detach_loop(HMODULE hInstance) {
  while (!detach) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  gInts.Panels->SetMouseInputEnabled(FocusOverlayPanel, false);
  
  gHooks.detach();
  gEvents.UndoEvents();
  
  //wndproc undo
  SetWindowLong(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(gMenu.windowProc));

  FreeLibraryAndExitThread(hInstance, 0);
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
    if (HANDLE handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)(dwMainThread), nullptr, 0, nullptr)) {
      CloseHandle(handle);
    }
    if (HANDLE handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)(detach_loop), hInstance, 0, nullptr)) {
      CloseHandle(handle);
    }
  }

	return TRUE;
}
