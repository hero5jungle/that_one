#pragma once
#include "../../sdk/sdk.h"

long __stdcall Hooked_EndScene( IDirect3DDevice9 * pDevice );
long __stdcall Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams );
