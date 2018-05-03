/*
	$Header: /Client/Game_FUNC.cpp 54    05-06-19 5:33p Navy $
*/
#include "stdAFX.h"
#include "IO_Effect.h"
#include "OBJECT.h"
#include "Game_FUNC.h"
#include "Event/Quest_FUNC.h"
#include "interface/it_mgr.h"
#include "gamedata/cbank.h"
#include "gamedata/cseparate.h"
#include "gamedata/cupgrade.h"
#include "gamedata/cclan.h"
#include "interface/command/uicommand.h"
#include "../System/CGame.h"
#include "../network/cnetwork.h"
//-------------------------------------------------------------------------------------------------

class GlobalSCRIPT {
private:
public :
	classLUA	   *m_pLUA;

	GlobalSCRIPT ();
	~GlobalSCRIPT ();
} ;
GlobalSCRIPT *g_pScript=NULL;

//-------------------------------------------------------------------------------------------------
GlobalSCRIPT::GlobalSCRIPT ()
{
	m_pLUA = new classLUA;

	int iResult = 0;

	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( "Scripts\\GlobalSCR.lua" ) == false )
	{
		g_pCApp->ErrorBOX( "File open error...", "Scripts\\GlobalSCR.lua" );
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );		
	}

	pFileSystem->ReadToMemory();

	iResult = m_pLUA->Do_Buffer( (const char*)( pFileSystem->GetData() ), pFileSystem->GetSize() );

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );


	switch( iResult ) {
		// error codes for lua_do* 
		case LUA_ERRRUN		:
		case LUA_ERRFILE	:	// 2
		case LUA_ERRSYNTAX	:	// 3
		case LUA_ERRMEM		:
		case LUA_ERRERR		:
			LogString (LOG_NORMAL, "Script File ERROR[ %d ] %s \n", iResult, "Scripts\\GlobalSCR.lua" );
			SAFE_DELETE( m_pLUA );
			return;
	}
}

GlobalSCRIPT::~GlobalSCRIPT ()
{
	SAFE_DELETE( m_pLUA );
}

//-------------------------------------------------------------------------------------------------
extern void GF_Init (lua_State *L);
bool InitScript ()
{
	g_pScript = new GlobalSCRIPT;
	GF_Init( g_pScript->m_pLUA->m_pState );
	return true;
}

void FreeScript ()
{
	SAFE_DELETE( g_pScript );
}





//-------------------------------------------------------------------------------------------------
void GF_log ( ZSTRING szStr )
{
#if	defined( _DEBUG ) || defined( _D_RELEASE ) 
	g_itMGR.AppendChatMsg((char*)szStr, IT_MGR::CHAT_TYPE_SYSTEM );
#endif
}




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이펙트 파일을 이펙트 리소스 관리자에 넣는다.
//----------------------------------------------------------------------------------------------------
int	GF_GetEffectUseFile ( ZSTRING szEffectFile )
{
	return (int)g_pEffectLIST->Add_EffectFILE( (char*)szEffectFile );
}


int	GF_GetEffectUseIndex ( int iEffectIndex )
{
	return (int)g_pEffectLIST->Add_EffectWithIDX( iEffectIndex, true );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief CObjCHAR 에 이펙트를 붙인다.
//----------------------------------------------------------------------------------------------------
void GF_EffectOnObject ( int iObject, int iEffectHash )
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
void GF_WeatherEffectOnObject ( int iObject, int iEffectHash )
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iObject, false );
	if ( pObjCHAR ) 
	{
		pObjCHAR->ShowEffectOnCharByHash( iEffectHash, 0, true );		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 특정 이펙트를 제거
//----------------------------------------------------------------------------------------------------
void	GF_DeleteEffectFromObject ( int iObject )
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iObject, false );
	if ( pObjCHAR ) 
	{
		pObjCHAR->DeleteWeatherEffect( );		
	}
}


int	GF_GetTarget ( int iObject )
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iObject, true );
	if ( pObjCHAR ) 
		return g_pObjMGR->Get_ClientObjectIndex( pObjCHAR->m_iServerTarget );
		
	return 0;
}

//-------------------------------------------------------------------------------------------------
int GF_GetMotionUseFile ( ZSTRING szMotionFile )
{
	t_HASHKEY HashMOTION = g_MotionFILE.Add_FILE( (char*)szMotionFile );
	return (int)HashMOTION;
}

void GF_SetMotion ( int iObject, int iMotionHashKEY, int iRepeatCnt )
{
	// iObject에 iMotionFileIDX의 모션을 적용해라 !!
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_CharOBJ( iObject, false );
	if ( pObjCHAR ) {
		pObjCHAR->Set_UserMOITON (iMotionHashKEY, 0, iRepeatCnt );
		///pObjCHAR->Set_STATE( CS_NEXT_STOP );
	}
}

//-------------------------------------------------------------------------------------------------
void	GF_openBank ( int iNPCNO )
{
	CBank::GetInstance().SetNpcClientIndex( iNPCNO );
	g_pNet->Send_cli_BANK_LIST_REQ( BANK_REQ_OPEN ,NULL );
}

//-------------------------------------------------------------------------------------------------
void	GF_openDeliveryStore ( )
{
	g_itMGR.OpenDialog( DLG_TYPE_DELIVERYSTORE );
}

//-------------------------------------------------------------------------------------------------
void	GF_repair ( int iNPCNO )
{
	CGame::GetInstance().BeginRepair( CGame::REPAIR_NPC );
	CGame::GetInstance().SetRepairNpcSvrIdx( g_pObjMGR->Get_ServerObjectIndex( iNPCNO) );
	g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
}

//-------------------------------------------------------------------------------------------------
void	GF_appraisal ( )
{
	//CGame::GetInstance().BeginRepair( CGame::REPAIR_NPC );
	//CGame::GetInstance().SetRepairNpcSvrIdx( g_pObjMGR->Get_ServerObjectIndex( iNPCNO) );
	CGame::GetInstance().BeginAppraisal();
	g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
}

//-------------------------------------------------------------------------------------------------
void	GF_openSeparate ( int iNPCNO )
{
	CSeparate::GetInstance().SetNpcSvrIdx( g_pObjMGR->Get_ServerObjectIndex( iNPCNO) );
	g_itMGR.OpenDialog( DLG_TYPE_ITEM , false );
	g_itMGR.OpenDialog( DLG_TYPE_SEPARATE, false ); 
}
//-------------------------------------------------------------------------------------------------
void	GF_openUpgrade ( int iNPCNO )
{
	CUpgrade::GetInstance().SetNpcSvrIdx( g_pObjMGR->Get_ServerObjectIndex( iNPCNO) );
	g_itMGR.OpenDialog( DLG_TYPE_ITEM , false );
	g_itMGR.OpenDialog( DLG_TYPE_UPGRADE, false );
}


GF_SCRIPT
void	GF_organizeClan ( int iNPCNO )
{
	if( CClan::GetInstance().GetClanNo() )
		g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_JOIN_HAS_CLAN );
	else
		g_itMGR.OpenDialog( DLG_TYPE_CLAN_ORGANIZE );
}

GF_SCRIPT
void	GF_disorganizeClan ( int iNPCNO )
{
	if( CClan::GetInstance().GetClass() == CClan::CLAN_MASTER )
	{
		if( CClan::GetInstance().GetMemberCount() >= 2 )
		{
			g_itMGR.OpenMsgBox( STR_CLAN_CANT_DISORGANIZE_MEMBERCOUNT );
		}
		else
		{
			CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_DISBAND, NULL );
			g_itMGR.OpenMsgBox( STR_CLAN_QUERY_DISORGANIZE, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
		}
	}
	else
	{
		g_itMGR.OpenMsgBox(  STR_CLAN_NO_RIGHT );
	}
}

//-------------------------------------------------------------------------------------------------
void scrTest (int i, int j, char *szStr )
{
	lua_CallIntFUNC (g_pScript->m_pLUA->m_pState, "test", 
				ZZ_PARAM_INT, i, 
				ZZ_PARAM_INT, j, 
				ZZ_PARAM_STRING, szStr,
				ZZ_PARAM_END);
}


void GF_error ( void )
{
}



//----------------------------------------------------------------------------------------------------
///
/// 게임내 이벤트에 따른 효과 출력
///
//----------------------------------------------------------------------------------------------------

int Call_Char_Lua_Func( int iObject, const char* pFunc )
{
	if ( !g_pScript->m_pLUA )
		return 0;

	int iResult = lua_CallIntFUNC (g_pScript->m_pLUA->m_pState, pFunc, ZZ_PARAM_INT, iObject, ZZ_PARAM_END);

	return iResult;
}

/// 아바타 처음 생성할 때 생기는 효과
void SE_CharCreate  ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_CharCreate" );
}

/// 아바타가 처음 월드에 조인할때
void SE_CharJoinZone( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_CharJoinZone" );
}

/// 아바타 사망시
void SE_CharDie		( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_CharDie" );
}

/// 아바타 워프
void SE_CharWarp	( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_CharWarp" );
}

/// 아바타 레벨업
void SE_CharLevelUp ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_CharLevelUp" );
}

/// 파티 레벨업
void SE_PartyLevelUp ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_PartyLevelUp" );
}



/*
몬스터 관련
*/

/// 몬스터 죽을때
void SE_MobDie ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_MobDie" );
}

///제조 시작
void SE_StartMake ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_StartMake" );
}
///제련 시작
void SE_StartUpgrade ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_StartUpgrade" );
}
/// 제조 성공
void SE_SuccessMake ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_SuccessMake" );
}
/// 제련 성공
void SE_SuccessUpgrade ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_SuccessUpgrade" );
}
/// 분리/분해 성공
void SE_SuccessSeparate ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_SuccessSeparate" );
}
/// 제조 실패
void SE_FailMake ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_FailMake" );
}
///제련 실패
void SE_FailUpgrade ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_FailUpgrade" );
}

///스킬 레벨업
void SE_SkillLevelUp ( int iObject )
{
	int iResult = Call_Char_Lua_Func( iObject, "SE_SkillLevelUp" );
}


///
/// Log/OutputDebugString
///
void GF_LogString ( ZSTRING szMessage )
{
	LogString( LOG_NORMAL, "%s\n", szMessage );
}

/*
랜덤넘버 제네레이터
*/
int	GF_Random ( int iMax )
{
	return RANDOM( iMax );
}



//------------------------------------------------------------------------------------------------
/// 날씨 이펙트
//------------------------------------------------------------------------------------------------
void SE_WeatherEffect ( int iObject, int iWeatherType )
{
	if ( !g_pScript->m_pLUA )
		return;

	int iResult = lua_CallIntFUNC (g_pScript->m_pLUA->m_pState, "SE_WeatherEffect", ZZ_PARAM_INT, iObject, ZZ_PARAM_INT, iWeatherType, ZZ_PARAM_END);
	
}


//------------------------------------------------------------------------------------------------
/// 컷씬 존관련
//------------------------------------------------------------------------------------------------
int GF_ChangeState ( int iNewState )
{
	CGame::GetInstance().ChangeState( iNewState );

	return 1;
}

