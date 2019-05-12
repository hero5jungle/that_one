#pragma once
#include "../../sdk/sdk.h"
namespace Util {
  float Distance( Vector vOrigin, Vector vLocalOrigin );
  
  void vector_transform( const Vector &vSome, const matrix3x4 vMatrix, Vector &vOut );
  
  float VectorialDistanceToGround( Vector origin );
  float DistanceToGround( CBaseEntity *ent );
  
  void FixMove( CUserCmd *pCmd, Vector m_vOldAngles, float m_fOldForward, float m_fOldSidemove );
  void lookAt( const bool silent, Vector vAngs, CUserCmd *pCommand );
  
  bool ShouldReflect( CBaseEntity *ent, int lTeamNum, const char *name );
  
  Vector CalcAngle( const Vector &src, const Vector &dst );
  float GetFOV( Vector viewAngle, const Vector &aimAngle );
  
  Vector EstimateAbsVelocity( CBaseEntity *ent );
  
  bool IsHeadshotWeapon( int Class, CBaseCombatWeapon *pWep );
  bool CanAmbassadorHeadshot( CBaseCombatWeapon *wpn );
  bool canBackstab( Vector from_angle, Vector to_angle, Vector wsc_spy_to_victim );
  bool canHeadshot( CBaseEntity *pLocal );
  
  void minDist( weaponid id, float &dist );
  bool projSetup( float &speed, float &chargetime, float &gravity, bool &quick_release, weaponid id, CBaseCombatWeapon *wpn );
  
  Color team_color( CBaseEntity *pEntity, CBaseEntity *pLocal );
}
