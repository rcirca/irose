#include "stdafx.h"
#include ".\gamestatemoveplanet.h"
#include "CCamera.h"
#include "IO_Terrain.h"
#include "CGame.h"
#include "CSkyDome.h"
#include "SystemProcScript.h"
#include "System_FUNC.h"
#include "../GameProc/LiveCheck.h"
#include "../CClientStorage.h"

#include "SFX/SFXManager.h"


const int CUTSCENE_ZONE_NO = 19;
int CGameStateMovePlanet::m_iTargetPlanet = 1;


CGameStateMovePlanet::CGameStateMovePlanet(void)
{
}

CGameStateMovePlanet::CGameStateMovePlanet( int iStateID )
{
	m_iStateID = iStateID;
}

CGameStateMovePlanet::~CGameStateMovePlanet(void)
{
}

int CGameStateMovePlanet::Update( bool bLostFocus )
{
	

	g_pCamera->Update ();
	///D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	///g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );
	g_pTerrain->SetCenterPosition( 520000.0f, 520000.0f );


	g_pEffectLIST->Proc ();


	/// SFX
	CSFXManager::GetSingleton().Update();


	g_DayNNightProc.Proc();

	CSystemProcScript::GetSingleton().CallLuaFunction( "ProcMovePlanetCutScene", ZZ_PARAM_INT, g_DayNNightProc.GetZoneTime(), ZZ_PARAM_END );
	CLiveCheck::GetSingleton().Check();

	::updateScene ();

	// processing  ...
	if ( !bLostFocus ) 
	{
		if ( ::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			::setClearColor( 0.0f, 0.0f, 0.0f );
			::clearScreen();
			::renderScene();
			
			
			/// 스프라이트 출력..

			::beginSprite( D3DXSPRITE_ALPHABLEND );

			/// 현재 sFX 들은 대부분 스프라이트 출력으로 구성되어 있다.
			/// 그러니 요 사이에..
			/// SFX
			CSFXManager::GetSingleton().Draw();			
			
			::endSprite();	



			/// for test
			///Render_GameMENU();

			::endScene ();
			::swapBuffers();
		}
	}

	///::updateSceneTransform ();	// 이동 애니메이션 처리...	

	return 0;
}

int CGameStateMovePlanet::Enter( int iPrevStateID )
{
	::SetOceanSFXOnOff( false );

	/// 현재 존데이터 풀고 새로운 존 데이터 로드
	g_pTerrain->FreeZONE();

	g_GameDATA.m_bJustObjectLoadMode = true;

	/// 새로운 컷씬 존 로드
	g_pTerrain->LoadZONE( CUTSCENE_ZONE_NO );
	g_pTerrain->InitZONE( 0, 0 );


	HNODE hCameraMotion = ::findNode( "MovePlanet_Camera01" );
	if ( hCameraMotion == 0 )
		hCameraMotion = ::loadMotion( "MovePlanet_Camera01", "3DData\\CUTSCENE\\WARP01\\Camera01.ZMO",1, 4, 3, 1, 0 );
	
	HNODE motion_camera = ::findNode( "MovePlanet_camera" );
	if( motion_camera == 0 )
		motion_camera = ::loadCamera( "MovePlanet_camera", "cameras\\camera01.zca", hCameraMotion );

	
		
	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, ZONE_BG_IMAGE( g_pTerrain->GetZoneNO() ) );

	g_pCamera->Init( motion_camera );

	CSystemProcScript::GetSingleton().CallLuaFunction( "InitPlanetMoveWorld", ZZ_PARAM_INT, 1, ZZ_PARAM_END );
	
	///
	/// 카메라 모션은 32_32 기준으로 만들어졌다.. 모션적용을 위해서 보정한다.
	///
	D3DVECTOR PosENZIN;
	PosENZIN.x = 520000.0f;
	PosENZIN.y = 520000.0f;
	PosENZIN.z = 0.0f;

	g_pCamera->Set_Position ( PosENZIN );

	/// 포그를 멀리..
	::setFogRange( 70000, 80000 );	
	setAlphaFogRange( 70000, 80000 );

	///setCameraPerspective( motion_camera, CAMERA_FOV(0), atof(CAMERA_ASPECT_RATIO(0)), CAMERA_NEAR_PLANE(0) * 100, CAMERA_FAR_PLANE(0) * 10000);


	g_DayNNightProc.SetWorldTime( g_pTerrain->GetZoneNO(), 0 );	
	g_DayNNightProc.SetDayEnviTick( 100 );
	
	D3DXVECTOR3 posAVATAR;
	posAVATAR.x = 0;
	posAVATAR.y = 0;
	posAVATAR.z = 0.0f;
	g_pAVATAR->ResetCUR_POS( posAVATAR );


	///SFX
	CSFXManager::GetSingleton().StartWideScreenEffect();
	
	return 0;
}

