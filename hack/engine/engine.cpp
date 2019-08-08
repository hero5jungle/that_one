#include "engine.h"
namespace EnginePred {
	float curtime;
	float frametime;
	CMoveData moveData;
	void Start( CBaseEntity* pLocal, CUserCmd* cmd ) {
		if( Global.engine.value ) {
			// backup time
			curtime = Int::globals->curtime;
			frametime = Int::globals->frametime;
			//edge jump start
			int past_GroundEntity = pLocal->GetGroundEntity();
			//make moveData
			memset( &moveData, 0, sizeof( CMoveData ) );
			// correct time
			Int::globals->curtime = pLocal->GetTickBase() * Int::globals->interval_per_tick;
			Int::globals->frametime = Int::globals->interval_per_tick;
			// set host
			pLocal->SetCurrentCommand( cmd );
			// start prediction
			Int::GameMovement->StartTrackPredictionErrors( pLocal );
			Int::Prediction->SetupMove( pLocal, cmd, &moveData );
			Int::GameMovement->ProcessMovement( pLocal, &moveData );
			Int::Prediction->FinishMove( pLocal, cmd, &moveData );
			//edge jump end
			int current_GroundEntity = pLocal->GetGroundEntity();

			if( Global.engine_edgejump.value ) {
				if( past_GroundEntity != -1 && current_GroundEntity == -1 ) {
					cmd->buttons |= IN_JUMP;
				}
			}
		}
	}
	void End( CBaseEntity* pLocal, CUserCmd* cmd ) {
		if( Global.engine.value ) {
			// finish prediction
			Int::GameMovement->FinishTrackPredictionErrors( pLocal );
			// restore time
			Int::globals->curtime = curtime;
			Int::globals->frametime = frametime;
			// reset host
			pLocal->SetCurrentCommand( nullptr );
			//adjust tickbase
			pLocal->UpTickBase();
		}
	}
}