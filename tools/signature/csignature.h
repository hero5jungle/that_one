#pragma once

#include "../../sdk/sdk.h"

namespace Signatures {
  DWORD dwFindPattern( DWORD dwAddress, DWORD dwLength, const char *szPattern );
  HMODULE GetModuleHandleSafe( const char *pszModuleName );
  DWORD GetClientSignature( char *chPattern );
  DWORD GetEngineSignature( char *chPattern );
}
