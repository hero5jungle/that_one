#include "../../sdk/sdk.h"
#include "client.h"
#include "../../hack/hack.h"
#include "../../tools/util/util.h"
#include "../../sdk/cmat/cmat.h"

int __fastcall hkSendDatagram( CNetChan* netchan, PVOID, bf_write* datagram ) {
	auto sendDatagram = Hook::SendDatagram.get_original();

	if( !Global.latency.value || datagram ) {
		return sendDatagram( netchan, datagram );
	}

	int instate = netchan->m_nInReliableState;
	int insequencenr = netchan->m_nInSequenceNr;
	Latency::AddLatencyToNetchan( netchan, Global.latency_amount.value / 1000.0f );
	int Return = sendDatagram( netchan, datagram );
	netchan->m_nInReliableState = instate;
	netchan->m_nInSequenceNr = insequencenr;
	return Return;
}

Vector qLASTTICK;

bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* cmd ) {
	bool bReturn = Hook::CreateMove.get_original()( ClientMode, input_sample_frametime, cmd );
	uintptr_t _bp;
	__asm mov _bp, ebp;
	bool* send_packet = (bool*)( ***(uintptr_t * **)_bp - 1 );
	Latency::UpdateIncomingSequences();

	if( !cmd->command_number ) {
		return false;
	} else {
		Global.last_cmd_number = cmd->command_number;
	}

	CNetChan* ch = Int::Engine->GetNetChannelInfo();

	if( ch ) {
		if( ch != old_ch ) {
			Hook::SendDatagram.setup( ch, gOffsets::SendDatagram, &hkSendDatagram );
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
		Aimbot::Run( pLocal, cmd, send_packet );
		Fake::Run( pLocal, cmd, send_packet );
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
	CBaseEntity* pLocal = GetBaseEntity( me );

	if( Int::Engine->IsInGame() && pLocal ) {

		if( Global.ESP_building.value == 2 || Global.ESP_player.value == 2 || Global.ESP_object.value == 2 ) {
			if( Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
				int max = Int::EntityList->GetHighestEntityIndex();
				for( int n = 1; n <= max; n++ ) {
					CBaseEntity* pEntity = GetBaseEntity( n );

					if( pEntity == nullptr || pEntity == pLocal )
						continue;

					bool valid = !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE;
					bool team = !Global.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum();
					bool model = pEntity->GetModel();

					switch( (classId)pEntity->GetClassId() ) {
						case classId::CTFPlayer:
						{
							if( Global.ESP_player.value == 2 ) {
								//if( !pEntity->HasGlowEffect() ) {
								//	pEntity->registerGlowObject( Util::team_color( pLocal, pEntity ), true, true );
								//}
								pEntity->SetGlowEnabled( valid && team );
							}

							break;
						}

						case classId::CObjectSentrygun:
						case classId::CObjectDispenser:
						case classId::CObjectTeleporter:
						{
							if( Global.ESP_building.value == 2 ) {
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
							if( Global.ESP_object.value == 2 ) {
								if( !pEntity->HasGlowEffect() ) {
									pEntity->registerGlowObject( Global.color_objects.color, true, true );
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
				for( int i = 0; i < Int::GlowManager->m_GlowObjectDefinitions.Count(); i++ ) {
					GlowObjectDefinition_t& GlowObject = Int::GlowManager->m_GlowObjectDefinitions[i];

					if( GlowObject.m_nNextFreeSlot != ENTRY_IN_USE )
						continue;

					CBaseEntity* pEntity = Int::EntityList->GetClientEntityFromHandle( GlowObject.m_hEntity );
					if( pEntity ) {
						Color color;
						switch( (classId)pEntity->GetClassId() ) {
							case classId::CTFPlayer:
							{
								if( Global.ESP_player.value == 2 ) {
									color = Util::team_color( pLocal, pEntity );
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									Int::GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}

							case classId::CObjectSentrygun:
							case classId::CObjectDispenser:
							case classId::CObjectTeleporter:
							{
								if( Global.ESP_building.value == 2 ) {
									color = Util::team_color( pLocal, pEntity );
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									Int::GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}
							case classId::CBaseAnimating:
							case classId::CTFAmmoPack:
							{
								if( Global.ESP_object.value == 2 ) {
									color = Global.color_objects.color;
									GlowObject.m_vGlowColor = color.rgb();
									GlowObject.m_flGlowAlpha = color[3] / 255.0f;
								} else {
									Int::GlowManager->m_GlowObjectDefinitions.Remove( i );
								}
								break;
							}
							default:
								break;
						}
					}
				}
			}
		} else {
			if( Int::GlowManager->m_GlowObjectDefinitions.Count() ) {
				Int::GlowManager->m_GlowObjectDefinitions.RemoveAll();
			}
		}
		if( Global.sniper_nozoom.value ) {
			pLocal->SetFov( pLocal->GetDefaultFov() );
			pLocal->set( 0xE5C, 0.0f );//m_flFOVRate
		}

		if( Global.NoRecoil.value ) {
			pLocal->set( 0xE8C, Vector() );
		}

		static bool Thirdperson_enabled = false;

		if( Global.Thirdperson.KeyDown() ) {
			pLocal->SetThirdpersonView( qLASTTICK );

			if( pLocal->GetLifeState() == LIFE_ALIVE ) {
				pLocal->SetThirdperson( true );
				Thirdperson_enabled = true;
				if( Global.Thirdperson_scoped.value && pLocal->GetCond() & TFCond_Zoomed ) {
					pLocal->RemoveNoDraw();
					if( auto wpn = pLocal->GetActiveWeapon() ) {
						wpn->RemoveNoDraw();
					}
				}
			}
		} else if( !Thirdperson_enabled || !Global.Thirdperson.KeyDown() ) {
			pLocal->SetThirdperson( false );
			Thirdperson_enabled = false;
		}

	}

	static std::unordered_map<MaterialHandle_t, Color> worldmats_new, worldmats_old;

	if( ( !Int::Engine->IsInGame() || !Global.world_enabled.value ) && worldmats_old.size() ) {
		if( Int::Engine->IsInGame() ) {
			for( auto& hMat : worldmats_old ) { // Reset the material colors
				IMaterial* mat = Int::MatSystem->GetMaterial( hMat.first );

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

	if( Global.world_enabled.value && Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
		Latency::UpdateIncomingSequences();

		for( MaterialHandle_t i = Int::MatSystem->FirstMaterial(); i != Int::MatSystem->InvalidMaterial(); i = Int::MatSystem->NextMaterial( i ) ) {
			IMaterial* mat = Int::MatSystem->GetMaterial( i );

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
				color = Global.sky_clr.bDef ? Color( 255 ) : Global.sky_clr.get_color();
			} else {
				color = Global.world_clr.bDef ? Color( 255 ) : Global.world_clr.get_color();
			}

			if( worldmats_new.at( i ) != color ) {
				float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
				mat->ColorModulate( blend[0], blend[1], blend[2] );
				mat->AlphaModulate( blend[3] );
				worldmats_new.at( i ) = color;
			}
		}
	}

	if( !Int::Engine->IsInGame() ) {
		Latency::ClearIncomingSequences();
	}

	return Hook::FrameStageNotifyThink.get_original()( CHLClient, _this, Stage );
}

void __stdcall Hooked_DrawModelExecute( void* state, ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld ) {
	Hook::DrawModelExecute.unhook();

	CBaseEntity* pEntity = GetBaseEntity( pInfo.entity_index );
	CBaseEntity* pLocal = GetBaseEntity( me );

	if( !pEntity || !pLocal ) {
		Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
		Hook::DrawModelExecute.rehook();
		return;
	}

	const char* model_name = Int::ModelInfo->GetModelName( pInfo.pModel );
	Color team_color = Util::team_color( pLocal, pEntity );
	IMaterial* wanted_material = Global.ESP_cham_mat.value ? Materials::glow : Materials::shaded;

	if( Global.ESP_hand.value ) {
		if( strstr( model_name, "arms" ) ) {
			if( Global.ESP_hand.value == 1 ) {
				Int::RenderView->SetBlend( 0 );
			} else if( Global.ESP_hand.value == 2 ) {
				Int::RenderView->SetBlend( 0.5 );
			} else if( Global.ESP_hand.value == 3 ) {
				Materials::ForceMaterial( wanted_material, team_color );
			}
		}
	}

	switch( (classId)pEntity->GetClassId() ) {
		case classId::CTFWearable:
		{
			if( Global.ESP_hat.value ) {
				Hook::DrawModelExecute.rehook();
				return;
			}
			break;
		}

		case classId::CTFPlayer:
		{
			if( !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE && ( !Global.ESP_enemy.value || pEntity->GetTeamNum() != pLocal->GetTeamNum() ) ) {
				//backtrack
				if( Global.ESP_backtrack.value && Global.Backtrack.value ) {
					if( Global.aim_index == pInfo.entity_index ) {
						int ticks = 0;
						for( int tick = 0; tick < (int)BacktrackData[Global.aim_index].size() && ticks < 12; tick++ ) {
							if( Backtrack::is_tick_valid( BacktrackData[Global.aim_index][tick].simtime ) ) {
								ticks++;
								if( BacktrackData[Global.aim_index][tick].valid && BacktrackData[Global.aim_index][tick].velocity.Length() > 45.0f ) {
									if( Global.ESP_player.value == 1 ) {
										Color tick_color = tick == Global.backtrack_arr ? Global.color_cham_tick.get_color() : Global.color_cham_history.get_color();
										//Hidden
										wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
										Materials::ForceMaterial( wanted_material, tick_color );
										Int::MdlRender->DrawModelExecute( state, pInfo, BacktrackData[Global.aim_index][tick].boneMatrix );
										//Visible
										wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
										Materials::ForceMaterial( wanted_material, tick_color );
										Int::MdlRender->DrawModelExecute( state, pInfo, BacktrackData[Global.aim_index][tick].boneMatrix );
										Materials::ResetMaterial();
									} else {
										Int::MdlRender->DrawModelExecute( state, pInfo, BacktrackData[Global.aim_index][tick].boneMatrix );
									}
								}
							}
						}
					}
				}
				if( Global.ESP_player.value == 1 ) {
					//player
					//Hidden
					wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
					Materials::ForceMaterial( wanted_material, team_color );
					Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
					//Visible
					wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
					Materials::ForceMaterial( wanted_material, team_color );
					Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
					Materials::ResetMaterial();
				} else {
					Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
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
			if( Global.ESP_building.value == 1 ) {
				if( !strstr( model_name, "blueprint" ) ) {
					if( !pEntity->IsDormant() && pEntity->GetLifeState() == LIFE_ALIVE ) {
						//Hidden
						wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
						Materials::ForceMaterial( wanted_material, team_color );
						Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
						//Visible
						wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
						Materials::ForceMaterial( wanted_material, team_color );
						Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
						Materials::ResetMaterial();
					}
				}
			} else {
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		case classId::CTFProjectile_SentryRocket:
		case classId::CTFStickBomb:
		case classId::CTFGrenadePipebombProjectile:
		case classId::CTFProjectile_Rocket:
		{
			if( Global.ESP_proj_cham.value ) {
				Color RGBA = Global.color_objects.get_color();
				//Hidden
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
				Materials::ForceMaterial( wanted_material, RGBA );
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				//Visible
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
				Materials::ForceMaterial( wanted_material, RGBA );
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				Materials::ResetMaterial();
			} else {
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		case classId::CBaseAnimating:
		case classId::CTFAmmoPack:
		{
			if( Global.ESP_object.value == 1 && model_name[24] != 'h' ) {//skip "models/effects/cappoint_hologram.mdl"

				Color RGBA = Global.color_objects.get_color();
				//Hidden
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
				Materials::ForceMaterial( wanted_material, RGBA );
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				//Visible
				wanted_material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
				Materials::ForceMaterial( wanted_material, RGBA );
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
				Materials::ResetMaterial();
			} else {
				Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			}
			break;
		}

		default:
		{
			Int::MdlRender->DrawModelExecute( state, pInfo, pCustomBoneToWorld );
			break;
		}
	}

	Hook::DrawModelExecute.rehook();
}