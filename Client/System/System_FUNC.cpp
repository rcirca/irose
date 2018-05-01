#include "stdafx.h"
#include "System_FUNC.h"
#include "IO_Basic.h"
#include "../CJustModelAVT.h"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/ExternalUI/CCreateAVata.h"
#include "../Interface/ExternalUI/CExternalUI.h"
#include "../Interface/it_MGR.h"
#include "../Interface/dlgs/notifydlg.h"
#include "../Interface/dlgs/cminimapdlg.h"
#include "../ObjFixedEvent.h"
#include "../Object.h"
#include "SystemProcScript.h"
#include "CGame.h"
#include "IO_Event.h"
#include "TButton.h"
#include "../Tutorial/TutorialEventUtility.h"
#include "../Tutorial/TutorialEventManager.h"
#include "../Event/Quest_FUNC.h"
#include "../Game_Func.h"


void SC_InitGame()
{
}


///
/// 새로운스크립트 실행
///
int SC_DoScript ( ZSTRING strName )
{
	return CSystemProcScript::GetSingleton().DoScript( strName );
}



///
/// 캐릭터 생성관련
///

HNODE SC_CreateAVT( ZSTRING strName, int iRace, int iFACE, int iHAIR, int iHELMET, 
				   int iARMOR, int iGAUNTLET, int iBOOTS, 
				   int iFACE_ITEM, int iKNAPSACK, int iWEAPON_R, int iWEAPON_L )
{
	CJustModelAVT* pAvt = new CJustModelAVT();

	pAvt->Init( strName, iFACE, iHAIR, iHELMET, iARMOR, iGAUNTLET, iBOOTS, iFACE_ITEM, iKNAPSACK, iWEAPON_R, iWEAPON_L );
	pAvt->SetRace( iRace );
	pAvt->UpdateModel();
	pAvt->LoadModelVisible();
	pAvt->InsertToScene();

	return reinterpret_cast<HNODE>(pAvt);
}

/// 아바타 삭제
void SC_DestroyAVT ( HNODE hAVT )
{
	CJustModelAVT* pAvt = reinterpret_cast<CJustModelAVT*>(hAVT);

	if( pAvt != NULL )
	{
		pAvt->RemoveFromScene();
		pAvt->UnloadModelVisible();

		delete pAvt;
		pAvt = NULL;
	}
}

/// 모션 세팅
void SC_SetAvatarMotionByName ( ZSTRING strName, ZSTRING strMotion )
{
	CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( strName );
	if( pAvt )
		pAvt->SetMotion( strMotion );
}

void SC_SetAvatarMotionByIndex ( ZSTRING strName, int iMotionIDX )
{
	CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( strName );
	if( pAvt )
		pAvt->SetMotion( iMotionIDX );
}

void SC_SetVisible ( HNODE hAVT, int iIsVisible )
{
	CJustModelAVT* pAvt = reinterpret_cast<CJustModelAVT*>(hAVT);

	if( pAvt != NULL )
		pAvt->SetVisible( iIsVisible );
}



///
/// adjust transform
///

void SC_SetPosition ( HNODE hNODE, float x, float y, float z )
{
	::setPosition( hNODE, x, y, z );
}








HNODE SC_LoadModel( ZSTRING pModelName,
					HNODE hSkeleton,
					HNODE hMotion,
					float ScaleInLoad)
{
	HNODE hNodeMODEL = ::loadModel ( pModelName, hSkeleton, hMotion, 1.0f );

	if( hNodeMODEL == NULL )
		Log_String( LOG_NORMAL, "SC_LoadModel failed(in system sript)[ %s ]\n", pModelName );

	return hNodeMODEL;
}

HNODE SC_LoadSkeleton ( ZSTRING pSkeletonName, ZSTRING pSkeletonPath )
{
	HNODE hSkeleton = ::loadSkeleton( pSkeletonName, pSkeletonPath );

	if( hSkeleton == NULL )
		Log_String( LOG_NORMAL, "SC_LoadSkeleton failed(in system sript)[ %s ]\n", pSkeletonName );

	return hSkeleton;
}

HNODE SC_GetSkeleton ( int iIsfeMale )
{
	HNODE hSkeleton = g_DATA.Get_SKELETON( iIsfeMale );

	if( hSkeleton == NULL )
		Log_String( LOG_NORMAL, "SC_GetSkeleton failed(in system sript)[ %d ]\n", iIsfeMale );

	return hSkeleton;
}

HNODE SC_FindNode ( ZSTRING pNodeName )
{
	HNODE hNode = ::findNode( pNodeName );

	if( hNode == NULL )
		Log_String( LOG_NORMAL, "SC_FindNode failed(in system sript)[ %s ]\n", pNodeName );

	return hNode;
}

