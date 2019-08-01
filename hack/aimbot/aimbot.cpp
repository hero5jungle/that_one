#include "aimbot.h"

#include "../../tools/util/util.h"
#include "../backtrack/backtrack.h"

namespace Aimbot {

	enum MODE {
		NONE = -1,
		PLAYER,
		BUILDING,
		BACKTRACK
	};

	void Run( CBaseEntity* pLocal, CUserCmd* pCommand, bool packet ) {
		if( !gCvars.Aimbot_enable.value || !gCvars.Aimbot_auto_aim.KeyDown() ) {
			return;
		}

		Vector OldAngles = pCommand->viewangles;
		float  OldForward = pCommand->forwardmove;
		float  OldSidemove = pCommand->sidemove;


		CBaseCombatWeapon* wpn = pLocal->GetActiveWeapon();
		bool sniper_headshot = Util::canHeadshot( pLocal );


		if( !wpn ) {
			return;
		}

		GetBestTarget( pLocal, pCommand, wpn, gCvars.aim_index, gCvars.aim_mode );

		if( gCvars.aim_mode == MODE::NONE ) {
			return;
		}

		gCvars.aim_spot = GetBestHitbox( pLocal, pCommand, wpn );

		if( gCvars.aim_spot.IsZero() ) {
			gCvars.aim_spot = GetBaseEntity( gCvars.aim_index )->GetWorldSpaceCenter();
			return;
		}

		CBaseEntity* pEntity = GetBaseEntity( gCvars.aim_index );

		if( !pLocal->CanSee( pEntity, gCvars.aim_spot ) ) {
			return;
		}

		weaponid id = wpn->GetItemDefinitionIndex();
		float speed = -1;
		float chargetime = 0;
		float gravity = 0;
		bool quick_release = false;

		if( !Util::weaponSetup( speed, chargetime, gravity, quick_release, id, wpn ) ) {
			return;
		}

		Vector vLocal = pLocal->GetEyePosition();

		float minimalDistance = 9999.0f;
		int wpn_slot = wpn->GetSlot();
		bool is_melee = wpn_slot == 2;

		if( is_melee ) {
			minimalDistance = wpn->GetSwingRange( pLocal );
		} else {
			Util::minDist( id, minimalDistance );
		}

		if( Util::Distance( vLocal, gCvars.aim_spot ) > minimalDistance ) {
			return;
		}

		Vector vAngs;
		VectorAngles( ( gCvars.aim_spot - vLocal ), vAngs );
		ClampAngle( vAngs );

		int Class = pLocal->GetClass();

		bool fov = Util::GetFOV( pCommand->viewangles, Util::CalcAngle( vLocal, gCvars.aim_spot ) ) < gCvars.Aimbot_fov.value;
		bool pyro = ( gCvars.pyro_lazy.value && Class == TF2_Pyro && wpn_slot == 0 && !( wpn->GetItemDefinitionIndex() == weaponid::Pyro_m_DragonsFury ) );
		bool lazy_melee = is_melee && gCvars.Aimbot_melee.value;

		if( ( fov || lazy_melee || pyro ) && Util::CanShoot( pLocal, wpn ) ) {

			if( gCvars.backtrack_tick != -1 && gCvars.backtrack_arr != -1 ) {
				pCommand->tick_count = gCvars.backtrack_tick;
			}

			if( gCvars.Aimbot_auto_shoot.KeyDown() ) {
				if( quick_release ) {
					// i usually prefer to shoot these weapons myself
				} else if( Class == TF2_Heavy ) {
					// minigun
					if( wpn_slot == 0 ) {
						if( pCommand->buttons & IN_ATTACK2 )
							pCommand->buttons |= IN_ATTACK;
					} else {
						pCommand->buttons |= IN_ATTACK;
					}
				} else if( Class == TF2_Sniper && wpn_slot == 0 ) {
					if( Util::isHeadshotWeapon( Class, wpn ) ) {
						if( pLocal->GetCond() & tf_cond::TFCond_Zoomed ) {
							if( sniper_headshot || !gCvars.sniper_delay.value ) {
								pCommand->buttons |= IN_ATTACK;
							}
						} else if( !gCvars.sniper_zoomed.value ) {
							pCommand->buttons |= IN_ATTACK;
						}
					}
				} else if( wpn->GetClassId() == (int)classId::CTFKnife ) {
					if( gCvars.aim_mode == MODE::BACKTRACK ) {
						if( Util::canBackstab( pCommand->viewangles, BacktrackData[gCvars.aim_index][gCvars.backtrack_arr].angle, BacktrackData[gCvars.aim_index][gCvars.backtrack_arr].wsc - pLocal->GetWorldSpaceCenter() ) ) {
							pCommand->buttons |= IN_ATTACK;
						}
					} else {
						if( Util::canBackstab( pCommand->viewangles, pEntity->GetEyeAngles(), pEntity->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter() ) ) {
							pCommand->buttons |= IN_ATTACK;
						}
					}
				} else if( Class == TF2_Spy ) {
					if( Util::isHeadshotWeapon( Class, wpn ) ) {
						if( Util::canAmbassadorHeadshot( wpn ) ) {
							pCommand->buttons |= IN_ATTACK;
						}
					} else {
						pCommand->buttons |= IN_ATTACK;
					}
				} else {
					pCommand->buttons |= IN_ATTACK;
				}

				if( pCommand->buttons & IN_ATTACK ) {
					if( !gCvars.fake.value || packet ) {
						Util::lookAt( gCvars.Aimbot_silent.value, vAngs, pCommand );
					} else {
						pCommand->buttons &= ~IN_ATTACK;
					}
				}
			}
		}

		Util::FixMove( pCommand, OldAngles, OldForward, OldSidemove );

	}

