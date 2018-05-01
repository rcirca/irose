/*
	$Header: /Client/Network/SendPACKET.cpp 210   05-10-18 3:10p Young $
*/
#include "stdAFX.h"
#include "CNetwork.h"
#include "..\util\classMD5.h"
#include "Game.h"
#include "IO_Terrain.h"
#include "..\common\Calculation.h"
#include "..\common\IO_AI.h"
#include "../Interface/it_MGR.h"
#include "Util/CCheat.h"
#include "util/classTIME.h"
#include "../CommandFilter.h"
#include "../GameData/CPrivateStore.h"
#include "GameProc/preventduplicatedcommand.h"
#include "../common/io_stb.h"
#include "../Country.h"
#include "../System/CGame.h"
#include "../System/GameStateMovePlanet.h"

#ifdef	__VIRTUAL_SERVER
void CSendPACKET::Send_gsv_ADD_CHAR (int iObjectIndex, short nCharIdx, tPOINTF &PosSET)
{
	m_pSendPacket->m_HEADER.m_wType = GSV_MOB_CHAR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_MOB_CHAR );

	m_pSendPacket->m_gsv_MOB_CHAR.m_wObjectIDX = iObjectIndex;
	m_pSendPacket->m_gsv_MOB_CHAR.m_nCharIdx = nCharIdx;
	m_pSendPacket->m_gsv_MOB_CHAR.m_PosCUR = PosSET;
	m_pSendPacket->m_gsv_MOB_CHAR.m_bRunning = true;//NPC_SPEED( nCharIdx );

	this->Send_PACKET( m_pSendPacket );
}
void CSendPACKET::Send_gsv_ADD_USER (CObjAVT *pAvtCHAR)
{
	/*
	m_pSendPacket->m_HEADER.m_wType = GSV_ADD_USER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_ADD_USER );

	m_pSendPacket->m_gsv_ADD_USER.m_wObjectIDX;
	m_pSendPacket->m_gsv_ADD_USER.m_bIsFemale = pAvtCHAR->IsFemale ();
	::CopyMemory( m_pSendPacket->m_gsv_ADD_USER.m_nPartItemIDX , pAvtCHAR->m_nPartItemIDX, sizeof(short)*MAX_BODY_PART );
	Packet_AppendString (m_pSendPacket, pAvtCHAR->Get_NAME() );

	this->Send_PACKET( m_pSendPacket );
	*/
}
void CSendPACKET::Send_gsv_ADD_FIELDITEM (int iObjectIndex, tagITEM &sITEM)
{
    m_pSendPacket->m_HEADER.m_wType = GSV_ADD_FIELDITEM;
    m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_ADD_FIELDITEM );

    m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX = iObjectIndex;
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_ITEM			= sITEM;
    m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX   = iObjectIndex;
    m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME    = 20*1000;	// 20 sec

	D3DXVECTOR3 CurPos = g_pAVATAR->Get_CurPOS();
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_PosCUR.x = CurPos.x;
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_PosCUR.y = CurPos.y;

	this->Send_PACKET( m_pSendPacket );
}
#endif


//-------------------------------------------------------------------------------------------------
CSendPACKET::CSendPACKET ()
{
	m_pSendPacket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];
}

CSendPACKET::~CSendPACKET()
{
	SAFE_DELETE_ARRAY( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_HEADER (WORD wType, bool bSendToWorld )
{
#ifdef	__VIRTUAL_SERVER
	;
	return;
	;
#else
	m_pSendPacket->m_HEADER.m_wType = wType;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
#endif

	this->Send_PACKET( m_pSendPacket, bSendToWorld );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_LOGIN_REQ (char *szAccount, char *szPassword ,bool bEncode )
{
	if ( !szAccount || !szAccount[0] ) 
		return;

	int iStrLen=strlen(szAccount);
	for (int iC=0; iC<iStrLen; iC++) {
		if ( szAccount[iC] == '\'' )	// 계정에 ' 문자는 들어가면 안됨...
			return;
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_LOGIN_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_LOGIN_REQ );

	if( bEncode )
	{
		GetMD5 (m_pMD5Buff, (unsigned char*)szPassword, strlen( szPassword ) );
		::CopyMemory( m_pSendPacket->m_cli_LOGIN_REQ.m_MD5Password, m_pMD5Buff, 32);
		Packet_AppendString (m_pSendPacket, szAccount);
	}
	else
	{

		::CopyMemory( m_pMD5Buff, g_GameDATA.m_PasswordMD5, 32);
		::CopyMemory( m_pSendPacket->m_cli_LOGIN_REQ.m_MD5Password, g_GameDATA.m_PasswordMD5, 32);
		Packet_AppendString (m_pSendPacket, g_GameDATA.m_Account.Get() );
	}

#if defined(_TAIWAN) || defined(_PHILIPPIN)
	short codepage = 950;
	Packet_AppendData( m_pSendPacket, &codepage, sizeof( codepage ) );
#endif

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SELECT_SERVER( int iServerID ,int iChannelNo)
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SELECT_SERVER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SELECT_SERVER );
	m_pSendPacket->m_cli_SELECT_SERVER.m_dwServerID = (DWORD)iServerID;
	m_pSendPacket->m_cli_SELECT_SERVER.m_btChannelNO = (BYTE)iChannelNo;
#endif

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
/*
void CSendPACKET::Send_cli_JOIN_WORLD (DWORD dwLSVID)
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_JOIN_WORLD;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_JOIN_WORLD );
	m_pSendPacket->m_cli_JOIN_WORLD.m_dwID = dwLSVID;
	::CopyMemory( m_pSendPacket->m_cli_JOIN_WORLD.m_MD5Password, m_pMD5Buff, sizeof(BYTE)*32);
#endif

	this->Send_PACKET( m_pSendPacket );
}
*/
void CSendPACKET::Send_cli_JOIN_SERVER_REQ (DWORD dwLSVID, bool bWorldServer)
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_JOIN_SERVER_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_JOIN_SERVER_REQ );
	m_pSendPacket->m_cli_JOIN_SERVER_REQ.m_dwID = dwLSVID;
	::CopyMemory( m_pSendPacket->m_cli_JOIN_SERVER_REQ.m_MD5Password, m_pMD5Buff, sizeof(BYTE)*32);
#endif

	this->Send_PACKET( m_pSendPacket, bWorldServer );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_LOGOUT_REQ ()
{
#ifdef	__VIRTUAL_SERVER
	;
	//_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_LOGOUT_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_LOGOUT_REQ );
#endif
	this->Send_PACKET( m_pSendPacket );
}
void CSendPACKET::Send_cli_LOGOUT_CANCEL ()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_LOGOUT_CANCEL;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
	this->Send_PACKET( m_pSendPacket );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHAR_LIST ()
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_CHAR_LIST;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAR_LIST );
#endif
	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CREATE_CHAR (char *szCharName, BYTE btCharRACE, char cBoneStone, char cHairIdx, char cFaceIdx, char cWeaponType, char cZoneNo)
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_CREATE_CHAR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CREATE_CHAR );

	m_pSendPacket->m_cli_CREATE_CHAR.m_btCharRACE	= btCharRACE;
	m_pSendPacket->m_cli_CREATE_CHAR.m_cBoneSTONE	= cBoneStone;
	m_pSendPacket->m_cli_CREATE_CHAR.m_cHairIDX		= cHairIdx;
	m_pSendPacket->m_cli_CREATE_CHAR.m_cFaceIDX		= cFaceIdx;
	m_pSendPacket->m_cli_CREATE_CHAR.m_cWeaponTYPE	= cWeaponType;
	m_pSendPacket->m_cli_CREATE_CHAR.m_nZoneNO		= cZoneNo;
	Packet_AppendString (m_pSendPacket, szCharName);
#endif	
	this->Send_PACKET( m_pSendPacket ,true);
}
//-------------------------------------------------------------------------------------------------
/// @param bDeleteOrRevive - true : delete, false - Revive
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_DELETE_CHAR (BYTE btCharNO, bool bDeleteOrRevive, char *szCharName)
{
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_DELETE_CHAR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_DELETE_CHAR );
	m_pSendPacket->m_cli_DELETE_CHAR.m_bDelete = bDeleteOrRevive;
	m_pSendPacket->m_cli_DELETE_CHAR.m_btCharNO = btCharNO;
	Packet_AppendString (m_pSendPacket, szCharName);
#endif
	this->Send_PACKET( m_pSendPacket ,true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SELECT_CHAR (BYTE btCharNO, char *szCharName)
{
	
#ifdef	__VIRTUAL_SERVER
	;
	_ASSERT( 0 );
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SELECT_CHAR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SELECT_CHAR );
	m_pSendPacket->m_cli_SELECT_CHAR.m_btCharNO = btCharNO;
	Packet_AppendString (m_pSendPacket, szCharName);
#endif
	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_JOIN_ZONE ( BYTE btWeightRate )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_JOIN_ZONE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_JOIN_ZONE );
	m_pSendPacket->m_cli_JOIN_ZONE.m_btWeightRate = btWeightRate;
	this->Send_PACKET( m_pSendPacket );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_REVIVE_REQ (BYTE btReviveTYPE)
{
#ifdef	__VIRTUAL_SERVER
	return;

#else
	g_pNet->m_bWarping = true;

	m_pSendPacket->m_HEADER.m_wType = CLI_REVIVE_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_REVIVE_REQ );
	
	m_pSendPacket->m_cli_REVIVE_REQ.m_btReviveTYPE = btReviveTYPE;
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_SET_REVIVE_POS ()
{
#ifdef	__VIRTUAL_SERVER
	return;

#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_REVIVE_POS;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_REVIVE_POS );
#endif

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SET_VAR_REQ (BYTE btVarTYPE, int iValue)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_VAR_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_VAR_REQ );
	m_pSendPacket->m_cli_SET_VAR_REQ.m_btVarTYPE = btVarTYPE;
	m_pSendPacket->m_cli_SET_VAR_REQ.m_iValue = iValue;
#endif
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SET_MOTION ( short nMotionNO, BYTE btStopCMD, WORD wETC )
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_SET_MOTION;
	m_pSendPacket->m_HEADER.m_nSize	= sizeof( gsv_SET_MOTION );
	m_pSendPacket->m_gsv_SET_MOTION.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_SET_MOTION.m_nMotionNO = nMotionNO;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_MOTION;
	m_pSendPacket->m_HEADER.m_nSize	= sizeof( cli_SET_MOTION );
	m_pSendPacket->m_cli_SET_MOTION.m_nMotionNO = nMotionNO;
	m_pSendPacket->m_cli_SET_MOTION.m_wIsSTOP = btStopCMD;
	m_pSendPacket->m_cli_SET_MOTION.m_wETC = wETC;
#endif

	g_CommandFilter.SetPrevCommand( NULL );

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_TOGGLE (short nToggleTYPE)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType			 = GSV_TOGGLE;
	m_pSendPacket->m_HEADER.m_nSize			 = sizeof( gsv_TOGGLE );
	m_pSendPacket->m_gsv_TOGGLE.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_TOGGLE.m_btTYPE	 = nToggleTYPE;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_TOGGLE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_TOGGLE );
	m_pSendPacket->m_cli_TOGGLE.m_btTYPE      = (BYTE)nToggleTYPE;
#endif

	g_CommandFilter.SetPrevCommand( NULL );

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_PARTY_CHAT(char *szMsg)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_PARTY_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_PARTY_CHAT );
	Packet_AppendString (m_pSendPacket, szMsg);
	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_CHAT (char *szMsg)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_CHAT );
	m_pSendPacket->m_gsv_CHAT.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	Packet_AppendString (m_pSendPacket, szMsg);
#else
	if ( g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAT );
	Packet_AppendString (m_pSendPacket, szMsg);

	/// 클라이언트에서만 사용하는 치트 사용..
	/// 클라이언트에서 사용하는 치트가 동작되면.. 쳇팅을 날리지 마라..
	if( g_Cheat.DoCheat( szMsg, false ) == CHEAT_RESULT_CLIENT )
		return;

#endif
	this->Send_PACKET( m_pSendPacket );
}


void CSendPACKET::Send_cli_ALLIED_CHAT( char* pszMsg )
{
	assert( pszMsg );
	
	if ( pszMsg == NULL || g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_ALLIED_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ALLIED_CHAT );
	Packet_AppendString (m_pSendPacket, pszMsg);

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_ALLIED_SHOUT( char* pszMsg )
{
	assert( pszMsg );
	if ( pszMsg == NULL || g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_ALLIED_SHOUT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ALLIED_SHOUT );
	Packet_AppendString (m_pSendPacket, pszMsg);

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_WHISPER (char *szTargetAccount, char *szMsg)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_WHISPER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_WHISPER );
	Packet_AppendString (m_pSendPacket, szTargetAccount);
	Packet_AppendString (m_pSendPacket, szMsg);
#else
	if ( g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_WHISPER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_WHISPER );
	Packet_AppendString (m_pSendPacket, szTargetAccount);
	Packet_AppendString (m_pSendPacket, szMsg);
#endif
	this->Send_PACKET( m_pSendPacket );
}
void CSendPACKET::Send_cli_SHOUT (char *szMsg)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_SHOUT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_SHOUT );
	Packet_AppendString (m_pSendPacket, g_pAVATAR->Get_NAME() );
	Packet_AppendString (m_pSendPacket, szMsg);
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SHOUT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SHOUT );
	Packet_AppendString (m_pSendPacket, szMsg);
#endif
	this->Send_PACKET( m_pSendPacket );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_STOP (D3DVECTOR &PosCUR)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_STOP;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_STOP );
	m_pSendPacket->m_gsv_STOP.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_STOP.m_PosCUR.x = PosCUR.x;
	m_pSendPacket->m_gsv_STOP.m_PosCUR.y = PosCUR.y;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_STOP;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_STOP );
	m_pSendPacket->m_cli_STOP.m_PosCUR.x = PosCUR.x;
	m_pSendPacket->m_cli_STOP.m_PosCUR.y = PosCUR.y;
#endif

	g_CommandFilter.SetPrevCommand( NULL );

	this->Send_PACKET( m_pSendPacket );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이동의 목적에 사용
/// @bug 조심...m_nPosZ가 short 형이기때문에...
//----------------------------------------------------------------------------------------------------

void CSendPACKET::Send_cli_MOUSECMD (int iClientTarget, D3DVECTOR &PosTO)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_MOUSECMD;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_MOUSECMD);
	m_pSendPacket->m_gsv_MOUSECMD.m_wSourObjIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_MOUSECMD.m_wDestObjIDX = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );
	m_pSendPacket->m_gsv_MOUSECMD.m_PosCUR.x = g_pAVATAR->m_PosCUR.x;
	m_pSendPacket->m_gsv_MOUSECMD.m_PosCUR.y = g_pAVATAR->m_PosCUR.y;
	m_pSendPacket->m_gsv_MOUSECMD.m_PosTO.x  = PosTO.x;
	m_pSendPacket->m_gsv_MOUSECMD.m_PosTO.y  = PosTO.y;
#else
	if ( g_pNet->m_bWarping )
		return;

	//-------------------------------------------------------------------------------
	/// 앉기 상태에선 금지..
	//-------------------------------------------------------------------------------				
	if( g_pAVATAR->Get_STATE() == CS_SIT )
		Send_cli_TOGGLE( TOGGLE_TYPE_SIT );

	m_pSendPacket->m_HEADER.m_wType = CLI_MOUSECMD;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MOUSECMD );
	m_pSendPacket->m_cli_MOUSECMD.m_wTargetObjectIDX = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );
	m_pSendPacket->m_cli_MOUSECMD.m_PosTO.x		 = PosTO.x;
	m_pSendPacket->m_cli_MOUSECMD.m_PosTO.y		 = PosTO.y;
	
	// 아바타의 이동 시작 위치의 높이를 같이 실어 보낸다.
	// 이 높이는 최종 위치의 높이가 아니라, 현재(즉, 이동 시작시)의 높이임을 명심하자.
	m_pSendPacket->m_cli_MOUSECMD.m_nPosZ		 = (short)(g_pAVATAR->Get_CurPOS().z);
#endif

	g_CommandFilter.SetPrevCommand( NULL );

	this->Send_PACKET( m_pSendPacket );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 클라이언트 판단에 의해 아바타가 움직이지 못할때 통보하는 패킷
/// @bug 조심...m_nPosZ가 short 형이기때문에...
//----------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CANTMOVE ()
{
#ifndef	__VIRTUAL_SERVER
	if ( g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_CANTMOVE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CANTMOVE );

	m_pSendPacket->m_cli_CANTMOVE.m_PosCUR.x = g_pAVATAR->Get_CurPOS().x;
	m_pSendPacket->m_cli_CANTMOVE.m_PosCUR.y = g_pAVATAR->Get_CurPOS().y;
	m_pSendPacket->m_cli_CANTMOVE.m_nPosZ	 = (short)(g_pAVATAR->Get_CurPOS().z);

	g_CommandFilter.SetPrevCommand( NULL );

	this->Send_PACKET( m_pSendPacket );	
#endif
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_ATTACK (int iClientTarget)
{
	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
		{
            g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ATTACK );
			return;
		}        
	}

#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_ATTACK;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_ATTACK );
	m_pSendPacket->m_gsv_ATTACK.m_wAtkObjIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_ATTACK.m_wDefObjIDX = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );
#else
	//-------------------------------------------------------------------------------
	/// 앉기 상태에선 금지..
	//-------------------------------------------------------------------------------				
	if( g_pAVATAR->Get_STATE() == CS_SIT )
		Send_cli_TOGGLE( TOGGLE_TYPE_SIT );

	m_pSendPacket->m_HEADER.m_wType = CLI_ATTACK;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ATTACK );
	m_pSendPacket->m_cli_ATTACK.m_wTargetObjectIndex = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );
#endif

	//----------------------------------------------------------------------------------------------------
	/// PVP 가능존일경우에는 PVP 플래그가 ON 상태가 아니라면 모든 공격명령 패지
	//----------------------------------------------------------------------------------------------------
	if( g_pTerrain->IsPVPZone() )
	{
		if( g_GameDATA.m_iPvPState == PVP_CANT )
			return;
	}

	g_CommandFilter.SetPrevCommand( NULL );



	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_DAMAGE (CObjCHAR *pAtkOBJ, CObjCHAR *pDefOBJ, WORD wDamage)
{
	if ( !pAtkOBJ )
		return;

	uniDAMAGE sDamage;
	sDamage.m_wDamage = wDamage;
	short nDamage = sDamage.m_wVALUE;

#ifdef	__VIRTUAL_SERVER
	switch( pDefOBJ->Get_TYPE() ) {
		case OBJ_USER :
			if ( pDefOBJ->Get_HP() - nDamage <= 0 ) {
				// 죽지 않게..
				 //nDamage *= -1;
				pDefOBJ->Set_HP( ((CObjUSER*)pDefOBJ)->Get_MaxHP() );
				nDamage = 0;
			}
			break;
		//case OBJ_NPC   :
		case OBJ_MOB   :
			if ( pDefOBJ->Get_HP() - nDamage <= 0 ) {
				if ( pAtkOBJ->IsUSER() ) {
					tagITEM sITEM;
					if ( CCal::Get_DropITEM ( pAtkOBJ->Get_LEVEL()-pDefOBJ->Get_LEVEL(), (CObjMOB*)pDefOBJ, sITEM ) ) {
						// Create field item ...
						this->Send_gsv_ADD_FIELDITEM ( 0, sITEM );
					}
				} else {
					// 죽였을때..
					g_AI_LIST.AI_Kill( NPC_AI_TYPE( pAtkOBJ->Get_CharNO() ), pAtkOBJ, pDefOBJ, nDamage );
				}

				if ( pAtkOBJ->m_iServerTarget == pDefOBJ->m_nIndex )	// 타겟 삭제.
					pAtkOBJ->m_iServerTarget = 0;

				// 죽을때...
				pDefOBJ->Check_EVENT( pAtkOBJ, QUEST_EVENT_ON_DEAD );	// on_dead ..
				g_AI_LIST.AI_Dead( NPC_AI_TYPE( pDefOBJ->Get_CharNO() ), pDefOBJ, pAtkOBJ, nDamage );
				sDamage.m_wACTION |= DMG_ACT_DEAD;

				LogString (LOG_NORMAL, " Obj:%d ,  %s dead .. damage : %d \n", pDefOBJ->m_nIndex, pDefOBJ->Get_NAME(), nDamage);
			} else  {
				// 몹이면 반격.
				if ( pAtkOBJ && pAtkOBJ->Get_HP() > 0 ) {
					// 공격자가 살아 있으면...
					pDefOBJ->SetCMD_ATTACK( g_pObjMGR->Get_ServerObjectIndex( pAtkOBJ->Get_INDEX() ) );
				}
				g_AI_LIST.AI_Damaged( NPC_AI_TYPE( pDefOBJ->Get_CharNO() ), pDefOBJ, pAtkOBJ, nDamage );
			}
			break;
	}

	m_pSendPacket->m_HEADER.m_wType = GSV_DAMAGE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_DAMAGE );
	m_pSendPacket->m_gsv_DAMAGE.m_wAtkObjIDX = g_pObjMGR->Get_ServerObjectIndex( pAtkOBJ->m_nIndex );
	m_pSendPacket->m_gsv_DAMAGE.m_wDefObjIDX = g_pObjMGR->Get_ServerObjectIndex( pDefOBJ->m_nIndex );
	m_pSendPacket->m_gsv_DAMAGE.m_Damage = sDamage;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_DAMAGE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_DAMAGE );
	m_pSendPacket->m_cli_DAMAGE.m_wDefObjIDX = g_pObjMGR->Get_ServerObjectIndex( pDefOBJ->m_nIndex );
	m_pSendPacket->m_cli_DAMAGE.m_wRandomIDX = 0;
#endif
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
// wNPCObjIDX : 거래할 NPC ...
// cBuyCNT    : 살 아이템 갯수
// cSellCNT   : 팔 아이템 갯수
// pINDEXs[]  : 거래 npc의 sell tab index + my avatar inventory item list index
void CSendPACKET::Send_cli_STORE_TRADE_REQ(WORD wNPCObjIDX, char cBuyCNT, char cSellCNT, tag_BUY_ITEM *pBuyITEMs, tag_SELL_ITEM *pSellITEMs)
{
	_ASSERT( cBuyCNT >= 0 && cSellCNT >= 0 );

#ifdef	__VIRTUAL_SERVER
	;
	;
	;
	return;
#else
	short nI;
	#ifdef	_DEBUG
	for (nI=0; nI<cBuyCNT; nI++) {
		_ASSERT( pBuyITEMs[ nI ].m_cColNO >= 0 );
		_ASSERT( pBuyITEMs[ nI ].m_cTabNO >= 0 );
		_ASSERT( pBuyITEMs[ nI ].m_wDupCNT >= 0 );
	}
	for (nI=0; nI<cSellCNT; nI++) {
		_ASSERT( pSellITEMs[ nI ].m_btInvIDX >  0 );
		_ASSERT( pSellITEMs[ nI ].m_wDupCNT  >= 0 );
	}
	#endif

	for (nI=0; nI<cSellCNT; nI++) {
		if ( g_pAVATAR->m_Inventory.m_ItemLIST[ pSellITEMs[ nI ].m_btInvIDX ].IsEmpty() ) {
			_ASSERT( 0 );
			return;
		}
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_STORE_TRADE_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_STORE_TRADE_REQ );

	m_pSendPacket->m_cli_STORE_TRADE_REQ.m_wNPCObjIdx = wNPCObjIDX,
	m_pSendPacket->m_cli_STORE_TRADE_REQ.m_cBuyCNT  = cBuyCNT;
	m_pSendPacket->m_cli_STORE_TRADE_REQ.m_cSellCNT = cSellCNT;
	m_pSendPacket->m_cli_STORE_TRADE_REQ.m_dwEconomyTIME = g_pTerrain->m_Economy.m_dwUpdateTIME;

	if ( cBuyCNT > 0 )
		Packet_AppendData( m_pSendPacket, pBuyITEMs,	sizeof( tag_BUY_ITEM )  * cBuyCNT );
	if ( cSellCNT > 0 )
		Packet_AppendData( m_pSendPacket, pSellITEMs,	sizeof( tag_SELL_ITEM ) * cSellCNT );
#endif

	this->Send_PACKET( m_pSendPacket );
}

/// 서버에 해당 오브젝트 HP 정보 요구
void CSendPACKET::Send_cli_HP_REQ( int iClientTarget )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_HP_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_HP_REQ );
	m_pSendPacket->m_cli_HP_REQ.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );	

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
extern bool IsTWAIN();
bool CSendPACKET::Send_cli_USE_ITEM ( short nInventoryIndex, bool bTargetSkill, int iClientTarget )
{
	tagITEM sITEM = g_pAVATAR->Get_InventoryITEM( nInventoryIndex );
	if ( sITEM.GetTYPE() != ITEM_TYPE_USE )
		return false;
	if ( sITEM.m_uiQuantity <= 0 )
		return false;
	
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_USE_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_USE_ITEM );
	m_pSendPacket->m_gsv_USE_ITEM.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_USE_ITEM.m_nUseItemNO = sITEM.m_nItemNo;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_USE_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_USE_ITEM );
	m_pSendPacket->m_cli_USE_ITEM.m_nInventoryIndex = nInventoryIndex;

	if( bTargetSkill )
	{
		WORD wServerIndex = g_pObjMGR->Get_ServerObjectIndex( iClientTarget );
		Packet_AppendData( m_pSendPacket, &wServerIndex,	sizeof( WORD ) );
	}
#endif
	this->Send_PACKET( m_pSendPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHANGE_SKIN (BYTE btBodyIDX, short nItemNO)
{
#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_CHANGE_SKIN;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_CHANGE_SKIN );

	m_pSendPacket->m_gsv_CHANGE_SKIN.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_CHANGE_SKIN.m_btBodyIDX  = btBodyIDX;
	m_pSendPacket->m_gsv_CHANGE_SKIN.m_nItemNO	  = nItemNO;
#else
	/*m_pSendPacket->m_HEADER.m_wType = CLI_CHANGE_SKIN;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHANGE_SKIN );

	m_pSendPacket->m_cli_CHANGE_SKIN.m_btBodyIDX  = btBodyIDX;
	m_pSendPacket->m_cli_CHANGE_SKIN.m_nItemNO	  = nItemNO;*/
#endif
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_EQUIP_ITEM (short nEquipInvIDX, short nWeaponInvIDX)
{	
	if( g_pAVATAR->m_bCastingSTART && g_pAVATAR->bCanActionActiveSkill() )
	{
		return;
	}

	if( CPrivateStore::GetInstance().IsOpened() )
		return;


#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 0;

	if ( 0 == nWeaponInvIDX ) {
		tagITEM *pEquipITEM = &g_pAVATAR->m_Inventory.m_ItemLIST[ nEquipInvIDX ];

		short nInvIDX = g_pAVATAR->Add_ITEM( *pEquipITEM );
		// 장비 탈거...
		if ( nInvIDX > 0 ) {
			m_pSendPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

			m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = nEquipInvIDX;
			m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM.m_dwITEM = 0;

			m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX =  nInvIDX;
			m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = *pEquipITEM;
		} // else 빈 인벤토리가 없어서 장비를 벗을수 없다...
	} else {
		m_pSendPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

		// 장비 교환.
		m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = nWeaponInvIDX;
		m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = g_pAVATAR->m_Inventory.m_ItemLIST[ nEquipInvIDX ];

		m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = nEquipInvIDX;
		m_pSendPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = g_pAVATAR->m_Inventory.m_ItemLIST[ nWeaponInvIDX ];
	}

	if ( m_pSendPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ) {
		m_pSendPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + 2 * sizeof( tag_SET_INVITEM );

		this->Send_PACKET( m_pSendPacket );

		m_pSendPacket->m_HEADER.m_wType = GSV_EQUIP_ITEM;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_EQUIP_ITEM);
		m_pSendPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX  = g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );
		m_pSendPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex = nEquipInvIDX;
		m_pSendPacket->m_gsv_EQUIP_ITEM.m_nItemNO	  = g_pAVATAR->m_Inventory.m_ItemLIST[ nWeaponInvIDX ].GetItemNO();

	} else
		return;
#else
	if ( nWeaponInvIDX && EQUIP_IDX_WEAPON_L == nEquipInvIDX && g_pAVATAR->m_Inventory.m_ItemLIST[ EQUIP_IDX_WEAPON_R ].IsTwoHands() ) {
		// 양손 무기 장착 상태에서 왼손 무기를 등록할수 없다.
		return;
	}

	//----------------------------------------------------------------------------------------------
	/// 장비교체를 할수없는 상태 체크
	//----------------------------------------------------------------------------------------------

	/// 개인상점 상태.
	/*if( g_pAVATAR->IsPersonalStoreMode() )
		return;*/



	_ASSERT( nEquipInvIDX >= 1 && nEquipInvIDX < MAX_EQUIP_IDX );

	if ( 0 == nWeaponInvIDX ) {
		// 장비 탈거...
		_ASSERT( g_pAVATAR->m_Inventory.m_ItemLIST[ nEquipInvIDX ].GetTYPE() && 
			     g_pAVATAR->m_Inventory.m_ItemLIST[ nEquipInvIDX ].GetTYPE() < ITEM_TYPE_USE );
	} else {
		_ASSERT( nWeaponInvIDX >= MAX_EQUIP_IDX && nWeaponInvIDX < MAX_EQUIP_IDX+INVENTORY_PAGE_SIZE );

		_ASSERT( g_pAVATAR->m_Inventory.m_ItemLIST[ nWeaponInvIDX ].GetTYPE() && 
			     g_pAVATAR->m_Inventory.m_ItemLIST[ nWeaponInvIDX ].GetTYPE() < ITEM_TYPE_USE );
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_EQUIP_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_EQUIP_ITEM );
	m_pSendPacket->m_cli_EQUIP_ITEM.m_nEquipInvIDX  = nEquipInvIDX;
	m_pSendPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX = nWeaponInvIDX;
#endif

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
// 필드에 아이템을 떨어 뜨린다...
// 아이템이 돈일 경우 nInventoryIndex == 0 !!!
void CSendPACKET::Send_cli_DROP_ITEM ( short nInventoryIndex, int iQuantity )
{
#ifdef	__VIRTUAL_SERVER	
	m_pSendPacket->m_HEADER.m_wType = GSV_ADD_FIELDITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_ADD_FIELDITEM );
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX		= g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );;
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME		= 2000;
	
	D3DXVECTOR3 PosCur = g_pAVATAR->Get_CurPOS();
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_PosCUR.x = PosCur.x;
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_PosCUR.y = PosCur.y;
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_ITEM = g_pAVATAR->m_Inventory.LST_GetITEM( nInventoryIndex );
	m_pSendPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX = nInventoryIndex;	
	
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_DROP_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_DROP_ITEM );
	m_pSendPacket->m_cli_DROP_ITEM.m_btInventoryIndex = (BYTE) nInventoryIndex;
	m_pSendPacket->m_cli_DROP_ITEM.m_uiQuantity		  = iQuantity;
#endif
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
bool CSendPACKET::Send_cli_GET_FIELDITEM_REQ( CGameOBJ *pUSER, int iServerObject )
{
	if( !pUSER )
		return false;

	
	if(pUSER->Get_TYPE() == OBJ_CART||pUSER->Get_TYPE() ==OBJ_CGEAR) //카트이거나 캐슬기어일경우
	{
		if(((CObjCART*)pUSER)->GetParent() != g_pAVATAR) // 드라이버가 내가 아니면..
			return true;
	}
	else if(pUSER->Get_TYPE() != OBJ_USER||pUSER != g_pAVATAR)  //유저가 아니거나 내가 아니면.. 
	{
		return true;
	}



	CObjITEM *pITEM = g_pObjMGR->Get_ClientItemOBJ( iServerObject );

	if( pITEM && pUSER )
	{
		if ( !pUSER->IsA( OBJ_USER ) && !pUSER->IsA( OBJ_CART) )	
		{
			if ( pITEM->m_wOwnerServerObjIDX == g_pObjMGR->Get_ServerObjectIndex(  pUSER->m_nIndex ) ) 
				return true;

			return false;
		}
	}

	
	if ( pITEM ) {
#ifdef	__VIRTUAL_SERVER
		m_pSendPacket->m_HEADER.m_wType = GSV_GET_FIELDITEM_REPLY;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_GET_FIELDITEM_REPLY );

		m_pSendPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX   = iServerObject;
		m_pSendPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult         = REPLY_GET_FIELDITEM_REPLY_OK;
		m_pSendPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO = 0;

		memcpy( &m_pSendPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM ,&(pITEM->m_ITEM), sizeof( tagITEM ) );
#else
		DWORD dwPassTIME = g_GameDATA.GetGameTime() - pITEM->m_dwCreatedTIME;
		if ( pITEM->m_wOwnerServerObjIDX &&  (int)(pITEM->m_wRemainTIME -  dwPassTIME) >  62 * 1000 ) {
			// 획득 권한이 있는지 조사...
			if ( pITEM->m_wOwnerServerObjIDX != g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex ) ) 
			{
				g_itMGR.AppendChatMsg( STR_NOTIFY_02, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
		}

		m_pSendPacket->m_HEADER.m_wType = CLI_GET_FIELDITEM_REQ;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_GET_FIELDITEM_REQ );

		m_pSendPacket->m_cli_GET_FIELDITEM_REQ.m_wServerItemIDX  = iServerObject;
#endif

		g_CommandFilter.SetPrevCommand( NULL );

	

		
		

		this->Send_PACKET( m_pSendPacket );

		return true;
	}
	return false;
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_TELEPORT_REQ( CGameOBJ *pUSER, short nWarpIDX)
{

	if(CCountry::GetSingleton().IsJapan())
	{
		int iZoneNum = TELEPORT_ZONE(nWarpIDX);
		if( ZONE_PVP_STATE( iZoneNum ) == 11 )
		{
			if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
			{
				g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CLANAGIT );
				return;
			}        
		}
		if( ZONE_PLANET_NO(g_pTerrain->GetZoneNO()) != ZONE_PLANET_NO(iZoneNum) )
		{
			if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
			{
				g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET );
				return;
			} 
		}

		if( ZONE_IS_UNDERGROUND(iZoneNum) )
		{
			if( ZONE_PLANET_NO(g_pTerrain->GetZoneNO()) == PLANET_JUNON )
			{
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
				{
					g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ENTER_DUNGEON );
					return;
				} 
			}
			else
			{
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_DUNGEON_ADV) )
				{
					g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ENTER_DUNGEON );
					return;
				} 
			}			
		}
	}
	

	if ( !pUSER->IsA( OBJ_USER ) )	return;


#ifdef	__VIRTUAL_SERVER
	m_pSendPacket->m_HEADER.m_wType = GSV_TELEPORT_REPLY;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_TELEPORT_REPLY );

	m_pSendPacket->m_gsv_TELEPORT_REPLY.m_wObjectIDX  = g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO = TELEPORT_ZONE( nWarpIDX );
	Packet_AppendString (m_pSendPacket, TELEPORT_EVENT_POS(nWarpIDX) );
#else
	g_pNet->m_bWarping = true;

	m_pSendPacket->m_HEADER.m_wType = CLI_TELEPORT_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_TELEPORT_REQ );

	m_pSendPacket->m_cli_TELEPORT_REQ.m_nWarpIDX = nWarpIDX;
#endif
	m_pSendPacket->m_cli_TELEPORT_REQ.m_PosCUR.x = g_pAVATAR->Get_CurXPOS();
	m_pSendPacket->m_cli_TELEPORT_REQ.m_PosCUR.y = g_pAVATAR->Get_CurYPOS();
	
	this->Send_PACKET( m_pSendPacket );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SET_HOTICON (BYTE btListIDX, tagHotICON HotICON)
{
#ifdef	__VIRTUAL_SERVER
	;
	return;
	;
#else
	if ( g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_SET_HOTICON ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_HOTICON );

	m_pSendPacket->m_cli_SET_HOTICON.m_btListIDX = btListIDX;
	m_pSendPacket->m_cli_SET_HOTICON.m_HotICON   = HotICON;
#endif
	this->Send_PACKET( m_pSendPacket );
}

/*
void CSendPACKET::Send_cli_CLICK_HOTICON (BYTE btListIDX)
{
	/// 등록되어 있는지 점검..
	if ( g_pAVATAR->m_HotICONS.m_IconLIST[ btListIDX ].m_cType == 0 )
		return;

#ifdef	__VIRTUAL_SERVER
	;
	return;
	;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_HOTICON ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_HOTICON );

	m_pSendPacket->m_cli_CLICK_HOTICON.m_btListIDX = btListIDX;
#endif
	this->Send_PACKET( m_pSendPacket );
}
*/

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_USE_BPOINT_REQ (BYTE btAbilityTYPE)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_USE_BPOINT_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_USE_BPOINT_REQ );
	m_pSendPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO = btAbilityTYPE;

#endif
	this->Send_PACKET( m_pSendPacket );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SKILL_LEVELUP_REQ (BYTE btSkillSLOT, short nNextLevelSkillIDX)
{
	short nCurLevelSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[ btSkillSLOT ];

	if ( 0 == nNextLevelSkillIDX ) {
		/// 일반적으로 다음 레벨의 스킬이 현재 레벨 스킬의 다음 라인에 와야 하지만
		/// 그렇지 않을경우 클라이언트에서 stb를 뒤쳐서 다음 레벨의 스킬 인덱스를 얻어야 한다.
		nNextLevelSkillIDX = nCurLevelSkillIDX + 1;
	}

	if ( SKILL_1LEV_INDEX( nCurLevelSkillIDX) != SKILL_1LEV_INDEX( nNextLevelSkillIDX ) ||
		 SKILL_LEVEL( nCurLevelSkillIDX )+1   != SKILL_LEVEL( nNextLevelSkillIDX ) ) {
		_ASSERT( 0 );
		return;
	}

#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SKILL_LEVELUP_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SKILL_LEVELUP_REQ );
	m_pSendPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT = btSkillSLOT;					/// 현재 레벨업을 할 스킬이 있는 슬롯
	m_pSendPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX = nNextLevelSkillIDX;	/// 레벨업 하려는 스킬번호.
#endif
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_P_STORE_OPEN ( BYTE btSellItemCount,BYTE btBuyItemCount, tagPS_ITEM* SellItems, tagPS_ITEM* BuyItems, char* pszTitle )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_P_STORE_OPEN;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_P_STORE_OPEN ) + 
		btSellItemCount * sizeof( tagPS_ITEM ) +
		btBuyItemCount * sizeof( tagPS_ITEM );

	m_pSendPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT = btSellItemCount;
	m_pSendPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT = btBuyItemCount;

	assert( btSellItemCount > 0 || btBuyItemCount > 0 );
	if( btSellItemCount > 0 || btBuyItemCount > 0 )
	{
		for( int i = 0; i < btSellItemCount; ++i  )
			memcpy(  &m_pSendPacket->m_cli_P_STORE_OPEN.m_ITEMs[i], &SellItems[i], sizeof( tagPS_ITEM ) );

		for( int i = btSellItemCount; i < btSellItemCount + btBuyItemCount; ++i )
			memcpy(  &m_pSendPacket->m_cli_P_STORE_OPEN.m_ITEMs[i], &BuyItems[i - btSellItemCount], sizeof( tagPS_ITEM ) );

		Packet_AppendString( m_pSendPacket, pszTitle );
		Send_PACKET( m_pSendPacket );
	}
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_P_STORE_CLOSE (void)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_P_STORE_CLOSE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_P_STORE_CLOSE );
	Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_P_STORE_LIST_REQ ( WORD wStoreObjectIDX )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_P_STORE_LIST_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_P_STORE_LIST_REQ );
	assert( wStoreObjectIDX );
	m_pSendPacket->m_cli_P_STORE_LIST_REQ.m_wStoreObjectIDX = wStoreObjectIDX;
	Send_PACKET( m_pSendPacket );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_P_STORE_SELL_REQ( WORD wStoreObjectIDX, int iItemCount, tagSELL_ITEM*	pSellItems )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_P_STORE_SELL_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_P_STORE_SELL_REQ ) + iItemCount * sizeof( tagSELL_ITEM );
	m_pSendPacket->m_cli_P_STORE_SELL_REQ.m_wStoreObjectIDX = wStoreObjectIDX;
	m_pSendPacket->m_cli_P_STORE_SELL_REQ.m_btItemCNT = iItemCount;
	assert( iItemCount );
	if( iItemCount > 0 )
	{
		for( int i = 0 ; i < iItemCount; ++i )
			memcpy( &m_pSendPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[i], &pSellItems[i], sizeof(tagSELL_ITEM));
			
		Send_PACKET( m_pSendPacket );
	}
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_P_STORE_BUY_REQ (WORD wStoreObjectIDX, int iItemCount, tagPS_SLOT_ITEM* pBuyItems)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_P_STORE_BUY_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_P_STORE_BUY_REQ ) + iItemCount * sizeof( tagPS_SLOT_ITEM );
	m_pSendPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX	= wStoreObjectIDX;
	m_pSendPacket->m_cli_P_STORE_BUY_REQ.m_btItemCNT		= iItemCount;

	assert( iItemCount );
	if( iItemCount > 0 )
	{
		for( int i = 0 ; i < iItemCount; ++i )
			memcpy( &m_pSendPacket->m_cli_P_STORE_BUY_REQ.m_BuyITEMs[i], &pBuyItems[i], sizeof(tagPS_SLOT_ITEM));

		Send_PACKET( m_pSendPacket );
	}
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SELF_SKILL (BYTE btSkillSLOT)
{
	/*
	short nNeedMP = 10+ (nSkillIDX/2)*2;
	if ( g_pAVATAR->Get_MP() < nNeedMP )
		return;
	*/
#ifdef	__VIRTUAL_SERVER
	//g_pAVATAR->Sub_MP( nNeedMP );

	m_pSendPacket->m_HEADER.m_wType = GSV_SELF_SKILL;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SELF_SKILL );

	m_pSendPacket->m_gsv_SELF_SKILL.m_wSourObjIDX = g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );
	m_pSendPacket->m_gsv_SELF_SKILL.m_nSkillIDX   = g_pAVATAR->m_Skills.m_nSkillINDEX[ btSkillSLOT ];
#else
	if ( g_pNet->m_bWarping )
		return;

	g_CommandFilter.SetPrevCommand( NULL );

	/// 이전명령 필터링...
	if( !g_CommandFilter.CanSendSelfSkillCommand( btSkillSLOT ) )
		return;


	//--------------------------------------------------------------------------------------
	/// 스킬 연타 방지..
	//--------------------------------------------------------------------------------------
	if( CPreventDuplicatedCommand::GetSingleton().CanSendSelfSkillCommand( btSkillSLOT ) )
	{
		CPreventDuplicatedCommand::GetSingleton().PushSelfSkillCommand( btSkillSLOT );
	}else
		return;



	m_pSendPacket->m_HEADER.m_wType = CLI_SELF_SKILL;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SELF_SKILL );

	m_pSendPacket->m_cli_SELF_SKILL.m_btSkillSLOT = btSkillSLOT;
#endif

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_TARGET_SKILL (int iClientTarget, BYTE btSkillSLOT)
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ATTACK );
			return;
		}        
	}

#ifdef	__VIRTUAL_SERVER
		m_pSendPacket->m_HEADER.m_wType = GSV_TARGET_SKILL;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( gsv_TARGET_SKILL );

		m_pSendPacket->m_gsv_TARGET_SKILL.m_wSourObjIDX = g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex );
		m_pSendPacket->m_gsv_TARGET_SKILL.m_wDestObjIDX = g_pObjMGR->Get_ServerObjectIndex(  iClientTarget );
		m_pSendPacket->m_gsv_TARGET_SKILL.m_nSkillIDX   = g_pAVATAR->m_Skills.m_nSkillINDEX[ btSkillSLOT ];
#else
		
		g_CommandFilter.SetPrevCommand( NULL );
		/// 이전명령 필터링...
		if( !g_CommandFilter.CanSendTargetSkillCommand( iClientTarget, btSkillSLOT ) )
			return;


		//--------------------------------------------------------------------------------------
		/// 스킬 연타 방지..
		//--------------------------------------------------------------------------------------
		if( CPreventDuplicatedCommand::GetSingleton().CanSendTargetSkillCommand( iClientTarget, btSkillSLOT ) )
		{
			CPreventDuplicatedCommand::GetSingleton().PushTargetSkillCommand( iClientTarget, btSkillSLOT );
		}else		
			return;



		if ( g_pNet->m_bWarping )
			return;

		m_pSendPacket->m_HEADER.m_wType = CLI_TARGET_SKILL;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_TARGET_SKILL );

		m_pSendPacket->m_cli_TARGET_SKILL.m_wDestObjIDX = g_pObjMGR->Get_ServerObjectIndex(  iClientTarget );
		m_pSendPacket->m_cli_TARGET_SKILL.m_btSkillSLOT	= btSkillSLOT;
#endif
		Log_String( LOG_NORMAL, "Send Target Skill[%d]\n", g_pAVATAR->m_Skills.m_nSkillINDEX[ btSkillSLOT ] );
		this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_POSITION_SKILL (D3DVECTOR &PosTO, BYTE btSkillSLOT)
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ATTACK );
			return;
		}        
	}

#ifdef	__VIRTUAL_SERVER
		return;
#else
		if ( g_pNet->m_bWarping )
			return;

		g_CommandFilter.SetPrevCommand( NULL );
		/// 이전명령 필터링...
		if( !g_CommandFilter.CanSendPositionSkillCommand( PosTO, btSkillSLOT ) )
			return;


		//--------------------------------------------------------------------------------------
		/// 스킬 연타 방지..
		//--------------------------------------------------------------------------------------
		if( CPreventDuplicatedCommand::GetSingleton().CanSendPositionSkillCommand( PosTO, btSkillSLOT ) )
		{
			CPreventDuplicatedCommand::GetSingleton().PushPositionSkillCommand( PosTO, btSkillSLOT );
		}else
			return;



		m_pSendPacket->m_HEADER.m_wType = CLI_POSITION_SKILL;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_POSITION_SKILL );

		m_pSendPacket->m_cli_POSITION_SKILL.m_PosTARGET.x = PosTO.x;
		m_pSendPacket->m_cli_POSITION_SKILL.m_PosTARGET.y = PosTO.y;
		m_pSendPacket->m_cli_POSITION_SKILL.m_btSkillSLOT = btSkillSLOT;
#endif
		this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_QUEST_REQ (BYTE btReqTYPE, BYTE btQuestSLOT, int iQuestID, char *szQuestTriggerName)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_QUEST_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_QUEST_REQ );

	m_pSendPacket->m_cli_QUEST_REQ.m_btTYPE	= btReqTYPE;
	m_pSendPacket->m_cli_QUEST_REQ.m_btQuestSLOT = btQuestSLOT;

	if ( szQuestTriggerName ) {
		m_pSendPacket->m_cli_QUEST_REQ.m_TriggerHash = StrToHashKey( szQuestTriggerName );
	} else {
		m_pSendPacket->m_cli_QUEST_REQ.m_iQuestID = iQuestID;
	}
