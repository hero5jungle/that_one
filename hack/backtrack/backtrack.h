#pragma once
#include "../../sdk/sdk.h"
#include <deque>
#include <cmath>
struct LagRecord {
  bool valid;
  float simtime;
  float movement;
  Vector hitbox;
  matrix3x4 boneMatrix[128];
};
extern std::deque<LagRecord> BacktrackData[64];

namespace Backtrack {

  template<typename T> constexpr T clamp( T val, T  min, T max ) {
    return ( ( ( val ) > ( max ) ) ? ( max ) : ( ( ( val ) < ( min ) ) ? ( min ) : ( val ) ) );
  }
  void collect_tick();
  float lerp_time();
  bool is_tick_valid( float simtime );
  
}