#pragma once

#include "../../sdk/sdk.h"

bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand );
int __fastcall Hooked_KeyEvent( PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding );
void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );
void __fastcall FrameStageNotifyThink( PVOID CHLClient, void *_this, ClientFrameStage_t Stage );
bool __fastcall hk_ShouldDrawViewModel( PVOID CHLClient, void *_this );
bool __fastcall hk_ShouldDrawLocalPlayer( PVOID CHLClient, void *_this, CBaseEntity *pPlayer );