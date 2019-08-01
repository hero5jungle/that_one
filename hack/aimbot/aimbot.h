#pragma once
#include "../../sdk/sdk.h"
namespace Aimbot {
	void Run( CBaseEntity* pLocal, CUserCmd* pCommand, bool packet );
	void GetBestTarget( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn, int& index, int& mode );
	Vector GetBestHitbox( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn );
}
