#pragma once
#include <cassert>
//written by a pleb, inspiration taken from aixxe's https://github.com/aixxe/vmthook code
#ifdef _WIN64
using DAWORD = DWORD64;
#else
using DAWORD = DWORD;
#endif
class vmt_hook {
 private:
  DAWORD **baseclass = nullptr;
  std::unique_ptr<DAWORD[]> current;
  DAWORD *original = nullptr;
  int size = 0;
 public:
  vmt_hook( ) = default;
  
  void setup( void *base, const int index, void *function ) {
    baseclass = static_cast<DAWORD **>( base );
    
    while( static_cast<DAWORD *>( *baseclass )[size] ) {
      size++;
    }
    
    original = *baseclass;
    current = std::make_unique<DAWORD[]>( size );
    std::memcpy( current.get(), original, size * sizeof( DAWORD ) );
    *baseclass = current.get();
    hook( function, index );
  };
  
  template <typename Fn>
  const inline Fn get_original( const int index ) {
    assert( index <= size );
    return reinterpret_cast<Fn>( original[index] );
  }
  
  const inline void hook( void *function, const int index ) {
    assert( index <= size );
    current[index] = reinterpret_cast<DAWORD>( function );
  }
  
  const inline void unhook( const int index ) {
    assert( index <= size );
    current[index] = original[index];
  }
  
};
// a quick and dirty wrapper for tables with only one function to hook.
template<typename Fn>
class vmt_single : vmt_hook {
 private:
  int Index = 0;
  void *Function = nullptr;
 public:
  void setup( void *base, const int index, void *function ) {
    Index = index;
    Function = function;
    vmt_hook::setup( base, index, function );
  };
  
  const inline Fn get_original( ) {
    return vmt_hook::get_original<Fn>( Index );
  }
  
  const inline void rehook( ) {
    vmt_hook::hook( Function, Index );
  }
  
  const inline void unhook( ) {
    vmt_hook::unhook( Index );
  }
};