#pragma once
#include "../../sdk/sdk.h"
namespace Util {
	float Distance( const Vector& vOrigin, const Vector& vLocalOrigin );

	void vector_transform( const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut );

	void FixMove( CUserCmd* pCmd, Vector m_vOldAngles, float m_fOldForward, float m_fOldSidemove );
	void lookAt( const bool silent, Vector vAngs, CUserCmd* pCommand );

	float DistanceToGround( CBaseEntity* ent );
	float DistanceToGround( Vector origin, Vector mins );
	float DistanceToGround( Vector origin );

	bool shouldReflect( CBaseEntity* pLocal, CBaseEntity* pEntity, int Class );

	Vector CalcAngle( const Vector& src, const Vector& dst );
	float GetFOV( const Vector& viewAngle, const Vector& aimAngle );
	float GetClockwiseAngle( const Vector& viewAngle, const Vector& aimAngle );

	Vector EstimateAbsVelocity( CBaseEntity* ent );

	bool isHeadshotWeapon( int Class, CBaseCombatWeapon* pWep );
	bool canAmbassadorHeadshot( CBaseCombatWeapon* wpn );
	bool canBackstab( const Vector& from_angle, const Vector& to_angle, Vector wsc_spy_to_victim );
	bool canHeadshot( CBaseEntity* pLocal );

	void minDist( weaponid id, float& dist );
	bool weaponSetup( float& speed, float& chargetime, float& gravity, bool& quick_release, weaponid id, CBaseCombatWeapon* wpn );

	Color team_color( CBaseEntity* pLocal, CBaseEntity* pEntity );
}
