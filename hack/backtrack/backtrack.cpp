#include "backtrack.h"
#include "../../tools/util/Util.h"
std::deque<LagRecord> BacktrackData[64];
#define BONE_USED_BY_HITBOX 0x100
#define BONE_USED_BY_ANYTHING 0x7FF00
namespace Backtrack {
  void collect_tick() {
    CBaseEntity *pLocal = GetBaseEntity( me );
    
    if( pLocal ) {
      for( int i = 1; i <= gInts.Engine->GetMaxClients(); i++ ) {
        CBaseEntity *pEntity = GetBaseEntity( i );
        
        if( !pEntity ) {
          continue;
        }
        
        if( pEntity->IsDormant() ) {
          continue;
        }
        
        if( pEntity->GetLifeState() != LIFE_ALIVE ) {
          continue;
        }
        
        if( pEntity->GetTeamNum() == pLocal->GetTeamNum() ) {
          continue;
        }
        
        Vector hitbox = pEntity->GetHitboxPosition( gCvars.hitbox != -1 ? gCvars.hitbox : 0 );
        BacktrackData[i].push_front( LagRecord{ false, pEntity->flSimulationTime(), Util::EstimateAbsVelocity( pEntity ).Length(), hitbox } );
        BacktrackData[i].front().valid = pEntity->SetupBones( BacktrackData[i].front().boneMatrix, BONE_USED_BY_HITBOX | BONE_USED_BY_ANYTHING, 256, gInts.globals->curtime );
        
        if( BacktrackData[i].size() > 70 ) {
          BacktrackData[i].pop_back();
        }
      }
    }
  }
  float lerp_time() {
    static ConVar *c_updaterate = gInts.cvar->FindVar( "cl_updaterate" );
    static ConVar *c_minupdate = gInts.cvar->FindVar( "sv_minupdaterate" );
    static ConVar *c_maxupdate = gInts.cvar->FindVar( "sv_maxupdaterate" );
    static ConVar *c_lerp = gInts.cvar->FindVar( "cl_interp" );
    static ConVar *c_cmin = gInts.cvar->FindVar( "sv_client_min_interp_ratio" );
    static ConVar *c_cmax = gInts.cvar->FindVar( "sv_client_max_interp_ratio" );
    static ConVar *c_ratio = gInts.cvar->FindVar( "cl_interp_ratio" );
    float lerp = c_lerp->GetFloat();
    float maxupdate = c_maxupdate->GetFloat();
    int updaterate = c_updaterate->GetInt();
    float ratio = c_ratio->GetFloat();
    int sv_maxupdaterate = c_maxupdate->GetInt();
    int sv_minupdaterate = c_minupdate->GetInt();
    float cmin = c_cmin->GetFloat();
    float cmax = c_cmax->GetFloat();
    
    if( sv_maxupdaterate && sv_minupdaterate ) {
      updaterate = maxupdate;
    }
    
    if( ratio == 0 ) {
      ratio = 1.0f;
    }
    
    if( cmin && cmax && cmin != 1 ) {
      ratio = clamp( ratio, cmin, cmax );
    }
    
    return fmax( lerp, ratio / updaterate );
  }
  CachedINetChannel INetChannel_cache;
  void cache_INetChannel( INetChannel *ch ) {
    INetChannel_cache = CachedINetChannel( ch );
  }
  bool is_tick_valid( float simtime ) {
    float correct = 0;
    correct += INetChannel_cache.flow_incoming;
    correct += INetChannel_cache.flow_outgoing;
    correct += INetChannel_cache.lerptime;
    
    if( gCvars.latency.value ) {
      correct += ( gCvars.latency_amount.value + gCvars.ping_diff.value ) / 1000.0f;
    }
    
    correct = clamp( correct, 0.0f, 1.0f );
    float deltaTime = correct - ( gInts.globals->curtime - simtime );
    return fabs( deltaTime ) <= 0.2f;
  }
}