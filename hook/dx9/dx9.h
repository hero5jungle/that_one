#pragma once
#include "../../sdk/sdk.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
long __stdcall Hooked_EndScene( IDirect3DDevice9 * pDevice );
long __stdcall Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams );
