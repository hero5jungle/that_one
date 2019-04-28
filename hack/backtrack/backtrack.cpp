#include "backtrack.h"
#include "../../tools/util/Util.h"
std::deque<LagRecord> BacktrackData[64];

namespace Backtrack {


  void Run( CUserCmd *cmd ) {
    int iBestTarget = -1;
    float bestFov = 99999;
    gCvars.backtrack_tick = 0;
    CBaseEntity *pLocal = gInts.EntList->GetClientEntity( me );
    
    if( !pLocal ) return;
    
    if( !gCvars.Backtrack.value ) return;
    
    if( pLocal->GetLifeState() != LIFE_ALIVE || pLocal->GetHealth() == 1 ) {
      return;
    }
    
    int iBestHitbox = gCvars.hitbox != -1 ? gCvars.hitbox : 0;
    Vector vLocal = pLocal->GetEyePosition();
    
    for( int i = 1; i <= gInts.Engine->GetMaxClients(); i++ ) {
      CBaseEntity *pEntity = GetBaseEntity( i );
      
      if( !pEntity )
        continue;
        
      if( pEntity->IsDormant() )
        continue;
        
      if( pEntity->GetLifeState() != LIFE_ALIVE )
        continue;
        
      if( pEntity->GetTeamNum() == pLocal->GetTeamNum() )
        continue;
        
      CBaseCombatWeapon *wpn = pLocal->GetActiveWeapon();
      
      if( !wpn )
        continue;
        
      Vector hitbox = pEntity->GetHitboxPosition( iBestHitbox );
      BacktrackData[i].push_front( LagRecord{ false, pEntity->flSimulationTime(), Util::EstimateAbsVelocity( pEntity ).Length(), hitbox } );
      BacktrackData[i].front().valid = pEntity->SetupBones( BacktrackData[i].front().boneMatrix, 128, 256, gInts.globals->curtime );
      
      while( BacktrackData[i].size() && !is_tick_valid( BacktrackData[i].back().simtime ) )  {
        BacktrackData[i].pop_back();
      }
      
      Vector angle = Util::CalcAngle( vLocal, hitbox );
      float FOVDistance = Util::GetFOV( cmd->viewangles, angle );
      float dist = Util::Distance( hitbox, vLocal );
      
      if( wpn->GetSlot() == 2 ) {
        if( dist < bestFov ) {
          bestFov = FOVDistance;
          iBestTarget = i;
        }
      } else if( bestFov > FOVDistance ) {
        bestFov = FOVDistance;
        iBestTarget = i;
      }
    }
    
    if( iBestTarget != -1 && BacktrackData[iBestTarget].size() ) {
      CBaseEntity *entity = GetBaseEntity( iBestTarget );
      float bestTargetSimTime = -1;
      float minFov = FLT_MAX;
      
      for( int t = 0; t < ( int )BacktrackData[iBestTarget].size(); t++ ) {
        if( !BacktrackData[iBestTarget][t].valid ) continue;
        
        Vector angle = Util::CalcAngle( vLocal, BacktrackData[iBestTarget][t].hitbox );
        float tempFOVDistance = Util::GetFOV( cmd->viewangles, angle );
        
        if( minFov > tempFOVDistance && BacktrackData[iBestTarget][t].simtime > ( pLocal->flSimulationTime() - 1 ) ) {
          if( pLocal->CanSee( entity, BacktrackData[iBestTarget][t].hitbox ) ) {
            minFov = tempFOVDistance;
            bestTargetSimTime = BacktrackData[iBestTarget][t].simtime;
            gCvars.backtrack_arr = t;
          }
        }
      }
      
      if( bestTargetSimTime >= 0 )
        gCvars.backtrack_tick = TIME_TO_TICKS( bestTargetSimTime );
    }
  }
}