HNODE SC_LoadMotion ( ZSTRING pMotionName, ZSTRING pMotionFileName, int bUseLoop, int InterpTypePosition, int InterpTypeRotation, float Scale, int bForModel )
{
	HNODE hNode = ::loadMotion( pMotionName, 
								pMotionFileName, 
								bUseLoop, 
								InterpTypePosition, 
								InterpTypeRotation, 
								Scale, 
								bForModel );

	if( hNode == NULL )
		Log_String( LOG_NORMAL, "SC_LoadMotion failed(in system sript)[ %s ]\n", pMotionName );

	Log_String( LOG_NORMAL, "SC_LoadMotion (in system sript)[ %s ]\n", pMotionName );

	return hNode;
}


void SC_AttachMotion ( HNODE hNode, HNODE hMotion )
{
	if( hNode == 0 )
	{
		Log_String( LOG_NORMAL, "SC_AttachMotion failed(in system sript)\n" );
		return;
	}

	::attachMotion( hNode, hMotion );
}

//------------------------------------------------------------------------------------------
/// 모션 조작 관련
//------------------------------------------------------------------------------------------
void SC_SetMotionFrame ( HNODE hNode, int iFrame )
{
	::setAnimatableFrame ( hNode, iFrame );
}

int SC_GetTotalFrame ( HNODE hNode )
{
	return ::getMotionTotalFrame ( hNode );
}

void SC_ControlAnimation ( HNODE hNode, int iControl )
{
	::controlAnimatable( hNode, iControl );
}

HNODE SC_LoadCamera ( ZSTRING pCameraName, ZSTRING pCameraPath, HNODE hCameraMotion )
{
	HNODE hNode = ::loadCamera( pCameraName, 
								pCameraPath, 
								hCameraMotion );

	if( hNode == NULL )
		Log_String( LOG_NORMAL, "SC_LoadCamera failed(in system sript)[ %s ]", pCameraPath );

	::controlAnimatable( hNode, 0 );
	::setCameraAspectRatio( hNode, 1.3333 );
	::setCameraPerspective( hNode, 50, 1.333, 100, 30000 );

	return hNode;
}

HNODE SC_SetCameraDefault ( HNODE hCamera )
{
	return ::setCameraDefault( hCamera );
}

void SC_UnloadNode( HNODE hNode )
{
	if( hNode == 0 )
	{
		Log_String( LOG_NORMAL, "SC_UnloadNode failed(in system sript)" );
		return;
	}
	unloadNode( hNode );
}

int SC_SetRepeatCount ( HNODE hAnimatable, int iRepeatCount )
{
	if( hAnimatable == 0 )
	{
		Log_String( LOG_NORMAL, "SC_SetRepeatCount failed(in system sript)" );
		return 0;
	}
	return setRepeatCount ( hAnimatable, iRepeatCount );
}
HNODE SC_GetMotion ( HNODE hAnimatable )
{
	if( hAnimatable == 0 )
	{
		Log_String( LOG_NORMAL, "SC_GetMotion failed(in system sript)" );
		return 0;
	}
	return getMotion( hAnimatable );
}

void SC_SetModelPosition ( HNODE hModel, float x, float y, float z )
{
	if( hModel == 0 )
		Log_String( LOG_NORMAL, "SC_SetModelPosition failed(in system sript)");

	::setPosition( hModel, x, y, z );	
}

void SC_SetModelDirection ( HNODE hModel, float fAngleDegree )
{
	if( hModel == 0 )
	{
		Log_String( LOG_NORMAL, "SC_SetModelDirection failed(in system sript)" );
		return;
	}
	int iRet = setModelDirection( hModel, fAngleDegree, false );
}

void SC_SetScale ( HNODE hVisible, float x, float y, float z )
{
	if( hVisible == 0 )
	{
		Log_String( LOG_NORMAL, "SC_SetScale failed(in system sript)" );
		return;
	}
	setScale( hVisible, x, y, z );

}

///
/// 존 관리
///

int SC_GetBGZoneNO ( void )
{
	return 4;
}

HNODE SC_AddCreateAVT ( ZSTRING pszName, int iRace, int iFace, int iHair, int iHelmet,
					 int iArmor, int iGauntlet, int iBoots,
					 int iFaceItem, int iKnapsack, int iRWeapon, int iLWeapon )
{
	CJustModelAVT* pAvt = new CJustModelAVT();
	pAvt->Init( pszName, iFace, iHair, iHelmet, iArmor, iGauntlet, iBoots, iFaceItem, iKnapsack, iRWeapon, iLWeapon );
	pAvt->SetRace( iRace );
	pAvt->UpdateModel();
	pAvt->LoadModelVisible();
	pAvt->InsertToScene();
	
	CGameDataCreateAvatar::GetInstance().AddCreateAvatar( pAvt );

	return pAvt->GetModelNode();
}


