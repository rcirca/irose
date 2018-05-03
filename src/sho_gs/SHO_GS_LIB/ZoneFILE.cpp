
#include "LIB_gsMAIN.h"
#include "ZoneLIST.h"
#include "ZoneFILE.h"
#include "CRegenAREA.h"

//-------------------------------------------------------------------------------------------------


enum MAP_LUMP_TYPE {
	LUMP_MAPINFO = 0,
	LUMP_TERRAIN_OBJECT,
	LUMP_TERRAIN_MOB,
	LUMP_TERRAIN_CNST,
	LUMP_TERRAIN_SOUND,
	LUMP_TERRAIN_EFFECT,
	LUMP_TERRAIN_MORPH,
	LUMP_TERRAIN_WATER,
	LUMP_TERRAIN_REGEN,

	LUMP_TERRAIN_EVENT = 12,

	LUMP_MAP_MAX
} ;

enum ZONE_LUMP_TYPE
{
	LUMP_ZONE_INFO = 0,
	LUMP_EVENT_OBJECT,
	LUMP_ZONE_TILE,
	LUMP_TILE_TYPE,
	LUMP_ECONOMY,
	LUMP_ZONE_MAX
} ;


CZoneFILE::CZoneFILE (bool CreateSuspended) : classTHREAD(CreateSuspended)
{
/*
    m_pEventPOS= NULL;
    m_iEventPosCNT = 0;
*/
	this->m_iAgitCNT = 0;
	this->m_pAgitPOS = NULL;

	for (short nI=0; nI<MAP_MOVE_ATTR_GRID_CNT; nI++)
		m_ppMoveATTR[ nI ] = NULL;

	m_iSectorXCnt = 0;
	m_iSectorYCnt = 0;
	m_pPosLIST = new classHASH< tagEVENTPOS* >( 64 );

	m_StrVAR.Alloc( 1024, 1024 );
}
__fastcall CZoneFILE::~CZoneFILE ()
{
	this->FreeZONE ();
	SAFE_DELETE( m_pPosLIST );
}

BYTE CZoneFILE::Is_DAY ()
{
	// 낮이냐?
	int iMOD = ::Get_WorldTIME() % ZONE_DAY_CYCLE( m_nZoneNO );
	if ( iMOD >= m_nDayTimeFROM && iMOD <= m_nDayTimeTO )
		return 1;

	return 0;
}
//-------------------------------------------------------------------------------------------------
/*
tPOINTF CZoneFILE::Get_EventPOS( short nIndex )
{
	tPOINTF OutPOS;
	if ( nIndex < 0 || nIndex >= m_iEventPosCNT ) {
		OutPOS.x = 0;
		OutPOS.y = 0;
	} else {
        OutPOS = m_pEventPOS[ nIndex ];
	}

	return OutPOS;
}
*/
tPOINTF CZoneFILE::Get_RevivePOS( tPOINTF PosCUR )
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
		pEventPos = ( pHashNode ) ? pHashNode->m_DATA : NULL;
	}

	if ( NULL == pNearstPos ) {
		// 부활 위치를 찾지 못했다면 그냥 시작 부활 장소로 하자...
		return this->m_PosSTART;
	}

	return pNearstPos->m_Position;
}

//-------------------------------------------------------------------------------------------------
void CZoneFILE::ReadObjINFO(FILE *fp, long lOffset, int iLumpType, short nMapXIDX, short nMapYIDX)
{
	fseek(fp, lOffset, SEEK_SET);

	int iObjCNT, iObjType, iObjID, iMapXPos, iMapYPos;

    float fPosX, fPosY;

	fread (&iObjCNT, sizeof(int), 1, fp);

//	t_HASHKEY HashEVENT;
	int  iEventID;
	char *szName=this->m_StrVAR.Get();	// CStr::GetString ();
	BYTE cNameLen;
	short nShort;
	FLOAT fQX, fQY, fQZ, fQW;
	for (int iC=0; iC<iObjCNT; iC++) {
		fread (&cNameLen,	sizeof(char),	1,			fp);
		if ( cNameLen > 0 ) {
			fread (szName,		sizeof(char),	cNameLen,	fp);
			szName[ cNameLen ] = 0;

            /*
			HashEVENT = g_pEventLIST->Add_EVENT( szName );
			if ( HashEVENT ) {
				LogString (LOG_NORMAL, "Event File[ %s ]\n", szName);
			}
            */
		} else {
			szName[ 0 ] = 0;
			// HashEVENT = 0;
		}
		fread (&nShort,		sizeof(short),	    1,			fp);		// 클라이언트용 warpID
		fread (&nShort,		sizeof(short),	    1,			fp);
		iEventID = nShort;

		fread (&iObjType,	sizeof(int),	    1,			fp);
		fread (&iObjID,		sizeof(int),	    1,			fp);
		fread (&iMapXPos,	sizeof(int),	    1,			fp);
		fread (&iMapYPos,	sizeof(int),	    1,			fp);

        // rotate
		fread (&fQX,		sizeof(FLOAT),		1,			fp);
		fread (&fQY,		sizeof(FLOAT),		1,			fp);
		fread (&fQZ,		sizeof(FLOAT),		1,			fp);
		fread (&fQW,		sizeof(FLOAT),		1,			fp);

        // position
        fread (&fPosX,      sizeof(float),      1,          fp);
        fread (&fPosY,      sizeof(float),      1,          fp);
        fseek ( fp,         sizeof(float),      SEEK_CUR);

        // scale
        fseek ( fp,         sizeof(float)*3,    SEEK_CUR);

		fPosX += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fPosY += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );

		switch( iLumpType ) {
			case LUMP_TERRAIN_REGEN  :
			{
				Add_RegenPOINT( fp, fPosX, fPosY );
				/*
				CRegenPOINT *pNewPOINT = new CRegenPOINT( this, fPosX, fPosY );
				m_RegenLIST.AllocNAppend( pNewPOINT );

				pNewPOINT->Load( fp );
				*/
				continue;
			}

			case LUMP_TERRAIN_MOB :
			{
				short nQuestIDX;
				int iAI;
				fread (&iAI,	sizeof(int),	1,	fp);
				
				// ai file name..
				// quest id !!!
				fread (&cNameLen,	sizeof(char),	1,			fp);
				fread (szName,		sizeof(char),	cNameLen,	fp);	// sound file name ..		
				szName[ cNameLen ] = 0;

				if ( strlen( szName ) ) {
					nQuestIDX = g_TblEVENT.GetRowIndex( szName );
					/*
					if ( nIndex ) {
						HashEVENT = g_pEventLIST->Add_EVENT( QUEST_FILE( nIndex ) );
						if ( HashEVENT ) {
							LogString (LOG_NORMAL, "Event File[ %s ]\n", szName);
						}
					}
					*/
				} else 
					nQuestIDX = 0;

				if ( iObjID < 1 || iObjID >= g_TblNPC.m_nDataCnt ) {
					continue;
				}
				if ( !NPC_NAME( iObjID ) ) {
					continue;
				}

				CObjNPC *pNewCHAR;
				if ( NPC_TYPE( iObjID ) == 999 ) {

					#ifdef	__NPC_TEST
						if ( 1013 != iObjID || this->Get_ZoneNO() != 99 ) continue;
					#endif

					pNewCHAR = new CObjNPC;
					float fModelDIR = quaternionToModelDirection( fQW, fQX, fQY, fQZ );
					( (CObjNPC*)pNewCHAR )->Set_DIR( fModelDIR );
					if ( !pNewCHAR->Init( this->GetZonePTR(), iObjID, fPosX, fPosY, TEAMNO_NPC, NULL, nQuestIDX ) ) {
						SAFE_DELETE( pNewCHAR );
						continue;
					}

					g_pZoneLIST->Add_LocalNPC( pNewCHAR );
					// NPC는 무적 모드로..
					pNewCHAR->m_IngSTATUS.SetSubFLAG( FLAG_CHEAT_INVINCIBLE );
				} else {
					continue;
				}

				g_LOG.CS_ODS( LOG_DEBUG, "Set Default CHAR( %d:%s ), ( %f, %f ) \n", iObjID, pNewCHAR->Get_NAME(), fPosX, fPosY );
				continue;
			}
///*
			case LUMP_TERRAIN_EVENT :
			{
				char szTmp1[ 257 ], szTmp2[ 257 ];
				fread (&cNameLen,	sizeof(char),	1,			fp);
				if ( cNameLen > 0 ) {
					fread (szTmp1,		sizeof(char),	cNameLen,	fp);
					szTmp1[ cNameLen ] = 0;
				}
				fread (&cNameLen,	sizeof(char),	1,			fp);
				if ( cNameLen > 0 ) {
					fread (szTmp2,		sizeof(char),	cNameLen,	fp);
					szTmp2[ cNameLen ] = 0;
				}
				if ( iEventID ) {
					LogString (0xffff, "Terrain object type : %d[%d,%d], name:%s, map(%d,%d), pos(%3.0f,%3.0f), id:%d, %s/%s \n", 
							iLumpType, 
							nMapXIDX, nMapYIDX,
							szName,
							iMapXPos, iMapYPos, fPosX, fPosY, iEventID, szTmp1, szTmp2 );

					CObjEVENT *pNewEVENT = new CObjEVENT;
					if ( !pNewEVENT->Init( this->GetZonePTR(), nMapXIDX, nMapYIDX, fPosX, fPosY, iEventID ) ) {
						SAFE_DELETE( pNewEVENT );
						continue;
					}
					g_pZoneLIST->Add_EventOBJ( pNewEVENT );
					continue;
				}
				continue;
			}
//*/
		}

		// LogString (LOG_DEBUG, "Object:: Map[%02d,%02d], Type:%d, ID: %d, Pos[%d,%d] \n", m_nZoneMapXIDX, m_nZoneMapYIDX, iObjType, iObjID, iMapXPos, iMapYPos);
	}
}

//-------------------------------------------------------------------------------------------------
void CZoneFILE::LoadMOV (char *szFileName, short nMapXIDX, short nMapYIDX)
{
	// *.MOV
	char *szFullPath;
	FILE *fp;

    szFullPath = this->m_StrVAR.Printf( "%s.MOV", szFileName );	// CStr::Printf( "%s.IFO", szFileName );

    if ( !CUtil::Is_FileExist( szFullPath ) ) {
        return;
	}
    fp = fopen( szFullPath, "rb");
    if ( !fp )
        return;
	
	int iWidth, iHeight;

	fread( &iWidth,		sizeof(int),	1,	fp);
	fread( &iHeight,	sizeof(int),	1,	fp);

	BYTE btATTR;
	int iX, iY, iGridX, iGridY;
	for (iY=0; iY<iHeight; iY++) {
		iGridY = nMapYIDX * PATCH_COUNT_PER_MAP_AXIS * 2 + iY;
		for (iX=0; iX<iWidth; iX++) {
			fread( &btATTR,	sizeof(BYTE),	1,	fp);

			iGridX = nMapXIDX * PATCH_COUNT_PER_MAP_AXIS * 2 + iX;

			assert( iGridX < MAP_MOVE_ATTR_GRID_CNT && iGridY < MAP_MOVE_ATTR_GRID_CNT );
			if ( 0 == btATTR )
				this->m_ppMoveATTR[ iGridY ]->ClearBit( iGridX );
		}
	}

    fclose( fp );
}

//-------------------------------------------------------------------------------------------------
void CZoneFILE::LoadMAP (char *szFileName, short nMapXIDX, short nMapYIDX)
{
    char *szFullPath;
    FILE *fp;

    // skip *.HIM
    // skip *.TIL

    szFullPath = this->m_StrVAR.Printf( "%s.IFO", szFileName );	// CStr::Printf( "%s.IFO", szFileName );

    if ( !CUtil::Is_FileExist( szFullPath ) ) {
        return;
	}

//	g_LOG.CS_ODS( 0xffff, "MapFile:: %s \n", szFullPath );
    fp = fopen( szFullPath, "rb");
    if ( !fp )
        return;

	int  iLumpCNT, iType;
	int  iOffset;
	long lCurPtr;

	fread (&iLumpCNT, sizeof(int), 1, fp);
	for (int iH=0; iH<iLumpCNT; iH++) {
		fread (&iType,	 sizeof(int), 1, fp);
		fread (&iOffset, sizeof(int), 1, fp);

		lCurPtr = ftell (fp);

		switch( iType ) {
			case LUMP_MAPINFO :
				// ReadMapINFO(fp, iOffset);
				break;

			case LUMP_TERRAIN_REGEN  :
			case LUMP_TERRAIN_MOB    :
			case LUMP_TERRAIN_CNST   :
			case LUMP_TERRAIN_EVENT  :
				ReadObjINFO(fp, iOffset, iType, nMapXIDX, nMapYIDX);
				break;

			case LUMP_TERRAIN_MORPH  :
			case LUMP_TERRAIN_EFFECT :
			case LUMP_TERRAIN_SOUND  :
			case LUMP_TERRAIN_OBJECT :
			case LUMP_TERRAIN_WATER  :
			default :
				break;
		}

		fseek (fp, lCurPtr, SEEK_SET);
	}

    fclose( fp );
}


//-------------------------------------------------------------------------------------------------
void CZoneFILE::ReadZoneINFO (FILE *fp, long lOffset)
{
	int   iValue;
	float fValue;
	fseek(fp, lOffset,		SEEK_SET);

	fseek (fp, sizeof(int),	SEEK_CUR);

	fread (&iValue,			sizeof(int),	1,	fp);	// witdh
	LogString (LOG_DEBUG, "Width:: %d ... \n", iValue);

	fread (&iValue,			sizeof(int),	1,	fp);	// height
	LogString (LOG_DEBUG, "Height:: %d ... \n", iValue);


	// appended 03. 07. 25
	fread (&iValue,		sizeof(int),		1,	fp);	// patch grid count
	fread (&fValue,		sizeof(float),		1,	fp);	// grid size
	m_nGridPerPATCH = iValue;
	m_nGridSIZE     = (short)fValue;
	m_nPatchSIZE    = m_nGridSIZE * m_nGridPerPATCH;

	// 전체 맵 크기 default : m_nGridPerPATCH=4,  m_nGridSIZE=250
	// 가로 64장(MAP_COUNT_PER_ZONE_AXIS) * 16개 패치(PATCH_COUNT_PER_MAP_AXIS) * m_nPatchSIZE
	int iZoneSIZE = MAP_COUNT_PER_ZONE_AXIS * PATCH_COUNT_PER_MAP_AXIS * m_nPatchSIZE;

    short nX, nY;

    m_iSectorXCnt = iZoneSIZE / m_nSectorSIZE + 1;
    m_iSectorYCnt = iZoneSIZE / m_nSectorSIZE + 1;

    m_ppSECTOR = new CZoneSECTOR* [ m_iSectorYCnt ];
	::ZeroMemory (m_ppSECTOR,	sizeof( CZoneSECTOR* ) * m_iSectorYCnt );
    for (nY=0; nY<m_iSectorYCnt; nY++) {
		if ( AGIT_ZONE_TYPE == ZONE_PVP_STATE( this->m_nZoneNO ) ) {
			// 아지트존
		    m_ppSECTOR[ nY ] = new CAgitSECTOR [ m_iSectorXCnt ];
		} else
		    m_ppSECTOR[ nY ] = new CNormalSECTOR [ m_iSectorXCnt ];

		for (nX=0; nX<m_iSectorXCnt; nX++) 
			m_ppSECTOR[ nY ][ nX ].SetCenterPOS ( nX*m_nSectorSIZE+DIV02(m_nSectorSIZE), nY*m_nSectorSIZE+DIV02(m_nSectorSIZE) );
	}

/*
	// iMapSize = 16000, iBlockSize = 1000
	m_iMapSIZE = ( m_nPatchSIZE * 16 );

	fread (&iValue, sizeof(int), 1, fp);	// start pos
	m_nCenterMapXIDX = iValue;

	fread (&iValue, sizeof(int), 1, fp);	// start pos
	m_nCenterMapYIDX = iValue;

	LogString (LOG_DEBUG, "Start (X:%d, Y:%d) \n", m_nCenterMapXIDX, m_nCenterMapYIDX);
*/
}
bool CZoneFILE::ReadECONOMY( FILE *fp, long lOffset )
{
	fseek(fp, lOffset, SEEK_SET);
	
	BYTE btStrLen;
	char *pStr=this->m_StrVAR.Get();	// CStr::GetString();

	// zone name
	fread(&btStrLen,	sizeof(BYTE),	1,			fp);
	fread( pStr,		sizeof(char),	btStrLen,	fp);
	pStr[ btStrLen ] = 0;

	fread(&m_iIsDungeon,sizeof(int),	1,			fp);

	// background music ...
	fread(&btStrLen,	sizeof(BYTE),	1,			fp);
	fread( pStr,		sizeof(char),	btStrLen,	fp);
	pStr[ btStrLen ] = 0;

	// background model ...
	fread(&btStrLen,	sizeof(BYTE),	1,			fp);
	fread( pStr,		sizeof(char),	btStrLen,	fp);
	pStr[ btStrLen ] = 0;

	Load_Economy( fp );

	return true;
}

