#include "sdk.h"
#include "../tools/util/Util.h"
#include "../tools/signature/csignature.h"

bool CBaseEntity::CanSee( CBaseEntity* pEntity, const Vector& pos ) {
	trace_t tr;
	Ray_t ray;
	CTraceFilterNothing filter;
	ray.Init( this->GetEyePosition(), pos );
	gInts.EngineTrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	if( tr.m_pEnt == nullptr || pEntity == nullptr ) {
		return tr.fraction == 1.0f;
	}
	return ( tr.m_pEnt->GetIndex() == pEntity->GetIndex() ) || ( tr.m_pEnt->GetTeamNum() == pEntity->GetTeamNum() );
}

Vector CBaseEntity::CanSeeSpot( CBaseEntity* pEntity, const Vector& pos ) {
	trace_t tr;
	Ray_t ray;
	CTraceFilterNothing filter;
	ray.Init( this->GetEyePosition(), pos );
	gInts.EngineTrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	if( tr.m_pEnt == nullptr || pEntity == nullptr ) {
		if( tr.fraction == 1.0f )
			return tr.endpos;
	}

	if( ( tr.m_pEnt->GetIndex() == pEntity->GetIndex() ) || ( tr.m_pEnt->GetTeamNum() == pEntity->GetTeamNum() ) ) {
		return tr.endpos;
	}

	return Vector();
}

void CBaseEntity::RemoveNoDraw() {
	*(byte*)( this + 0x7C ) &= ~32;//m_fEffects
	static auto add_to_leaf_system = ( int( __thiscall* )( void*, int ) )( Signatures::GetClientSignature( "55 8B EC 56 FF 75 08 8B F1 B8" ) );
	if( add_to_leaf_system )
		add_to_leaf_system( this, 7 );
}

int CBaseEntity::registerGlowObject( Color color, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded ) {

	using registerFn = int( __thiscall* )( CGlowObjectManager*, CBaseEntity*, Vector&, float, bool, bool, int );
	static DWORD dwFn = Signatures::GetClientSignature( "55 8B EC 51 53 56 8B F1 57 8B 5E 14" );
	static registerFn Register = (registerFn)dwFn;

	return Register( gInts.GlowManager, this, color.rgb(), color[3] / 255.0f, bRenderWhenOccluded, bRenderWhenUnoccluded, -1 );
}

bool CBaseEntity::HasGlowEffect() {
	for( int n = 0; n < gInts.GlowManager->m_GlowObjectDefinitions.Count(); n++ ) {
		GlowObjectDefinition_t& GlowObject = gInts.GlowManager->m_GlowObjectDefinitions[n];
		CBaseEntity* ent = gInts.EntList->GetClientEntityFromHandle( GlowObject.m_hEntity );
		if( ent == this )
			return true;
	}
	return false;
}

Vector CBaseEntity::GetHitbox( CBaseEntity* pLocal, int hitbox, bool blind ) {
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

	Vector center = ( box->bbmin + box->bbmax ) * 0.5f;
	Vector vHitbox;
	Util::vector_transform( center, matrix[box->bone], vHitbox );
	vHitbox = pLocal->CanSeeSpot( this, vHitbox );
	if( blind || !vHitbox.IsZero() ) {
		return vHitbox;
	} else {
		Util::vector_transform( center, matrix[box->bone], vHitbox );
		return vHitbox;
	}
}

Vector CBaseEntity::GetMultipoint( CBaseEntity* pLocal, int hitbox, bool blind ) {
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

	Vector min = box->bbmin * 0.9f;
	Vector max = box->bbmax * 0.9f;

	Vector center = ( min + max ) * 0.5f;
	Vector vHitbox;

	Util::vector_transform( center, matrix[box->bone], vHitbox );

	if( pLocal->CanSee( this, vHitbox ) )
		return vHitbox;

	Vector Points[]{
		Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( min.x, min.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, max.z ),
		Vector( max.x, max.y, max.z ),
	};

	for( auto point : Points ) {
		Util::vector_transform( point, matrix[box->bone], vHitbox );
		vHitbox = pLocal->CanSeeSpot( this, vHitbox );
		if( !vHitbox.IsZero() )
			return vHitbox;
	}

	if( blind ) {
		Util::vector_transform( center, matrix[box->bone], vHitbox );
		return vHitbox;
	} else {
		return Vector();
	}
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon() {
	DYNVAR( pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon" );
	return (CBaseCombatWeapon*)gInts.EntList->GetClientEntityFromHandle( pHandle.GetValue( this ) );
}
