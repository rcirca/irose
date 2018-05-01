
#include "stdAFX.h"
#include "IO_STB.h"
#include "WS_ZoneLIST.h"
#include "../../SHO_GS/SHO_GS_LIB/ETC_Math.h"
#include "CWS_Server.h"
#include "SHO_WS_LIB.h"
#include "WS_ThreadSQL.h"
//---------------------------------------------------------------------------------------
tPOINTF WS_Zone::Get_RevivePOS( tPOINTF PosCUR )
{
	tagEVENTPOS *pEventPos, *pNearstPos=NULL;
	float fDist, fNearDist=(float)MAX_DWORD;

	tagHASH< tagEVENTPOS* > *pHashNode;
	pHashNode = m_pPosLIST->Search( this->m_HashKeyRevivePOS );
	pEventPos = pHashNode ? pHashNode->m_DATA : NULL;
	while( pEventPos ) {
		fDist = ::distance_square( PosCUR, pEventPos->m_Position );
		if ( fDist < fNearDist ) {
			fNearDist  = fDist;
			pNearstPos = pEventPos;
		}

		pHashNode = m_pPosLIST->SearchContinue( pHashNode, this->m_HashKeyRevivePOS );
		pEventPos = pHashNode ? pHashNode->m_DATA : NULL;
	}

	if ( NULL == pNearstPos ) {
		// 부활 위치를 찾지 못했다면 그냥 시작 부활 장소로 하자...
		return m_pStartPOS->m_Position;
	}

	return pNearstPos->m_Position;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
WS_ZoneLIST::~WS_ZoneLIST ()
{
	SAFE_DELETE_ARRAY( m_pZONE );
	
	t_HASHKEY	 HashKEY;
	tagEVENTPOS* pEventPOS;
	
	while( m_PosLIST.GetFirst( &HashKEY, &pEventPOS ) ) {
		SAFE_DELETE( pEventPOS );
		m_PosLIST.Delete( HashKEY );
	}
}

//---------------------------------------------------------------------------------------
void WS_ZoneLIST::Set_WorldVAR (short nVarIDX, short nValue)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	CWorldVAR::Set_WorldVAR( nVarIDX, nValue );

	pCPacket->m_HEADER.m_wType = WSV_SET_WORLD_VAR;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SET_WORLD_VAR );

	pCPacket->m_srv_SET_WORLD_VAR.m_nVarIDX = nVarIDX;
	pCPacket->m_srv_SET_WORLD_VAR.m_nValue[0] = nValue;

	g_pListSERVER->Send_ToWORLD( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	this->Save_WorldVAR ();
}

//---------------------------------------------------------------------------------------
bool WS_ZoneLIST::Save_WorldVAR ()
{
	return g_pThreadSQL->Save_WorldVAR( this->m_pVAR, sizeof( tagWorldVAR ) );
}

