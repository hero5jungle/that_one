#pragma once

#include "../../sdk/sdk.h"
int __fastcall hkSendDatagram( CNetChan* netchan, PVOID, bf_write* datagram );
bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand );
void __fastcall Hooked_FrameStageNotifyThink( PVOID CHLClient, void* _this, ClientFrameStage_t Stage );
void __stdcall Hooked_DrawModelExecute( void* state, ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld );
