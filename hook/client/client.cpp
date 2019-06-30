#include "../../sdk/sdk.h"
#include "client.h"
#include "../../tools/util/util.h"
#include "../../hack/aimbot/aimbot.h"
#include "../../hack/misc/misc.h"
#include "../../hack/sticky/sticky.h"
#include "../../hack/airblast/airblast.h"
#include "../../hack/backtrack/backtrack.h"
#include "../../hack/backtrack/latency.h"
#include "../../hack/engine/engine.h"
#include "../../sdk/cmat/cmat.h"
#include <unordered_map>

int __fastcall hkSendDatagram( CNetChan *netchan, PVOID, bf_write *datagram ) {
  auto sendDatagram = gHooks.SendDatagram.get_original( );
  
  if( !gCvars.latency.value || datagram ) {
    return sendDatagram( netchan, datagram );
  }
  
  int instate = netchan->m_nInReliableState;
  int insequencenr = netchan->m_nInSequenceNr;
  Latency::AddLatencyToNetchan( netchan, gCvars.latency_amount.value / 1000.0f );
  int Return = sendDatagram( netchan, datagram );
  netchan->m_nInReliableState = instate;
  netchan->m_nInSequenceNr = insequencenr;
  return Return;
}

Vector qLASTTICK;
bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *cmd ) {
  bool bReturn = gHooks.CreateMove.get_original( )( ClientMode, input_sample_frametime, cmd );
  //uintptr_t _bp;
  //__asm mov _bp, ebp;
  //bool *send_packet = ( bool * )( * **( uintptr_t ** * )_bp - 1 );
  Latency::UpdateIncomingSequences();
  
  if( !cmd->command_number ) {
    return false;
  } else {
    gCvars.last_cmd_number = cmd->command_number;
  }
  
  INetChannel *ch = gInts.Engine->GetNetChannelInfo();
  
  if( ch ) {
    if( ch != old_ch ) {
      gHooks.SendDatagram.setup( ch, gOffsets::SendDatagram, &hkSendDatagram );
      old_ch = ch;
    }
  }
  
  CBaseEntity *pLocal = GetBaseEntity( me );
  
  if( !pLocal ) {
    return bReturn;
  }
  
  if( !pLocal->IsDormant() && pLocal->GetLifeState() == LIFE_ALIVE ) {
      Misc::Run( pLocal, cmd );
      EnginePred::Start( pLocal, cmd );
      Backtrack::Run( pLocal, ch );
      Aimbot::Run( pLocal, cmd );
      Airblast::Run( pLocal, cmd );
      DemoSticky::Run( pLocal, cmd );
      EnginePred::End( pLocal, cmd );
  }
  
  qLASTTICK = cmd->viewangles;
  return false;
}

