#include "aimbot.h"
#include "../backtrack/backtrack.h"
#include "../event/event.h"
#include "../../tools/util/util.h"
#define tick_interval gInts.globals->interval_per_tick
#define tick_intervalsqr tick_interval * tick_interval
#define TIME_TO_TICKS( dt )   ( (int)( 0.5f + (float)(dt) / gInts.globals->interval_per_tick  ) )

namespace Aimbot {

  void Run( CBaseEntity *pLocal, CUserCmd *pCommand ) {
    Vector m_vOldViewAngle = pCommand->viewangles;
    float m_fOldSideMove = pCommand->sidemove;
    float m_fOldForwardMove = pCommand->forwardmove;
    CBaseCombatWeapon *wpn = pLocal->GetActiveWeapon();
    gCvars.aim_index = -1;
    gCvars.aim = Vector();
    
    if( !gCvars.Aimbot_enable.value ) {
      return;
    }
    
    if( !gCvars.Aimbot_auto_aim.KeyDown() ) {
      return;
    }
    
    if( !wpn ) {
      return;
    }
    
    int wpn_slot = wpn->GetSlot();
    bool is_melee = ( wpn_slot == 2 );
    int Class = pLocal->GetClass();
    int index = GetBestTarget( pLocal, pCommand );
    
    if( index == -1 ) {
      return;
    }
    
    CBaseEntity *pEntity = GetBaseEntity( index );
    gCvars.aim_index = index;
    
    if( !pEntity ) {
      return;
    }
    
    if( pEntity->IsDormant() ) {
      return;
    }
    
    if( pEntity->GetLifeState() != LIFE_ALIVE ) {
      return;
    }
    
    Vector vLocal = pLocal->GetEyePosition();
    int iBestHitbox = -1;
    
    if( is_melee ) {
      iBestHitbox = 4;
    } else if( gCvars.Aimbot_hitbox.value == 0 || gCvars.Aimbot_hitbox.value == 1 ) {
      float last = FLT_MAX;
      bool head = Util::IsHeadshotWeapon( Class, wpn );
      
      if( gCvars.sniper_body.value )
        if( Class == TF2_Sniper ) {
          head = head && pEntity->GetHealth() > 50;
        }
        
      for( int box = head ? 0 : 4; box < 17; box++ ) {
        Vector temp = pEntity->GetHitbox( box );
        Vector angle = Util::CalcAngle( vLocal, temp );
        float fov = Util::GetFOV( pCommand->viewangles, angle );
        
        if( pLocal->CanSee( pEntity, temp ) ) {
          if( gCvars.Aimbot_hitbox.value == 1 ) {
            iBestHitbox = box;
            last = fov;
            break;
          } else if( fov < last ) {
            iBestHitbox = box;
            last = fov;
          }
        }
      }
      
      if( iBestHitbox == -1 ) {
        gCvars.aim = Vector();
        return;
      }
    } else if( gCvars.Aimbot_hitbox.value == 2 ) {
      iBestHitbox = 0;
    } else if( gCvars.Aimbot_hitbox.value == 3 ) {
      iBestHitbox = 4;
    }
    
    gCvars.hitbox = iBestHitbox;
    
    Vector vEntity;
    
    weaponid id = wpn->GetItemDefinitionIndex();
    
    float speed = -1;
    float chargetime = 0;
    float gravity = 0;
    bool quick_release = false;
    
    if( !Util::projSetup( speed, chargetime, gravity, quick_release, id, wpn ) ) {
      return;
    }
    
    if( speed != -1 ) {
      bool on_ground = pEntity->GetFlags() & FL_ONGROUND;
      
      if( id == weaponid::Soldier_s_TheRighteousBison || id == weaponid::Demoman_m_TheLooseCannon ) {
        vEntity = pEntity->GetHitbox( gCvars.hitbox );
      } else if( Class == TF2_Demoman || Class == TF2_Soldier ) {
        vEntity = pEntity->GetAbsOrigin();
        vEntity[2] += 15.0f;
      } else if( id == weaponid::Sniper_m_TheHuntsman || id == weaponid::Sniper_m_FestiveHuntsman || id == weaponid::Sniper_m_TheFortifiedCompound ) {
        vEntity = on_ground ? pEntity->GetHitbox( 0 ) : pEntity->GetHitbox( 4 );
      } else {
        vEntity = pEntity->GetHitbox( gCvars.hitbox );
      }
      
      if( gCvars.Aimbot_proj_mode.value ) {
        vEntity = Util::ProjectilePrediction_Engine( pLocal, pEntity, vEntity, speed, gravity );
      } else {
        vEntity = Util::ProjectilePrediction( pLocal, pEntity, vEntity, speed, gravity );
      }
    } else {
      if( gCvars.backtrack_arr != -1 && ( int )BacktrackData[index].size() > gCvars.backtrack_arr ) {
        if( Backtrack::is_tick_valid( BacktrackData[index][gCvars.backtrack_arr].simtime ) ) {
          vEntity = BacktrackData[index][gCvars.backtrack_arr].hitbox;
        }
      } else if( !gCvars.latency.value ) {
        vEntity = pEntity->GetHitbox( gCvars.hitbox );
      }
    }
    
    
    
    if( vEntity.IsZero() || !pLocal->CanSee( pEntity, vEntity ) ) {
      return;
    }
    
    float minimalDistance = 9999.0f;
    float distance = Util::Distance( vLocal, vEntity );
    
    if( is_melee ) {
      minimalDistance = wpn->GetSwingRange( pLocal );
    } else {
      Util::minDist( id, minimalDistance );
    }
    
    if( distance > minimalDistance ) {
      return;
    }
    
    gCvars.aim = vEntity;
    
    Vector vAngs;
    VectorAngles( ( vEntity - vLocal ), vAngs );
    ClampAngle( vAngs );
    
    if( gCvars.Aimbot_smooth.value > 0.0 && !gCvars.Aimbot_silent.value ) {
      Vector vDelta( pCommand->viewangles - vAngs );
      AngleNormalize( vDelta );
      vAngs = pCommand->viewangles - vDelta / gCvars.Aimbot_smooth.value;
    }
    
    Vector angle_fov = Util::CalcAngle( vLocal, vEntity );
    Vector angle_proj = Util::CalcAngle( vLocal, pEntity->GetHitbox( 4 ) );
    bool fov = ( Util::GetFOV( pCommand->viewangles, angle_fov ) < gCvars.Aimbot_fov.value );
    bool pyro = ( gCvars.pyro_lazy.value && Class == TF2_Pyro && wpn_slot == 0 && !( wpn->GetItemDefinitionIndex() == weaponid::Pyro_m_DragonsFury ) );
    bool proj = ( speed != -1 && gCvars.Aimbot_proj_lazy.value && ( Util::GetFOV( pCommand->viewangles, angle_proj ) < gCvars.Aimbot_fov.value ) );
    bool lazy_melee = is_melee && gCvars.Aimbot_melee.value;
    bool sniper_headshot = Util::canHeadshot( pLocal );
    
    if( fov || lazy_melee || pyro || proj ) {
      Util::lookAt( gCvars.Aimbot_silent.value, vAngs, pCommand );
      
      if( gCvars.backtrack_tick != -1 && gCvars.backtrack_arr != -1 ) {
        pCommand->tick_count = gCvars.backtrack_tick;
      }
      
      if( gCvars.Aimbot_auto_shoot.KeyDown() ) {
        static int last_ammo = 0;
        int ammo = pLocal->ammo();
        
        if( last_ammo != ammo ) {
          if( last_ammo == ammo + 1 ) {
            shots[index].shots++;
            last_ammo = ammo;
          }
          
          if( last_ammo < ammo ) {
            last_ammo = ammo;
          }
        }
        
        if( quick_release ) {
          //i usually prefer to shoot these weapons myself
        } else if( Class == TF2_Heavy ) {
          //minigun
          if( wpn_slot == 0 && pCommand->buttons & IN_ATTACK2 ) {
            pCommand->buttons |= IN_ATTACK;
          } else if( wpn_slot != 0 ) {
            pCommand->buttons |= IN_ATTACK;
          }
        } else if( Class == TF2_Sniper && wpn_slot == 0 ) {
          if( Util::IsHeadshotWeapon( Class, wpn ) ) {
            if( pLocal->GetCond() & tf_cond::TFCond_Zoomed ) {
              if( pLocal->GetCanSeeHitbox( pEntity, vEntity ) == 0 || gCvars.backtrack_arr != -1 || gCvars.Aimbot_hitbox.value != 2 )  {
                if( sniper_headshot || !gCvars.sniper_delay.value )  {
                  pCommand->buttons |= IN_ATTACK;
                }
              }
            } else if( !gCvars.sniper_zoomed.value ) {
              pCommand->buttons |= IN_ATTACK;
            }
          } else {
            pCommand->buttons |= IN_ATTACK;
          }
        } else if( wpn->GetClassId() == ( int )classId::CTFKnife ) {
          if( gCvars.backtrack_arr != -1 ) {
            if( Util::canBackstab( pCommand->viewangles, BacktrackData[index][gCvars.backtrack_arr].angle, BacktrackData[index][gCvars.backtrack_arr].wsc - pLocal->GetWorldSpaceCenter() ) ) {
              pCommand->buttons |= IN_ATTACK;
            }
          } else {
            if( Util::canBackstab( pCommand->viewangles, pEntity->GetEyeAngles(), pEntity->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter() ) ) {
              pCommand->buttons |= IN_ATTACK;
            }
          }
        } else if( Class == TF2_Spy ) {
          if( Util::IsHeadshotWeapon( Class, wpn ) ) {
            if( Util::CanAmbassadorHeadshot( wpn ) ) {
              pCommand->buttons |= IN_ATTACK;
            }
          } else {
            pCommand->buttons |= IN_ATTACK;
          }
        } else {
          pCommand->buttons |= IN_ATTACK;
        }
      }
      
    }
    
    
    
    Util::FixMove( pCommand, m_vOldViewAngle, m_fOldForwardMove, m_fOldSideMove );
  }
  
