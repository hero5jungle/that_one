#include "dx9.h"

long __stdcall Hooked_EndScene( IDirect3DDevice9* pDevice ) {
	return Hook::EndScene.get_original()( pDevice );
}

long __stdcall Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams ) {
	return Hook::Reset.get_original()( pDevice, pPresentParams );
}