#include "esp.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../tools/util/Util.h"
#include "../../sdk/headers/bspflags.h"

namespace ESP {
  int iWidth, iHeight;
  
  void Run( CBaseEntity *pLocal ) {
    gInts.Engine->GetScreenSize( iWidth, iHeight );
    
    if( !gCvars.ESP_enable.value )
      return;
      
    if( gCvars.ESP_fov.value ) {
      int radius = tanf( DEG2RAD( gCvars.Aimbot_fov.value ) / 2 ) / tanf( DEG2RAD( ( pLocal->GetCond() & tf_cond::TFCond_Zoomed && !gCvars.Nozoom.value ) ? 30.0f : 90.0f ) / 2 ) * iWidth;
      
      if( gCvars.ESP_fov.value == 1 )
        DrawManager::DrawCircle( ( float )iWidth / 2.0f, ( float )iHeight / 2.0f, radius, 16.0f, gCvars.color_fov.get_color() );
      else if( gCvars.ESP_fov.value == 2 )
        DrawManager::Rotating_Dot_Circle( ( float )iWidth / 2.0f, ( float )iHeight / 2.0f, radius, 7.0f, gCvars.color_fov.get_color() );
    }
    
    if( ( gCvars.ESP_target.value == 1 || gCvars.ESP_target.value == 3 ) && gCvars.aim_index != -1 ) {
      Vector draw0, draw1;
      
      if( !gCvars.aim.IsZero() )
        if( DrawManager::WorldToScreen( gCvars.aim, draw0 ) && DrawManager::WorldToScreen( pLocal->GetAbsOrigin(), draw1 ) ) {
          DrawManager::DrawLine( draw0.x, draw0.y, draw1.x, draw1.y, gCvars.color_aim.get_color() );
          DrawManager::DrawRect( draw0.x - 5, draw0.y - 5, 10, 10, gCvars.color_aim.get_color() );
        }
    }
    
    if( gCvars.ESP_obj.value ) {
      for( int i = 1; i <= gInts.EntList->GetHighestEntityIndex(); i++ ) {
        CBaseEntity *pEnt = gInts.EntList->GetClientEntity( i );
        
        if( !pEnt )
          continue;
          
        if( pEnt->IsDormant() )
          continue;
          
        if( pEnt == pLocal )
          continue;
          
        Vector center, pos;
        pEnt->GetWorldSpaceCenter( center );
        string name = pEnt->GetClientClass()->chName;
        int id = pEnt->GetClientClass()->iClassID;
        float distance = Util::Distance( pLocal->GetEyePosition(), center );
        Color team = Util::team_color( pLocal, pEnt );
        
        if( distance < 100 )
          if( DrawManager::WorldToScreen( center, pos ) ) {
            if( id == 86 ) {
              CObjectDispenser *pDispenser = ( CObjectDispenser * )( pEnt );
              
              if( pDispenser == nullptr )
                return;
                
              if( !pDispenser->GetLevel() )
                return;
                
              if( gCvars.ESP_build_text.value ) {
                DrawManager::DrawString( pos.x, pos.y, team, "Dispenser: " + to_string( pDispenser->GetLevel() ), gFonts.anon );
                pos.y += 11;
                
                if( pDispenser->IsSapped() ) {
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, "**SAPPED**", gFonts.anon );
                  pos.y += 11;
                }
                
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "HP: " + to_string( pDispenser->GetHealth() ), gFonts.anon );
                pos.y += 11;
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "Ammo: " + to_string( pDispenser->GetMetalReserve() ), gFonts.anon );
                pos.y += 11;
              }
              
              continue;
            } else if( id == 88 ) {
              CObjectSentryGun *pSentryGun = ( CObjectSentryGun * )( pEnt );
              
              if( pSentryGun == nullptr )
                return;
                
              if( !pSentryGun->GetLevel() )
                return;
                
              if( gCvars.ESP_build_text.value ) {
                DrawManager::DrawString( pos.x, pos.y, team, "Sentry: " + to_string( pSentryGun->GetLevel() ), gFonts.anon );
                pos.y += 11;
                
                if( pSentryGun->IsSapped() ) {
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, "**SAPPED**", gFonts.anon );
                  pos.y += 11;
                }
                
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "HP: " + to_string( pSentryGun->GetHealth() ), gFonts.anon );
                pos.y += 11;
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "ammo: " + to_string( pSentryGun->GetAmmo() ), gFonts.anon );
                pos.y += 11;
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "rockets: " + to_string( pSentryGun->GetRocket() ), gFonts.anon );
                pos.y += 11;
                DrawManager::DrawString( pos.x, pos.y, Colors::White, pSentryGun->GetStateString(), gFonts.anon );
                pos.y += 11;
              }
              
              continue;
            } else if( id == 89 ) {
              CObjectTeleporter *pTeleporter = ( CObjectTeleporter * )( pEnt );
              
              if( pTeleporter == nullptr )
                return;
                
              if( !pTeleporter->GetLevel() )
                return;
                
              if( gCvars.ESP_build_text.value ) {
                DrawManager::DrawString( pos.x, pos.y, team, "Teleporter: " + to_string( pTeleporter->GetLevel() ), gFonts.anon );
                pos.y += 11;
                
                if( pTeleporter->IsSapped() ) {
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, "**SAPPED**" );
                  pos.y += 11;
                }
                
                DrawManager::DrawString( pos.x, pos.y, Colors::White, "HP: " + to_string( pTeleporter->GetHealth() ), gFonts.anon );
                pos.y += 11;
              }
              
              continue;
            } else if( id == 26 ) {
              if( gCvars.ESP_item_text.value ) {
                DrawManager::DrawString( pos.x, pos.y, team, "intel", gFonts.anon, true );
              }
              
              continue;
            }
          }
          
        const char *model_orig = gInts.ModelInfo->GetModelName( pEnt->GetModel() );
        string model_name( model_orig );
        
        if( model_name.rfind( "models/items/" ) == 0 ) {
          model_name.erase( 0, 13 );
          model_name.erase( model_name.end() - 4, model_name.end() );
          
          if( gCvars.ESP_item_text.value ) {
            DrawManager::DrawString( pos.x, pos.y, Colors::White, model_name, gFonts.anon, true );
          }
          
          continue;
        } else if( model_name.rfind( "models/props_halloween/" ) == 0 ) {
          model_name.erase( 0, 23 );
          model_name.erase( model_name.end() - 4, model_name.end() );
          
          if( gCvars.ESP_item_text.value ) {
            DrawManager::DrawString( pos.x, pos.y, Colors::White, model_name, gFonts.anon, true );
          }
          
          continue;
        } else if( model_name.rfind( "models/bots/boss_bot/boss_tank" ) == 0 ) {
          model_name.erase( 0, 21 );
          model_name.erase( model_name.end() - 4, model_name.end() );
          
          if( gCvars.ESP_build_text.value ) {
            DrawManager::DrawString( pos.x, pos.y, Colors::White, "tank: " + to_string( pEnt->GetHealth() ), gFonts.anon, true );
          }
          
          continue;
        }
      }
    }
  }
}