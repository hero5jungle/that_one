#pragma once
#include "../../sdk/sdk.h"
namespace Aimbot {

  void Run( CBaseEntity *pLocal, CUserCmd *pCommand );
  int GetBestTarget( CBaseEntity *pLocal, CUserCmd *pCommand );
}
