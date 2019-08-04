#include "dx9.h"
long __stdcall Hooked_EndScene( IDirect3DDevice9* pDevice ) {

	long original = gHooks.EndScene.get_original()( pDevice );

	///

	return original;
}

long __stdcall Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams ) {
	return gHooks.Reset.get_original()( pDevice, pPresentParams );
}