//-------------------------------------------------------------------------------------------------
void CZoneFILE::ReadEventObjINFO (FILE *fp, long lOffset)
{
	int iEventPosCNT;

	fseek(fp, lOffset, SEEK_SET);

	fread( &iEventPosCNT, sizeof(int), 1, fp);
	/*
	m_iEventPosCNT = iEventPosCNT;
	if ( m_iEventPosCNT > 0 ) {
		m_pEventPOS = new tPOINTF[ m_iEventPosCNT ];
	}
	*/

	char *szName=this->m_StrVAR.Get();	// CStr::GetString ();
	BYTE  btStrLen;
    float fX, fY, fZ;

	t_HASHKEY HashKEY;
	tagEVENTPOS *pEventPOS;

	for (int iC=0; iC<iEventPosCNT; iC++) {
        fread( &fX,     sizeof(float),  1,  fp);
		// exchange y <--> z
        fread( &fZ,     sizeof(float),  1,  fp);
        fread( &fY,     sizeof(float),  1,  fp);

		fX += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fY += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );

		fread( &btStrLen,	sizeof(BYTE),	1,			fp);
		fread( szName,		sizeof(char),	btStrLen,	fp);
		szName[ btStrLen ] = 0;

		/*
        m_pEventPOS[ iC ].x = fX;
        m_pEventPOS[ iC ].y = fY;
		*/

		HashKEY = StrToHashKey( szName );

		pEventPOS = g_pZoneLIST->Add_EventPOS( HashKEY, m_nZoneNO, szName, fX, fY );
		m_pPosLIST->Insert( HashKEY, pEventPOS );
	}


	m_PosSTART.x  = m_PosSTART.y  = 0;
	pEventPOS = g_pZoneLIST->Get_EventPOS( m_nZoneNO, ZONE_START_POS ( m_nZoneNO ) );
	if ( pEventPOS ) {
		m_PosSTART = pEventPOS->m_Position;
	}

	m_HashKeyRevivePOS = ::StrToHashKey( ZONE_REVIVE_POS( m_nZoneNO ) );
	assert( m_HashKeyRevivePOS && "NULL == ZONE_REVIVE_POS( m_nZoneNO )" );

	// 각존의 부활 위치는 반드시 존재해야 한다...
	m_PosREVIVE = this->Get_RevivePOS( m_PosSTART );
	if ( m_nZoneNO != 98 )
		assert ( m_PosREVIVE.x != 0 && m_PosREVIVE.y != 0 );

	LogString( LOG_DEBUG, "[[[ Zone %d, StartPOS :: ( %d, %d ) ]]]\n",  m_nZoneNO, m_PosSTART.x, m_PosSTART.y);
}

