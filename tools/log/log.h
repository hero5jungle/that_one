#pragma once
#include "../../sdk/sdk.h"
#ifdef __DEBUG
#define LOGDEBUG( X, ... ) Log::Debug( X, __VA_ARGS__ )
#else
#define LOGDEBUG( X, ... )
#endif

#define XASSERT( x ) if( !(x) ) MessageBoxA( 0, #x, 0, 0 );

namespace Log {

  void Init( HMODULE hModule );
  void Debug( const char *fmt, ... );
  void Msg( const char *fmt, ... );
  void Error( const char *fmt, ... );
  void Fatal( const char *fmt, ... );
  
}