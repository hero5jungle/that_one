#pragma once
//written by a pleb
class vmt_hook {
private:
	uintptr_t** baseclass = nullptr;
	std::unique_ptr<uintptr_t[]> current;
	uintptr_t* original = nullptr;
	int size = 0;
public:
	void setup( void* base ) {
		baseclass = static_cast<uintptr_t * *>( base );

		if( size == 0 ) {
			while( static_cast<uintptr_t*>( *baseclass )[size] ) {
				size++;
			}
		}
		original = *baseclass;
		current = std::make_unique<uintptr_t[]>( size );
		std::memcpy( current.get(), original, size * sizeof( uintptr_t ) );
		*baseclass = current.get();
	};

	~vmt_hook() {
		restore();
	}

	template <typename Fn>
	const inline Fn get_original( const int index ) {
		return reinterpret_cast<Fn>( original[index] );
	}

	const inline void hook( const int index, void* function ) {
		current[index] = reinterpret_cast<uintptr_t>( function );
	}

	const inline void unhook( const int index ) {
		current[index] = original[index];
	}
	const inline void restore() {
		if( original )
			* baseclass = original;
	}
};
// a quick and dirty wrapper for tables with only one function to hook.
template<typename Fn>
class vmt_single : vmt_hook {
private:
	int Index = 0;
	void* Function = nullptr;
public:
	void setup( void* base, const int index, void* function ) {
		Index = index;
		Function = function;
		vmt_hook::setup( base );
		rehook();
	};

	~vmt_single() {
		vmt_hook::restore();
	}

	const inline Fn get_original() {
		return vmt_hook::get_original<Fn>( Index );
	}

	const inline void rehook() {
		vmt_hook::hook( Index, Function );
	}

	const inline void unhook() {
		vmt_hook::unhook( Index );
	}
	const inline void restore() {
		vmt_hook::restore();
	}
};
// a quick and dirty wrapper for tables with more than one function to hook.
template<typename Fn>
class vmt_func {
private:
	int Index = 0;
	void* Function = nullptr;
	vmt_hook* Hook;
public:
	void setup(  const int index, void* function ) {
		Index = index;
		Function = function;
		Hook->hook( index, function );
	};

	vmt_func( vmt_hook* hook) {
		Hook = hook;
	}

	~vmt_func() {
		Hook->unhook( Index );
	}

	const inline Fn get_original() {
		return Hook->get_original<Fn>( Index );
	}

	const inline void rehook() {
		Hook->hook( Index, Function );
	}

	const inline void unhook() {
		Hook->unhook( Index );
	}
	const inline void restore() {
		Hook->restore();
	}
};