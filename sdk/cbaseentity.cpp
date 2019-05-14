#include "sdk.h"
#include "../tools/util/Util.h"

bool CBaseEntity::CanSee( CBaseEntity *pEntity, const Vector &pos ) {
  trace_t tr;
  Ray_t ray;
  CTraceFilter filter;
  ray.Init( this->GetEyePosition(), pos );
  gInts.EngineTrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );
  
  if( tr.m_pEnt == nullptr || pEntity == nullptr ) {
    return tr.fraction == 1.0f;
  }
  
  return ( tr.m_pEnt->GetIndex() == pEntity->GetIndex() );
}

int CBaseEntity::GetCanSeeHitbox( CBaseEntity *pEntity, const Vector &pos ) {
  trace_t tr;
  Ray_t ray;
  CTraceFilter filter;
  ray.Init( this->GetEyePosition(), pos );
  gInts.EngineTrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );
  
  if( tr.m_pEnt && tr.m_pEnt->GetIndex() == pEntity->GetIndex() ) {
    return tr.hitbox;
  }
  
  return -1;
}

Vector CBaseEntity::GetHitbox( int hitbox ) {
  DWORD *model = GetModel();
  
  if( !model ) {
    return Vector();
  }
  
  studiohdr_t *hdr = gInts.ModelInfo->GetStudiomodel( model );
  
  if( !hdr ) {
    return Vector();
  }
  
  matrix3x4 matrix[128];
  
  if( !SetupBones( matrix, 128, 0x100, gInts.globals->curtime ) ) {
    return Vector();
  }
  
  mstudiohitboxset_t *set = hdr->GetHitboxSet( GetHitboxSet() );
  
  if( !set ) {
    return Vector();
  }
  
  mstudiobbox_t *box = set->pHitbox( hitbox );
  
  if( !box ) {
    return Vector();
  }
  
  Vector center = ( box->bbmin + box->bbmax ) * 0.5f;
  Vector vHitbox;
  Util::vector_transform( center, matrix[box->bone], vHitbox );
  return vHitbox;
}

CBaseCombatWeapon *CBaseEntity::GetActiveWeapon() {
  DYNVAR( pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon" );
  return ( CBaseCombatWeapon * )gInts.EntList->GetClientEntityFromHandle( pHandle.GetValue( this ) );
}