void SC_RemoveCreateAVT ( ZSTRING pszName )
{
	CGameDataCreateAvatar::GetInstance().RemoveCreateAvatar( pszName );
}

void SC_AddSelectItem4CreateAvatar ( int iType, ZSTRING pszItem, ZSTRING pszValue )
{
	CCreateAvata* pDialog = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	pDialog->AddSelectItem( iType, pszItem, pszValue );
}

void SC_ClearSelectItem4CreateAvatar ( int iType )
{
	CCreateAvata* pDialog = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	pDialog->ClearSelectItem( iType );
}

void SC_SelectItem4CreateAvatar ( int iType, int iSubscript )
{
	CCreateAvata* pDialog = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	pDialog->SelectItem( iType, iSubscript );
}

void SC_SetAvatarFace ( ZSTRING pszName, int iFace )
{
	CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszName );
	if( pAvt )
	{
		pAvt->SetPart(  BODY_PART_FACE , iFace );
		pAvt->UpdateModel();
	}
}

void SC_SetAvatarHair ( ZSTRING pszName, int iHair )
{
	CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszName );
	if( pAvt )
	{
		pAvt->SetPart( BODY_PART_HAIR, iHair );
		pAvt->UpdateModel();
	}
}



///
/// 이벤트 오브젝트 관련..
/// type 0 : 포지션 노드
/// type 1 : 애니매이션 노드
///
HNODE SC_GetEventObject ( int iEventID, int iType )
{
	CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( iEventID );	
	if( pObj == NULL )
		return 0;

	return ( iType == 0 )? pObj->GetPositionNode() : pObj->GetAnimatibleNode();	
}

int SC_GetEventObjectIndex ( int iEventID )
{
	CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( iEventID );	
	if( pObj == NULL )
		return 0;

	return pObj->Get_INDEX();	
}



///
/// Con 파일 실행
/// NPC 대화창..
///

void SC_RunEvent ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX )
{
	if( g_itMGR.IsDlgOpened( DLG_TYPE_DIALOG ) )
		return;

	if( g_pEventLIST->Run_EVENT( iOwnerIndex, (char*)szQuestFILE, iEventIDX, EVENT_DLG_NPC ) == false )
	{
		;
	}
}

///
/// Con 파일 실행
/// 필드 워프창
///

void SC_RunEventFieldWarp ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX )
{
	if( g_itMGR.IsDlgOpened( DLG_TYPE_SELECTEVENT ) )
		return;

	if( g_pEventLIST->Run_EVENT( iOwnerIndex, (char*)szQuestFILE, iEventIDX, EVENT_DLG_WARP ) == false )
	{
		;
	}
}

///
/// Con 파일 실행
/// 이벤트 대화창 생성
///

void SC_RunEventObjectEvent ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX )
{

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EVENTDIALOG ) )
		return;

	if( g_pEventLIST->Run_EVENT( iOwnerIndex, (char*)szQuestFILE, iEventIDX, EVENT_DLG_EVENT_OBJECT ) == false )
	{
		;
	}
}

///
/// 알림 메세지박스
///
void SC_ShowNotifyMessage ( int iStringIDX )
{
	const char* pMsg = CStringManager::GetSingleton().GetNotifyMessageString( iStringIDX );
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_NOTIFY );
	assert( pDlg );
	if( pDlg )
	{
		CNotifyDlg* p = (CNotifyDlg*)pDlg;
		p->SetScript( pMsg );
		g_itMGR.OpenDialog( DLG_TYPE_NOTIFY , false);
	}
	
	//SC_OpenMsgbox( pMsg );
}

///
/// Log/OutputDebugString
///
void SC_LogString ( ZSTRING szMessage )
{
	LogString( LOG_NORMAL, "%s\n", szMessage );
}


//------------------------------------------------------------------------------------------------
/// 컷씬 존관련
//------------------------------------------------------------------------------------------------
int SC_ChangeState ( int iNewState )
{
	CGame::GetInstance().ChangeState( iNewState );

	return 1;
}

