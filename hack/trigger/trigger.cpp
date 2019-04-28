#include "trigger.h"
#include "../../tools/util/Util.h"
#include "../../sdk/headers/weaponList.h"
#include "../../sdk/headers/bspflags.h"

namespace Triggerbot {

  void Run( CBaseEntity *pLocal, CUserCmd *pCommand ) {
    float minimalDistance = FLT_MAX;
    
    if( !gCvars.Trigger_enable.value )
      return;
      
    if( !gCvars.Trigger_key.KeyDown() )
      return;
      
    if( pLocal->GetLifeState() != LIFE_ALIVE )
      return;
      
    CBaseCombatWeapon *wpn = pLocal->GetActiveWeapon();
    
    if( !wpn )
      return;
      
    weaponid id = ( weaponid )wpn->GetItemDefinitionIndex();
    
    if( wpn->GetSlot() == 2 ) {
      minimalDistance = 8.4f;
    }
    
    Util::minDist( id, minimalDistance );
    Ray_t ray;
    trace_t trace;
    CTraceFilter filt;
    Vector vAim;
    gInts.Engine->GetViewAngles( vAim ); //We use getviewangles so that this can work with anti-aim when it gets added
    Vector vForward = AngleVector( vAim );;
    vForward = vForward * minimalDistance * 100 + pLocal->GetEyePosition();
    filt.pSkip = pLocal;
    ray.Init( pLocal->GetEyePosition(), vForward );
    gInts.EngineTrace->TraceRay( ray, MASK_SHOT, &filt, &trace );
    
    if( !trace.m_pEnt )
      return;
      
    if( trace.hitgroup < 1 )
      return;
      
    if( trace.m_pEnt->GetTeamNum() == pLocal->GetTeamNum() )
      return;
      
    if( trace.m_pEnt->GetCond() & TFCond_Ubercharged ||
        trace.m_pEnt->GetCond() & TFCond_UberchargeFading ||
        trace.m_pEnt->GetCond() & TFCond_Bonked )
      return;
      
    if( trace.m_pEnt->GetCond() & TFCond_Cloaked && gCvars.Ignore_T_cloak.value )
      return;
      
    if( trace.m_pEnt->GetCond() & TFCond_Taunting && gCvars.Ignore_T_taunt.value )
      return;
      
    if( trace.m_pEnt->GetCond() & TFCond_Disguised && gCvars.Ignore_T_disguise.value )
      return;
      
    if( gCvars.Trigger_zoom.value )
      if( strcmp( pLocal->szGetClass(), "Sniper" ) == 0 && wpn->GetSlot() == 0 && wpn->GetItemDefinitionIndex() != weaponid::Sniper_m_TheClassic && !( pLocal->GetCond() & tf_cond::TFCond_Zoomed ) )
        return;
        
    if( gCvars.Trigger_head.value && trace.hitbox != 0 )
      return;
      
    if( !gCvars.Trigger_melee.value && wpn->GetSlot() == 2 )
      return;
      
    if( wpn->GetItemDefinitionIndex() == weaponid::Sniper_m_TheClassic ) {
      if( wpn->GetChargeDamage() > 0 )
        pCommand->buttons &= ~IN_ATTACK;
    } else {
      pCommand->buttons |= IN_ATTACK;
    }
  }
}