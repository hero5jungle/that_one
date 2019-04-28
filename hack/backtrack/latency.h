#pragma once
#include "../../sdk/sdk.h"
#include <deque>

extern INetChannel *old_ch;
extern VMTBaseManager *datagramHook;
extern std::deque<CIncomingSequence>sequences;
extern int lastincomingsequencenumber;

namespace Latency {

  void UpdateIncomingSequences();
  
  void ClearIncomingSequences();
  
  void AddLatencyToNetchan( CNetChan *netchan, float Latency );
}