#endif

	this->Send_PACKET( m_pSendPacket );
}
/*
void CSendPACKET::Send_cli_ADD_QUEST (BYTE btQuestIDX, int iQuestID)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_ADD_QUEST;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ADD_QUEST );

	m_pSendPacket->m_cli_ADD_QUEST.m_btQuestSLOT = btQuestIDX;
	m_pSendPacket->m_cli_ADD_QUEST.m_iQuestID   = iQuestID;
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_DEL_QUEST (BYTE btQuestIDX, int iQuestID)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_DEL_QUEST;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_DEL_QUEST );

	m_pSendPacket->m_cli_DEL_QUEST.m_btQuestSLOT = btQuestIDX;
	m_pSendPacket->m_cli_DEL_QUEST.m_iQuestID   = iQuestID;
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_QUEST_DATA_REQ (BYTE btQuestIDX)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_QUEST_DATA_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_QUEST_DATA_REQ );

	m_pSendPacket->m_cli_QUEST_DATA_REQ.m_btQuestSLOT = btQuestIDX;
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_SET_QUEST_VAR (BYTE btQuestIDX, BYTE btVarNO, int iValue)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_QUEST_VAR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_QUEST_VAR );

	m_pSendPacket->m_cli_SET_QUEST_VAR.m_btQuestSLOT = btQuestIDX;
	m_pSendPacket->m_cli_SET_QUEST_VAR.m_btVarNO = btVarNO;
	m_pSendPacket->m_cli_SET_QUEST_VAR.m_iValue = iValue;
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_SET_QUEST_SWITCH (BYTE btQuestIDX)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_QUEST_SWITCH;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_QUEST_SWITCH );

	m_pSendPacket->m_cli_SET_QUEST_SWITCH.m_btQuestSLOT = btQuestIDX;
	m_pSendPacket->m_cli_SET_QUEST_SWITCH.m_dwSWITCHES = g_pAVATAR->m_Quests.m_QUEST[ btQuestIDX ].Get_SWITCHs();
#endif

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_SET_QUEST_FAMEnPROGRESS (BYTE btUnionIDX, char cToAddFAME, char cToADDProgress)
{
#ifdef	__VIRTUAL_SERVER
	return;
#else
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_QUEST_FAMEnPROGRESS;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_QUEST_FAMEnPROGRESS );

	m_pSendPacket->m_cli_SET_QUEST_FAMEnPROGRESS.m_btUnionIDX = btUnionIDX;
	m_pSendPacket->m_cli_SET_QUEST_FAMEnPROGRESS.m_nToAdd_QuestFameOfUnion = cToAddFAME;
	m_pSendPacket->m_cli_SET_QUEST_FAMEnPROGRESS.m_nToAdd_QuestProcOfUnion = cToADDProgress;
#endif

	this->Send_PACKET( m_pSendPacket );
}
*/

bool CSendPACKET::Send_cli_TRADE_P2P( WORD wServerIdx, BYTE btResult ,char cSlotIdx )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_TRADE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_TRADE );
			return false;
		}        
	}


	m_pSendPacket->m_HEADER.m_wType = CLI_TRADE_P2P;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_TRADE_P2P );
	m_pSendPacket->m_cli_TRADE_P2P.m_btRESULT	= btResult;
	m_pSendPacket->m_cli_TRADE_P2P.m_wObjectIDX = wServerIdx;
	m_pSendPacket->m_cli_TRADE_P2P.m_cTradeSLOT = cSlotIdx;
	this->Send_PACKET( m_pSendPacket );
	return true;
}

void CSendPACKET::Send_cli_TRADE_P2P_ITEM( char	cTradeSLOT,short nInventoryIndex, DWORD	iQuantity )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_TRADE_P2P_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_TRADE_P2P_ITEM );

	m_pSendPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT		= cTradeSLOT;
	m_pSendPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex	= nInventoryIndex;
	m_pSendPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity		= iQuantity;

	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_PARTY_REQ(BYTE btRequest, DWORD dwDestIDXorTAG )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_INVITE_PARTY );
			return;
		}        
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_PARTY_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_PARTY_REQ );

	m_pSendPacket->m_cli_PARTY_REQ.m_btREQUEST = btRequest;
	m_pSendPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG = dwDestIDXorTAG;
	this->Send_PACKET( m_pSendPacket );

}
void CSendPACKET::Send_cli_PARTY_REPLY(BYTE btRequest, DWORD dwDestIDXorTAG )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_PARTY_REPLY;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_PARTY_REPLY );

	m_pSendPacket->m_cli_PARTY_REPLY.m_btREPLY = btRequest;
	m_pSendPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG = dwDestIDXorTAG;
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
///제조 관련
void CSendPACKET::Send_cli_CREATE_ITEM_REQ( BYTE btSkillSLOT, char cTargetItemTYPE, short nTargetItemNO, short* pnUseItemINV )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CREATE_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CREATE_ITEM_REQ );
	m_pSendPacket->m_cli_CREATE_ITEM_REQ.m_btSkillSLOT = btSkillSLOT;
	m_pSendPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE = cTargetItemTYPE;
	m_pSendPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO  = nTargetItemNO;
	memcpy( m_pSendPacket->m_cli_CREATE_ITEM_REQ.m_nUseItemINV, pnUseItemINV , sizeof( short ) * CREATE_ITEM_STEP );
	this->Send_PACKET( m_pSendPacket );
}
///제조 결과 Animaition후에 얻은 경험치를 달라고 요청한다.
//void CSendPACKET::Send_cli_CREATE_ITEM_EXP_REQ()
//{
//	m_pSendPacket->m_HEADER.m_wType = CLI_CREATE_ITEM_EXP_REQ;
//	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CREATE_ITEM_EXP_REQ );
//	this->Send_PACKET( m_pSendPacket );
//}


void CSendPACKET::Send_cli_ITEM_RESULT_REPORT( BYTE btREPORT, BYTE btItemType, short nItemNo )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_ITEM_RESULT_REPORT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ITEM_RESULT_REPORT );
	m_pSendPacket->m_cli_ITEM_RESULT_REPORT.m_btREPORT   = btREPORT;
	m_pSendPacket->m_cli_ITEM_RESULT_REPORT.m_btItemType = btItemType;
	m_pSendPacket->m_cli_ITEM_RESULT_REPORT.m_nItemNO    = nItemNo;
	Send_PACKET( m_pSendPacket );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_MOVE_ITEM( BYTE btMoveTYPE,BYTE btFromIDX,tagITEM& MoveITEM,bool bPlatinum  )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_STOCK_SPACE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK );
			return;
		}		
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_MOVE_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MOVE_ITEM );
	m_pSendPacket->m_cli_MOVE_ITEM.m_btMoveTYPE = btMoveTYPE;
	m_pSendPacket->m_cli_MOVE_ITEM.m_btFromIDX  = btFromIDX;
	m_pSendPacket->m_cli_MOVE_ITEM.m_MoveITEM   = MoveITEM;

	if( bPlatinum && btMoveTYPE == MOVE_ITEM_TYPE_INV2BANK )
		m_pSendPacket->m_cli_MOVE_ITEM.m_btUseSpecialTAB = 1;
	else
		m_pSendPacket->m_cli_MOVE_ITEM.m_btUseSpecialTAB = 0;

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_BANK_LIST_REQ( BYTE btREQ, char* pszPassword )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_BANK_LIST_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_BANK_LIST_REQ );
	m_pSendPacket->m_cli_BANK_LIST_REQ.m_btREQ = btREQ;
	if( pszPassword )
		Packet_AppendString( m_pSendPacket, pszPassword );
	this->Send_PACKET( m_pSendPacket );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SET_BULLET( BYTE btShotType, short nInvenIdx )
{
	/// 탈착시 nInvenIdx == 0;
	m_pSendPacket->m_HEADER.m_wType = CLI_SET_BULLET;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_BULLET );
	m_pSendPacket->m_cli_SET_BULLET.m_wShotTYPE = btShotType;
	m_pSendPacket->m_cli_SET_BULLET.m_wInventoryIDX = nInvenIdx;
	this->Send_PACKET( m_pSendPacket );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_ASSEMBLE_RIDE_ITEM( short nPartIdx, short nInvenIdx )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_ASSEMBLE_RIDE_ITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_ASSEMBLE_RIDE_ITEM );
	m_pSendPacket->m_cli_ASSEMBLE_RIDE_ITEM.m_nRidingInvIDX		= nInvenIdx;
	m_pSendPacket->m_cli_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX	= nPartIdx; 
	this->Send_PACKET( m_pSendPacket );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_SET_WEIGHT_RATE( BYTE btWeightRate )
{
	if ( g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_SET_WEIGHT_RATE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_WEIGHT_RATE );

	if( g_GameDATA.m_bNoWeight )
		m_pSendPacket->m_cli_SET_WEIGHT_RATE.m_btWeightRate = 0;
	else
		m_pSendPacket->m_cli_SET_WEIGHT_RATE.m_btWeightRate = btWeightRate;

	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_SET_WISHITEM( int iIndex, tagITEM& Item )
{
	if( iIndex < 0 || iIndex >= MAX_WISH_ITEMS )
	{
		assert( iIndex >= 0 && iIndex < MAX_WISH_ITEMS );
		return;
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_SET_WISHITEM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SET_WISHITEM );

	m_pSendPacket->m_cli_SET_WISHITEM.m_ITEM = Item;
	m_pSendPacket->m_cli_SET_WISHITEM.m_btWishSLOT = (BYTE)iIndex;

	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_CHANNEL_LIST_REQ( int iServerID )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHANNEL_LIST_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHANNEL_LIST_REQ );
	m_pSendPacket->m_cli_SELECT_SERVER.m_dwServerID = (DWORD)iServerID;

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
// 친구 추가 요청
void CSendPACKET::Send_cli_MCMD_APPEND_REQ (char *szName)
{

	m_pSendPacket->m_HEADER.m_wType = CLI_MESSENGER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MCMD_APPEND_REQ );

	m_pSendPacket->m_cli_MCMD_APPEND_REQ.m_btCMD = MSGR_CMD_APPEND_REQ;
	Packet_AppendString( m_pSendPacket, szName );

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
// 친구 추가 요청에 대한 응답
void CSendPACKET::Send_cli_MCMD_APPEND_REPLY (BYTE btCMD, WORD wUserIDX, char* pszName )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MESSENGER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MCMD_APPEND_REPLY );
	m_pSendPacket->m_cli_MCMD_APPEND_REPLY.m_btCMD = btCMD;
	m_pSendPacket->m_cli_MCMD_APPEND_REPLY.m_wUserIDX = wUserIDX;
	if( pszName )
		Packet_AppendString( m_pSendPacket, pszName );

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
// 대상을 삭제, 수신거부 상태로 변경.
void CSendPACKET::Send_cli_MCMD_TAG (BYTE btCMD, DWORD dwUserTAG)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MESSENGER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MCMD_TAG );
	m_pSendPacket->m_cli_MCMD_TAG.m_btCMD		= btCMD;
	m_pSendPacket->m_cli_MCMD_TAG.m_dwUserTAG	= dwUserTAG;

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
// 나의 상태 변경.
void CSendPACKET::Send_cli_MCMD_STATUS_REQ (BYTE btStatus)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MESSENGER;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MCMD_STATUS_REQ );
	m_pSendPacket->m_cli_MCMD_STATUS_REQ.m_btCMD = MSGR_CMD_CHANGE_STATUS;
	m_pSendPacket->m_cli_MCMD_STATUS_REQ.m_btStatus = btStatus;

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
// CHAT_REQ_LEFT
void CSendPACKET::Send_tag_CHAT_HEADER (BYTE btRoomCMD)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( tag_CHAT_HEADER );	
	
	m_pSendPacket->m_tag_CHAT_HEADER.m_btCMD = btRoomCMD;
	
	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHAT_ROOM_MAKE (BYTE btRoomTYPE, BYTE btMaxUSER, char *szRoomTitle, char *szPasswd)
{	
	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAT_ROOM_MAKE );	
	
	m_pSendPacket->m_cli_CHAT_ROOM_MAKE.m_btCMD		 = CHAT_REQ_MAKE;
	m_pSendPacket->m_cli_CHAT_ROOM_MAKE.m_btRoomTYPE = btRoomTYPE;
	m_pSendPacket->m_cli_CHAT_ROOM_MAKE.m_btMaxUSER	 = btMaxUSER;
	
	Packet_AppendString( m_pSendPacket, szRoomTitle );
	if ( szPasswd )
		Packet_AppendString( m_pSendPacket, szPasswd );

	this->Send_PACKET( m_pSendPacket, true );
}