/// 별로 할건없고.. 일반워프처럼 Warp 을 호출해 버린다.
int CGameStateMovePlanet::Leave( int iNextStateID )
{

	g_pCamera->ResetToAvatarCamera();
    
	g_GameDATA.m_bJustObjectLoadMode = false;

	g_DayNNightProc.ResetDayEnviTick();

	CSystemProcScript::GetSingleton().CallLuaFunction( "ClearMovePlanetCutScene", ZZ_PARAM_END );
	StopScreenFadeInOut();

	::SetOceanSFXOnOff( true );


	///<--------------------------------------------------
	///사용자가 저장한 시야옵션을 재설정해준다.
	t_OptionVideo option;
	g_ClientStorage.GetVideoOption(option);
	g_ClientStorage.ApplyCameraOption( option.iCamera );


	///SFX
	CSFXManager::GetSingleton().StopWideScreenEffect();

	return 0;
}


int CGameStateMovePlanet::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	POINT ptMouse = { LOWORD( lParam ), HIWORD( lParam ) };
	switch( uiMsg )
	{
		case WM_MOUSEMOVE:
			{
	#ifdef _DEBUG
				if( g_GameDATA.m_bObserverCameraMode )
				{
					if ( ( wParam & MK_RBUTTON ) ) 
					{
						ObserverCameraTransform( (short)(ptMouse.x - m_PosRButtonClick.m_nX), (short)(ptMouse.y - m_PosRButtonClick.m_nY) );
						/*if ( ptMouse.x - m_PosRButtonClick.m_nX ) 
							g_pCamera->Add_YAW( (short)(ptMouse.x - m_PosRButtonClick.m_nX) );
						if ( ptMouse.y - m_PosRButtonClick.m_nY ) 
							g_pCamera->Add_PITCH( (short)(ptMouse.y - m_PosRButtonClick.m_nY) );*/

						m_PosRButtonClick.m_nX = (short)ptMouse.x;
						m_PosRButtonClick.m_nY = (short)ptMouse.y; 					
					}
				}else
	#endif
				if ( ( wParam & MK_RBUTTON ) ) 
				{
					if ( ptMouse.x - m_PosRButtonClick.m_nX ) 
						g_pCamera->Add_YAW( (short)(ptMouse.x - m_PosRButtonClick.m_nX) );
					if ( ptMouse.y - m_PosRButtonClick.m_nY ) 
						g_pCamera->Add_PITCH( (short)(ptMouse.y - m_PosRButtonClick.m_nY) );

					m_PosRButtonClick.m_nX = (short)ptMouse.x;
					m_PosRButtonClick.m_nY = (short)ptMouse.y; 
				}				
				break;
			}
		case WM_RBUTTONDOWN:
			m_PosRButtonClick.m_nX = (short)ptMouse.x;
			m_PosRButtonClick.m_nY = (short)ptMouse.y; 
			break;

		case WM_MOUSEWHEEL:
			On_WM_MOUSEWHEEL( wParam, lParam);
			break;
		default:
			break;
	}
	return 0;
}

