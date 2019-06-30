#pragma once
#include "../../sdk/sdk.h"

struct Shots {
  int hits{ 0 };
  int headshots{ 0 };
  int deaths{ 0 };
  int shots{ 0 };
};

class CEvents : public IGameEventListener2 {
 public:
 
  void InitEvents();
  void UndoEvents();

  void FireGameEvent( IGameEvent *event );
};

extern CEvents gEvents;
extern Shots shots[64];