//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHAT_ROOM_JOIN (BYTE btRoomTYPE, WORD wRoomID, char *szPasswd)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAT_ROOM_JOIN );	
	
	m_pSendPacket->m_cli_CHAT_ROOM_JOIN.m_btCMD		 = CHAT_REQ_JOIN;
	m_pSendPacket->m_cli_CHAT_ROOM_JOIN.m_btRoomTYPE = btRoomTYPE;
	m_pSendPacket->m_cli_CHAT_ROOM_JOIN.m_wRoomID	 = wRoomID;
	
	if ( szPasswd )
		Packet_AppendString( m_pSendPacket, szPasswd );

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHAT_ROOM_KICK ( t_HASHKEY HashUSER )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAT_ROOM_KICK );	
	
	m_pSendPacket->m_tag_CHAT_HEADER.m_btCMD = CHAT_REQ_KICK_USER;
	m_pSendPacket->m_cli_CHAT_ROOM_KICK.m_HashUSER = HashUSER;

	this->Send_PACKET( m_pSendPacket, true );
}

//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHAT_ROOM_LIST (BYTE btRoomTYPE, WORD wFromRoomID)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHAT_ROOM_LIST );	
	
	m_pSendPacket->m_cli_CHAT_ROOM_LIST.m_btCMD		  = CHAT_REQ_ROOM_LIST;
	m_pSendPacket->m_cli_CHAT_ROOM_LIST.m_btRoomTYPE  = btRoomTYPE;
	m_pSendPacket->m_cli_CHAT_ROOM_LIST.m_wFromRoomID = wFromRoomID;
	
	this->Send_PACKET( m_pSendPacket, true );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CHATROOM_MSG( char* pszMsg )
{
	assert( pszMsg );
	if( pszMsg == NULL ) return;
	assert( strlen( pszMsg ) >= 1 );
	if( strlen( pszMsg ) < 1 ) return;

	m_pSendPacket->m_HEADER.m_wType = CLI_CHATROOM_MSG;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CHATROOM_MSG );
	Packet_AppendString( m_pSendPacket, pszMsg );
	this->Send_PACKET( m_pSendPacket, true );

}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_MESSENGER_CHAT( DWORD dwUserTag, char* pszMsg )
{
	assert( pszMsg );
	if( pszMsg == NULL ) return;

	m_pSendPacket->m_HEADER.m_wType = CLI_MESSENGER_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MESSENGER_CHAT );

	m_pSendPacket->m_cli_MESSENGER_CHAT.m_dwUserTAG = dwUserTag;
	Packet_AppendString( m_pSendPacket, pszMsg );
	this->Send_PACKET( m_pSendPacket, true );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_MEMO( char* pszName, char* pszMemo )
{
	assert( pszName && pszMemo );
	if( pszName && pszMemo )
	{
		int iStrLen = strlen( pszMemo );
		assert( iStrLen < 255 );
		if( iStrLen < 255 )
		{
			for (int iC=0; iC<iStrLen; iC++) {
				if ( pszMemo[iC] == '\'' ) pszMemo[iC] = ' ';	// 메모 내용에 '문자 올수 없음
			}

			m_pSendPacket->m_HEADER.m_wType = CLI_MEMO;
			m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MEMO );
			m_pSendPacket->m_cli_MEMO.m_btTYPE = MEMO_REQ_SEND;
			Packet_AppendString( m_pSendPacket, pszName );
			Packet_AppendString( m_pSendPacket, pszMemo );
			this->Send_PACKET( m_pSendPacket, true );
		}
	}
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_MEMO_REQ()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MEMO;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MEMO );
	m_pSendPacket->m_cli_MEMO.m_btTYPE = MEMO_REQ_CONTENTS;
	this->Send_PACKET( m_pSendPacket, true );
}
//-------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_MEMO_CNT_REQ()
{
	m_pSendPacket->m_HEADER.m_wType		= CLI_MEMO;
	m_pSendPacket->m_HEADER.m_nSize		= sizeof( cli_MEMO );
	m_pSendPacket->m_cli_MEMO.m_btTYPE	= MEMO_REQ_RECEIVED_CNT;
	this->Send_PACKET( m_pSendPacket, true );
}


//----------------------------------------------------------------------------------------------------	
/// @param btEquipInvNO : 재밍될 장비 슬롯 번호
/// @param btGemInvNO : 재밍에 사용할 보석의 슬롯 번호
/// @brief 재밍 재련관련
//----------------------------------------------------------------------------------------------------
void CSendPACKET::Send_cli_CRAFT_GEMMING_REQ( BYTE btEquipInvIDX, BYTE btGemInvIDX )
{
	m_pSendPacket->m_HEADER.m_wType		= CLI_CRAFT_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize		= sizeof( cli_CRAFT_GEMMING_REQ );

	m_pSendPacket->m_cli_CRAFT_GEMMING_REQ.m_btTYPE = CRAFT_GEMMING_REQ;
	m_pSendPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX = btEquipInvIDX;
	m_pSendPacket->m_cli_CRAFT_GEMMING_REQ.m_btJemInvIDX = btGemInvIDX;

	this->Send_PACKET( m_pSendPacket, false );
}	

//----------------------------------------------------------------------------------------------------	
/// @param nSkillSLOTorNpcIDX;					// 사용 스킬슬롯 번호또는 npc번호
/// @param btTargetInvIDX;						// 분리할 아이템 인벤 번호
/// @brief 재밍 재련관련
//----------------------------------------------------------------------------------------------------	
void CSendPACKET::Send_cli_CRAFT_BREAKUP_REQ( BYTE btType, short nSkillSLOTorNpcIDX, BYTE btTargetInvIDX )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CRAFT_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CRAFT_BREAKUP_REQ );

	m_pSendPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTYPE = btType;
	m_pSendPacket->m_cli_CRAFT_BREAKUP_REQ.m_nSkillSLOTorNpcIDX = nSkillSLOTorNpcIDX;
	m_pSendPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX	= btTargetInvIDX;

	Send_PACKET( m_pSendPacket );
}