//-------------------------------------------------------------------------------------------------
char*CZoneFILE::GetMapFILE(short nMapX, short nMapY)
{
	if ( nMapX < 0 || nMapX >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;
	if ( nMapY < 0 || nMapY >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;

//	char *szMapFile = CStr::Printf ("%s%d_%d", m_ZoneDIR.Get(), nMapX, MAP_COUNT_PER_ZONE_AXIS-nMapY);
	char *szMapFile = this->m_StrVAR.Printf ("%s%d_%d", m_ZoneDIR.Get(), nMapX, MAP_COUNT_PER_ZONE_AXIS-nMapY);
	m_MapFileName.Set( szMapFile );

	return m_MapFileName.Get();
}

//-------------------------------------------------------------------------------------------------
bool CZoneFILE::LoadZONE (char *szBaseDIR, short nZoneNO)
{
/*
게임상 낮 : 존타임이 [ (낮시간/2) ~ (저녁시간 + 밤시간)/2 ] 사이에 있을 때
게임상 밤 : (낮시간/2) 보다 작을 때,  [ (저녁시간 + 밤시간)/2 ] 보다 클 때	
*/
	m_nDayTimeFROM = ZONE_DAY_TIME(nZoneNO)/2;
	m_nDayTimeTO   = ( ZONE_EVENING_TIME(nZoneNO) + ZONE_NIGHT_TIME(nZoneNO) ) / 2;

	m_HashJoinTRIGGER = ZONE_JOIN_TRIGGER( nZoneNO ) ? ::StrToHashKey( ZONE_JOIN_TRIGGER( nZoneNO ) ) : 0;
	m_HashKillTRIGGER = ZONE_KILL_TRIGGER( nZoneNO ) ? ::StrToHashKey( ZONE_KILL_TRIGGER( nZoneNO ) ) : 0;
	m_HashDeadTRIGGER = ZONE_DEAD_TRIGGER( nZoneNO ) ? ::StrToHashKey( ZONE_DEAD_TRIGGER( nZoneNO ) ) : 0;

    short nX, nY;
	for (nY=0; nY<MAP_MOVE_ATTR_GRID_CNT; nY++) {
		this->m_ppMoveATTR[ nY ] = new C1BITARRAY( MAP_MOVE_ATTR_GRID_CNT );
		this->m_ppMoveATTR[ nY ]->FillAll();
	}

	//if ( m_HashJoinTRIGGER | m_HashKillTRIGGER | m_HashDeadTRIGGER ) {
	//	int i = 0;
	//}
	m_nSectorSIZE  = g_pZoneLIST->GetSectorSIZE( nZoneNO );
	m_iSectorLIMIT = DIV02( m_nSectorSIZE ) + BUF_SECTOR_SIZE( m_nSectorSIZE );

    FILE *fp;
	char *szZoneFILE;

	szZoneFILE = this->m_StrVAR.Printf("%s%s", szBaseDIR, ZONE_FILE( nZoneNO ));
	fp = fopen(szZoneFILE, "rb");
    if ( !fp ) {
		LogString (0xffff, "zone file open error:: [ %s / %s ] = %s \n", szBaseDIR, ZONE_FILE(nZoneNO), szZoneFILE );
        return false;
	}

	m_nZoneNO = nZoneNO;
	int iCount, iType, iOffset;
	iCount = CUtil::ExtractFileName( NULL, szZoneFILE );
	if ( iCount > 0 ) {
		char *pStr=this->m_StrVAR.Get();	// CStr::GetString ();
		strncpy( pStr, szZoneFILE, iCount );
		pStr[ iCount ] = 0;
		m_ZoneDIR.Set( pStr );
		LogString (LOG_NORMAL, "zone folder :: %s \n", pStr );
	}

	long lCurPtr;
	fread (&iCount, sizeof(int), 1, fp);
	for (int iC=0; iC<iCount; iC++) {
		fread( &iType,		sizeof(int),	1, fp);
		fread( &iOffset,	sizeof(int),	1, fp);

		lCurPtr = ftell (fp);

		switch( iType ) {
			case LUMP_ZONE_INFO :
				ReadZoneINFO( fp, iOffset );
				break;
			case LUMP_EVENT_OBJECT :
				ReadEventObjINFO( fp, iOffset );
				break;
			case LUMP_ZONE_TILE :
			case LUMP_TILE_TYPE :
				break;
			case LUMP_ECONOMY :
				ReadECONOMY (fp, iOffset );
				break;
		}

		fseek (fp, lCurPtr, SEEK_SET);
	}
	fclose (fp);

	char *szMapFile;
    for (nY=0; nY<MAP_COUNT_PER_ZONE_AXIS; nY++) {
        for (nX=0; nX<MAP_COUNT_PER_ZONE_AXIS; nX++) {
			szMapFile = GetMapFILE( nX, nY );
            this->LoadMAP( szMapFile, nX, nY );
			this->LoadMOV( szMapFile, nX, nY );
		}
    }

	this->m_iAgitCNT = 0;
	if ( AGIT_ZONE_TYPE == ZONE_PVP_STATE( this->m_nZoneNO ) ) {
		// 아지트 존이면... 부활위치를 기준으로... 아지트 구성...
		tagEVENTPOS *pRevivePos;

		tagHASH< tagEVENTPOS* > *pHashNode;
		pHashNode = m_pPosLIST->Search( this->m_HashKeyRevivePOS );

		int iPos = m_pPosLIST->GetCount();
		tPOINTF *pPos = new tPOINTF[ iPos ];

		pRevivePos = pHashNode ? pHashNode->m_DATA : NULL;
		while( pRevivePos ) {
			pPos[ this->m_iAgitCNT++ ] = pRevivePos->m_Position;
			pHashNode = m_pPosLIST->SearchContinue( pHashNode, this->m_HashKeyRevivePOS );
			pRevivePos = ( pHashNode ) ? pHashNode->m_DATA : NULL;
		}
		this->m_pAgitPOS = new tPOINTF[ this->m_iAgitCNT ];
		
		for (iPos=0; iPos<this->m_iAgitCNT; iPos++) {
			g_LOG.CS_ODS( 0xfff, "		Agit ZoneNO:%d, Pos( %f, %f ) \n", this->m_nZoneNO, pPos[ iPos ].x, pPos[ iPos ].y );
			this->m_pAgitPOS[ iPos ] = pPos[ iPos ];
		}
		SAFE_DELETE_ARRAY( pPos );
	}

    return true;
}

void CZoneFILE::FreeZONE (void)
{
/*
    SAFE_DELETE_ARRAY( m_pEventPOS );
    m_iEventPosCNT = 0;
*/
	t_HASHKEY	 HashKEY;
	tagEVENTPOS* pEventPOS;
	
	while( m_pPosLIST->GetFirst( &HashKEY, &pEventPOS ) ) {
		// SAFE_DELETE( pEventPOS ); <-- zonelist에서 삭제된다 !!
		m_pPosLIST->Delete( HashKEY );
	}

	SAFE_DELETE_ARRAY( this->m_pAgitPOS );

    if ( m_ppSECTOR ) {
        for (short nY=0; nY<m_iSectorYCnt; nY++)
            SAFE_DELETE_ARRAY( m_ppSECTOR[ nY ] );

        SAFE_DELETE_ARRAY( m_ppSECTOR );
    }

	for (short nI=0; nI<MAP_MOVE_ATTR_GRID_CNT; nI++) {
		SAFE_DELETE( m_ppMoveATTR[ nI ] );
	}
}

//-------------------------------------------------------------------------------------------------
