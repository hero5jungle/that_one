#pragma once
#include "../../sdk/sdk.h"
#include <deque>
#include <cmath>

struct LagRecord {
  bool valid;
  float simtime;
  float movement;
  Vector hitbox;
  Vector angle;
  Vector wsc;
  matrix3x4 boneMatrix[128];
};

extern std::deque<LagRecord> BacktrackData[64];

namespace Backtrack {


  template<typename T> constexpr T clamp( T val, T  min, T max ) {
    return ( ( ( val ) > ( max ) ) ? ( max ) : ( ( ( val ) < ( min ) ) ? ( min ) : ( val ) ) );
  }
  
  void collect_tick();
  float lerp_time();
  void cache_INetChannel( INetChannel *ch );
  bool is_tick_valid( float simtime );
  
  const inline void Run( INetChannel *ch ) {
    if( gCvars.Backtrack.value ) {
      collect_tick();
      cache_INetChannel( ch );
    }
  };
  
  struct CachedINetChannel {
    float flow_incoming;
    float flow_outgoing;
    float lerptime;
    CachedINetChannel() = default;
    CachedINetChannel( INetChannel *ch ) {
      flow_incoming = ch->GetAvgLatency( FLOW_INCOMING );
      flow_outgoing = ch->GetAvgLatency( FLOW_OUTGOING );
      lerptime = lerp_time();
    }
  };
  
}