//----------------------------------------------------------------------------------------------------	
/// @param btType								//
/// @param nSkillSLOTorNpcIDX;					// 사용 스킬슬롯 번호또는 npc번호
/// @param btTargetInvIDX;						// 분리할 아이템 인벤 번호
/// @param btUseItemINV[ UPGRADE_ITEM_STEP ];	// 재련시 소모할 아이템 인벤토리 번호
/// @brief 제련
//----------------------------------------------------------------------------------------------------	
void CSendPACKET::Send_cli_CRAFT_UPGRADE_REQ( BYTE btType, short nSkillSLOTorNpcIDX, BYTE btTargetInvIDX, BYTE btUseItemINV[ UPGRADE_ITEM_STEP ] )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CRAFT_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CRAFT_UPGRADE_REQ );

	m_pSendPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTYPE = btType;
	m_pSendPacket->m_cli_CRAFT_UPGRADE_REQ.m_nSkillSLOTorNpcIDX = nSkillSLOTorNpcIDX;
	m_pSendPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX = btTargetInvIDX;

	for( int i = 0 ; i < UPGRADE_ITEM_STEP; ++i )
		m_pSendPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[i] = btUseItemINV[i];

	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_USE_ITEM_TO_REPAIR( short nUsedItemInvenIdx, short nTargetItemInvenIdx )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_USE_ITEM_TO_REPAIR;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_USE_ITEM_TO_REPAIR );
	m_pSendPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX = nUsedItemInvenIdx;
	m_pSendPacket->m_cli_USE_ITEM_TO_REPAIR.m_nRepairTargetInvIDX = nTargetItemInvenIdx;
	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_REPAIR_FROM_NPC( WORD wNpcSvrIdx, short nTargetItemInvenIdx )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_REPAIR_FROM_NPC;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_REPAIR_FROM_NPC );
	m_pSendPacket->m_cli_REPAIR_FROM_NPC.m_wNPCObjIDX = wNpcSvrIdx;
	m_pSendPacket->m_cli_REPAIR_FROM_NPC.m_nRepairTargetInvIDX = nTargetItemInvenIdx;
	this->Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_PARTY_RULE( BYTE btRule )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_PARTY_RULE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_PARTY_RULE );
	m_pSendPacket->m_cli_PARTY_RULE.m_btPartyRUEL = btRule;
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_CHAR_CHANGE()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CHAR_CHANGE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_APPRAISAL_REQ( int iInventoryIndex )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_APPRAISAL_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_APPRAISAL_REQ );
	m_pSendPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex = iInventoryIndex;
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_CLAN_COMMAND( BYTE btCmd , char* pszMsg )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
	m_pSendPacket->m_cli_CLAN_COMMAND.m_btCMD = btCmd;
	
	if( pszMsg )
		Packet_AppendString( m_pSendPacket, pszMsg );

	Send_PACKET( m_pSendPacket ,true );
}

void CSendPACKET::Send_cli_CLAN_CREATE( WORD wMarkBack, WORD wMarkCenter, char* pszName, char* pszSlogan )
{
	assert( pszName );
	assert( pszSlogan );
	assert( wMarkCenter > 0 );
	assert( wMarkBack > 0 );
	if( pszName && pszSlogan && wMarkCenter && wMarkBack )
	{
		m_pSendPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
		m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_CREATE );
		m_pSendPacket->m_cli_CLAN_CREATE.m_btCMD = GCMD_CREATE;
		m_pSendPacket->m_cli_CLAN_CREATE.m_wMarkIDX[0] = wMarkBack;
		m_pSendPacket->m_cli_CLAN_CREATE.m_wMarkIDX[1] = wMarkCenter;
		Packet_AppendString( m_pSendPacket, pszName );
		Packet_AppendString( m_pSendPacket, pszSlogan );
		Send_PACKET( m_pSendPacket );
	}
}

void CSendPACKET::Send_cli_CLAN_CHAT( char *szMsg )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CLANCHAT );
			return;
		}        
	}


	assert( szMsg );
	if ( szMsg == NULL || g_pNet->m_bWarping )
		return;

	m_pSendPacket->m_HEADER.m_wType = CLI_CLAN_CHAT;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_CHAT );
	Packet_AppendString (m_pSendPacket, szMsg);
	this->Send_PACKET( m_pSendPacket, true );
}

void CSendPACKET::Send_cli_CLANMARK_REQ( int iClanID )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CLANMARK_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLANMARK_REQ );

	m_pSendPacket->m_cli_CLANMARK_REQ.m_dwClanID = iClanID;
	this->Send_PACKET( m_pSendPacket, true );
}

void CSendPACKET::Send_cli_CLANMARK_SET( unsigned char* pDATA, int iSize , WORD crc16)
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CLANMARK_SET;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLANMARK_SET );


	m_pSendPacket->m_cli_CLANMARK_SET.m_wMarkCRC16 = crc16;
	Packet_AppendData( m_pSendPacket, pDATA, iSize );
	this->Send_PACKET( m_pSendPacket, true );
}

void CSendPACKET::Send_cli_CLAN_MEMBER_JOBnLEV( short nLev, short nJob )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_MEMBER_JOBnLEV );
	m_pSendPacket->m_cli_CLAN_MEMBER_JOBnLEV.m_btCMD = GCMD_MEMBER_JOBnLEV;
	m_pSendPacket->m_cli_CLAN_MEMBER_JOBnLEV.m_nJOB = nJob;
	m_pSendPacket->m_cli_CLAN_MEMBER_JOBnLEV.m_nLEVEL = nLev;
	Send_PACKET( m_pSendPacket, true );

}

void CSendPACKET::Send_cli_MALL_ITEM_LIST_REQ()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MALL_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ );
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_LIST;
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_MAIL_ITEM_FIND_CHAR( char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return;
	m_pSendPacket->m_HEADER.m_wType = CLI_MALL_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ );
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_FIND_CHAR;
	Packet_AppendString( m_pSendPacket, pszName );
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_MALL_ITEM_GIVE( BYTE slotindex, char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return;
	m_pSendPacket->m_HEADER.m_wType = CLI_MALL_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ ) + 1;
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_GIVE;
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] = slotindex;
	Packet_AppendString( m_pSendPacket, pszName );
	Packet_AppendString( m_pSendPacket,"");
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_MALL_ITEM_BRING( BYTE slotindex, short nDupCount )
{
	m_pSendPacket->m_HEADER.m_wType = CLI_MALL_ITEM_REQ;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ ) + 1;
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_BRING;
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] = slotindex;
	m_pSendPacket->m_cli_MALL_ITEM_REQ.m_nDupCnt     = nDupCount;
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_CLANMARK_REG_TIME()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_CLANMARK_REG_TIME;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CLANMARK_REG_TIME );
	Send_PACKET( m_pSendPacket, true );
}

void CSendPACKET::Send_cli_MOVE_ZULY_INV2BANK( __int64 i64MoveZuly )
{
	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_STOCK_SPACE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK_MONEY );
			return;
		}        
	}


	m_pSendPacket->m_HEADER.m_wType = CLI_MOVE_ZULY;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MOVE_ZULY );
	m_pSendPacket->m_cli_MOVE_ZULY.m_btMoveTYPE = MOVE_ZULY_TYPE_INV2BANK;
	m_pSendPacket->m_cli_MOVE_ZULY.m_i64MoveZuly = i64MoveZuly;
	Send_PACKET( m_pSendPacket );
}

void CSendPACKET::Send_cli_MOVE_ZULY_BANK2INV( __int64 i64MoveZuly )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_STOCK_SPACE) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK_MONEY );
			return;
		}        
	}

	m_pSendPacket->m_HEADER.m_wType = CLI_MOVE_ZULY;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_MOVE_ZULY );
	m_pSendPacket->m_cli_MOVE_ZULY.m_btMoveTYPE = MOVE_ZULY_TYPE_BANK2INV;
	m_pSendPacket->m_cli_MOVE_ZULY.m_i64MoveZuly = i64MoveZuly;
	Send_PACKET( m_pSendPacket );
}

//2인승 카트.
void CSendPACKET::Send_cli_CART_RIDE( BYTE bType, WORD wOwnerObjIDX_, WORD wGuestObjIDX_ )
{
	
	m_pSendPacket->m_HEADER.m_wType = CLI_CART_RIDE;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_CART_RIDE );
	
	m_pSendPacket->m_cli_CART_RIDE.m_btType = bType;
	m_pSendPacket->m_cli_CART_RIDE.m_wOwnerObjIDX = wOwnerObjIDX_;
	m_pSendPacket->m_cli_CART_RIDE.m_wGuestObjIDX = wGuestObjIDX_;
	
	Send_PACKET( m_pSendPacket );
}


void  CSendPACKET::Send_cli_SCREEN_SHOT_TIME()
{
	m_pSendPacket->m_HEADER.m_wType = CLI_SCREEN_SHOT_TIME;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( cli_SCREEN_SHOT_TIME );
	m_pSendPacket->m_cli_SCREEN_SHOT_TIME.wCnt = 1;
	Send_PACKET( m_pSendPacket );
}

void  CSendPACKET::Send_cli_UPDATE_NAME( char* szName )
{
	m_pSendPacket->m_HEADER.m_wType = PXY_UPDATE_NAME;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( pxy_UPDATE_NAME );
	Packet_AppendString( m_pSendPacket, szName );

	Send_PACKET( m_pSendPacket );
}
void  CSendPACKET::Send_cli_SET_RIGHTS( DWORD dwRight )
{
	m_pSendPacket->m_HEADER.m_wType = PXY_SET_RIGHTS;
	m_pSendPacket->m_HEADER.m_nSize = sizeof( pxy_SET_RIGHTS );
	m_pSendPacket->m_pxy_SET_RIGHTS.wRIGHT = dwRight;

	Send_PACKET( m_pSendPacket );
}