	void GetBestTarget( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn, int& index, int& mode ) {

		int best_target = -1;
		float best_score = FLT_MAX;

		Vector local_pos = pLocal->GetEyePosition();
		bool melee = wpn->GetSlot() == 2;

		gCvars.backtrack_arr = -1;
		gCvars.backtrack_tick = -1;
		mode = MODE::NONE;
		index = -1;

		if( !gCvars.Aimbot_melee.value && melee ) {

			return;
		}

		int my_index = me;
		int max = gInts.Engine->GetMaxClients();
		int max_ent = gInts.EntList->GetHighestEntityIndex();

		for( int i = 1; i <= max; i++ ) {
			if( i == my_index ) continue;

			CBaseEntity* pEntity = GetBaseEntity( i );

			if( !pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE ) {
				continue;
			}

			int cond = pEntity->GetCond();

			if( cond & ( TFCond_Ubercharged | TFCond_UberchargeFading | TFCond_Bonked ) )
				continue;

			if( cond & TFCond_Cloaked && gCvars.Ignore_A_cloak.value )
				continue;

			if( cond & TFCond_Taunting && gCvars.Ignore_A_taunt.value )
				continue;

			if( cond & TFCond_Disguised && gCvars.Ignore_A_disguise.value )
				continue;

			static ConVar* mp_friendlyfire = gInts.cvar->FindVar( "mp_friendlyfire" );

			if( mp_friendlyfire->GetInt() == 0 && pEntity->GetTeamNum() == pLocal->GetTeamNum() )
				continue;

			if( !gCvars.latency.value || ( gCvars.latency_amount.value + gCvars.ping_diff.value ) < 200 ) {
				int iBestHitbox = -1;

				if( melee ) {
					iBestHitbox = 4;
				} else if( !gCvars.Aimbot_hitbox.value ) {
					if( Util::isHeadshotWeapon( pEntity->GetClass(), wpn ) ) {
						iBestHitbox = 0;
					} else {
						iBestHitbox = 4;
					}
				} else if( gCvars.Aimbot_hitbox.value == 1 ) {
					iBestHitbox = 0;
				} else if( gCvars.Aimbot_hitbox.value == 2 ) {
					iBestHitbox = 4;
				}

				Vector vEntity = pEntity->GetHitbox( pLocal, iBestHitbox );

				if( vEntity.IsZero() ) {
					continue;
				}

				Vector angle = Util::CalcAngle( local_pos, vEntity );
				float fov = Util::GetFOV( pCommand->viewangles, angle );
				float distance = Util::Distance( vEntity, pLocal->GetEyePosition() );

				if( melee ? ( distance < best_score ) : ( fov < best_score ) ) {
					best_target = i;
					best_score = melee ? distance : fov;
					mode = MODE::PLAYER;
				}
			}

			if( gCvars.Backtrack.value ) {
				int ticks = 0;

				for( int t = 0; t < (int)BacktrackData[i].size() && ticks < 12; t++ ) {
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
								gCvars.backtrack_tick = (int)( 0.5f + (float)( BacktrackData[i][t].simtime ) / gInts.globals->interval_per_tick );
								mode = MODE::BACKTRACK;
							}
						}

					}
				}
			}
		}

		for( int i = max; i <= max_ent; i++ ) {
			CBaseEntity* pEntity = GetBaseEntity( i );

			if( !pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE ) {
				continue;
			}

			switch( (classId)pEntity->GetClassId() ) {
				case classId::CObjectSentrygun:
				case classId::CObjectTeleporter:
				case classId::CObjectDispenser:
				{
					break;
				}
				default:
				{
					continue;
				}
			}

			static ConVar* mp_friendlyfire = gInts.cvar->FindVar( "mp_friendlyfire" );

			if( mp_friendlyfire->GetInt() == 0 && pEntity->GetTeamNum() == pLocal->GetTeamNum() )
				continue;

			Vector vEntity = pEntity->GetWorldSpaceCenter();
			Vector angle = Util::CalcAngle( local_pos, vEntity );
			float fov = Util::GetFOV( pCommand->viewangles, angle );
			float distance = Util::Distance( vEntity, pLocal->GetEyePosition() );

			if( melee ? ( distance < best_score ) : ( fov < best_score ) ) {
				best_score = melee ? distance : fov;
				best_target = i;
				mode = MODE::BUILDING;
			}

		}
		index = best_target;
	}

	Vector GetBestHitbox( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn ) {

		CBaseEntity* pEntity = GetBaseEntity( gCvars.aim_index );

		if( !pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE ) {
			return Vector();
		}

		switch( gCvars.aim_mode ) {
			case MODE::PLAYER:
			{
				if( wpn->GetSlot() == 2 ) return pEntity->GetHitbox( pLocal, 4 );

				switch( gCvars.Aimbot_hitbox.value ) {
					case 0:
					case 1:
					{
						float score = FLT_MAX;
						int hitbox_ind = -1;
						Vector hitbox;

						for( int box = Util::isHeadshotWeapon( pLocal->GetClass(), wpn ) ? 0 : 4; box < 17; box++ ) {
							Vector pos = gCvars.Aimbot_multipoint.value ? pEntity->GetMultipoint( pLocal, box ) : pEntity->GetHitbox( pLocal, box );
							float fov = Util::GetFOV( pCommand->viewangles, Util::CalcAngle( pLocal->GetEyePosition(), pos ) );

							if( !pos.IsZero() && fov < score ) {
								hitbox_ind = box;
								score = fov;
								if( gCvars.Aimbot_hitbox.value ) {
									return pos;
								} else {
									hitbox = pos;
								}
							}
						}

						if( hitbox_ind != -1 ) {
							gCvars.hitbox = hitbox_ind;
							return hitbox;
						} else {
							return Vector();
						}
					};

					case 2:
					{
						if( gCvars.Aimbot_multipoint.value )
							return pEntity->GetMultipoint( pLocal, 0 );
						else
							return pEntity->GetHitbox( pLocal, 0 );
					};

					case 3:
					{
						if( gCvars.Aimbot_multipoint.value )
							return pEntity->GetMultipoint( pLocal, 4 );
						else
							return pEntity->GetHitbox( pLocal, 4 );
					};

					default:
						break;
				}

				break;
			}

			case MODE::BACKTRACK:
			{
				if( BacktrackData[gCvars.aim_index].size() > gCvars.backtrack_arr && Backtrack::is_tick_valid( BacktrackData[gCvars.aim_index][gCvars.backtrack_arr].simtime ) ) {
					return BacktrackData[gCvars.aim_index][gCvars.backtrack_arr].hitbox;
				} else if( !gCvars.latency.value ) {
					return pEntity->GetHitbox( pLocal, gCvars.hitbox );
				} else {
					return Vector();
				}
			}

			case MODE::BUILDING:
			{
				Vector center = pEntity->GetWorldSpaceCenter();
				if( pLocal->CanSee( pEntity, center ) ) {
					return center;
				} else {
					return Vector();
				}
			}
		}
		return Vector();
	}
}