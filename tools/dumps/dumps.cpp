#include "dumps.h"
#include "../../sdk/headers/json.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <algorithm>
void Dumps::dumpClassIds() {

  vector< pair<string, int> > id;
  
  for( ClientClass *list = gInts.Client->GetAllClasses(); list != nullptr; list = list->next ) {
    id.push_back( { list->name, list->classId } );
  }
  
  sort( begin( id ), end( id ) );
  ofstream dump( "classId.txt" );
  dump << "enum class classId : int {\n";
  
  for( auto it = begin( id ); it != end( id ); it++ ) {
    dump << it->first << " = " << it->second << ",\n";
  }
  
  dump << "}";
  dump.close();
}


void DumpTable( RecvTable *table, ofstream &dump, int level ) {
  for( int i = 0; i < table->m_nProps; i++ ) {
    RecvProp *prop = table->m_pProps + i;
    
    if( !prop ) {
      continue;
    }
    
    if( strcmp( "baseclass", prop->m_pVarName ) != 0 ) {
      if( prop->m_pVarName[0] == '0' ) {
        dump << string( level, ' ' ) << " Table size " <<  table->m_pProps[table->m_nProps - 1].GetOffset() / 4 + 1 << '\n';
        break;
      } else {
        dump << string( level, ' ' ) << prop->m_pVarName << " = 0x" << hex << prop->GetOffset() << '\n';
      }
    }
    
    auto table = prop->GetDataTable();
    
    if( table != NULL ) {
      dump << string( level, ' ' ) << table->m_pNetTableName << "{\n";
      DumpTable( prop->GetDataTable(), dump, level + 2 );
      dump << string( level, ' ' ) << "}\n";
    }
  }
}
void Dumps::dumpNetvars() {
  ofstream dump( "netvar.txt" );
  
  for( ClientClass *list = gInts.Client->GetAllClasses(); list != nullptr; list = list->next ) {
    dump << list->name  << " : id = " << list->classId << "{\n";
    DumpTable( list->table, dump, 0 );
    dump << "}\n";
  }
  
  dump << '\n';
  dump.close();
}
