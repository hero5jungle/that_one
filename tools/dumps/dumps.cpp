#include "../../sdk/sdk.h"
#include <algorithm>
#include <fstream>
void Dumps::dumpClassIds() {

	std::vector< std::pair<std::string, int> > id;

	for( ClientClass* list = Int::Client->GetAllClasses(); list != nullptr; list = list->next ) {
		id.push_back( { list->name, list->classId } );
	}

	std::sort( begin( id ), end( id ) );
	std::ofstream dump( "classId.txt" );
	dump << "enum class classId : int {\n";

	for( auto it = begin( id ); it != end( id ); it++ ) {
		dump << it->first << " = " << it->second << ",\n";
	}

	dump << "}";
	dump.close();
}


void DumpTable( RecvTable* table, std::ofstream& dump, int level ) {
	for( int i = 0; i < table->m_nProps; i++ ) {
		RecvProp* prop = table->m_pProps + i;

		if( !prop ) {
			continue;
		}

		if( strcmp( "baseclass", prop->m_pVarName ) != 0 ) {
			if( prop->m_pVarName[0] == '0' ) {
				dump << std::string( level, ' ' ) << " Table size " << table->m_pProps[table->m_nProps - 1].GetOffset() / 4 + 1 << '\n';
				break;
			} else {
				dump << std::string( level, ' ' ) << prop->m_pVarName << " = 0x" << std::hex << prop->GetOffset() << '\n';
			}
		}

		auto Table = prop->GetDataTable();

		if( Table != NULL ) {
			dump << std::string( level, ' ' ) << Table->m_pNetTableName << " {\n";
			DumpTable( prop->GetDataTable(), dump, level + 2 );
			dump << std::string( level, ' ' ) << "}\n";
		}
	}
}
void Dumps::dumpNetvars() {
	std::ofstream dump( "netvar.txt" );

	for( ClientClass* list = Int::Client->GetAllClasses(); list != nullptr; list = list->next ) {
		dump << list->name << " : id = " << list->classId << "{\n";
		DumpTable( list->table, dump, 0 );
		dump << "}\n";
	}

	dump << '\n';
	dump.close();
}
