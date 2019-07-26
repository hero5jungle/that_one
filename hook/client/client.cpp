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

int __fastcall hkSendDatagram( CNetChan* netchan, PVOID, bf_write* datagram ) {
	auto sendDatagram = gHooks.SendDatagram.get_original();

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
bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* cmd ) {
	bool bReturn = gHooks.CreateMove.get_original()(ClientMode, input_sample_frametime, cmd);
	//uintptr_t _bp;
	//__asm mov _bp, ebp;
	//bool *send_packet = ( bool * )( * **( uintptr_t ** * )_bp - 1 );
	Latency::UpdateIncomingSequences();

	if( !cmd->command_number ) {
		return false;
	} else {
		gCvars.last_cmd_number = cmd->command_number;
	}

	INetChannel* ch = gInts.Engine->GetNetChannelInfo();

	if( ch ) {
		if( ch != old_ch ) {
			gHooks.SendDatagram.setup( ch, gOffsets::SendDatagram, &hkSendDatagram );
			old_ch = ch;
		}
	}

	CBaseEntity* pLocal = GetBaseEntity( me );

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



void __fastcall Hooked_FrameStageNotifyThink( PVOID CHLClient, void* _this, ClientFrameStage_t Stage ) {
	if( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
		Latency::UpdateIncomingSequences();
	}
	if( gInts.Engine->IsInGame() ) {
		CBaseEntity* pLocal = GetBaseEntity( me );

		if( gCvars.ESP_building.value == 2 || gCvars.ESP_player.value == 2 || gCvars.ESP_object.value == 2 ) {
			if( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
				int max = gInts.EntList->GetHighestEntityIndex();
				for( int n = 1; n <= max; n++ ) {
					CBaseEntity* pEntity = GetBaseEntity( n );

					if( pEntity == nullptr || pEntity == pLocal )
						continue;

					bool valid = !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE;
					bool team = !gCvars.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum();

					switch( (classId)pEntity->GetClassId() ) {
						case classId::CTFPlayer:
						{
							if( gCvars.ESP_player.value == 2 ) {
								if( !pEntity->HasGlowEffect() ) {
									pEntity->registerGlowObject( Util::team_color( pLocal, pEntity ), true, true );
								}
								pEntity->SetGlowEnabled( valid && team );
							}

							break;
						}

						case classId::CObjectSentrygun:
						case classId::CObjectDispenser:
						case classId::CObjectTeleporter:
						{
							if( gCvars.ESP_building.value == 2 ) {
								if( !pEntity->HasGlowEffect() ) {
									pEntity->registerGlowObject( Util::team_color( pLocal, pEntity ), true, true );
								}
								pEntity->SetGlowEnabled( valid && team );
							}

							break;
						}

						case classId::CBaseAnimating:
						case classId::CTFAmmoPack:
						{
							if( gCvars.ESP_object.value == 2 ) {
								if( !pEntity->HasGlowEffect() ) {
									pEntity->registerGlowObject( gCvars.color_objects.color, true, true );
								}
								pEntity->SetGlowEnabled( valid && team );
							}
						}

						default:
							break;
					}
				}
			}

			if( Stage == FRAME_RENDER_START ) {
				for( int i = 0; i < gInts.GlowManager->m_GlowObjectDefinitions.Count(); i++ ) {
					GlowObjectDefinition_t& GlowObject = gInts.GlowManager->m_GlowObjectDefinitions[i];

					if( GlowObject.m_nNextFreeSlot != ENTRY_IN_USE )
						continue;

					CBaseEntity* pEntity = gInts.EntList->GetClientEntityFromHandle( GlowObject.m_hEntity );
					if( pEntity ) {
						Color color;
						switch( (classId)pEntity->GetClassId() ) {
							case classId::CTFPlayer:
							{
								if( gCvars.ESP_player.value == 2 ) {
									color = Util::team_color( pLocal, pEntity );
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									gInts.GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}

							case classId::CObjectSentrygun:
							case classId::CObjectDispenser:
							case classId::CObjectTeleporter:
							{
								if( gCvars.ESP_building.value == 2 ) {
									color = Util::team_color( pLocal, pEntity );
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									gInts.GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}
							case classId::CBaseAnimating:
							case classId::CTFAmmoPack:
							{
								if( gCvars.ESP_object.value == 2 ) {
									color = gCvars.color_objects.color;
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									gInts.GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}
							default:
								break;
						}


					} else {
						gInts.GlowManager->m_GlowObjectDefinitions.Remove( i );
					}

				}
			}
		} else {
			if( gInts.GlowManager->m_GlowObjectDefinitions.Count() ) {
				gInts.GlowManager->m_GlowObjectDefinitions.RemoveAll();
			}
		}

		if( gCvars.sniper_nozoom.value ) {
			pLocal->SetFov( pLocal->GetDefaultFov() );
			pLocal->set( 0xE5C, 0.0f );//m_flFOVRate
		}

		if( gCvars.NoRecoil.value ) {
			pLocal->set( 0xE8C, Vector() );
		}

		static bool Thirdperson_enabled = false;

		if( gCvars.Thirdperson.KeyDown() ) {
			pLocal->SetThirdpersonView( qLASTTICK );

			if( pLocal->GetLifeState() == LIFE_ALIVE ) {
				pLocal->SetThirdperson( true );
				Thirdperson_enabled = true;
				if( gCvars.Thirdperson_scoped.value && pLocal->GetCond() & TFCond_Zoomed ) {
					pLocal->RemoveNoDraw();
					if( auto wpn = pLocal->GetActiveWeapon() ) {
						wpn->RemoveNoDraw();
					}
				}
			}
		} else if( !Thirdperson_enabled || !gCvars.Thirdperson.KeyDown() ) {
			pLocal->SetThirdperson( false );
			Thirdperson_enabled = false;
		}

	}

	static unordered_map<MaterialHandle_t, Color> worldmats_new, worldmats_old;

	if( (!gInts.Engine->IsInGame() || !gCvars.world_enabled.value) && worldmats_old.size() ) {
		if( gInts.Engine->IsInGame() ) {
			for( auto& hMat : worldmats_old ) { // Reset the material colors
				IMaterial* mat = gInts.MatSystem->GetMaterial( hMat.first );

				if( !mat ) {
					continue;
				}

				Color color = hMat.second;
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
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
			IMaterial* mat = gInts.MatSystem->GetMaterial( i );

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
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
				mat->ColorModulate( blend[0], blend[1], blend[2] );
				mat->AlphaModulate( blend[3] );
				worldmats_new.at( i ) = color;
			}
		}
	}

	if( !gInts.Engine->IsInGame() ) {
		Latency::ClearIncomingSequences();
	}

	return gHooks.FrameStageNotifyThink.get_original()(CHLClient, _this, Stage);
}

void __stdcall Hooked_DrawModelExecute( void* state, ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld ) {
	gHooks.DrawModelExecute.unhook();

	CBaseEntity* pEntity = GetBaseEntity( pInfo.entity_index );
	CBaseEntity* pLocal = GetBaseEntity( me );

	if( !pEntity || !pLocal ) {
		gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
		gHooks.DrawModelExecute.rehook();
		return;
	}

	const char* model_name = gInts.ModelInfo->GetModelName( pInfo.pModel );
	Color team_color = Util::team_color( pLocal, pEntity );
	IMaterial* wanted_material = gCvars.ESP_cham_mat.value ? Materials::glow : Materials::shaded;

	if( gCvars.ESP_hand.value ) {
		if( strstr( model_name, "arms" ) ) {
			if( gCvars.ESP_hand.value == 1 ) {
				gInts.RenderView->SetBlend( 0 );
			} else if( gCvars.ESP_hand.value == 2 ) {
				gInts.RenderView->SetBlend( 0.5 );
			} else if( gCvars.ESP_hand.value == 3 ) {
				Materials::ForceMaterial( wanted_material, team_color );
			}
		}
	}

	switch( (classId)pEntity->GetClassId() ) {
		case classId::CTFWearable:
		{
			if( gCvars.ESP_hat.value ) {
				gHooks.DrawModelExecute.rehook();
				return;
			}
			break;
		}

		case classId::CTFPlayer:
		{
			if( !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE && (!gCvars.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum()) ) {
				//backtrack
				if( gCvars.ESP_backtrack.value && gCvars.Backtrack.value ) {
					if( gCvars.aim_index == pInfo.entity_index ) {
						int ticks = 0;
						for( int tick = 0; tick < (int)BacktrackData[gCvars.aim_index].size() && ticks < 12; tick++ ) {
							if( Backtrack::is_tick_valid( BacktrackData[gCvars.aim_index][tick].simtime ) ) {
								ticks++;
								if( BacktrackData[gCvars.aim_index][tick].valid && BacktrackData[gCvars.aim_index][tick].velocity.Length() > 45.0f ) {
									if( gCvars.ESP_player.value == 1 ) {
										Color tick_color = tick == gCvars.backtrack_arr ? gCvars.color_cham_tick.get_color() : gCvars.color_cham_history.get_color();
										//Hidden
										wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
										Materials::ForceMaterial( wanted_material, tick_color );
										gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][tick].boneMatrix );
										//Visible
										wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
										Materials::ForceMaterial( wanted_material, tick_color );
										gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][tick].boneMatrix );
										Materials::ResetMaterial();
									} else {
										gInts.MdlRender->DrawModelExecute( state, pInfo, BacktrackData[gCvars.aim_index][tick].boneMatrix );
									}
								}
							}
						}
					}
				}
				if( gCvars.ESP_player.value == 1 ) {
					//player
					//Hidden
					wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
					Materials::ForceMaterial( wanted_material, team_color );
					gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
					//Visible
					wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
					Materials::ForceMaterial( wanted_material, team_color );
					gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
					Materials::ResetMaterial();
				} else {
					gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				}
			}
			break;
		}

		case classId::CTFTankBoss:
		case classId::CObjectDispenser:
		case classId::CObjectSentrygun:
		case classId::CObjectTeleporter:
		case classId::CCaptureFlag:
		{
			if( gCvars.ESP_building.value == 1 ) {
				if( !strstr( model_name, "blueprint" ) ) {
					if( !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE ) {
						//Hidden
						wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
						Materials::ForceMaterial( wanted_material, team_color );
						gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
						//Visible
						wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
						Materials::ForceMaterial( wanted_material, team_color );
						gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
						Materials::ResetMaterial();
					}
				}
			} else {
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		case classId::CTFProjectile_SentryRocket:
		case classId::CTFStickBomb:
		case classId::CTFGrenadePipebombProjectile:
		case classId::CTFProjectile_Rocket:
		{
			if( gCvars.ESP_proj_cham.value ) {
				Color RGBA = gCvars.color_objects.get_color();
				//Hidden
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
				Materials::ForceMaterial( wanted_material, RGBA );
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				//Visible
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
				Materials::ForceMaterial( wanted_material, RGBA );
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				Materials::ResetMaterial();
			} else {
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		case classId::CBaseAnimating:
		case classId::CTFAmmoPack:
		{
			if( gCvars.ESP_object.value == 1 ) {
				Color RGBA = gCvars.color_objects.get_color();
				//Hidden
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
				Materials::ForceMaterial( wanted_material, RGBA );
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				//Visible
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
				Materials::ForceMaterial( wanted_material, RGBA );
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				Materials::ResetMaterial();
			} else {
				gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		default:
		{
			gInts.MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			break;
		}
	}

	gHooks.DrawModelExecute.rehook();
}