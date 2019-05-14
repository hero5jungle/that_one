#include "esp.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../tools/util/util.h"
namespace ESP {

  void Run( CBaseEntity *pLocal ) {
    if( !gCvars.ESP_enable.value ) {
      return;
    }
    
    if( gCvars.ESP_misc.value ) {
      if( gCvars.ESP_fov.value ) {
        int iWidth, iHeight;
        gInts.Engine->GetScreenSize( iWidth, iHeight );
        int radius = tanf( DEG2RAD( gCvars.Aimbot_fov.value ) / 2 ) / tanf( DEG2RAD( ( pLocal->GetCond() & tf_cond::TFCond_Zoomed && !gCvars.sniper_nozoom.value ) ? 30.0f : 90.0f ) / 2 ) * iWidth;
        
        if( gCvars.ESP_fov.value == 1 ) {
          DrawManager::DrawCircle( ( float )iWidth / 2.0f, ( float )iHeight / 2.0f, radius, 16.0f, gCvars.color_fov.get_color() );
        } else if( gCvars.ESP_fov.value == 2 ) {
          DrawManager::Rotating_Dot_Circle( ( float )iWidth / 2.0f, ( float )iHeight / 2.0f, radius, 7.0f, gCvars.color_fov.get_color() );
        }
      }
      
      if( ( gCvars.ESP_target.value == 1 || gCvars.ESP_target.value == 3 ) && gCvars.aim_index != -1 ) {
        Vector draw0, draw1;
        
        if( !gCvars.aim.IsZero() )
          if( DrawManager::WorldToScreen( gCvars.aim, draw0 ) && DrawManager::WorldToScreen( pLocal->GetAbsOrigin(), draw1 ) ) {
            DrawManager::DrawLine( draw0.x, draw0.y, draw1.x, draw1.y, gCvars.color_aim.get_color() );
            DrawManager::DrawRect( draw0.x - 5, draw0.y - 5, 10, 10, gCvars.color_aim.get_color() );
          }
      }
    }
    
    if( gCvars.ESP_text.value ) {
      for( int i = 1; i <= gInts.EntList->GetHighestEntityIndex(); i++ ) {
        CBaseEntity *pEnt = gInts.EntList->GetClientEntity( i );
        
        if( !pEnt ) {
          continue;
        }
        
        if( pEnt->IsDormant() ) {
          continue;
        }
        
        if( pEnt == pLocal ) {
          continue;
        }
        
        Vector center = pEnt->GetWorldSpaceCenter();
        Vector pos;
        string name = pEnt->GetClientClass()->chName;
        classId id = ( classId )pEnt->GetClassId();
        float distance = Util::Distance( pLocal->GetEyePosition(), center );
        Color team = Util::team_color( pLocal, pEnt );
        
        if( distance < 100 )
          if( DrawManager::WorldToScreen( center, pos ) ) {
            if( gCvars.ESP_building_text.value ) {
              if( id == classId::CObjectDispenser ) {
                CObjectDispenser *pDispenser = ( CObjectDispenser * )( pEnt );
                
                if( pDispenser == nullptr ) {
                  return;
                }
                
                if( !pDispenser->GetLevel() ) {
                  return;
                }
                
                if( gCvars.ESP_building_text.value ) {
                  DrawManager::DrawString( pos.x, pos.y, team, L"Dispenser: " + to_wstring( pDispenser->GetLevel() ) );
                  pos.y += DrawManager::GetESPHeight();
                  
                  if( pDispenser->IsSapped() ) {
                    DrawManager::DrawString( pos.x, pos.y, Colors::White, L"**SAPPED**", gFonts.anon );
                    pos.y += DrawManager::GetESPHeight();
                  }
                  
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"HP: " + to_wstring( pDispenser->GetHealth() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"Ammo: " + to_wstring( pDispenser->GetMetalReserve() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                }
                
                continue;
              } else if( id == classId::CObjectSentrygun ) {
                CObjectSentryGun *pSentryGun = ( CObjectSentryGun * )( pEnt );
                
                if( pSentryGun == nullptr ) {
                  return;
                }
                
                if( !pSentryGun->GetLevel() ) {
                  return;
                }
                
                if( gCvars.ESP_building_text.value ) {
                  DrawManager::DrawString( pos.x, pos.y, team, L"Sentry: " + to_wstring( pSentryGun->GetLevel() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                  
                  if( pSentryGun->IsSapped() ) {
                    DrawManager::DrawString( pos.x, pos.y, Colors::White, L"**SAPPED**", gFonts.anon );
                    pos.y += DrawManager::GetESPHeight();
                  }
                  
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"HP: " + to_wstring( pSentryGun->GetHealth() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"ammo: " + to_wstring( pSentryGun->GetAmmo() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"rockets: " + to_wstring( pSentryGun->GetRocket() ), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, pSentryGun->GetStateString(), gFonts.anon );
                  pos.y += DrawManager::GetESPHeight();
                }
                
                continue;
              } else if( id == classId::CObjectTeleporter ) {
                CObjectTeleporter *pTeleporter = ( CObjectTeleporter * )( pEnt );
                
                if( pTeleporter == nullptr ) {
                  return;
                }
                
                if( !pTeleporter->GetLevel() ) {
                  return;
                }
                
                if( gCvars.ESP_building_text.value ) {
                  DrawManager::DrawString( pos.x, pos.y, team, L"Teleporter: " + to_wstring( pTeleporter->GetLevel() ), gFonts.anon );
                  pos.y += 11;
                  
                  if( pTeleporter->IsSapped() ) {
                    DrawManager::DrawString( pos.x, pos.y, Colors::White, L"**SAPPED**" );
                    pos.y += 11;
                  }
                  
                  DrawManager::DrawString( pos.x, pos.y, Colors::White, L"HP: " + to_wstring( pTeleporter->GetHealth() ), gFonts.anon );
                  pos.y += 11;
                }
                
                continue;
              } else if( id == classId::CCaptureFlag ) {
                if( gCvars.ESP_item_text.value ) {
                  DrawManager::DrawString( pos.x, pos.y, team, L"Intel", gFonts.anon, true );
                }
                
                continue;
              }
            }
          }
          
        if( gCvars.ESP_item_text.value ) {
          const char *model_orig = gInts.ModelInfo->GetModelName( pEnt->GetModel() );
          string model_name( model_orig );
          
          if( model_name.rfind( "models/items", 0 ) == 0 ) {
            if( gCvars.ESP_item_text.value ) {
              DrawManager::DrawString( pos.x, pos.y, Colors::White, ToWchar( model_orig ).substr( 13, model_name.size() - 17 ), gFonts.anon, true );
            }
            
            continue;
          } else if( model_name.rfind( "models/props_halloween", 0 ) == 0 ) {
            if( gCvars.ESP_item_text.value ) {
              DrawManager::DrawString( pos.x, pos.y, Colors::White, ToWchar( model_orig ).substr( 23, model_name.size() - 27 ), gFonts.anon, true );
            }
            
            continue;
          } else if( model_name.rfind( "models/props_medieval", 0 ) == 0 ) {
            if( gCvars.ESP_item_text.value ) {
              DrawManager::DrawString( pos.x, pos.y, Colors::White, ToWchar( model_orig ).substr( 22, model_name.size() - 26 ), gFonts.anon, true );
            }
            
            continue;
          } else if( model_name.rfind( "models/pickups", 0 ) == 0 ) {
            if( gCvars.ESP_item_text.value ) {
              DrawManager::DrawString( pos.x, pos.y, Colors::White, ToWchar( model_orig ).substr( 15, model_name.size() - 19 ), gFonts.anon, true );
            }
            
            continue;
          }
        }
      }
    }
  }
}