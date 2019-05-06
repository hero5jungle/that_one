#include "engine.h"
namespace EnginePred {
  float curtime;
  float frametime;
  CMoveData moveData;
  void Start( CBaseEntity *pLocal, CUserCmd *cmd ) {
    if( gCvars.engine.value ) {
      // backup time
      float curtime = gInts.globals->curtime;
      float frametime = gInts.globals->frametime;
      //edge jump start
      int past_GroundEntity = pLocal->GetGroundEntity();
      //make moveData
      memset( &moveData, 0, sizeof( CMoveData ) );
      // correct time
      gInts.globals->curtime = pLocal->GetTickBase() * gInts.globals->interval_per_tick;
      gInts.globals->frametime = gInts.globals->interval_per_tick;
      // set host
      pLocal->SetCurrentCommand( cmd );
      // start prediction
      gInts.GameMovement->StartTrackPredictionErrors( pLocal );
      gInts.Prediction->SetupMove( pLocal, cmd, &moveData );
      gInts.GameMovement->ProcessMovement( pLocal, &moveData );
      gInts.Prediction->FinishMove( pLocal, cmd, &moveData );
      //edge jump end
      int current_GroundEntity = pLocal->GetGroundEntity();
      
      if( past_GroundEntity != -1 && current_GroundEntity == -1 ) {
        cmd->buttons |= IN_JUMP ;
      }
    }
  }
  void End( CBaseEntity *pLocal, CUserCmd *cmd ) {
    if( gCvars.engine.value ) {
      // finish prediction
      gInts.GameMovement->FinishTrackPredictionErrors( pLocal );
      // restore time
      gInts.globals->curtime = curtime;
      gInts.globals->frametime = frametime;
      // reset host
      pLocal->SetCurrentCommand( nullptr );
      //adjust tickbase
      pLocal->UpTickBase();
    }
  }
}