#include "fake.h"
#include "../../tools/util/util.h"

// Initialize Edge vars
float edgeYaw = 0;
float edgeToEdgeOn = 0;

float edgeDistance( CBaseEntity* pLocal, float edgeRayYaw ) {
	trace_t trace;
	Ray_t ray;
	CTraceFilterPlayers filter;
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf( DEG2RAD( edgeRayYaw ) ); // yaw
	cy = cosf( DEG2RAD( edgeRayYaw ) );
	sp = sinf( DEG2RAD( 0 ) ); // pitch
	cp = cosf( DEG2RAD( 0 ) );
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 300.0f + pLocal->GetShootPosition();
	ray.Init( pLocal->GetShootPosition(), forward );
	gInts.EngineTrace->TraceRay( ray, 0x4200400B, &filter, &trace );
	float edgeDistance = ( sqrt( pow( trace.startpos.x - trace.endpos.x, 2 ) + pow( trace.startpos.y - trace.endpos.y, 2 ) ) );
	return edgeDistance;
}

bool findEdge( CBaseEntity* pLocal, float edgeOrigYaw ) {
	float edgeLeftDist = edgeDistance( pLocal, edgeOrigYaw - 21 );
	edgeLeftDist = edgeLeftDist + edgeDistance( pLocal, edgeOrigYaw - 27 );
	float edgeRightDist = edgeDistance( pLocal, edgeOrigYaw + 21 );
	edgeRightDist = edgeRightDist + edgeDistance( pLocal, edgeOrigYaw + 27 );

	if( edgeLeftDist >= 260 )
		edgeLeftDist = 999999999.0f;
	if( edgeRightDist >= 260 )
		edgeRightDist = 999999999.0f;

	if( edgeLeftDist == edgeRightDist )
		return false;

	if( edgeRightDist < edgeLeftDist ) {
		edgeToEdgeOn = 1;
		return true;
	} else {
		edgeToEdgeOn = 2;
		return true;
	}
}

float useEdge( float edgeViewAngle ) {
	bool edgeTest = true;
	if( ( ( edgeViewAngle < -135 ) || ( edgeViewAngle > 135 ) ) && edgeTest == true ) {
		if( edgeToEdgeOn == 1 )
			edgeYaw = (float)-90;
		if( edgeToEdgeOn == 2 )
			edgeYaw = (float)90;
		edgeTest = false;
	}
	if( ( edgeViewAngle >= -135 ) && ( edgeViewAngle < -45 ) && edgeTest == true ) {
		if( edgeToEdgeOn == 1 )
			edgeYaw = (float)0;
		if( edgeToEdgeOn == 2 )
			edgeYaw = (float)179;
		edgeTest = false;
	}
	if( ( edgeViewAngle >= -45 ) && ( edgeViewAngle < 45 ) && edgeTest == true ) {
		if( edgeToEdgeOn == 1 )
			edgeYaw = (float)90;
		if( edgeToEdgeOn == 2 )
			edgeYaw = (float)-90;
		edgeTest = false;
	}
	if( ( edgeViewAngle <= 135 ) && ( edgeViewAngle >= 45 ) && edgeTest == true ) {
		if( edgeToEdgeOn == 1 )
			edgeYaw = (float)179;
		if( edgeToEdgeOn == 2 )
			edgeYaw = (float)0;
		edgeTest = false;
	}
	return edgeYaw;
}


void Fake::Run( CBaseEntity* pLocal, CUserCmd* pCommand, bool* packet ) {
	if( !gCvars.fake.value )
		return;

	if( pCommand->buttons & IN_ATTACK )
		return;

	Vector OldAngles = pCommand->viewangles;
	float  OldForward = pCommand->forwardmove;
	float  OldSidemove = pCommand->sidemove;

	CNetChan* netchan = gInts.Engine->GetNetChannelInfo();

	*packet = !( netchan->m_nChokedPackets < (int)gCvars.fake_amount.value );

	static bool flip = false;

	if( *packet )
		flip = !flip;

	pCommand->viewangles.x = -89.0f;
	if( findEdge( pLocal, pCommand->viewangles.y ) ) {
		if( *packet ) {//fake
			pCommand->viewangles.y = useEdge( pCommand->viewangles.y ) + 180.0f;
		} else {
			pCommand->viewangles.y = useEdge( pCommand->viewangles.y );
		}
	} else {
		Vector vAngs;
		VectorAngles( ( gCvars.aim_spot - pLocal->GetShootPosition() ), vAngs );
		ClampAngle( vAngs );
		if( *packet ) {//fake
			pCommand->viewangles.y = vAngs.y + flip ? 90.0f : -90.0f;
		} else {
			pCommand->viewangles.y = vAngs.y + flip ? -90.0f : 90.0f;
		}
	}
	ClampAngle( pCommand->viewangles );

	Util::FixMove( pCommand, OldAngles, OldForward, OldSidemove );
}
