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
		if( !Global.Aimbot_enable.value || !Global.Aimbot_auto_aim.KeyDown() ) {
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

		weaponid id = wpn->GetItemDefinitionIndex();
		float speed = -1;
		float chargetime = 0;
		float gravity = 0;
		bool quick_release = false;
		bool valid = Util::weaponSetup( speed, chargetime, gravity, quick_release, id, wpn );

		GetBestTarget( pLocal, pCommand, wpn, Global.aim_index, Global.aim_mode, speed );

		if( Global.aim_mode == MODE::NONE ) {
			return;
		}

		Global.aim_spot = GetBestHitbox( pLocal, pCommand, wpn, speed, gravity );

		if( Global.aim_spot.IsZero() ) {
			Global.aim_spot = GetBaseEntity( Global.aim_index )->GetWorldSpaceCenter();
			return;
		}

		if( !valid ) {
			return;
		}

		CBaseEntity* pEntity = GetBaseEntity( Global.aim_index );

		if( !pLocal->CanSee( pEntity, Global.aim_spot ) ) {
			return;
		}

		int Class = pLocal->GetClass();
		Vector vLocal = pLocal->GetEyePosition();

		if( Class == TF2_Soldier && speed != -1 && wpn->GetSlot() == 0 ) {//fix rocket launcher offset
			Vector vecForward, vecRight, vecUp;
			AngleVectors( pLocal->GetEyeAngles(), &vecForward, &vecRight, &vecUp );

			Vector offset{ 23.5f, 12.0f, -3.0f };
			
			if( pLocal->GetFlags() & FL_DUCKING ) {
				offset.z = 8.0f;
			}

			vLocal += ( vecForward * offset.x ) + ( vecRight * offset.y ) + ( vecUp * offset.z );
		}

		float minimalDistance = 9999.0f;
		int wpn_slot = wpn->GetSlot();
		bool is_melee = wpn_slot == 2;

		if( is_melee ) {
			minimalDistance = wpn->GetSwingRange( pLocal );
		} else {
			Util::minDist( id, minimalDistance );
		}

		if( Util::Distance( vLocal, Global.aim_spot ) > minimalDistance ) {
			return;
		}

		Vector vAngs;
		VectorAngles( ( Global.aim_spot - vLocal ), vAngs );
		ClampAngle( vAngs );


		bool fov = Util::GetFOV( pCommand->viewangles, Util::CalcAngle( vLocal, Global.aim_spot ) ) < Global.Aimbot_fov.value;
		bool lazy_melee = is_melee && Global.Aimbot_melee.value;
		bool lazy_proj = Util::GetFOV( pCommand->viewangles, Util::CalcAngle( vLocal, pEntity->GetWorldSpaceCenter() ) ) < Global.Aimbot_fov.value;

		if( fov || lazy_melee || lazy_proj ) {

			if( Global.backtrack_tick != -1 && Global.backtrack_arr != -1 ) {
				pCommand->tick_count = Global.backtrack_tick;
			}

			if( Global.Aimbot_auto_shoot.KeyDown() ) {
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
							if( sniper_headshot || !Global.sniper_delay.value ) {
								pCommand->buttons |= IN_ATTACK;
							}
						} else if( !Global.sniper_zoomed.value ) {
							pCommand->buttons |= IN_ATTACK;
						}
					}
				} else if( wpn->GetClassId() == (int)classId::CTFKnife ) {
					if( Global.aim_mode == MODE::BACKTRACK ) {
						if( Util::canBackstab( pCommand->viewangles, BacktrackData[Global.aim_index][Global.backtrack_arr].angle, BacktrackData[Global.aim_index][Global.backtrack_arr].wsc - pLocal->GetWorldSpaceCenter() ) ) {
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
			}

			if( pCommand->buttons & IN_ATTACK && !Util::CanShoot( pLocal, wpn ) ) {
				pCommand->buttons &= ~IN_ATTACK;
			}

			if( packet ) {
				Util::lookAt( Global.Aimbot_silent.value, vAngs, pCommand );
			}
		}

		Util::FixMove( pCommand, OldAngles, OldForward, OldSidemove );

	}

	void GetBestTarget( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn, int& index, int& mode, float& speed ) {

		int best_target = -1;
		float best_score = FLT_MAX;

		Vector local_pos = pLocal->GetEyePosition();
		bool melee = wpn->GetSlot() == 2;

		Global.backtrack_arr = -1;
		Global.backtrack_tick = -1;
		mode = MODE::NONE;
		index = -1;

		if( !Global.Aimbot_melee.value && melee ) {

			return;
		}

		int my_index = me;
		int max = Int::Engine->GetMaxClients();
		int max_ent = Int::EntityList->GetHighestEntityIndex();

		for( int i = 1; i <= max; i++ ) {
			if( i == my_index ) continue;

			CBaseEntity* pEntity = GetBaseEntity( i );

			if( !pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE ) {
				continue;
			}

			int cond = pEntity->GetCond();

			if( cond & ( TFCond_Ubercharged | TFCond_UberchargeFading | TFCond_Bonked ) )
				continue;

			if( cond & TFCond_Cloaked && Global.Ignore_A_cloak.value )
				continue;

			if( cond & TFCond_Taunting && Global.Ignore_A_taunt.value )
				continue;

			if( cond & TFCond_Disguised && Global.Ignore_A_disguise.value )
				continue;

			static ConVar* mp_friendlyfire = Int::cvar->FindVar( "mp_friendlyfire" );

			if( mp_friendlyfire->GetInt() == 0 && pEntity->GetTeamNum() == pLocal->GetTeamNum() )
				continue;

			if( !Global.latency.value || ( Global.latency_amount.value + Global.ping_diff.value ) < 200 ) {
				int iBestHitbox = -1;

				if( melee ) {
					iBestHitbox = 4;
				} else if( !Global.Aimbot_hitbox.value ) {
					if( Util::isHeadshotWeapon( pEntity->GetClass(), wpn ) ) {
						iBestHitbox = 0;
					} else {
						iBestHitbox = 4;
					}
				} else if( Global.Aimbot_hitbox.value == 1 ) {
					iBestHitbox = 0;
				} else if( Global.Aimbot_hitbox.value == 2 ) {
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

			if( Global.Backtrack.value && speed == -1 ) {
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
								Global.backtrack_arr = t;
								Global.backtrack_tick = (int)( 0.5f + (float)( BacktrackData[i][t].simtime ) / Int::globals->interval_per_tick );
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

			static ConVar* mp_friendlyfire = Int::cvar->FindVar( "mp_friendlyfire" );

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

	Vector GetBestHitbox( CBaseEntity* pLocal, CUserCmd* pCommand, CBaseCombatWeapon* wpn, float& speed, float& gravity ) {

		CBaseEntity* pEntity = GetBaseEntity( Global.aim_index );
		int Class = pLocal->GetClass();

		if( !pEntity || pEntity->IsDormant() || pEntity->GetLifeState() != LIFE_ALIVE ) {
			return Vector();
		}

		switch( Global.aim_mode ) {
			case MODE::PLAYER:
			{
				if( wpn->GetSlot() == 2 ) return pEntity->GetHitbox( pLocal, 4 );

				if( speed != -1 ) {
					Vector Hitbox;
					weaponid id = wpn->GetItemDefinitionIndex();

					if( id == weaponid::Soldier_s_TheRighteousBison || id == weaponid::Demoman_m_TheLooseCannon ) {
						Hitbox = pEntity->GetHitbox( pLocal, 4, true );
					} else if( Class == TF2_Demoman || Class == TF2_Soldier ) {
						Hitbox = pEntity->GetAbsOrigin();
						Hitbox[2] += 25.0f;
					} else if( id == weaponid::Sniper_m_TheHuntsman || id == weaponid::Sniper_m_FestiveHuntsman || id == weaponid::Sniper_m_TheFortifiedCompound ) {
						Hitbox = pEntity->GetHitbox( pLocal, 0, true );
					} else {
						Hitbox = pEntity->GetHitbox( pLocal, 4, true );
					}

					return Util::ProjectilePrediction( pLocal, pEntity, Hitbox, speed, gravity );

				}
				switch( Global.Aimbot_hitbox.value ) {
					case 0:
					case 1:
					{
						float score = FLT_MAX;
						int hitbox_ind = -1;
						Vector hitbox;

						for( int box = Util::isHeadshotWeapon( Class, wpn ) ? 0 : 4; box < 17; box++ ) {
							Vector pos = Global.Aimbot_multipoint.value ? pEntity->GetMultipoint( pLocal, box ) : pEntity->GetHitbox( pLocal, box );
							float fov = Util::GetFOV( pCommand->viewangles, Util::CalcAngle( pLocal->GetEyePosition(), pos ) );

							if( !pos.IsZero() && fov < score ) {
								hitbox_ind = box;
								score = fov;
								if( Global.Aimbot_hitbox.value ) {
									return pos;
								} else {
									hitbox = pos;
								}
							}
						}

						if( hitbox_ind != -1 ) {
							Global.hitbox = hitbox_ind;
							return hitbox;
						} else {
							return Vector();
						}
						break;
					};

					case 2:
					{
						if( Global.Aimbot_multipoint.value )
							return pEntity->GetMultipoint( pLocal, 0 );
						else
							return pEntity->GetHitbox( pLocal, 0 );
					};

					case 3:
					{
						if( Global.Aimbot_multipoint.value )
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
				if( BacktrackData[Global.aim_index].size() > Global.backtrack_arr && Backtrack::is_tick_valid( BacktrackData[Global.aim_index][Global.backtrack_arr].simtime ) ) {
					return BacktrackData[Global.aim_index][Global.backtrack_arr].hitbox;
				} else if( !Global.latency.value ) {
					return pEntity->GetHitbox( pLocal, Global.hitbox );
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