unordered_map<MaterialHandle_t, Color> worldmats_new, worldmats_old;
void __fastcall Hooked_FrameStageNotifyThink( PVOID CHLClient, void *_this, ClientFrameStage_t Stage ) {
  if( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
    Latency::UpdateIncomingSequences();
  }
  
  if( gInts.Engine->IsInGame() && Stage == FRAME_RENDER_START ) {
    CBaseEntity *pLocal = GetBaseEntity( me );
    int m_flFOVRate = 0xE5C;
    int &fovPtr = *( int * )( pLocal + gNetVars.get_offset( "DT_BasePlayer", "m_iFOV" ) );
    int defaultFov = *( int * )( pLocal + gNetVars.get_offset( "DT_BasePlayer", "m_iDefaultFOV" ) );
    
    if( gCvars.sniper_nozoom.value ) {
      fovPtr = defaultFov;
      pLocal->set( m_flFOVRate, 0.0f );
    }
    
    if( gCvars.NoRecoil.value ) {
      pLocal->set( 0xE8C, Vector() );
    }
    
    static bool Thirdperson_enabled = false;
    bool *thirdperson = ( bool * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_TFPlayer", "m_nForceTauntCam" ) );
    
    if( gCvars.Thirdperson.KeyDown() ) {
      auto *yaw = ( float * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_BasePlayer", "pl", "deadflag" ) + 4 );
      auto *pitch = ( float * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_BasePlayer", "pl", "deadflag" ) + 8 );
      *yaw = qLASTTICK.x;
      *pitch = qLASTTICK.y;
      
      if( pLocal->GetLifeState() == LIFE_ALIVE ) {
        *thirdperson = true;
        Thirdperson_enabled = true;
      }
    } else if( !Thirdperson_enabled || !gCvars.Thirdperson.KeyDown() ) {
      *thirdperson = false;
      Thirdperson_enabled = false;
    }
  }
  
  bool bReset = !gInts.Engine->IsInGame() || !gCvars.world_enabled.value;
  
  if( bReset && worldmats_old.size() ) {
    if( gInts.Engine->IsInGame() ) {
      for( auto &hMat : worldmats_old ) { // Reset the material colors
        IMaterial *mat = gInts.MatSystem->GetMaterial( hMat.first );
        
        if( !mat ) {
          continue;
        }
        
        Color color = hMat.second;
        float blend[4] = { ( float )color[0] / 255.f, ( float )color[1] / 255.f, ( float )color[2] / 255.f, ( float )color[3] / 255.f };
        mat->ColorModulate( blend[0], blend[1], blend[2] );
        mat->AlphaModulate( blend[3] );
      }
    }
    
    worldmats_new.clear();  // Clear cache of materials
    worldmats_old.clear();  //
  }
  
  if( gCvars.world_enabled.value && Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
    Latency::UpdateIncomingSequences();
    
    for( MaterialHandle_t i = gInts.MatSystem->FirstMaterial(); i != gInts.MatSystem->InvalidMaterial(); i = gInts.MatSystem->NextMaterial( i ) ) {
      IMaterial *mat = gInts.MatSystem->GetMaterial( i );
      
      if( !mat ) {
        continue;
      }
      
      bool bIsSkybox = !strcmp( mat->GetTextureGroupName(), "SkyBox textures" );
      
      if( !bIsSkybox && strcmp( mat->GetTextureGroupName(), "World textures" ) != 0 ) {
        continue;
      }
      
      if( worldmats_new.find( i ) == worldmats_new.end() ) {
        float r, g, b, a = mat->GetAlphaModulation();
        mat->GetColorModulation( &r, &g, &b );
        Color old_color( r * 255.f, g * 255.f, b * 255.f, a * 255.f );
        worldmats_old.emplace( i, old_color );
        worldmats_new.emplace( i, old_color );
      }
      
      Color color = worldmats_old.at( i );
      
      if( bIsSkybox ) {
        color = gCvars.sky_clr.bDef ? Color( 255 ) : gCvars.sky_clr.get_color();
      } else {
        color = gCvars.world_clr.bDef ? Color( 255 ) : gCvars.world_clr.get_color();
      }
      
      if( worldmats_new.at( i ) != color ) {
        float blend[4] = { ( float )color[0] / 255.f, ( float )color[1] / 255.f, ( float )color[2] / 255.f, ( float )color[3] / 255.f };
        mat->ColorModulate( blend[0], blend[1], blend[2] );
        mat->AlphaModulate( blend[3] );
        worldmats_new.at( i ) = color;
      }
    }
  }
  
  if( !gInts.Engine->IsInGame() ) {
    Latency::ClearIncomingSequences();
  }
  
  return gHooks.FrameStageNotifyThink.get_original( )( CHLClient, _this, Stage );
}

void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld ) {
  gHooks.DrawModelExecute.unhook( );
  
  if( gCvars.ESP_cham.value ) {
  
    const char *model_name = gInts.ModelInfo->GetModelName( pInfo.pModel );
    CBaseEntity *pEntity = ( CBaseEntity * )gInts.EntList->GetClientEntity( pInfo.entity_index );
    CBaseEntity *pLocal = ( CBaseEntity * )gInts.EntList->GetClientEntity( gInts.Engine->GetLocalPlayer() );
    
    if( (!pEntity || !pLocal) || (gCvars.ESP_hat.value && strstr(model_name, "player/items")) ) {
      gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
      gHooks.DrawModelExecute.rehook();
      return;
    }
    
    Color team_color = Util::team_color(pLocal, pEntity);
    IMaterial *wanted_material = gCvars.ESP_cham_mat.value ? wanted_material = Materials::glow : Materials::shaded;
    
    if( wanted_material ) {
      const auto player_check = []( CBaseEntity * pLocal, CBaseEntity * pEntity, const char *model_name ) {
        bool entity = pEntity && !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE;
        bool model = entity && ( strstr( model_name, "models/player" ) || strstr( model_name, "models/bots" ) );
        bool team = model && ( !gCvars.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum() );
        return team;
      };
      
      if( gCvars.ESP_player_cham.value ) {
        if( player_check( pLocal, pEntity, model_name ) ) {
          //backtrack
          if( gCvars.ESP_backtrack.value && gCvars.Backtrack.value ) {
            if( gCvars.aim_index == pInfo.entity_index ) {
              int ticks = 0;
              
              for( int tick = 0; tick < ( int )BacktrackData[gCvars.aim_index].size() && ticks < 12; tick++ ) {
                if( Backtrack::is_tick_valid( BacktrackData[gCvars.aim_index][tick].simtime ) ) {
                  ticks++;
                  
                  if( BacktrackData[gCvars.aim_index][tick].valid && BacktrackData[gCvars.aim_index][tick].velocity.Length() > 45.0f ) {
                    Color tick_color = tick == gCvars.backtrack_arr ? gCvars.color_cham_tick.get_color() : gCvars.color_cham_history.get_color();
                    //Hidden
                    wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
                    Materials::ForceMaterial( wanted_material, tick_color );
                    gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][tick].boneMatrix );
                    //Visible
                    wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
                    Materials::ForceMaterial( wanted_material, tick_color );
                    gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][tick].boneMatrix );
                  }
                }
              }
            }
          }
          
          //player
          //Hidden
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
          Materials::ForceMaterial( wanted_material, team_color );
          gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
          //Visible
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
          Materials::ForceMaterial( wanted_material, team_color );
        } else {
          gInts.MdlRender->ForcedMaterialOverride( nullptr );
        }
      }
      
      if( gCvars.ESP_hand.value ) {
        if( strstr( model_name, "arms" ) ) {
          if( gCvars.ESP_hand.value == 1 ) {
            gInts.RenderView->SetBlend( 0 );
          } else if( gCvars.ESP_hand.value == 2 ) {
            gInts.RenderView->SetBlend( 0.5 );
          } else if( gCvars.ESP_hand.value == 3 ) {
            Materials::ForceMaterial( wanted_material, team_color );
          } else {
            gInts.MdlRender->ForcedMaterialOverride( nullptr );
          }
        } else {
          gInts.MdlRender->ForcedMaterialOverride( nullptr );
        }
      }
      
      if( gCvars.ESP_building_cham.value ) {
        if( pEntity ) {
          const auto is_building = []( const classId id, const char *model_name ) {
            switch (id) {
              case classId::CObjectDispenser:
              case classId::CObjectSentrygun:
              case classId::CObjectTeleporter:
              case classId::CCaptureFlag: {
                return !strstr(model_name, "blueprint");
              }
              default:
                return false;
            }
          };
          
          if( is_building( ( classId )pEntity->GetClassId(), model_name ) ) {
            if( !pEntity->IsDormant() )
              if( pEntity->GetLifeState() == LIFE_ALIVE ) {
                //Hidden
                wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
                Materials::ForceMaterial( wanted_material, team_color );
                gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
                //Visible
                wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
                Materials::ForceMaterial( wanted_material, team_color );
              }
          }
        }
      }
      
      if( gCvars.ESP_object_cham.value ) {
        const auto is_object = []( const char *name ) {
          bool item = strstr( name, "models/items/" );
          bool halloween = strstr( name, "models/props_halloween/" );
          bool pickup = strstr( name, "models/pickups" );
          bool medieval = strstr( name, "models/props_medieval" );
          return item || halloween || pickup || medieval;
        };
        
        if( is_object( model_name ) ) {
          Color RGBA = gCvars.color_items.get_color();
          //Hidden
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
          Materials::ForceMaterial( wanted_material, RGBA );
          gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
          //Visible
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
          Materials::ForceMaterial( wanted_material, RGBA );
        }
      }
      
      if( pEntity ) {
        if( gCvars.ESP_proj_cham.value ) {
          const auto should_cham_proj = []( CBaseEntity * pLocal, CBaseEntity * pEntity, int Class ) -> bool {
          
            if( pLocal->GetTeamNum() != pEntity->GetTeamNum() ) {
              switch( ( classId )Class ) {
              case classId::CTFProjectile_SentryRocket:
              case classId::CTFStickBomb:
              case classId::CTFGrenadePipebombProjectile:
              case classId::CTFProjectile_Rocket:
                return true;
              }
            }
            
            return false;
          };
          
          if( should_cham_proj( pLocal, pEntity, pEntity->GetClientClass()->classId ) ) {
            Color RGBA = gCvars.color_items.get_color();
            //Hidden
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
            Materials::ForceMaterial( wanted_material, RGBA );
            gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
            //Visible
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
            Materials::ForceMaterial( wanted_material, RGBA );
          }
        }
      }
    }
  }
  
  gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
  Materials::ResetMaterial();
  gHooks.DrawModelExecute.rehook();
}