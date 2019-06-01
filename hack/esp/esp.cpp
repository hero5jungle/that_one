#include "esp.h"
#include "../../tools/draw/cdrawmanager.h"
#include "../../tools/util/util.h"
#include <unordered_map>
namespace ESP {
  void Run( CBaseEntity *pLocal ) {
  
    if( !gCvars.ESP_enable.value ) {
      return;
    }
    
    if( gCvars.ESP_fov.value || gCvars.ESP_around_fov.value ) {
      int iWidth, iHeight;
      gInts.Engine->GetScreenSize( iWidth, iHeight );
      float cx = ( float )iWidth / 2.0f;
      float cy = ( float )iHeight / 2.0f;
      
      int radius = tanf( DEG2RAD( gCvars.Aimbot_fov.value ) / 2 ) / tanf( DEG2RAD( ( pLocal->GetCond() & tf_cond::TFCond_Zoomed && !gCvars.sniper_nozoom.value ) ? 30.0f : 90.0f ) / 2 ) * iWidth;
      
      if( gCvars.ESP_fov.value == 1 ) {
        DrawManager::DrawCircle( cx, cy, radius, 16.0f, gCvars.color_fov.get_color() );
      } else if( gCvars.ESP_fov.value == 2 ) {
        DrawManager::Rotating_Dot_Circle( cx, cy, radius, 14.0f, gCvars.color_fov.get_color() );
      }
      
      if( gCvars.ESP_around_fov.value ) {
        for( int i = 1; i <= gInts.Engine->GetMaxClients(); i++ ) {
          if( i == me ) {
            continue;
          }
          
          CBaseEntity *pEntity = GetBaseEntity( i );
          
          if( !pEntity ) {
            continue;
          }
          
          if( pEntity->IsDormant() ) {
            continue;
          }
          
          if( pEntity->GetLifeState() != LIFE_ALIVE ) {
            continue;
          }
          
          static ConVar *mp_friendlyfire = gInts.cvar->FindVar( "mp_friendlyfire" );
          
          if( mp_friendlyfire->GetInt() == 0 && pEntity->GetTeamNum() == pLocal->GetTeamNum() ) {
            continue;
          }
          
          Vector angle = Util::CalcAngle( pLocal->GetVecOrigin(), pEntity->GetVecOrigin() );
          Vector view_angle;
          gInts.Engine->GetViewAngles( view_angle );
          const float deg = Util::GetClockwiseAngle( view_angle, angle );
          //rotation
          const float xrot = cos( deg - PI / 2 );
          const float yrot = sin( deg - PI / 2 );
          //start and end
          const float x1 = ( radius + 5.0f ) * xrot;
          const float y1 = ( radius + 5.0f ) * yrot;
          const float x2 = ( radius + 15.0f ) * xrot;
          const float y2 = ( radius + 15.0f ) * yrot;
          //arrow constants
          constexpr float arrow_angle = DEG2RAD( 90.0f );
          constexpr float arrow_lenght = 6.0f;
          //arrow math
          const Vector line{ x2 - x1, y2 - y1, 0.0f };
          const float length = line.Length();
          //base of arrow
          const float fpoint_on_line = arrow_lenght / ( atanf( arrow_angle ) * length );
          const Vector point_on_line = Vector( x2, y2, 0 ) + ( line * fpoint_on_line * -1.0f );
          const Vector normal_vector{ -line.y, line.x, 0.0f };
          const Vector normal = Vector( arrow_lenght, arrow_lenght, 0.0f ) / ( length * 2 );
          //left and right points
          const Vector rotation = normal * normal_vector;
          const Vector left  = point_on_line + rotation;
          const Vector right = point_on_line - rotation;
          //arrow
          //  ^
          //  |
          DrawManager::DrawLine( cx + x2, cy + y2, cx + left.x, cy + left.y, gCvars.color_fov.get_color() );
          DrawManager::DrawLine( cx + x2, cy + y2, cx + right.x, cy + right.y, gCvars.color_fov.get_color() );
          DrawManager::DrawLine( cx + x1, cy + y1, cx + x2, cy + y2, gCvars.color_fov.get_color() );
        }
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
    
    
    if( gCvars.ESP_building_text.value || gCvars.ESP_item_text.value ) {
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
        string name = pEnt->GetClientClass()->name;
        
        if( DrawManager::WorldToScreen( center, pos ) && Util::Distance( pLocal->GetEyePosition(), center ) < 100 ) {
          if( gCvars.ESP_building_text.value ) {
            switch( ( classId )pEnt->GetClassId() ) {
            case classId::CObjectDispenser: {
              CObjectDispenser *pDispenser = ( CObjectDispenser * )( pEnt );
              
              if( pDispenser == nullptr ) {
                continue;
              }
              
              if( !pDispenser->GetLevel() ) {
                continue;
              }
              
              DrawManager::DrawString( pos.x, pos.y, Util::team_color( pLocal, pEnt ), L"Dispenser: " + to_wstring( pDispenser->GetLevel() ) );
              pos.y += DrawManager::GetESPHeight();
              
              if( pDispenser->IsSapped() ) {
                DrawManager::DrawString( pos.x, pos.y, Colors::White, L"**SAPPED**", gFonts.anon );
                pos.y += DrawManager::GetESPHeight();
              }
              
              DrawManager::DrawString( pos.x, pos.y, Colors::White, L"HP: " + to_wstring( pDispenser->GetHealth() ), gFonts.anon );
              pos.y += DrawManager::GetESPHeight();
              DrawManager::DrawString( pos.x, pos.y, Colors::White, L"Ammo: " + to_wstring( pDispenser->GetMetalReserve() ), gFonts.anon );
              pos.y += DrawManager::GetESPHeight();
              continue;
            }
            
            case classId::CObjectSentrygun: {
              CObjectSentryGun *pSentryGun = ( CObjectSentryGun * )( pEnt );
              
              if( pSentryGun == nullptr ) {
                continue;
              }
              
              if( !pSentryGun->GetLevel() ) {
                continue;
              }
              
              DrawManager::DrawString( pos.x, pos.y, Util::team_color( pLocal, pEnt ), L"Sentry: " + to_wstring( pSentryGun->GetLevel() ), gFonts.anon );
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
              
              continue;
            }
            
            case classId::CObjectTeleporter : {
              CObjectTeleporter *pTeleporter = ( CObjectTeleporter * )( pEnt );
              
              if( pTeleporter == nullptr ) {
                continue;
              }
              
              if( !pTeleporter->GetLevel() ) {
                continue;
              }
              
              DrawManager::DrawString( pos.x, pos.y, Util::team_color( pLocal, pEnt ), L"Teleporter: " + to_wstring( pTeleporter->GetLevel() ), gFonts.anon );
              pos.y += 11;
              
              if( pTeleporter->IsSapped() ) {
                DrawManager::DrawString( pos.x, pos.y, Colors::White, L"**SAPPED**" );
                pos.y += 11;
              }
              
              DrawManager::DrawString( pos.x, pos.y, Colors::White, L"HP: " + to_wstring( pTeleporter->GetHealth() ), gFonts.anon );
              pos.y += 11;
              continue;
            }
            
            case classId::CCaptureFlag : {
              if( gCvars.ESP_item_text.value ) {
                DrawManager::DrawString( pos.x, pos.y, Util::team_color( pLocal, pEnt ), L"Intel", gFonts.anon, true );
              }
              
              continue;
            }
            }
          }
          
          if( gCvars.ESP_item_text.value ) {
            static unordered_map<string, wstring> cached_convert;
            
            string model = gInts.ModelInfo->GetModelName( pEnt->GetModel() );
            int size = model.size();
            
            if( size < 4 || size > 100 ) {//skip nonsense
              continue;
            }
            
            auto cache = [&]( int start ) {
              if( cached_convert.find( model ) == cached_convert.end() ) {
                cached_convert[model] = ToWstring( model.c_str() ).substr( start, model.size() - start - 4 );
              }
            };
            
            if( model.rfind( "models/items", 0 ) == 0 ) {
              cache( 13 );
              DrawManager::DrawString( pos.x, pos.y, Colors::White, cached_convert[model], gFonts.anon, true );
              continue;
            } else if( model.rfind( "models/props_halloween", 0 ) == 0 ) {
              cache( 23 );
              DrawManager::DrawString( pos.x, pos.y, Colors::White, cached_convert[model], gFonts.anon, true );
              continue;
            } else if( model.rfind( "models/props_medieval", 0 ) == 0 ) {
              cache( 22 );
              DrawManager::DrawString( pos.x, pos.y, Colors::White, cached_convert[model], gFonts.anon, true );
              continue;
            } else if( model.rfind( "models/pickups", 0 ) == 0 ) {
              cache( 15 );
              DrawManager::DrawString( pos.x, pos.y, Colors::White, cached_convert[model], gFonts.anon, true );
              continue;
            }
          }
          
        }
      }
    }
  }
}