/*
	$Header: /Client/event/GF_SYSTEM.CPP 21    04-11-15 11:40a Navy $
*/

#include "stdAFX.h"
#include "Quest_FUNC.h"
#include "Game_FUNC.h"
#include "OBJECT.h"
#include "Network\CNetwork.h"
#include "IO_Terrain.h"
#include "Game.h"
#include "..\\interface\\it_mgr.h"
#include "../GameData/CStore.h"

//-------------------------------------------------------------------------------------------------
int		GF_getVariable ( int iVarTYPE )
{
	// CObjCHAR *pChar = g_pObjectMGR( iObjectNO );
	switch( iVarTYPE ) {
		case SV_SEX		:	return g_pAVATAR->Get_SEX ();
		case SV_BIRTH	:	return g_pAVATAR->Get_BIRTH();
		case SV_CLASS	:	return g_pAVATAR->Get_JOB ();
		case SV_UNION	:	return g_pAVATAR->Get_UNION ();
		case SV_RANK	:	return g_pAVATAR->Get_RANK ();
		case SV_FAME	:	return g_pAVATAR->Get_FAME ();

		case SV_STR		:	return g_pAVATAR->Get_STR ();
		case SV_DEX		:	return g_pAVATAR->Get_DEX ();
		case SV_INT		:	return g_pAVATAR->Get_INT ();
		case SV_CON		:	return g_pAVATAR->Get_CON ();
		case SV_CHA		:	return g_pAVATAR->Get_CHARM ();
		case SV_SEN		:	return g_pAVATAR->Get_SENSE ();

		case SV_LEVEL	:	return g_pAVATAR->Get_LEVEL ();
		case SV_EXP		:	return g_pAVATAR->Get_EXP ();
	}

	return -1;
}

ZSTRING	GF_getName ( void )
{
	return g_pAVATAR->Get_NAME ();
}

void	GF_setVariable ( int iVarTYPE, int iValue )
{
	LogString (LOG_DEBUG, "GF_setVariable( %d, %d ) \n", iVarTYPE, iValue);

	g_pNet->Send_cli_SET_VAR_REQ( iVarTYPE, iValue );
	/*
	switch( iVarTYPE ) {
		case SV_FAME :		g_pAVATAR->Set_FAME (iValue);	break;
		case SV_UNION :		g_pAVATAR->Set_UNION (iValue);	break;
		case SV_SEX :		g_pAVATAR->Set_SEX (iValue);	break;
		case SV_LEVEL :		g_pAVATAR->Set_LEVEL (iValue);	break;
		case SV_CLASS :		g_pAVATAR->Set_JOB (iValue);	break;
		//case SV_GRADE :		g_pAVATAR->Set_RANK (iValue);	break;
		case SV_STR :		g_pAVATAR->Set_STR (iValue);	break;
		case SV_DEX :		g_pAVATAR->Set_DEX (iValue);	break;
		case SV_WIS :		g_pAVATAR->Set_INT (iValue);	break;
		case SV_CON :		g_pAVATAR->Set_CON (iValue);	break;
		case SV_CHARM :		g_pAVATAR->Set_CHARM (iValue);	break;
		case SV_EMOTION :	g_pAVATAR->Set_SENSE (iValue);	break;
	}
	*/
}

int		GF_getZone ( void )
{
	return -1;
}

ZSTRING	GF_getDate ( void )
{
	return NULL;
}

ZSTRING	GF_getGameVersion ( void )
{
	return NULL;
}

int		GF_getTownVar ( int iVarTYPE )
{
	return -1;
}



//-------------------------------------------------------------------------------------------------
void	GF_moveXY ( int iMapNo, int iX, int iY )
{
	//D3DVECTOR	d3dvecPos ;
	//d3dvecPos.x = (float)iX ;
	//d3dvecPos.y = (float)iY ;
	//g_pAVATAR->Move( d3dvecPos );
}

int		GF_movableXY ( int iMapNo, int iX, int iY )
{
	return 0;
}

int		GF_moveEvent ( int iEffectNo )
{
	return 0;
}


//-------------------------------------------------------------------------------------------------
void	GF_spawnMonXY ( int iMonNo, int iMonLevel, int iX, int iY )
{
}

void	GF_spawnMonAtEvent ( int iMonNo, int iMonLevel, int iEvnetNo )
{
}

//----------------------------------[ 워프 함수 ]---------------------------------------------------
ZSTRING	GF_getReviveZoneName ( )
{
	// 현재 설정된 부활 맵의 이름을 얻는다..
	if ( g_pAVATAR->m_nReviveZoneNO > 0 )
		return ZONE_NAME( g_pAVATAR->m_nReviveZoneNO );
	return NULL;
}

void	GF_setRevivePosition ( )
{
	/// 현재 존을 부활 장소 지정 요청한다..
	g_pNet->Send_cli_SET_REVIVE_POS ();
	/// client는 별도로 현재 존을 부활장소로 가지고 있는다.
	g_pAVATAR->m_nReviveZoneNO = g_pTerrain->GetZoneNO();
}


void	GF_warp ( int iWarpIndex )
{
	g_pNet->Send_cli_TELEPORT_REQ ( g_pAVATAR, iWarpIndex /* 이 값은 warp.stb에 있는 줄번호 값 */);
}


//----------------------------------[  경제 관련 ]-------------------------------------------------
// TOWN_RATE 설정하기
void	GF_setTownRate ( int iTownRate )
{
	::g_pTerrain->m_Economy.SetTownRATE (iTownRate /* 이 값으로 TOWN_RATE가 조정된다 */);
}

// TOWN_RATE 가져오기
int		GF_getTownRate ( void )
{						  
	return ::g_pTerrain->m_Economy.Get_TownRATE ();
}

// WORLD_RATE 가져오기
int		GF_getWorldRate ( void )
{
	return ::Get_WorldRATE ();
}

void	GF_setWorldRate ( int iWorldRate )
{
	::Set_WorldRATE( iWorldRate );
}

// 아이템 물가 알아내기
int		GF_getItemRate ( int iItemTYPE )
{
	return ::g_pTerrain->m_Economy.Get_ItemRATE (iItemTYPE);
}

// 아이템 보유량 가져오기
int		GF_checkTownItem ( int iItemTYPE )
{
#ifdef	__VIRTUAL_SERVER
	return ::g_pTerrain->m_Economy.Get_TownITEM (iItemTYPE);	
#else
	return 1000;
#endif
}

//-------------------------------------------------------------------------------------------------
/// iNPCNO : 오브젝트 번호가 넘어온다.
void	GF_openStore ( int iNPCNO, int bSpecialTab )
{
	if( CStore::GetInstance().ChangeStore( iNPCNO, bSpecialTab ) )
	{
		g_itMGR.OpenDialog( DLG_TYPE_STORE, false );
		g_itMGR.OpenDialog( DLG_TYPE_DEAL, false );
		g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
	}
}


//=========================================================================
// 공용 함수. CF_xxx
//=========================================================================
void CF_addMsgToChatWND (char *pStr, ...)
{
	va_list	vlist;
	char *	pBuff = NULL;

	pBuff = new char[ strlen (pStr) + 255 ];
	if(!pBuff) { return; }

	va_start (vlist, pStr);
	sprintf (pBuff, pStr, vlist);
	va_end (vlist);

	g_itMGR.AppendChatMsg(pBuff, IT_MGR::CHAT_TYPE_SYSTEM );
	if(pBuff) { delete pBuff; }
}