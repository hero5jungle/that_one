#include "sdk.h"
#include "../tools/util/Util.h"
#include "../tools/signature/csignature.h"

bool CBaseEntity::CanSee( CBaseEntity* pEntity, const Vector& pos ) {
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	ray.Init( this->GetEyePosition(), pos );
	gInts.EngineTrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	if( tr.m_pEnt == nullptr || pEntity == nullptr ) {
		return tr.fraction == 1.0f;
	}

	return (tr.m_pEnt->GetIndex() == pEntity->GetIndex());
}

int CBaseEntity::GetCanSeeHitbox( CBaseEntity* pEntity, const Vector& pos ) {
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

void CBaseEntity::RemoveNoDraw() {
	*(byte*)(this + 0x7C) &= ~32;//m_fEffects
	static auto add_to_leaf_system = (int( __thiscall* )(void*, int))(Signatures::GetClientSignature( "55 8B EC 56 FF 75 08 8B F1 B8" ));
	if( add_to_leaf_system )
		add_to_leaf_system( this, 7 );
}

Vector CBaseEntity::GetHitbox( int hitbox ) {
	DWORD* model = GetModel();

	if( !model ) {
		return Vector();
	}

	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel( model );

	if( !hdr ) {
		return Vector();
	}

	matrix3x4 matrix[128];

	if( !SetupBones( matrix, 128, 0x100, gInts.globals->curtime ) ) {
		return Vector();
	}

	mstudiohitboxset_t* set = hdr->GetHitboxSet( GetHitboxSet() );

	if( !set ) {
		return Vector();
	}

	mstudiobbox_t* box = set->pHitbox( hitbox );

	if( !box ) {
		return Vector();
	}

	Vector center = (box->bbmin + box->bbmax) * 0.5f;
	Vector vHitbox;
	Util::vector_transform( center, matrix[box->bone], vHitbox );
	return vHitbox;
}

Vector CBaseEntity::GetMultipoint( CBaseEntity* pLocal, int hitbox ) {
	DWORD* model = GetModel();

	if( !model ) {
		return Vector();
	}

	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel( model );

	if( !hdr ) {
		return Vector();
	}

	matrix3x4 matrix[128];

	if( !SetupBones( matrix, 128, 0x100, gInts.globals->curtime ) ) {
		return Vector();
	}

	mstudiohitboxset_t* set = hdr->GetHitboxSet( GetHitboxSet() );

	if( !set ) {
		return Vector();
	}

	mstudiobbox_t* box = set->pHitbox( hitbox );

	if( !box ) {
		return Vector();
	}

	Vector min = box->bbmin * 0.7f;
	Vector max = box->bbmax * 0.7f;

	Vector center = (min + max) * 0.5f;
	Vector temp;

	Util::vector_transform( center, matrix[box->bone], temp );

	if( pLocal->CanSee( this, temp ) )
		return temp;

	Vector Points[8]{
		Vector( max.x, min.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( max.x, max.y, max.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( min.x, min.y, min.z )
	};

	for( int i = 0; i < 8; i++ ) {
		Util::vector_transform( Points[i], matrix[box->bone], temp );
		if( pLocal->CanSee( this, temp ) )
			return temp;
	}

	return Vector();
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon() {
	DYNVAR( pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon" );
	return (CBaseCombatWeapon*)gInts.EntList->GetClientEntityFromHandle( pHandle.GetValue( this ) );
}