  int GetBestTarget( CBaseEntity *pLocal, CUserCmd *pCommand ) {
    int best_target = -1;
    float best_score = FLT_MAX;
    Vector local_pos = pLocal->GetEyePosition();
    CBaseCombatWeapon *wpn = pLocal->GetActiveWeapon();
    gCvars.backtrack_arr = -1;
    gCvars.backtrack_tick = -1;
    
    if( !wpn ) {
      return -1;
    }
    
    bool melee = wpn->GetSlot() == 2;
    
    if( !gCvars.Aimbot_melee.value && melee ) {
      return -1;
    }
    
    for( int i = 1; i <= gInts.Engine->GetMaxClients(); i++ ) {
      if( i == me ) {
        continue;
      }
      
      CBaseEntity *pEntity = GetBaseEntity( i );
      
      if( !pEntity ) {
        continue;
      }
      
      if( pEntity->IsDormant() ) {
        continue;
      }
      
      if( pEntity->GetLifeState() != LIFE_ALIVE ) {
        continue;
      }
      
      if( pEntity->GetCond() & TFCond_Ubercharged ||
          pEntity->GetCond() & TFCond_UberchargeFading ||
          pEntity->GetCond() & TFCond_Bonked ) {
        continue;
      }
      
      if( pEntity->GetCond() & TFCond_Cloaked && gCvars.Ignore_A_cloak.value ) {
        continue;
      }
      
      if( pEntity->GetCond() & TFCond_Taunting && gCvars.Ignore_A_taunt.value ) {
        continue;
      }
      
      if( pEntity->GetCond() & TFCond_Disguised && gCvars.Ignore_A_disguise.value ) {
        continue;
      }
      
      static ConVar *mp_friendlyfire = gInts.cvar->FindVar( "mp_friendlyfire" );
      
      if( mp_friendlyfire->GetInt() == 0 && pEntity->GetTeamNum() == pLocal->GetTeamNum() ) {
        continue;
      }
      
      if( !gCvars.latency.value || ( gCvars.latency_amount.value + gCvars.ping_diff.value ) < 200 ) {
        int iBestHitbox = -1;
        
        if( melee ) {
          iBestHitbox = 4;
        } else if( !gCvars.Aimbot_hitbox.value ) {
          if( Util::IsHeadshotWeapon( pEntity->GetClass(), wpn ) ) {
            iBestHitbox = 0;
          } else {
            iBestHitbox = 4;
          }
        } else if( gCvars.Aimbot_hitbox.value == 1 ) {
          iBestHitbox = 0;
        } else if( gCvars.Aimbot_hitbox.value == 2 ) {
          iBestHitbox = 4;
        }
        
        Vector vEntity = pEntity->GetHitbox( iBestHitbox );
        
        if( vEntity.IsZero() ) {
          continue;
        }
        
        Vector angle = Util::CalcAngle( local_pos, vEntity );
        float fov = Util::GetFOV( pCommand->viewangles, angle );
        float distance = Util::Distance( vEntity, pLocal->GetEyePosition() );
        
        if( melee ? ( distance < best_score ) : ( fov < best_score ) ) {
          best_target = i;
          best_score = melee ? distance : fov;
        }
      }
      
      if( gCvars.Backtrack.value ) {
        int ticks = 0;
        
        for( int t = 0; t < ( int )BacktrackData[i].size() && ticks < 12; t++ ) {
          if( BacktrackData[i][t].valid && Backtrack::is_tick_valid( BacktrackData[i][t].simtime ) ) {
            ticks++;
            Vector vEntity = BacktrackData[i][t].hitbox;
            Vector angle = Util::CalcAngle( local_pos, vEntity );
            float fov = Util::GetFOV( pCommand->viewangles, angle );
            float distance = Util::Distance( vEntity, pLocal->GetEyePosition() );
            
            if( BacktrackData[i][t].simtime > ( pLocal->flSimulationTime() - 1.0f ) ) {
              if( melee ? ( distance < best_score ) : ( fov < best_score ) ) {
                best_score = melee ? distance : fov;
                best_target = i;
                gCvars.backtrack_arr = t;
                gCvars.backtrack_tick = TIME_TO_TICKS( BacktrackData[i][t].simtime );
              }
            }
          }
        }
      }
    }
    
    return best_target;
  }
}