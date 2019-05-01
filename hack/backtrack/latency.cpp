#include "latency.h"

INetChannel *old_ch = nullptr;
std::deque<CIncomingSequence>sequences;
int lastincomingsequencenumber = 0;

namespace Latency {

  void UpdateIncomingSequences() {
    CNetChan *netchan = gInts.Engine->GetNetChannelInfo();
    
    if( netchan ) {
      if( lastincomingsequencenumber == 0 ) {
        lastincomingsequencenumber = netchan->m_nInSequenceNr;
      }
      
      if( netchan->m_nInSequenceNr > lastincomingsequencenumber ) {
        lastincomingsequencenumber = netchan->m_nInSequenceNr;
        sequences.push_front( CIncomingSequence( netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, gInts.globals->realtime ) );
      }
      
      if( sequences.size() > 2048 ) {
        sequences.pop_back();
      }
    }
  }
  
  
  void ClearIncomingSequences() {
    lastincomingsequencenumber = 0.0f;
    sequences.clear();
  }
  
  void AddLatencyToNetchan( CNetChan *netchan, float Latency ) {
    for( auto &seq : sequences ) {
      if( gInts.globals->realtime - seq.curtime >= Latency ) {
        netchan->m_nInReliableState = seq.inreliablestate;
        netchan->m_nInSequenceNr = seq.sequencenr;
        break;
      }
    }
  }
}