int CGameStateMovePlanet::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
		case WM_SYSKEYDOWN:
			{
				unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
				UINT vk = MapVirtualKey( oemScan, 1 );

				switch( vk )
				{
					///'0'
					case 0x30:
#ifdef _DEBUG
						g_GameDATA.m_bObserverCameraMode = !g_GameDATA.m_bObserverCameraMode;
						SetObserverCameraOnOff();
#endif
						break;
				}
			}
			break;

		case WM_KEYDOWN:
			{
				switch( wParam )
				{
					case VK_ESCAPE:
						{
							CGame::GetInstance().ChangeState( CGame::GS_WARP );
						}
						break;
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CGameStateMovePlanet::On_WM_MOUSEWHEEL (WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

#ifdef _DEBUG
	if( g_GameDATA.m_bObserverCameraMode )
	{
		ObserverCameraZoomInOut( (float)-(zDelta) );
	}
#endif
	
	return true;
}

void CGameStateMovePlanet::Render_GameMENU()	
{	
#ifdef _DEBUG
	{
			::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], 
						false, 5, 120, g_dwYELLOW, "bCastingStart[ %d ], DoingSkill[ %d ], Command[ %d ]",					
						g_pAVATAR->m_bCastingSTART, g_pAVATAR->m_nDoingSkillIDX, g_pAVATAR->Get_COMMAND() );

			//----------------------------------------------------------------------------------------------------
			/// 임시 소환몹 개수
			//----------------------------------------------------------------------------------------------------
			if( g_pAVATAR->GetCur_SummonCNT() > 0 )
			{
				::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], 
								false, 5, 150, g_dwYELLOW, "[ 현재 소환된 몹 수[ %d ] , 현재 사용한 소환량[%d] ]/ 현재 소환가능한 소환량[%d]",					
								g_pAVATAR->GetCur_SummonCNT(), 
								g_pAVATAR->GetCur_SummonUsedCapacity(),
								g_pAVATAR->GetCur_SummonMaxCapacity() );
			}

			


			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 25, g_dwBLUE, "%d FPS, Patch[ %d, %d ], Pos[ %f, %f, %f ]",
					g_pCApp->DisplayFrameRate (),
					g_GameDATA.m_PosPATCH.x, g_GameDATA.m_PosPATCH.y,
					g_pAVATAR->Get_CurPOS().x,	 g_pAVATAR->Get_CurPOS().y, g_pAVATAR->Get_CurPOS().z );

			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 40, g_dwRED, "TotOBJ: %d, CnstOBJ: %d, Item : %d, TreeOBJ: %d, MobCNT: %d, EffectCNT: %d, PatchCNT: %d ", 
					g_pObjMGR->Get_ObjectCount(),
					g_pObjMGR->Get_ObjectCount( OBJ_CNST ), 
					g_pObjMGR->Get_ObjectCount( OBJ_ITEM ), 
					g_pObjMGR->Get_ObjectCount( OBJ_GROUND ),
					g_pObjMGR->Get_ObjectCount( OBJ_MOB ),
					g_pEffectLIST->GetCount(),
					CTERRAIN::m_RegistedPatchCnt );			

			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 60, g_dwRED, "( 공속: %d, 이속: %f 기본속도: %f )",
					( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_nAttackSPEED() : g_pAVATAR->m_pObjCART->Get_nAttackSPEED(), 
					( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_MoveSPEED() : g_pAVATAR->m_pObjCART->Get_MoveSPEED(), 
					( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_DefaultSPEED() : g_pAVATAR->m_pObjCART->Get_DefaultSPEED() );

			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 80, g_dwRED, "( 월드타입: %d, 존타임:%d, 블랜딩비율:%f )",
															g_DayNNightProc.GetWorldTime(), g_DayNNightProc.GetZoneTime(), g_DayNNightProc.GetBlendFactor() );

			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 100, g_dwRED, "( WorldRATE: %d, WorldPROD:%d )",
															Get_WorldRATE(), Get_WorldPROD () );

	}	
#endif
}