int SC_ScreenFadeInStart ( float fade_in_t,float fade_m_t,float fade_out_t,int color_r,int color_g,int color_b )
{
	ScreenFadeInStart( fade_in_t, fade_m_t, fade_out_t, color_r, color_g, color_b );

	return 1;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이펙트 파일을 이펙트 리소스 관리자에 넣는다.
//----------------------------------------------------------------------------------------------------
int	SC_GetEffectUseFile ( ZSTRING szEffectFile )
{
	return (int)g_pEffectLIST->Add_EffectFILE( (char*)szEffectFile );
}


int	SC_GetEffectUseIndex ( int iEffectIndex )
{
	return (int)g_pEffectLIST->Add_EffectWithIDX( iEffectIndex, true );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief CObjCHAR 에 이펙트를 붙인다.
//----------------------------------------------------------------------------------------------------
void SC_EffectOnChar ( int iObject, int iEffectHash )
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iObject, false );
	if ( pObjCHAR ) 
	{
		pObjCHAR->ShowEffectOnCharByHash( iEffectHash );		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief CObjCHAR 에 이펙트를 붙인다.
//----------------------------------------------------------------------------------------------------
void SC_EffectOnObject ( HNODE hNode, int iEffectHash )
{
	CEffect *pEffect = g_pEffectLIST->Add_EFFECT( (t_HASHKEY)iEffectHash, true );
	pEffect->LinkNODE ( hNode );
}

//----------------------------------------------------------------------------------------------------
/// @param interface/interfacetype.h에 있는 DialogType
/// @brief 인자로 넘어온 Dialog를 연다
//----------------------------------------------------------------------------------------------------
void SC_OpenDialog ( int iDialog )
{
	g_itMGR.OpenDialog( iDialog , false );
}

//----------------------------------------------------------------------------------------------------
/// @param 출력될 메세지
/// @brief MODALESS 메세지 박스를 연다
//----------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void	SC_OpenMsgbox ( ZSTRING pszMsg )
{
	g_itMGR.OpenMsgBox( pszMsg ,CMsgBox::BT_OK, false );
}

//----------------------------------------------------------------------------------------------------
/// @param iParentDlg	Button의 Parent Dialog
/// @param iButtonID	Button의 ID( 해당 Dialog의 xml에서 ID 참조 )
/// @brief 해당 다이얼로그의 차일드 버튼을 Blink시킨다( Child의 Child일 경우 안된다 )
//----------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void	SC_SetButtonBlink ( int iParentDlg, int iButtonID )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( iParentDlg ) )
	{
		if(CWinCtrl* pCtrl = pDlg->Find( iButtonID ) )
		{
			if( pCtrl->GetControlType() == CTRL_BUTTON )
			{
				CTButton* pBtn = (CTButton*)pCtrl;
				pBtn->SetBlink( 1, 500 );
			}
		}
		else
		{
			assert( 0 && "Not Found Button" );
		}
	}
	else
	{
		assert( 0 && "Not Found Parent Dialog" );
	}
}

void	SC_CreateEventButton ( int iButtonIndex )
{
	CTutorialEventUtility::GetSingleton().CreateNotifyEventButton( iButtonIndex );
}


SYSTEM_SCRIPT
void	SC_AddNpcIndicator ( int npcno , int auto_remove )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MINIMAP ) )
	{
		CMinimapDLG* p = (CMinimapDLG*)pDlg;
		p->AddIndicatorNpc( npcno , auto_remove );
	}
}

SYSTEM_SCRIPT
void	SC_AddCoordinatesIndicator ( int index, int zoneno, float x, float y )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MINIMAP ) )
	{
		CMinimapDLG* p = (CMinimapDLG*)pDlg;
		p->AddIndicatorCoordinates( index, zoneno, x, y );
	}
}

SYSTEM_SCRIPT
void	SC_RemoveNpcIndicator ( int npcno )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MINIMAP ) )
	{
		CMinimapDLG* p = (CMinimapDLG*)pDlg;
		p->RemoveIndicatorNpc( npcno );
	}
}

SYSTEM_SCRIPT
void	SC_RemoveCoordinatesIndicator ( int index )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MINIMAP ) )
	{
		CMinimapDLG* p = (CMinimapDLG*)pDlg;
		p->RemoveIndicatorCoordinates( index );
	}
}

//------------------------------------------------------------------------------------------------
/// 아바타 정보
//------------------------------------------------------------------------------------------------

int	SC_GetAvatarEXP()
{
	return g_pAVATAR->Get_EXP();
}

int	SC_GetAvatarLEVEL()
{
	return g_pAVATAR->Get_LEVEL();
}



//------------------------------------------------------------------------------------------------
/// Send msg to web
//------------------------------------------------------------------------------------------------
void SC_SendMsgToWeb( ZSTRING pstrURL, ZSTRING pstrPage, ZSTRING pstrMsg )
{
	//TI_SendHttpPostData ( pstrURL, pstrPage, pstrMsg );
}

