#include "../../sdk/sdk.h"
#include "csignature.h"
namespace Signatures {
#define INRANGE(x,a,b)    ((x) >= (a) && (x) <= (b))
#define getBits( x )    (INRANGE(((x)&(~0x20)),'A','F') ? (((x)&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? (x) - '0' : 0))
#define getByte( x )    (getBits((x)[0]) << 4 | getBits((x)[1]))

  DWORD dwFindPattern( DWORD dwAddress, DWORD dwLength, const char *szPattern ) {
    const char *pat = szPattern;
    DWORD firstMatch = NULL;
    
    for( DWORD pCur = dwAddress; pCur < dwLength; pCur++ ) {
      if( !*pat ) return firstMatch;
      
      if( *( PBYTE )pat == '\?' || *( BYTE * )pCur == getByte( pat ) ) {
        if( !firstMatch ) firstMatch = pCur;
        
        if( !pat[2] ) return firstMatch;
        
        if( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' ) pat += 3;
        else pat += 2;
      } else {
        pat = szPattern;
        firstMatch = 0;
      }
    }
    
    return NULL;
  }
  
  HMODULE GetModuleHandleSafe( const char *pszModuleName ) {
    HMODULE hmModuleHandle = nullptr;
    
    do {
      hmModuleHandle = GetModuleHandle( pszModuleName );
      Sleep( 1 );
    } while( hmModuleHandle == nullptr );
    
    return hmModuleHandle;
  }
  
  DWORD GetClientSignature( char *chPattern ) {
    static HMODULE hmModule = GetModuleHandleSafe( "client.dll" );
    static PIMAGE_DOS_HEADER pDOSHeader = ( PIMAGE_DOS_HEADER )hmModule;
    static PIMAGE_NT_HEADERS pNTHeaders = ( PIMAGE_NT_HEADERS )( ( ( DWORD )hmModule ) + pDOSHeader->e_lfanew );
    return dwFindPattern( ( ( DWORD )hmModule ) + pNTHeaders->OptionalHeader.BaseOfCode, ( ( DWORD )hmModule ) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern );
  }
  
  DWORD GetEngineSignature( char *chPattern ) {
    static HMODULE hmModule = GetModuleHandleSafe( "engine.dll" );
    static PIMAGE_DOS_HEADER pDOSHeader = ( PIMAGE_DOS_HEADER )hmModule;
    static PIMAGE_NT_HEADERS pNTHeaders = ( PIMAGE_NT_HEADERS )( ( ( DWORD )hmModule ) + pDOSHeader->e_lfanew );
    return dwFindPattern( ( ( DWORD )hmModule ) + pNTHeaders->OptionalHeader.BaseOfCode, ( ( DWORD )hmModule ) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern );
  }
}
