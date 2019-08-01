#include "misc.h"
namespace Misc {


	void Run( CBaseEntity* pLocal, CUserCmd* pCommand ) {
		bool ground = ( pLocal->GetFlags() & FL_ONGROUND );
		bool jump = pCommand->buttons & IN_JUMP;

		if( pLocal->GetClass() != TF2_Scout ) {
			if( gCvars.Bunnyhop.value ) {
				if( !ground && jump ) {
					pCommand->buttons &= ~IN_JUMP;

					if( gCvars.Autostrafe.value )
						if( pCommand->mousedx > 1 || pCommand->mousedx < -1 ) { //> 1 < -1 so we have some wiggle room
							pCommand->sidemove = pCommand->mousedx > 1 ? 450.f : -450.f;
						}
				}
			}
		}

		static ConVar* sv_cheats = gInts.cvar->FindVar( "sv_cheats" );
		static bool cheat = false;

		if( gCvars.Sv_cheat.value ) {
			if( sv_cheats->GetInt() == 0 ) {
				sv_cheats->SetValue( 1 );
			}

			cheat = true;
		} else if( sv_cheats->GetInt() == 1 && cheat ) {
			sv_cheats->SetValue( 0 );
			cheat = false;
		}

		static ConVar* tf_avoidteammates_pushaway = gInts.cvar->FindVar( "tf_avoidteammates_pushaway" );

		if( gCvars.NoPush.value ) {
			if( tf_avoidteammates_pushaway->GetInt() == 1 ) {
				tf_avoidteammates_pushaway->SetValue( 0 );
			}
		} else if( tf_avoidteammates_pushaway->GetInt() == 0 ) {
			tf_avoidteammates_pushaway->SetValue( 1 );
		}
	}
}