//---------------------------------------------------------------------------------------
bool WS_ZoneLIST::Init (char *szBaseDIR)
{
	m_nZoneCNT = g_TblZONE.m_nDataCnt;

	if ( m_nZoneCNT < 1 ) 
		return false;
	
	this->m_pZONE = new WS_Zone[ m_nZoneCNT ];

	for (short nY=1; nY<m_nZoneCNT; nY++) {
		if ( 19 == nY )		// 행성 워프 존
			continue;

		if ( ZONE_FILE( nY ) ) {
			this->m_pZONE[ nY ].m_szZoneNAME = ZONE_NAME( nY );
			this->m_pZONE[ nY ].m_nZoneNO = nY;

			if ( this->LoadZONE( szBaseDIR, nY ) ) {
				this->m_pZONE[ nY ].m_pStartPOS = this->Get_EventPOS( nY, ZONE_START_POS( nY ) );

				this->m_pZONE[ nY ].m_HashKeyRevivePOS = ::StrToHashKey( ZONE_REVIVE_POS( nY ) );
				_ASSERT( this->m_pZONE[ nY ].m_HashKeyRevivePOS );

				// 각존의 부활 위치는 반드시 존재해야 한다...
				if ( this->m_pZONE[ nY ].m_pStartPOS ) {
					this->m_pZONE[ nY ].m_PosREVIVE = this->m_pZONE[ nY ].Get_RevivePOS( this->m_pZONE[ nY ].m_pStartPOS->m_Position );
				}

				_ASSERT ( this->m_pZONE[ nY ].m_PosREVIVE.x != 0 && this->m_pZONE[ nY ].m_PosREVIVE.y != 0 );
			}
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
enum ZONE_LUMP_TYPE
{
	LUMP_ZONE_INFO = 0,
	LUMP_EVENT_OBJECT,
	LUMP_ZONE_TILE,
	LUMP_TILE_TYPE,
	LUMP_ECONOMY,
	LUMP_ZONE_MAX
} ;
bool WS_ZoneLIST::LoadZONE (char *szBaseDIR, short nZoneNO)
{
    FILE *fp;
	char *szZoneFILE;

	szZoneFILE = CStr::Printf("%s%s", szBaseDIR, ZONE_FILE( nZoneNO ));
	fp = fopen(szZoneFILE, "rb");
    if ( !fp ) {
		g_LOG.CS_ODS (0xffff, "zone file open error:: [ %s / %s ] = %s \n", szBaseDIR, ZONE_FILE(nZoneNO), szZoneFILE );
        return false;
	}

	int iCount, iType, iOffset;
	long lCurPtr;
	fread (&iCount, sizeof(int), 1, fp);
	for (int iC=0; iC<iCount; iC++) {
		fread( &iType,		sizeof(int),	1, fp);
		fread( &iOffset,	sizeof(int),	1, fp);

		lCurPtr = ftell (fp);

		switch( iType ) {
			case LUMP_ZONE_INFO :
				this->ReadZoneINFO( fp, iOffset );
				break;
			case LUMP_EVENT_OBJECT :
				this->ReadEventObjINFO( fp, iOffset, nZoneNO );
				break;
			case LUMP_ZONE_TILE :
			case LUMP_TILE_TYPE :
			case LUMP_ECONOMY :
				break;
		}

		fseek (fp, lCurPtr, SEEK_SET);
	}
	fclose (fp);

    return true;
}

//-------------------------------------------------------------------------------------------------
void WS_ZoneLIST::ReadZoneINFO (FILE *fp, long lOffset)
{
	int   iValue;
	float fValue;
	fseek(fp, lOffset,		SEEK_SET);

	fseek (fp, sizeof(int),	SEEK_CUR);

	fread (&iValue,			sizeof(int),	1,	fp);	// witdh
	fread (&iValue,			sizeof(int),	1,	fp);	// height

	// appended 03. 07. 25
	fread (&iValue,		sizeof(int),		1,	fp);	// patch grid count
	fread (&fValue,		sizeof(float),		1,	fp);	// grid size

	m_nGridPerPATCH = iValue;
	m_nGridSIZE     = (short)fValue;
	m_nPatchSIZE    = m_nGridSIZE * m_nGridPerPATCH;
}
void WS_ZoneLIST::ReadEventObjINFO (FILE *fp, long lOffset, short nZoneNO)
{
	int iEventPosCNT;

	fseek(fp, lOffset, SEEK_SET);
	fread( &iEventPosCNT, sizeof(int), 1, fp);

	BYTE  btStrLen;
    float fX, fY, fZ;

	t_HASHKEY HashKEY;
	tagEVENTPOS *pEventPOS;

	char *szName;
	for (int iC=0; iC<iEventPosCNT; iC++) {
        fread( &fX,     sizeof(float),  1,  fp);
		// exchange y <--> z
        fread( &fZ,     sizeof(float),  1,  fp);
        fread( &fY,     sizeof(float),  1,  fp);

		fX += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fY += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );

		szName = CStr::GetString ();

		fread( &btStrLen,	sizeof(BYTE),	1,			fp);
		fread( szName,		sizeof(char),	btStrLen,	fp);
		szName[ btStrLen ] = 0;

		HashKEY = StrToHashKey( szName );

		pEventPOS = this->Add_EventPOS( HashKEY, nZoneNO, szName, fX, fY );
		this->m_pZONE[ nZoneNO ].m_pPosLIST->Insert( HashKEY, pEventPOS );
	}
}


//---------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
tagEVENTPOS *WS_ZoneLIST::Add_EventPOS (t_HASHKEY HashKEY, short nZoneNO, char *szPosName, float fXPos, float fYPos)
{
	tagEVENTPOS *pEventPos;

	LogString( 0xffff, "[[[ Zone %d:%s  Pos:: ( %f, %f ) ]]]\n", nZoneNO, szPosName, fXPos, fYPos);

	pEventPos = new tagEVENTPOS;

	pEventPos->m_nZoneNO = nZoneNO;
	pEventPos->m_Position.x = fXPos;
	pEventPos->m_Position.y = fYPos;
	pEventPos->m_Name.Set( szPosName );

	m_PosLIST.Insert( HashKEY, pEventPos );

	return pEventPos;
}

//---------------------------------------------------------------------------------------
tagEVENTPOS *WS_ZoneLIST::Get_EventPOS( short nZoneNO, char *szPosName )
{
	t_HASHKEY HashKEY = StrToHashKey( szPosName );

	tagHASH< tagEVENTPOS* > *pHashNode;
	pHashNode = m_PosLIST.Search( HashKEY );
	while( pHashNode ) {
		if ( pHashNode->m_DATA->m_nZoneNO == nZoneNO )
			return pHashNode->m_DATA;

		pHashNode = m_PosLIST.SearchContinue( pHashNode, HashKEY );
	}

	return NULL;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
