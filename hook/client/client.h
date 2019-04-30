#pragma once

#include "../../sdk/sdk.h"

bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand );
void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );
void __fastcall FrameStageNotifyThink( PVOID CHLClient, void *_this, ClientFrameStage_t Stage );