//------------------------------------------------------------------------------------------------
/// 퀘스트 트리거를 실행한다 2005-05-26 김창수
//------------------------------------------------------------------------------------------------
void SC_QF_doQuestTrigger ( ZSTRING TriggerName )
{
	::QF_doQuestTrigger ( TriggerName ) ;
}

//------------------------------------------------------------------------------------------------
/// 퀘스트 트리거 컨디션을 체크한. 2005-05-26 김창수
//------------------------------------------------------------------------------------------------
int SC_QF_checkQuestCondition ( ZSTRING szQuestTriggerName )
{
	return ::QF_checkQuestCondition ( szQuestTriggerName );
}

//------------------------------------------------------------------------------------------------
/// 
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void SC_GF_setRevivePosition ( )
{
	GF_setRevivePosition();
}

//-------------------------------------------------------------------------------------------------------------------
/// 튜토리얼 이미지를 화면에 지정된 위치에 일정시간동안 보여준다.
/// @param ZSTRING	filename: 이미지 화일네임( path/filename )
/// @param int		x		: 출력할 화면의 x좌표
/// @param int		y		: 출력할 화면의 y좌표
/// @param float	fade_time	: 출력중인 이미지가 화면에서 사라지기 시작하는 시간( max_time 보다는 작아야 한다 )
/// @param float	max_time	: 이미지가 화면에 표시될 총 시간
/// @param int		append_or_renewal	: 기존 출력중인 이미지가 있을경우 삭제하고 보여줄것인가? 출력이 끝난후 보여줄것인가?
//-------------------------------------------------------------------------------------------------------------------
void SC_ShowTutorialImage ( ZSTRING filename, int x, int y, float fadein_endtime, float fadeout_starttime, float max_time, int append_or_renewal )
{
	//CTutorialEventManager::GetSingleton().RegistImage( filename, x, y, fadein_time,fadeout_time, max_time, append_or_renewal );
	CTutorialEventManager::GetSingleton().RegistImage( filename, x, y, fadein_endtime, fadeout_starttime, max_time, append_or_renewal );
}


//------------------------------------------------------------------------------------------------
/// SC => GF/QF 연동 스크립드
/// 자세한 설명은 SC_를 뺀 스크립트 참조
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int SC_GF_getVariable ( int iVarTYPE )
{
	return GF_getVariable ( iVarTYPE );
}

SYSTEM_SCRIPT
int SC_QF_getNpcQuestZeroVal ( int iNpcNO )
{
	return QF_getNpcQuestZeroVal ( iNpcNO );
}

SYSTEM_SCRIPT
int SC_QF_findQuest ( int iQuestID )
{
	return QF_findQuest ( iQuestID );
}

SYSTEM_SCRIPT
int SC_QF_getEpisodeVAR ( int iVarNO )
{
	return QF_getEpisodeVAR ( iVarNO );
}

SYSTEM_SCRIPT
int SC_QF_getJobVAR ( int iVarNO )
{
	return	QF_getJobVAR ( iVarNO );
}

SYSTEM_SCRIPT
int SC_QF_getPlanetVAR ( int iVarNO )
{
	return QF_getPlanetVAR ( iVarNO );
}

SYSTEM_SCRIPT
int SC_QF_getUnionVAR ( int iVarNO )
{
	return QF_getUnionVAR ( iVarNO );
}

SYSTEM_SCRIPT
int SC_QF_getQuestVar ( int hQUEST, int iVarNO )
{
	return QF_getQuestVar ( hQUEST, iVarNO );
}

SYSTEM_SCRIPT
int SC_QF_getQuestSwitch ( int hQUEST, int iSwitchNO )
{
	return QF_getQuestSwitch ( hQUEST, iSwitchNO );
}

SYSTEM_SCRIPT
int SC_QF_getQuestItemQuantity ( int iQuestID, int iItemNo/*5자리:Type+No*/ )
{
	return QF_getQuestItemQuantity ( iQuestID, iItemNo );
}

SYSTEM_SCRIPT
int SC_QF_getUserSwitch ( int iSwitchNO )
{
	return QF_getUserSwitch ( iSwitchNO );
}

int SC_QF_GetNpcQuestStatus ( int npcindex )
{
	int iReturn;
	CSystemProcScript::GetSingleton().CallLuaFunction( iReturn, "SC_QF_GetNpcQuestStatus",ZZ_PARAM_INT, npcindex, ZZ_PARAM_END );
	return iReturn;
}