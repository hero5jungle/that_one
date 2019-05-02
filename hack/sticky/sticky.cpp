#include "sticky.h"
#include "../../tools/util/Util.h"

namespace DemoSticky {

  void Run( CBaseEntity *pLocal, CUserCmd *pUserCmd ) {
    Vector sticky_loc;
    float closest_dist = 0;
    
    if( !gCvars.auto_sticky.value ) {
      return;
    }
    
    if( pLocal->GetLifeState() != LIFE_ALIVE ) {
      return;
    }
    
    if( pLocal->GetClassNum() != TF2_Demoman ) {
      return;
    }
    
    for( int i = 0; i < gInts.EntList->GetHighestEntityIndex(); i++ ) {
      CBaseEntity *sticky = gInts.EntList->GetClientEntity( i );
      
      if( !sticky ) {
        continue;
      }
      
      if( sticky->GetTeamNum() != pLocal->GetTeamNum() ) {
        continue;
      }
      
      if( strstr( gInts.ModelInfo->GetModelName( sticky->GetModel() ), "sticky" ) ) {
        sticky->GetWorldSpaceCenter( sticky_loc );
        
        for( int j = 1; j < gInts.Engine->GetMaxClients(); j++ ) {
          if( j == me ) {
            continue;
          }
          
          CBaseEntity *pEntity = GetBaseEntity( j );
          
          if( !pEntity ) {
            continue;
          }
          
          if( pEntity == pLocal ) {
            continue;
          }
          
          if( pEntity->IsDormant() ) {
            continue;
          }
          
          if( pEntity->GetLifeState() != LIFE_ALIVE ) {
            continue;
          }
          
          if( pEntity->GetTeamNum() == pLocal->GetTeamNum() ) {
            continue;
          }
          
          if( pEntity->GetCond() & TFCond_Ubercharged ||
              pEntity->GetCond() & TFCond_UberchargeFading ||
              pEntity->GetCond() & TFCond_Bonked ) {
            continue;
          }
          
          Vector vent = pEntity->GetHitboxPosition( 4 );
          float dist = Util::Distance( sticky_loc, vent );
          
          if( dist < closest_dist || closest_dist == 0 ) {
            closest_dist = dist;
          }
        }
      }
    }
    
    if( closest_dist == 0 || closest_dist > 12.0f ) {
      return;
    }
    
    pUserCmd->buttons |= IN_ATTACK2;
  }
  
}