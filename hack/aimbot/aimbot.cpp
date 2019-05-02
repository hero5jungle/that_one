#include "aimbot.h"
#include "../../tools/util/Util.h"
#include "../backtrack/backtrack.h"
#include "../event/event.h"
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
    int Class = pLocal->GetClassNum();
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
      
      if( gCvars.head_body.value )
        if( Class == TF2_Sniper ) {
          head = head && pEntity->GetHealth() > 50;
        }
        
      for( int box = head ? 0 : 4; box < 17; box++ ) {
        Vector temp = pEntity->GetHitboxPosition( box );
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
      bool is_on_ground = pEntity->GetFlags() & FL_ONGROUND;
      
      if( id == weaponid::Soldier_s_TheRighteousBison || id == weaponid::Demoman_m_TheLooseCannon ) {
        vEntity = pEntity->GetHitboxPosition( gCvars.hitbox );
      } else if( Class == TF2_Demoman || Class == TF2_Soldier ) {
        vEntity = pEntity->GetAbsOrigin();
        vEntity[2] += 15.0f;
      } else {
        vEntity = pEntity->GetHitboxPosition( gCvars.hitbox );
      }
      
      vEntity = [&vLocal, &pEntity, &is_on_ground]( Vector hitbox, float speed, float gravity, float distance_to_ground ) -> Vector {
        float predicted_time = ( ( vLocal.DistTo( hitbox ) / speed ) + tick_interval );
        float server_gravity = gInts.cvar->FindVar( "sv_gravity" )->GetFloat();
        
        Vector vec_velocity = Util::EstimateAbsVelocity( pEntity );
        vec_velocity[2] += ( -server_gravity ) * tick_intervalsqr + ( gravity * tick_intervalsqr );
        
        hitbox[0] += ( vec_velocity[0] * predicted_time ) + tick_interval;
        hitbox[1] += ( vec_velocity[1] * predicted_time ) + tick_interval;
        hitbox[2] += ( is_on_ground ? ( vec_velocity[2] * predicted_time ) + tick_interval :
                       ( 0.5 * ( -server_gravity + gravity ) * pow( predicted_time, 2 ) + vec_velocity[2] * predicted_time ) ) + tick_interval;
                       
        if( distance_to_ground > 0.0f )
          if( hitbox[2] < hitbox[2] - distance_to_ground ) {
            hitbox[2] = hitbox[2] - distance_to_ground;
          }
          
        return hitbox;
      }( vEntity, speed, gravity, Util::DistanceToGround( pEntity ) );
    } else {
      if( gCvars.backtrack_arr != -1 && ( int )BacktrackData[index].size() > gCvars.backtrack_arr ) {
        if( Backtrack::is_tick_valid( BacktrackData[index][gCvars.backtrack_arr].simtime ) ) {
          vEntity = BacktrackData[index][gCvars.backtrack_arr].hitbox;
        }
      } else if( !gCvars.latency.value ) {
        vEntity = pEntity->GetHitboxPosition( gCvars.hitbox );
      }
    }
    
    gCvars.aim = vEntity;
    
    if( vEntity.IsZero() || !pLocal->CanSee( pEntity, vEntity ) ) {
      return;
    }
    
    float distance = Util::Distance( vLocal, vEntity );
    float minimalDistance = 9999.0f;
    
    if( is_melee ) {
      minimalDistance = 4.0f + wpn->GetSwingRange( pLocal ) / 10.0f;
    } else {
      Util::minDist( id, minimalDistance );
    }
    
    if( distance > minimalDistance ) {
      return;
    }
    
    static bool dunk = false;
    
    if( id == weaponid::Demoman_m_TheLooseCannon ) {
      static float timer = 0;
      static bool started = false;
      
      if( !( pCommand->buttons & IN_ATTACK ) ) {
        started = false;
        timer = 0;
      }
      
      if( !started ) {
        if( pCommand->buttons & IN_ATTACK ) {
          started = true;
          timer = gInts.globals->curtime;
        }
      }
      
      if( started ) {
        float time = fminf( 0.85f, ( fmaxf( 0, gInts.globals->curtime - timer ) ) );
        
        if( time == 0.85f ) {
          pCommand->buttons ^= IN_ATTACK;
        }
        
        float dist = ( ( speed / 100.0f ) / 0.4f ) * ( 1.0f - time );
        dunk = fabs( distance - dist ) < 0.1f;
      }
    }
    
    Vector vAngs;
    VectorAngles( ( vEntity - vLocal ), vAngs );
    ClampAngle( vAngs );
    
    if( gCvars.Aimbot_smooth.value > 0.0 && !gCvars.Aimbot_silent.value ) {
      Vector vDelta( pCommand->viewangles - vAngs );
      AngleNormalize( vDelta );
      vAngs = pCommand->viewangles - vDelta / gCvars.Aimbot_smooth.value;
    }
    
    Vector angle_fov = Util::CalcAngle( vLocal, vEntity );
    Vector angle_proj = Util::CalcAngle( vLocal, pEntity->GetHitboxPosition( 4 ) );
    bool fov = ( Util::GetFOV( pCommand->viewangles, angle_fov ) < gCvars.Aimbot_fov.value );
    bool pyro = ( gCvars.Aimbot_pyro.value && Class == TF2_Pyro && wpn_slot == 0 && !( wpn->GetItemDefinitionIndex() == weaponid::Pyro_m_DragonsFury ) );
    bool proj = ( speed != -1 && gCvars.Aimbot_proj.value && ( Util::GetFOV( pCommand->viewangles, angle_proj ) < gCvars.Aimbot_fov.value ) );
    bool lazy_melee = is_melee && gCvars.Aimbot_melee.value;
    
    if( fov || lazy_melee || pyro || proj ) {
      Util::lookAt( gCvars.Aimbot_silent.value, vAngs, pCommand );
      
      if( gCvars.backtrack_tick != -1 && gCvars.backtrack_arr != -1 ) {
        pCommand->tick_count = gCvars.backtrack_tick;
      }
      
      if( gCvars.Aimbot_auto_shoot.KeyDown() ) {
        static int last_ammo = 0;
        int ammo = pLocal->sniper_ammo();
        
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
          if( dunk ) {
            pCommand->buttons &= ~IN_ATTACK;
          }
        } else if( Class == TF2_Heavy ) {
          //minigun
          if( wpn_slot == 0 && pCommand->buttons & IN_ATTACK2 ) {
            pCommand->buttons |= IN_ATTACK;
          } else if( wpn_slot != 0 ) {
            pCommand->buttons |= IN_ATTACK;
          }
        } else if( Class == TF2_Sniper && wpn_slot == 0 ) {
          if( ( pLocal->GetCond() & tf_cond::TFCond_Zoomed && gCvars.Aimbot_zoom.value ) || !gCvars.Aimbot_zoom.value ) {
            pCommand->buttons |= IN_ATTACK;
          }
        } else if( Class == TF2_Spy ) {
          if( Util::IsHeadshotWeapon( Class, wpn ) ) {
            if( Util::CanAmbassadorHeadshot( wpn ) ) {
              pCommand->buttons |= IN_ATTACK;
            }
          } else if( is_melee ) {
            if( Util::canBackstab( pLocal, pEntity, vAngs ) ) {
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
          if( Util::IsHeadshotWeapon( pEntity->GetClassNum(), wpn ) ) {
            iBestHitbox = 0;
          } else {
            iBestHitbox = 4;
          }
        } else if( gCvars.Aimbot_hitbox.value == 1 ) {
          iBestHitbox = 0;
        } else if( gCvars.Aimbot_hitbox.value == 2 ) {
          iBestHitbox = 4;
        }
        
        Vector vEntity = pEntity->GetHitboxPosition( iBestHitbox );
        
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