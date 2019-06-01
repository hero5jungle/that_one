#pragma once
#include <cassert>
//written by a pleb
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
  void setup( void *base ) {
    baseclass = static_cast<DAWORD **>( base );
    
    while( static_cast<DAWORD *>( *baseclass )[size] ) {
      size++;
    }
    
    original = *baseclass;
    current = std::make_unique<DAWORD[]>( size );
    std::memcpy( current.get(), original, size * sizeof( DAWORD ) );
    *baseclass = current.get();
  };
  
  template <typename Fn>
  const inline Fn get_original( const int index ) {
    assert( index <= size );
    return reinterpret_cast<Fn>( original[index] );
  }
  
  const inline void hook( const int index, void *function ) {
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
    vmt_hook::setup( base );
    rehook( );
  };
  
  const inline Fn get_original( ) {
    return vmt_hook::get_original<Fn>( Index );
  }
  
  const inline void rehook( ) {
    vmt_hook::hook( Index, Function );
  }
  
  const inline void unhook( ) {
    vmt_hook::unhook( Index );
  }
};
// a quick and dirty wrapper that allows lazier vmt_hook management
template<typename Fn>
class vmt_func {
 private:
  int Index = 0;
  void *Function = nullptr;
  vmt_hook *Hook;
 public:
  void setup( vmt_hook *hook, const int index, void *function ) {
    Hook = hook;
    Index = index;
    Function = function;
    rehook();
  };
  
  const inline Fn get_original() {
    return Hook->get_original<Fn>( Index );
  }
  
  const inline void rehook() {
    Hook->hook( Index, Function );
  }
  
  const inline void unhook() {
    Hook->unhook( Index );
  }
};