#include "../../sdk/sdk.h"
#include "client.h"
#include "../../hack/aimbot/aimbot.h"
#include "../../hack/trigger/trigger.h"
#include "../../hack/misc/misc.h"
#include "../../hack/sticky/sticky.h"
#include "../../hack/airblast/airblast.h"
#include "../../hack/backtrack/backtrack.h"
#include "../../hack/backtrack/latency.h"
#include "../../sdk/cmat/cmat.h"
#include <unordered_map>
#include "../../tools/log/log.h"
#include "../../tools/util/util.h"

int __fastcall hkSendDatagram( CNetChan *netchan, PVOID, bf_write *datagram ) {
  VMTManager &hook = VMTManager::GetHook( netchan );
  auto orig = hook.GetMethod<int( __thiscall * )( CNetChan *, bf_write * ) >( 46 );
  
  if( !gCvars.latency.value || datagram ) {
    return orig( netchan, datagram );
  }
  
  int instate = netchan->m_nInReliableState;
  int insequencenr = netchan->m_nInSequenceNr;
  Latency::AddLatencyToNetchan( netchan, gCvars.latency_amount.value / 1000.0f );
  int Return = orig( netchan, datagram );
  netchan->m_nInReliableState = instate;
  netchan->m_nInSequenceNr = insequencenr;
  return Return;
}

Vector qLASTTICK;

bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand ) {
  VMTManager &hook = VMTManager::GetHook( ClientMode ); //Get a pointer to the instance of your VMTManager with the function GetHook.
  bool bReturn = hook.GetMethod<bool( __thiscall * )( PVOID, float, CUserCmd * ) >( 21 )( ClientMode, input_sample_frametime, pCommand ); //Call the original.
  //uintptr_t _bp;
  //__asm mov _bp, ebp;
  //bool *send_packet = ( bool * )( * **( uintptr_t ** * )_bp - 1 );
  Latency::UpdateIncomingSequences();
  
  if( !pCommand->command_number ) {
    return false;
  }
  
  INetChannel *ch = gInts.Engine->GetNetChannelInfo();
  
  if( ch ) {
    if( ch != old_ch ) {
      datagramHook->Init( ch );
      datagramHook->HookMethod( &hkSendDatagram, 46 );
      datagramHook->Rehook();
      old_ch = ch;
    }
  }
  
  CBaseEntity *pLocal = GetBaseEntity( me );
  
  if( !pLocal ) {
    return bReturn;
  }
  
  if( !pLocal->IsDormant() )
    if( pLocal->GetLifeState() == LIFE_ALIVE ) {
      try {
        Backtrack::collect_tick();
        Misc::Run( pLocal, pCommand );
        Aimbot::Run( pLocal, pCommand );
        Triggerbot::Run( pLocal, pCommand );
        Airblast::Run( pLocal, pCommand );
        DemoSticky::Run( pLocal, pCommand );
      } catch( ... ) {
        Log::Fatal( "Failed CreateMove" );
      }
    }
    
  qLASTTICK = pCommand->viewangles;
  return false;
}

unordered_map<MaterialHandle_t, Color> worldmats_new, worldmats_old;

