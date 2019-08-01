#include "keyvalues.h"
namespace Keyvalues {
	DWORD pLoadFromBuffer;
	DWORD pKeyValues;

	void GetOffsets() {
		pKeyValues = Signatures::GetEngineSignature( "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6" );
		pLoadFromBuffer = Signatures::GetEngineSignature( "55 8B EC 83 EC 38 53 8B 5D 0C" );
		pKeyValues -= 0x42;
	}
}
bool KeyValues::LoadFromBuffer( KeyValues* kv, char const* resourceName, const char* pBuffer, IBaseFileSystem* pFileSystem, const char* pPathID ) {
	typedef int( __thiscall * LoadFromBufferFn )( KeyValues * kv, char const*, const char*, IBaseFileSystem*, const char* );
	static LoadFromBufferFn Load = (LoadFromBufferFn)Keyvalues::pLoadFromBuffer;
	return Load( kv, resourceName, pBuffer, pFileSystem, pPathID );
}

KeyValues* KeyValues::Initialize( KeyValues* kv, char* name ) {
	typedef KeyValues* ( __thiscall * InitializeFn )( KeyValues*, char* );
	static InitializeFn Init = (InitializeFn)Keyvalues::pKeyValues;
	return Init( kv, name );
}