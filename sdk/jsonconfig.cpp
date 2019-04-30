#include "sdk.h"
#include "cglobalvars.h"
#include "../menu/gui/menu.h"
#include "headers/json.h"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

bool checkExists( string file ) {
  ifstream file_to_check( file );
  bool open = file_to_check.is_open();
  file_to_check.close();
  return open;
}

void SaveToJson() {
  json main;
  
  if( checkExists( "that_one.json" ) ) {
    ifstream read( "that_one.json" );
    read >> main;
    read.close();
  }
  
  string player_class;
  
  if( !gInts.Engine->IsInGame() ) {
    player_class = "general";
  } else if( gCvars.loadbyclass.value ) {
    CBaseEntity *pLocal = GetBaseEntity( me );
    
    if( pLocal ) {
      player_class = string( pLocal->szGetClass() );
    } else {
      player_class = "general";
    }
  } else {
    player_class = "general";
  }
  
  for( auto tab : gMenu.GetTabs()->tabs ) {
    for( auto setting : tab->children ) {
      switch( setting->type ) {
      case e_control::checkbox: {
        auto checkbox = ( Checkbox * )( setting );
        main[player_class][tab->name][checkbox->name] = checkbox->value;
        break;
      }
      
      case e_control::slider: {
        auto slider = ( Slider * )( setting );
        main[player_class][tab->name][slider->name] = slider->value;
        break;
      }
      
      case e_control::listbox: {
        auto list = ( Listbox * )( setting );
        main[player_class][tab->name][list->name] = list->value;
        break;
      }
      
      case e_control::colorpicker: {
        auto colorpicker = ( ColorPicker * )( setting );
        main[player_class][tab->name][colorpicker->name]["color"] = colorpicker->color.to_int();
        main[player_class][tab->name][colorpicker->name]["def"] = colorpicker->bDef;
        main[player_class][tab->name][colorpicker->name]["rainbow"] = colorpicker->rainbow;
        break;
      }
      
      case e_control::keybind: {
        auto keybind = ( KeyBind * )( setting );
        main[player_class][tab->name][keybind->name]["key"] = keybind->key;
        main[player_class][tab->name][keybind->name]["mode"] = keybind->mode;
        break;
      }
      
      // no values
      default: {
        break;
      }
      }
    }
  }
  
  ofstream write( "that_one.json" );
  write << setw( 4 ) << main << "\r\n";
  write.close();
}
void LoadFromJson() {
  if( checkExists( "that_one.json" ) ) {
    json main;
    string player_class;
    
    if( !gInts.Engine->IsInGame() ) {
      player_class = "general";
    } else if( gCvars.loadbyclass.value ) {
      CBaseEntity *pLocal = GetBaseEntity( me );
      
      if( pLocal ) {
        player_class = string( pLocal->szGetClass() );
      } else {
        player_class = "general";
      }
    } else {
      player_class = "general";
    }
    
    ifstream read( "that_one.json" );
    read >> main;
    read.close();
    
    if( !main[player_class].size() ) {
      player_class = "general";
    }
    
    for( auto tab : gMenu.GetTabs()->tabs ) {
      for( auto setting : tab->children ) {
        switch( setting->type ) {
        case e_control::checkbox: {
          auto checkbox = ( Checkbox * )( setting );
          checkbox->value = main[player_class][tab->name][checkbox->name];
          break;
        }
        
        case e_control::slider: {
          auto slider = ( Slider * )( setting );
          slider->value = main[player_class][tab->name][slider->name];
          break;
        }
        
        case e_control::listbox: {
          auto list = ( Listbox * )( setting );
          list->value = main[player_class][tab->name][list->name];
          break;
        }
        
        case e_control::colorpicker: {
          auto colorpicker = ( ColorPicker * )( setting );
          colorpicker->color.from_int( main[player_class][tab->name][colorpicker->name]["color"] );
          colorpicker->bDef = main[player_class][tab->name][colorpicker->name]["def"];
          colorpicker->rainbow = main[player_class][tab->name][colorpicker->name]["rainbow"];
          break;
        }
        
        case e_control::keybind: {
          auto keybind = ( KeyBind * )( setting );
          keybind->key = main[player_class][tab->name][keybind->name]["key"];
          keybind->mode = main[player_class][tab->name][keybind->name]["mode"];
          break;
        }
        
        default: {
          break;
        }
        }
      }
    }
  }
}