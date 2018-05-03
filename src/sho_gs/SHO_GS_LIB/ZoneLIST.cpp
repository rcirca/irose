
#include "LIB_gsMAIN.h"
#include "ZoneLIST.h"
#include "IO_STB.h"
#include "GS_ThreadSQL.h"
#include "GS_SocketLSV.h"

//-------------------------------------------------------------------------------------------------

IMPLEMENT_INSTANCE( CZoneLIST )

CZoneLIST::CZoneLIST ()
{
	m_ppThreadZONE = NULL;
	m_pValidZONE = NULL;

	m_nZoneCNT = g_TblZONE.m_nDataCnt;

	m_pSectorSIZE	= new short[ m_nZoneCNT ];
	m_pValidZONE	= new bool [ m_nZoneCNT ];
	m_ppThreadZONE	= new CZoneTHREAD* [ m_nZoneCNT ];
	m_ppNpcLIST		= new CObjNPC*[ g_TblNPC.m_nDataCnt ];
}
//-------------------------------------------------------------------------------------------------
CZoneLIST::~CZoneLIST ()
{
	this->FreeZoneLIST ();

	t_HASHKEY	 HashKEY;
	tagEVENTPOS* pEventPOS;
	
	while( m_PosLIST.GetFirst( &HashKEY, &pEventPOS ) ) {
		SAFE_DELETE( pEventPOS );
		m_PosLIST.Delete( HashKEY );
	}

	CObjEVENT *pObjEVENT;
	while( m_EventOBJECT.GetFirst( &HashKEY, &pObjEVENT ) ) {
		// SAFE_DELETE( pEventPOS );
		m_EventOBJECT.Delete( HashKEY );
	}

	for (short nI=0; nI<m_nZoneCNT; nI++) {
		if ( m_ppThreadZONE[ nI ] ) {
			SAFE_DELETE( m_ppThreadZONE[ nI ] );
		}
	}

	SAFE_DELETE_ARRAY( m_ppThreadZONE );
	SAFE_DELETE_ARRAY( m_pValidZONE );
	SAFE_DELETE_ARRAY( m_ppNpcLIST );
	SAFE_DELETE_ARRAY( m_pSectorSIZE );

	m_nZoneCNT = 0;
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Set_WorldVAR (short nVarIDX, short nValue)
{
	CWorldVAR::Set_WorldVAR( nVarIDX, nValue );
	g_pSockLSV->Send_srv_SET_WORLD_VAR( nVarIDX, nValue );
}

//-------------------------------------------------------------------------------------------------
bool CZoneLIST::InitZoneLIST (char *szBaseDIR)
{
#ifdef	__STRESS_TEST
	return true;
#endif

	::ZeroMemory( m_pValidZONE,		sizeof(bool) * m_nZoneCNT );
	::ZeroMemory( m_ppThreadZONE,	sizeof(CZoneTHREAD*) * m_nZoneCNT );
	::ZeroMemory( m_ppNpcLIST,		sizeof(CObjNPC*) * g_TblNPC.m_nDataCnt );

    char *szZoneFILE;
    short nZ;

	for (nZ=1; nZ<m_nZoneCNT; nZ++) {
		this->m_pSectorSIZE[ nZ ] = ZONE_SECTOR_SIZE(nZ);
		if ( this->m_pSectorSIZE[ nZ ] < MIN_SECTOR_SIZE )
			this->m_pSectorSIZE[ nZ ] = MIN_SECTOR_SIZE;
		else
		if ( this->m_pSectorSIZE[ nZ ] > MAX_SECTOR_SIZE )
			this->m_pSectorSIZE[ nZ ] = MAX_SECTOR_SIZE;

        if ( NULL == ZONE_FILE( nZ ) ) {
            continue;
		}
#ifdef	__INC_WORLD
		//if ( 24 != nZ ) continue;
#endif
        szZoneFILE = CStr::Printf("%s%s", szBaseDIR, ZONE_FILE( nZ ));
        if ( !CUtil::Is_FileExist( szZoneFILE ) ) {
            continue;
        }
		m_pValidZONE[ nZ ] = true;

		// 로컬로 실행되는 존인가 ???
		if ( !CLIB_GameSRV::GetInstance()->IsLocalCheckedZONE(nZ) )
			continue;

		// TODO:: if ( nz is checked !!!
	    m_ppThreadZONE[ nZ ] = new CZoneTHREAD( true );
		if ( !m_ppThreadZONE[ nZ ]->Init ( szBaseDIR, nZ ) ) {
			// Invalid Zone !!!
			m_ppThreadZONE[ nZ ]->Free ();
			SAFE_DELETE( m_ppThreadZONE[ nZ ] );
		}
	}

	for (nZ=1; nZ<m_nZoneCNT; nZ++) {
		if ( m_ppThreadZONE[ nZ ] ) {
			g_pThreadSQL->IO_ZoneDATA( m_ppThreadZONE[ nZ ] );

			m_ppThreadZONE[ nZ ]->m_pListITEM = CLIB_GameSRV::ExeAPI()->AddZoneITEM( m_ppThreadZONE[ nZ ], nZ, ZONE_NAME(nZ) );
			// m_ppThreadZONE[ nZ ]->Resume ();
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::FreeZoneLIST ()
{
	for (short nI=0; nI<g_TblNPC.m_nDataCnt; nI++) {
		if ( m_ppNpcLIST[ nI ] )
			g_pThreadSQL->IO_NpcObjDATA( m_ppNpcLIST[ nI ], true );
	}

	::ZeroMemory( m_pValidZONE,		sizeof(bool) * m_nZoneCNT );

	bool bAllClosed;
	do {
		bAllClosed=true;
		for (short nI=0; nI<m_nZoneCNT; nI++) {
			if ( m_ppThreadZONE[ nI ] ) {
				if ( m_ppThreadZONE[ nI ]->Free () ) {
					if ( m_ppThreadZONE[ nI ]->m_pListITEM ) {
						CLIB_GameSRV::ExeAPI()->DelZoneITEM( m_ppThreadZONE[ nI ]->m_pListITEM );
					}

					SAFE_DELETE( m_ppThreadZONE[ nI ] );
				} else
					bAllClosed = false;
			}
		}
	} while( !bAllClosed );
}


//-------------------------------------------------------------------------------------------------
void CZoneLIST::Add_LocalNPC ( CObjNPC *pObjNPC )
{
	if ( pObjNPC->Get_CharNO() < 1 || pObjNPC->Get_CharNO() >= g_TblNPC.m_nDataCnt )
		return;

	m_ppNpcLIST[ pObjNPC->Get_CharNO() ] = pObjNPC;

	g_pThreadSQL->IO_NpcObjDATA( pObjNPC );
}
CObjNPC *CZoneLIST::Get_LocalNPC( int iNpcNO )
{
	if ( iNpcNO < 1 || iNpcNO >= g_TblNPC.m_nDataCnt )
		return NULL;

	return m_ppNpcLIST[ iNpcNO ];
}
void CZoneLIST::Add_EventOBJ( CObjEVENT *pObjEVENT )
{
	if ( m_EventOBJECT.Search( pObjEVENT->Get_ID() ) ) {
		assert( 0 && " 0 == m_EventOBJECT.Search( pObjEVENT->Get_ID() )" );
		return;
	}

	m_EventOBJECT.Insert( pObjEVENT->Get_ID(), pObjEVENT );

	g_pThreadSQL->IO_EventObjDATA( pObjEVENT );
}
CObjEVENT *CZoneLIST::Get_EventOBJ( short nZoneNO, t_HASHKEY HashKEY )
{
	CObjEVENT *pEventOBJ;
	tagHASH< CObjEVENT* > *pHashNode;

	pHashNode = m_EventOBJECT.Search( HashKEY );
	pEventOBJ = pHashNode ? pHashNode->m_DATA : NULL;
	while( pEventOBJ ) {
		if ( pEventOBJ->GetZONE()->Get_ZoneNO() == nZoneNO )
			return pEventOBJ;

		pHashNode = m_EventOBJECT.SearchContinue( pHashNode, HashKEY );
		pEventOBJ = pHashNode ? pHashNode->m_DATA : NULL;
	}

	return NULL;
}

void CZoneLIST::Init_NpcObjVAR (short nNpcIDX, BYTE *pVAR)
{
	::CopyMemory( m_ppNpcLIST[ nNpcIDX ]->m_pVAR, pVAR, sizeof( tagObjVAR ) );
}
void CZoneLIST::Init_EventObjVAR (t_HASHKEY HashID, BYTE *pVAR)
{
	CObjEVENT *pObjEVT;

	tagHASH< CObjEVENT* > *pHashNode;
	pHashNode = m_EventOBJECT.Search( HashID );
	pObjEVT = pHashNode ? pHashNode->m_DATA : NULL;
	assert( pObjEVT && "m_EventOBJECT.Search( HashID )" );

	::CopyMemory( pObjEVT->m_pVAR, pVAR, sizeof( tagObjVAR ) );
}

//-------------------------------------------------------------------------------------------------
tagEVENTPOS *CZoneLIST::Add_EventPOS (t_HASHKEY HashKEY, short nZoneNO, char *szPosName, float fXPos, float fYPos)
{
	tagEVENTPOS *pEventPos;

/*
	pEventPos = m_PosLIST.Search( HashKEY );
	while( pEventPos ) {
		// 부활 포지션은 모두 같은 이름이다.
		//_ASSERT( pEventPos->m_nZoneNO != nZoneNO );
		pEventPos = m_PosLIST.SearchContinue( HashKEY );
	}
*/

	LogString( LOG_DEBUG, "[[[ Zone %d:%s  Pos:: ( %f, %f ) ]]]\n", nZoneNO, szPosName, fXPos, fYPos);


	pEventPos = new tagEVENTPOS;

	pEventPos->m_nZoneNO = nZoneNO;
	pEventPos->m_Position.x = fXPos;
	pEventPos->m_Position.y = fYPos;
	pEventPos->m_Name.Set( szPosName );

	m_PosLIST.Insert( HashKEY, pEventPos );

	return pEventPos;
}

tagEVENTPOS *CZoneLIST::Get_EventPOS( short nZoneNO, char *szPosName )
{
	t_HASHKEY HashKEY = StrToHashKey( szPosName );
	tagEVENTPOS *pEventPos;

	tagHASH< tagEVENTPOS* > *pHashNode;
	pHashNode = m_PosLIST.Search( HashKEY );
	pEventPos = pHashNode ? pHashNode->m_DATA : NULL;
	while( pEventPos ) {
		if ( pEventPos->m_nZoneNO == nZoneNO )
			return pEventPos;

		pHashNode = m_PosLIST.SearchContinue( pHashNode, HashKEY );
		pEventPos = pHashNode ? pHashNode->m_DATA : NULL;
	}

	return NULL;
}

/*
tagEVENTPOS *CZoneLIST::Get_NearstEventPOS( short nZoneNO, char *szPosName, tPOINTF PosCUR  )
{
	t_HASHKEY HashKEY = StrToHashKey( szPosName );

	tagEVENTPOS *pEventPos, *pNearstPos=NULL;
	float fDist, fNearDist=MAX_INT;

	pEventPos = m_PosLIST.Search( HashKEY );
	while( pEventPos ) {
		if ( pEventPos->m_nZoneNO == nZoneNO ) {
			fDist = ::distance_square( PosCUR, pEventPos->m_Position );
			if ( fDist < fNearDist ) {
				fNearDist  = fDist;
				pNearstPos = pEventPos;
			}
		}

		pEventPos = m_PosLIST.SearchContinue( HashKEY );
	}

	return pNearstPos;
}
*/

//-------------------------------------------------------------------------------------------------
CZoneTHREAD *CZoneLIST::GetZONE( short nZoneNO )
{
	if ( nZoneNO < 0 || nZoneNO >= m_nZoneCNT )
		return NULL;

	if ( NULL == m_ppThreadZONE[ nZoneNO ] )
		return NULL;

	return	m_ppThreadZONE[ nZoneNO ];
}

CObjMOB*CZoneLIST::RegenCharacter(short nZoneNO, float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO)
{
	CZoneTHREAD *pZONE = this->GetZONE( nZoneNO );
	if ( pZONE )
		return pZONE->RegenCharacter(fCenterX, fCenterY, iRange, iIndex, iCount, iTeamNO, true);

	return NULL;
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Send_gsv_ANNOUNCE_CHAT( CObjVAR *pObjVAR, char *szMessage )
{
	CGameOBJ *pObj = (CGameOBJ*) pObjVAR;
	if ( pObj->GetZONE() ) {
		this->Send_gsv_ANNOUNCE_CHAT( pObj->GetZONE()->Get_ZoneNO(), szMessage, /* pObj->Get_NAME() */ "SERVER" );
	}
}

void CZoneLIST::Send_gsv_ANNOUNCE_CHAT( short nZoneNO, char *szMessage, char *szAccount )
{
	if ( NULL == szMessage || NULL == m_ppThreadZONE[ nZoneNO ] )
		return;

	g_LOG.CS_ODS( 0xffff, "[%d/%s:존공지] %s\n", nZoneNO, m_ppThreadZONE[ nZoneNO ]->Get_NAME(), szMessage );

	t_PACKET *pPacket = (t_PACKET*)new char[ MAX_PACKET_SIZE ];

	pPacket->m_HEADER.m_wType = GSV_ANNOUNCE_CHAT;
	pPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );

	Packet_AppendString( pPacket, szMessage );
	if ( szAccount )
		Packet_AppendString( pPacket, szAccount );

	m_ppThreadZONE[ nZoneNO ]->SendPacketToZONE( pPacket );

	SAFE_DELETE_ARRAY( pPacket );
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Send_gsv_ANNOUNCE_CHAT( char *szMessage, char *szAccount )
{
	if ( NULL == szMessage )
		return;

	t_PACKET *pPacket = (t_PACKET*)new char[ MAX_PACKET_SIZE ];

	pPacket->m_HEADER.m_wType = GSV_ANNOUNCE_CHAT;
	pPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );

	Packet_AppendString( pPacket, szMessage );
	if ( szAccount ) {
		g_LOG.CS_ODS( 0xffff, "[전체공지:%s] %s\n", szAccount, szMessage );
		Packet_AppendString( pPacket, szAccount );
	} else {
		g_LOG.CS_ODS( 0xffff, "[전체공지] %s\n", szMessage );
	}
	
	this->Send_gsv_ANNOUNCE_CHAT( pPacket );

	SAFE_DELETE_ARRAY( pPacket );
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Send_gsv_ANNOUNCE_CHAT( t_PACKET *pSendPacket )
{
	for (short nZ=1; nZ<m_nZoneCNT; nZ++) {
		if ( NULL == m_ppThreadZONE[ nZ ] )
			continue;

		m_ppThreadZONE[ nZ ]->SendPacketToZONE( pSendPacket );
	}
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Set_PK_FLAG (short nZoneNO, BYTE btOnOff)
{
	CZoneTHREAD *pZONE = this->GetZONE( nZoneNO );
	if ( pZONE )
		pZONE->Set_PK_FLAG( btOnOff );
}
void CZoneLIST::Set_PK_FLAG (CAI_OBJ *pChar, BYTE btOnOff)
{
	CObjCHAR *pSourCHAR = static_cast< CObjCHAR* >( pChar );

	assert( pSourCHAR );

	pSourCHAR->GetZONE()->Set_PK_FLAG( btOnOff );
}

//-------------------------------------------------------------------------------------------------
bool CZoneLIST::Do_QuestTrigger( short nZoneNO, short nTeamNO, t_HASHKEY HashKEY )
{
	CZoneTHREAD *pZONE = this->GetZONE( nZoneNO );
	if ( pZONE ) {
		pZONE->Do_QuestTrigger( nTeamNO, HashKEY );
		return true;
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
void CZoneLIST::Toggle_RegenSYSTEM ( CGameOBJ *pObj )
{
	if ( pObj && pObj->GetZONE() ) {
		pObj->GetZONE()->Toggle_REGEN ();
	}
}
void CZoneLIST::Set_RegenSYSTEM( CGameOBJ *pObj, BYTE btOnOff )
{
	if ( pObj && pObj->GetZONE() ) {
		pObj->GetZONE()->Set_REGEN( btOnOff ? true : false );
	}
}

//-------------------------------------------------------------------------------------------------
void CZoneLIST::Toggle_RegenSYSTEM ( CAI_OBJ *pChar )
{
	CObjCHAR *pSourCHAR = static_cast< CObjCHAR* >( pChar );
	if ( pSourCHAR ) {
		pSourCHAR->GetZONE()->Toggle_REGEN ();
	}
}
void CZoneLIST::Set_RegenSYSTEM( CAI_OBJ *pChar, BYTE btOnOff )
{
	CObjCHAR *pSourCHAR = static_cast< CObjCHAR* >( pChar );
	if ( pSourCHAR ) {
		pSourCHAR->GetZONE()->Set_REGEN( btOnOff ? true : false );
	}
}

void CZoneLIST::Toggle_RegenSYSTEM ( short nZoneNO )
{
	CZoneTHREAD *pZONE = this->GetZONE( nZoneNO );
	if ( pZONE )
		pZONE->Toggle_REGEN ();
}
void CZoneLIST::Set_RegenSYSTEM( short nZoneNO, BYTE btOnOff )
{
	CZoneTHREAD *pZONE = this->GetZONE( nZoneNO );
	if ( pZONE )
		pZONE->Set_REGEN( btOnOff ? true : false );
}

bool CZoneLIST::IsAgitZONE(short nZoneNO)
{
	if ( IsValidZONE(nZoneNO) ) {
		return AGIT_ZONE_TYPE == ZONE_PVP_STATE(nZoneNO);
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