void __fastcall FrameStageNotifyThink( PVOID CHLClient, void *_this, ClientFrameStage_t Stage ) {
  if( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
    Latency::UpdateIncomingSequences();
  }
  
  if( gInts.Engine->IsInGame() && Stage == FRAME_RENDER_START ) {
    CBaseEntity *pLocal = GetBaseEntity( me );
    int m_flFOVRate = 0xE5C;// Broken: nv.get_offset("DT_BasePlayer", "localdata", "m_flFOVRate");
    int &fovPtr = *( int * )( pLocal + gNetVars.get_offset( "DT_BasePlayer", "m_iFOV" ) );
    int defaultFov = *( int * )( pLocal + gNetVars.get_offset( "DT_BasePlayer", "m_iDefaultFOV" ) );
    
    if( gCvars.Nozoom.value ) {
      fovPtr = defaultFov;
      *( float * )( pLocal + m_flFOVRate ) = 0;
    }
    
    if( gCvars.Norecoil.value ) {
      *( Vector * )( pLocal + 0xE8C ) = Vector();
    }
    
    static bool Thirdperson_enabled = false;
    bool *thirdperson = ( bool * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_TFPlayer", "m_nForceTauntCam" ) );
    
    if( gCvars.Thirdperson.value ) {
      auto *yaw = ( float * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_BasePlayer", "pl", "deadflag" ) + 4 );
      auto *pitch = ( float * )( ( DWORD )( pLocal ) + gNetVars.get_offset( "DT_BasePlayer", "pl", "deadflag" ) + 8 );
      *yaw = qLASTTICK.x;
      *pitch = qLASTTICK.y;
      
      if( pLocal->GetLifeState() == LIFE_ALIVE ) {
        *thirdperson = true;
        Thirdperson_enabled = true;
      }
    } else if( !Thirdperson_enabled || !gCvars.Thirdperson.value ) {
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
  
  VMTManager &FrameStageNotifyThunk = VMTManager::GetHook( CHLClient );
  return FrameStageNotifyThunk.GetMethod<void( __fastcall * )( PVOID, void *, ClientFrameStage_t ) >( 35 )( CHLClient, _this, Stage );
}
void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld ) {
  VMTManager &hook = VMTManager::GetHook( gInts.MdlRender );
  const char *pszModelName = gInts.ModelInfo->GetModelName( pInfo.pModel );
  hook.Unhook();
  CBaseEntity *pEntity = ( CBaseEntity * )gInts.EntList->GetClientEntity( pInfo.entity_index );
  CBaseEntity *pLocal = ( CBaseEntity * )gInts.EntList->GetClientEntity( gInts.Engine->GetLocalPlayer() );
  
  if( !pEntity || !pLocal ) {
    gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
    hook.Rehook();
    return;
  }
  
  Color team_color = Util::team_color( pLocal, pEntity );
  
  if( gCvars.ESP_hat.value && strstr( pszModelName, "player/items" ) ) {
    hook.Rehook();
    return;
  }
  
  IMaterial *wanted_material = nullptr;
  
  switch( gCvars.ESP_cham_mat.value ) {
  case 0: {
    wanted_material = Materials::shaded;
    break;
  }
  
  case 1: {
    wanted_material = Materials::glow;
    break;
  }
  
  case 2: {
    wanted_material = Materials::shiny;
    break;
  }
  
  default:
    break;
  }
  
  if( wanted_material ) {
    if( gCvars.ESP_cham.value ) {
      if( pEntity && !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE ) {
        if( strstr( pszModelName, "models/player" ) || strstr( pszModelName, "models/bots" ) ) {
          if( !gCvars.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum() ) {
            if( gCvars.ESP_backtrack.value && gCvars.Backtrack.value ) {
              if( gCvars.aim_index == pInfo.entity_index ) {
                int ticks = 0;
                
                for( int i = 0; i < ( int )BacktrackData[gCvars.aim_index].size() && ticks < 12; i++ ) {
                  if( Backtrack::is_tick_valid( BacktrackData[gCvars.aim_index][i].simtime ) ) {
                    ticks++;
                    
                    if( BacktrackData[gCvars.aim_index][i].valid && BacktrackData[gCvars.aim_index][i].movement > 45.0f ) {
                      Color tick = i == gCvars.backtrack_arr ? gCvars.color_cham_tick.get_color() : gCvars.color_cham_history.get_color();
                      //Hidden
                      wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
                      Materials::ForceMaterial( wanted_material, tick );
                      gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][i].boneMatrix );
                      //Visible
                      wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
                      Materials::ForceMaterial( wanted_material, tick );
                      gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][i].boneMatrix );
                    }
                  }
                }
              }
            }
            
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
    } else {
      gInts.MdlRender->ForcedMaterialOverride( nullptr );
    }
    
    if( gCvars.ESP_hand.value ) {
      if( strstr( pszModelName, "arms" ) ) {
        if( gCvars.ESP_hand.value == 1 ) {
          gInts.RenderView->SetBlend( 0 );
        } else if( gCvars.ESP_hand.value == 2 ) {
          gInts.RenderView->SetBlend( 0.5 );
        } else if( gCvars.ESP_hand.value == 3 ) {
          if( pLocal->GetLifeState() == LIFE_ALIVE && pLocal->GetHealth() > 0 && !pLocal->IsDormant() ) {
            Materials::ForceMaterial( wanted_material, team_color );
          }
        } else if( gCvars.ESP_hand.value == 4 ) {
          if( pLocal->GetLifeState() == LIFE_ALIVE && pLocal->GetHealth() > 0 && !pLocal->IsDormant() ) {
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, true );
            Materials::ForceMaterial( wanted_material, team_color );
          } else {
            gInts.MdlRender->ForcedMaterialOverride( nullptr );
          }
        }
      } else {
        gInts.MdlRender->ForcedMaterialOverride( nullptr );
      }
    }
    
    if( pEntity ) {
      int id = pEntity->GetClientClass()->iClassID;
      
      if( ( id == 86 || ( id == 88 && !( strstr( pszModelName, "blueprint" ) ) ) || id == 89 || id == 26 ) && gCvars.ESP_obj.value == 2 ) {
        if( !pEntity->IsDormant() )
          if( pEntity->GetLifeState() == LIFE_ALIVE ) {
            //Hidden UnUnLit
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
            Materials::ForceMaterial( wanted_material, team_color );
            gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
            //Visible UnUnLit
            wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
            Materials::ForceMaterial( wanted_material, team_color );
          }
      }
    }
    
    if( strstr( pszModelName, "models/items/" ) || strstr( pszModelName, "models/props_halloween/" ) ) {
      if( gCvars.ESP_obj.value != 3 ) {
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, false );
      }
      
      if( gCvars.ESP_obj.value == 2 ) {
        Color RGBA = gCvars.color_pickup.get_color();
        //Hidden UnUnLit
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
        Materials::ForceMaterial( wanted_material, RGBA );
        gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
        //Visible UnUnLit
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
        Materials::ForceMaterial( wanted_material, RGBA );
      } else if( gCvars.ESP_obj.value == 3 ) {
        Color RGBA = gCvars.color_pickup.get_color();
        //Hidden UnUnLit
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, true );
        Materials::ForceMaterial( wanted_material, RGBA );
        gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
        //Visible UnUnLit
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
        wanted_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, true );
        Materials::ForceMaterial( wanted_material, RGBA );
      }
    }
    
    auto should_cham_proj = []( CBaseEntity * ent, int lTeamNum, const char *name ) -> bool {
      return !strcmp( name, "CTFProjectile_SentryRocket" ) || !strcmp( name, "CTFProjectile_Rocket" ) || !strcmp(
        name, "CTFGrenadePipebombProjectile" ) && ent->GetTeamNum() != lTeamNum;
    };
    
    if( pEntity ) {
      if( should_cham_proj( pEntity, pLocal->GetTeamNum(), pEntity->GetClientClass()->chName ) ) {
        if( gCvars.ESP_proj_cham.value ) {
          Color RGBA = gCvars.color_pickup.get_color();
          //Hidden Lit
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
          Materials::ForceMaterial( wanted_material, RGBA );
          gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
          //Visible UnLit
          wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
          Materials::ForceMaterial( wanted_material, RGBA );
        }
      }
    }
  }
  
  gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
  Materials::ResetMaterial();
  hook.Rehook();
}