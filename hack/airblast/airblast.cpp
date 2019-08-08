#include "airblast.h"
#include "../../tools/util/util.h"
namespace Airblast {

	void Run( CBaseEntity* pLocal, CUserCmd* cmd ) {
		if( !Global.Airblast_enable.value ) {
			return;
		}

		if( !pLocal ) {
			return;
		}

		CBaseCombatWeapon* wpn = pLocal->GetActiveWeapon();

		if( !wpn ) {
			return;
		}

		if( pLocal->GetLifeState() != LIFE_ALIVE ) {
			return;
		}

		if( pLocal->GetClass() != TF2_Pyro ) {
			return;
		}

		if( wpn->GetSlot() != 0 || wpn->GetClassId() == weaponid::Pyro_m_ThePhlogistinator ) {
			return;
		}

		float closest_dist = 0.0f;
		Vector closest_vec;

		for( int i = 1; i < Int::EntityList->GetHighestEntityIndex(); i++ ) {
			CBaseEntity* ent = Int::EntityList->GetClientEntity( i );

			if( !ent ) {
				continue;
			}

			if( ent->IsDormant() ) {
				continue;
			}

			if( ent->GetTeamNum() == pLocal->GetTeamNum() ) {
				continue;
			}

			if( !Util::shouldReflect( pLocal, ent, ent->GetClassId() ) ) {
				continue;
			}

			INetChannelInfo* net = Int::Engine->GetNetChannelInfo();
			float latency = net->GetLatency( FLOW_OUTGOING ) + net->GetLatency( FLOW_INCOMING );
			Vector velocity = Util::EstimateAbsVelocity( ent );
			Vector predicted_proj = ent->GetAbsOrigin() + ( velocity * latency );
			float dist = predicted_proj.DistToSqr( pLocal->GetAbsOrigin() );

			if( dist < closest_dist || closest_dist == 0.0f ) {
				closest_dist = dist;
				closest_vec = predicted_proj;
			}
		}

		if( closest_dist == 0 || closest_dist > 34000 ) {
			return;
		}

		Vector previousAngles = cmd->viewangles;
		CBaseEntity* enem = Int::EntityList->GetClientEntity( Global.aim_index );
		Vector angles;
		Vector tr;

		if( pLocal->CanSee( nullptr, closest_vec ) ) {
			tr = ( closest_vec - pLocal->GetEyePosition() );
		} else {
			return;
		}

		VectorAngles( tr, angles );
		ClampAngle( angles );
		Util::lookAt( Global.Airblast_silent.value, angles, cmd );
		cmd->buttons |= IN_ATTACK2;

		if( !Global.Airblast_silent.value ) {
			cmd->viewangles = previousAngles;
		}

		return;
	}

}