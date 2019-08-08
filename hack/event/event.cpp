 #include "event.h"

CEvents gEvents;
Shots shots[64];

void CEvents::InitEvents() {
	Int::EventManager->AddListener( this, "player_hurt", false );
	Int::EventManager->AddListener( this, "player_death", false );
}
void CEvents::UndoEvents() {
	Int::EventManager->RemoveListener( this );
}

void CEvents::FireGameEvent( IGameEvent* event ) {
	if( strstr( event->GetName(), "player_hurt" ) ) {
		int userid = event->GetInt( "userid" );
		int attacker = event->GetInt( "attacker" );
		int crit = event->GetInt( "crit" );

		if( userid == attacker || attacker == 0 ) {
			return;
		}

		int ind = Int::Engine->GetPlayerForUserID( userid );
		shots[ind].hits++;

		if( crit ) {
			shots[ind].headshots++;
		}
	} else if( strstr( event->GetName(), "player_death" ) ) {
		int userid = event->GetInt( "userid" );
		int attacker = event->GetInt( "attacker" );

		if( userid == attacker || attacker == 0 ) {
			return;
		}

		player_info_t pInfo;

		if( !Int::Engine->GetPlayerInfo( userid, &pInfo ) ) {
			return;
		}

		int ind = Int::Engine->GetPlayerForUserID( userid );
		shots[ind].deaths++;
	}
}
