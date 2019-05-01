#pragma once

#include "../../sdk/sdk.h"
int __fastcall hkSendDatagram( CNetChan *netchan, PVOID, bf_write *datagram );
bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand );
void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );
void __fastcall FrameStageNotifyThink( PVOID CHLClient, void *_this, ClientFrameStage_t Stage );
using SendDatagramFn = int( __thiscall * )( CNetChan *, bf_write * );
using CreateMoveFn = bool( __thiscall * )( PVOID, float, CUserCmd * );
using FrameStageNotifyThinkFn = void( __fastcall * )( PVOID, void *, ClientFrameStage_t );
using DrawModelExecuteFn = void( __stdcall * )( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );