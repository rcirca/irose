#include "stdAFX.h"
#include <mmsystem.h>
#include "CAI_File.h"

#ifndef	__SERVER
#include "Game.h"
#include "..\\Util\\VFSManager.h"
#include "..\\Common\\IO_STB.h"
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

//-------------------------------------------------------------------------------------------------
int AI_Get_Ability (CAI_OBJ *pCHAR, int iAbTYPE)
{
	switch( iAbTYPE ) {
		case 0 :	//레벨
			return pCHAR->Get_LEVEL ();
		case 1 :	// 공격력
			return pCHAR->Get_ATK ();
		case 2 :	// 방어력
			return pCHAR->Get_DEF ();
		case 3 :	// 항마력
			return pCHAR->Get_RES ();
		case 4 :	// HP
			return pCHAR->Get_HP ();
		case 5 :	// 매력
			return pCHAR->Get_CHARM ();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CAI_EVENT::CAI_EVENT ()
{
	m_ppConditionLIST = NULL;
	m_ppActionLIST = NULL;

	m_iConditionCNT = 0;
	m_iActionCNT = 0;
}
CAI_EVENT::~CAI_EVENT ()
{
	int iL;

	if ( m_ppConditionLIST ) {
		for (iL=0; iL<m_iConditionCNT; iL++) {
			if ( m_ppConditionLIST[ iL ] ) {
				// delete[] ...
				SAFE_DELETE_ARRAY( m_ppConditionLIST[ iL ] );
			}
		}

		SAFE_DELETE_ARRAY( m_ppConditionLIST );
	}

	if ( m_ppActionLIST ) {
		for (iL=0; iL<m_iActionCNT; iL++) {
			if ( m_ppActionLIST[ iL ] ) {
				SAFE_DELETE_ARRAY( m_ppActionLIST[ iL ] );
			}
		}

		SAFE_DELETE_ARRAY( m_ppActionLIST );
	}
}

#ifdef	__SERVER
//-------------------------------------------------------------------------------------------------
bool CAI_EVENT::Load (FILE *fp, STBDATA *pSTB, int iLangCol)
{
	char szStr[ AIG_NAME_MAX+1 ];

	fread( szStr,	sizeof(char),	AIG_NAME_MAX,	fp);
	fread( &m_iConditionCNT,	sizeof(int),	1,	fp);
//	LogString (LOG_NORMAL, "	Condition: %s, %d \n", szStr, m_iConditionCNT );

	if ( m_iConditionCNT > 0 )
		m_ppConditionLIST = new AI_CONDITION* [ m_iConditionCNT ];
	for (int iC=0; iC<m_iConditionCNT; iC++) {
		stCondHead	sCondH;
		fread( &sCondH,		sizeof(stCondHead),	1,	fp);

		m_ppConditionLIST[ iC ] = (AI_CONDITION*) new BYTE [ sCondH.dwSize ];
		m_ppConditionLIST[ iC ]->m_Header = sCondH;
		fread( &m_ppConditionLIST[ iC ]->m_pDATA[ sizeof(stCondHead) ], sizeof(BYTE), sCondH.dwSize-sizeof(stCondHead),	fp );

		switch( sCondH.Type ) {
			case AICOND_00 :	break;
			case AICOND_01 :	break;
			case AICOND_02 :	m_ppConditionLIST[ iC ]->st02.iDistance *= 100;	
				if ( sCondH.dwSize < sizeof( AICOND02 ) ) {
					_ASSERT( 0 );
					return false;
					/*
					// 이전 데이타 포맷.. 2004.2.2
					AI_CONDITION *pNewCOND = (AI_CONDITION*) new BYTE [ sizeof( AICOND02 ) ];

					::CopyMemory ( pNewCOND, m_ppConditionLIST[ iC ], sCondH.dwSize );

					pNewCOND->st02.dwSize = sizeof( AICOND02 );
					pNewCOND->st02.cLevelDiff2 = 127;
					pNewCOND->st02.wChrNum = m_ppConditionLIST[ iC ]->st02.wChrNum_Before;

					SAFE_DELETE_ARRAY( m_ppConditionLIST[ iC ] );
					m_ppConditionLIST[ iC ] = pNewCOND;
					*/
				}
				break;
			case AICOND_03 :	m_ppConditionLIST[ iC ]->st03.iDistance *= 100;	break;
			case AICOND_04 :	m_ppConditionLIST[ iC ]->st04.iDistance *= 100;	break;
			case AICOND_05 :	break;
			case AICOND_06 :	break;
			case AICOND_07 :	break;
			case AICOND_08 :	m_ppConditionLIST[ iC ]->st08.iDistance *= 100;	
				if ( sCondH.dwSize < sizeof( AICOND02 ) ) {
					_ASSERT( 0 );
					return false;
				}
				break;
			case AICOND_09 :	break;
			case AICOND_10 :	break;
			case AICOND_11 :	break;
			case AICOND_12 :	break;
			case AICOND_13 :	break;
			case AICOND_14 :	break;
			case AICOND_15 :	break;
			case AICOND_16 :	break;
			case AICOND_18 :	m_ppConditionLIST[ iC ]->st18.iDistance *= 100;	break;
		} 
	}

	fread( &m_iActionCNT,	sizeof(int),		1,	fp);

//	LogString (LOG_NORMAL, "	Action : %d \n", m_iConditionCNT );

	if ( m_iActionCNT > 0 ) 
		m_ppActionLIST = new AI_ACTION* [ m_iActionCNT ];
	for (int iA=0; iA<m_iActionCNT; iA++) {
		stActHead	sActionH;

		fread( &sActionH,	sizeof(stActHead),	1,	fp);

		m_ppActionLIST[ iA ] = (AI_ACTION*) new BYTE[ sActionH.dwSize ];
		m_ppActionLIST[ iA ]->m_Header = sActionH;
		fread( &m_ppActionLIST[ iA ]->m_pDATA[ sizeof(stActHead) ], sizeof(BYTE), sActionH.dwSize-sizeof(stActHead), fp);

		switch( sActionH.Type ) {
			case AIACT_00 : break;
			case AIACT_01 : break;
			case AIACT_02 : 
			{
				char *szMsg = pSTB->GetValueSTR( iLangCol, m_ppActionLIST[ iA ]->st02.iStrID-1 );
				char *szNull = "unknown";
				if ( !szMsg ) 
					szMsg = szNull;
				int iStrLen = strlen(szMsg);

				AIACT02_STR *pNewACT = (AIACT02_STR*) new BYTE[ sizeof(AIACT02_STR) + iStrLen+1 ];
				memcpy( pNewACT, m_ppActionLIST[ iA ], sizeof(AIACT02)-sizeof(int) );
				strcpy( pNewACT->szCon, szMsg );
				pNewACT->szCon[ iStrLen ] = NULL;

				//LogString( 0xffff,"AI02 STR[%d:%d] :: %s\n", iLangCol, m_ppActionLIST[ iA ]->st02.iStrID-1, pNewACT->szCon );

				SAFE_DELETE_ARRAY( m_ppActionLIST[ iA ] );
				m_ppActionLIST[ iA ] = (AI_ACTION*)pNewACT;
				break;
			}
			case AIACT_03 : m_ppActionLIST[ iA ]->st03.iDistance *= 100;	break;
			case AIACT_04 : m_ppActionLIST[ iA ]->st04.iDistance *= 100;	break;
			case AIACT_05 : break;
			case AIACT_06 : m_ppActionLIST[ iA ]->st06.iDistance *= 100;	break;
			case AIACT_07 : break;
			case AIACT_08 : m_ppActionLIST[ iA ]->st08.iDistance *= 100;	break;
			case AIACT_09 : break;
			case AIACT_10 : break;
			case AIACT_11 : m_ppActionLIST[ iA ]->st11.iDistance *= 100;	break;
			case AIACT_12 : break;
			case AIACT_13 : break;
			case AIACT_14 : m_ppActionLIST[ iA ]->st14.iDistance *= 100;	break;
			case AIACT_15 : break;
			case AIACT_16 : m_ppActionLIST[ iA ]->st16.iDistance *= 100;	break;

			case AIACT_18 : m_ppActionLIST[ iA ]->st18.iDistance *= 100;	break;
			case AIACT_19 : break;
			case AIACT_20 : m_ppActionLIST[ iA ]->st20.iDistance *= 100;	break;

			case AIACT_28 : 
			{
				char *szMsg = pSTB->GetValueSTR( iLangCol, m_ppActionLIST[ iA ]->st28.iStrID-1 );
				char *szNull = "unknown";
				if ( !szMsg ) 
					szMsg = szNull;
				int iStrLen = strlen(szMsg);

				AIACT28_STR *pNewACT = (AIACT28_STR*) new BYTE[ sizeof(AIACT28_STR) + iStrLen+1 ];
				memcpy( pNewACT, m_ppActionLIST[ iA ], sizeof(AIACT28)-sizeof(int) );
				strcpy( pNewACT->szMsg, szMsg );
				pNewACT->szMsg[ iStrLen ] = NULL;

				//LogString( 0xffff,"AI28 STR[%d:%d] :: %s\n", iLangCol, m_ppActionLIST[ iA ]->st28.iStrID-1, pNewACT->szMsg );

				SAFE_DELETE_ARRAY( m_ppActionLIST[ iA ] );
				m_ppActionLIST[ iA ] = (AI_ACTION*)pNewACT;

				break;
			}
			case AIACT_30 : 
			{
				t_HASHKEY HashKey = ::StrToHashKey( m_ppActionLIST[ iA ]->st30.szTrigger );
				t_HASHKEY*pHash = (t_HASHKEY*)( m_ppActionLIST[ iA ]->st30.szTrigger );
				*pHash = HashKey;
				//m_ppActionLIST[ iA ]->st30.m_HashNextTrigger[0] = HashKey;
			}
		}
	}

	return true;
}
#else
bool CAI_EVENT::Load ( CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol )
{
	char szStr[ AIG_NAME_MAX+1 ];

	pFileSystem->Read( szStr, sizeof(char) * AIG_NAME_MAX );
	pFileSystem->ReadInt32( &m_iConditionCNT );

	if ( m_iConditionCNT > 0 )
		m_ppConditionLIST = new AI_CONDITION* [ m_iConditionCNT ];

	for (int iC=0; iC<m_iConditionCNT; iC++) 
	{
		stCondHead	sCondH;
		pFileSystem->Read( &sCondH, sizeof(stCondHead) );

		m_ppConditionLIST[ iC ] = (AI_CONDITION*) new BYTE [ sCondH.dwSize ];
		m_ppConditionLIST[ iC ]->m_Header = sCondH;
		pFileSystem->Read( &m_ppConditionLIST[ iC ]->m_pDATA[ sizeof(stCondHead) ], 
								sizeof(BYTE) * sCondH.dwSize-sizeof(stCondHead) );

		switch( sCondH.Type ) {
			case AICOND_02 :	m_ppConditionLIST[ iC ]->st02.iDistance *= 100;	
				if ( sCondH.dwSize < sizeof( AICOND02 ) ) {
					return false;
				}
				break;
			case AICOND_03 :	m_ppConditionLIST[ iC ]->st03.iDistance *= 100;	break;
			case AICOND_04 :	m_ppConditionLIST[ iC ]->st04.iDistance *= 100;	break;
			case AICOND_08 :	m_ppConditionLIST[ iC ]->st08.iDistance *= 100;	
				if ( sCondH.dwSize < sizeof( AICOND02 ) ) {
					return false;
				}
				break;
			case AICOND_18 :	m_ppConditionLIST[ iC ]->st18.iDistance *= 100;	break;
		} 
	}

	pFileSystem->ReadInt32( &m_iActionCNT );

	if ( m_iActionCNT > 0 ) 
		m_ppActionLIST = new AI_ACTION* [ m_iActionCNT ];

	for (int iA=0; iA<m_iActionCNT; iA++) 
	{
		stActHead	sActionH;

		pFileSystem->Read( &sActionH, sizeof(stActHead) );

		m_ppActionLIST[ iA ] = (AI_ACTION*) new BYTE[ sActionH.dwSize ];
		m_ppActionLIST[ iA ]->m_Header = sActionH;
		pFileSystem->Read( &m_ppActionLIST[ iA ]->m_pDATA[ sizeof(stActHead) ], 
								sizeof(BYTE) * sActionH.dwSize-sizeof(stActHead) );

		switch( sActionH.Type ) {
			case AIACT_00 : break;
			case AIACT_01 : break;
			case AIACT_02 :
			{
				char *szMsg = pSTB->GetValueSTR( iLangCol, m_ppActionLIST[ iA ]->st02.iStrID-1 );
				char *szNull = "unknown";
				if ( !szMsg ) 
					szMsg = szNull;
				int iStrLen = strlen(szMsg);

				AIACT02_STR *pNewACT = (AIACT02_STR*) new BYTE[ sizeof(AIACT02_STR) + iStrLen+1 ];
				memcpy( pNewACT, m_ppActionLIST[ iA ], sizeof(AIACT02)-sizeof(int) );
				strcpy( pNewACT->szCon, szMsg );
				pNewACT->szCon[ iStrLen ] = NULL;

				//LogString( 0xffff,"AI02 STR[%d:%d] :: %s\n", iLangCol, m_ppActionLIST[ iA ]->st02.iStrID-1, pNewACT->szCon );

				SAFE_DELETE_ARRAY( m_ppActionLIST[ iA ] );
				m_ppActionLIST[ iA ] = (AI_ACTION*)pNewACT;
				break;
			}
			case AIACT_03 : ( (AIACT03*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_04 : ( (AIACT04*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_05 : break;
			case AIACT_06 : ( (AIACT06*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_07 : break;
			case AIACT_08 : ( (AIACT08*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_09 : break;
			case AIACT_10 : break;
			case AIACT_11 : ( (AIACT11*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_12 : break;
			case AIACT_13 : break;
			case AIACT_14 : ( (AIACT14*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_15 : break;
			case AIACT_16 : ( (AIACT16*)m_ppActionLIST[ iA ] )->iDistance *= 100;	break;
			case AIACT_28 : 
			{
				char *szMsg = pSTB->GetValueSTR( iLangCol, m_ppActionLIST[ iA ]->st28.iStrID-1 );
				char *szNull = "unknown";
				if ( !szMsg ) 
					szMsg = szNull;
				int iStrLen = strlen(szMsg);

				AIACT28_STR *pNewACT = (AIACT28_STR*) new BYTE[ sizeof(AIACT28_STR) + iStrLen+1 ];
				memcpy( pNewACT, m_ppActionLIST[ iA ], sizeof(AIACT28)-sizeof(int) );
				strcpy( pNewACT->szMsg, szMsg );
				pNewACT->szMsg[ iStrLen ] = NULL;

				//LogString( 0xffff,"AI28 STR[%d:%d] :: %s\n", iLangCol, m_ppActionLIST[ iA ]->st28.iStrID-1, pNewACT->szMsg );

				SAFE_DELETE_ARRAY( m_ppActionLIST[ iA ] );
				m_ppActionLIST[ iA ] = (AI_ACTION*)pNewACT;

				break;
			}
		}
	}

	return true;
}
#endif
//-------------------------------------------------------------------------------------------------
bool CAI_EVENT::Check (t_AIPARAM *pAIParam)
{
	for (int iC=0; iC<m_iConditionCNT; iC++) {
		// check condition ...
		if ( !g_FuncCOND[ (m_ppConditionLIST[ iC ]->m_Header.Type & 0x00ff) ].m_fpCondition( &m_ppConditionLIST[ iC ]->m_Header, pAIParam ) )
			return false;
	}

	// doing action ...
	for (int iA=0; iA<m_iActionCNT; iA++) {
		g_FuncACTION[ (m_ppActionLIST[ iA ]->m_Header.Type & 0x00ff ) ].m_fpAction( &m_ppActionLIST[ iA ]->m_Header, pAIParam );
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
CAI_PATTERN::CAI_PATTERN ()
{
	m_iEventCNT = 0;
	m_pEvent = NULL;
}
CAI_PATTERN::~CAI_PATTERN ()
{
	SAFE_DELETE_ARRAY( m_pEvent );
}
//-------------------------------------------------------------------------------------------------
#ifdef	__SERVER
bool CAI_PATTERN::Load (FILE *fp, STBDATA *pSTB, int iLangCol)
{
	stPattern	Pattern;
	fread (&Pattern,	sizeof(stPattern),	1,	fp);

//	LogString (LOG_NORMAL, "%s, %d \n", Pattern.szName, Pattern.iNumOfEvent );
	m_iEventCNT = Pattern.iNumOfEvent;
	if ( m_iEventCNT > 0 )
		m_pEvent = new CAI_EVENT[ m_iEventCNT ];

	for (int iE=0; iE<Pattern.iNumOfEvent; iE++) {
		if ( !m_pEvent[ iE ].Load( fp, pSTB, iLangCol ) )
			return false;
	}

	return true;
}
#else
//-------------------------------------------------------------------------------------------------
bool CAI_PATTERN::Load (CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol )
{
	stPattern	Pattern;
	pFileSystem->Read( &Pattern, sizeof(stPattern) );

//	LogString (LOG_NORMAL, "%s, %d \n", Pattern.szName, Pattern.iNumOfEvent );
	m_iEventCNT = Pattern.iNumOfEvent;
	if ( m_iEventCNT > 0 )
		m_pEvent = new CAI_EVENT[ m_iEventCNT ];

	for (int iE=0; iE<Pattern.iNumOfEvent; iE++) {
		m_pEvent[ iE ].Load( pFileSystem, pSTB, iLangCol );
	}
	return true;
}
#endif
//-------------------------------------------------------------------------------------------------
void CAI_PATTERN::Check (t_AIPARAM *pAIParam)
{
	for (int iE=0; iE<m_iEventCNT; iE++) {
		if ( m_pEvent[ iE ].Check( pAIParam ) )
			return;
	}
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CAI_FILE::CAI_FILE ()
{
	m_iPatternCNT = 0;
	m_pPattern = NULL;
}
CAI_FILE::~CAI_FILE ()
{
	SAFE_DELETE_ARRAY( m_pPattern );
}

//-------------------------------------------------------------------------------------------------
#ifdef	__SERVER
bool CAI_FILE::Load (char *szFileName, STBDATA *pSTB, int iLangCol)
{
	FILE *fp;

	fp = fopen(szFileName, "rb");
	if ( fp == NULL ) {
		return false;
	}

	AI_FILE_HEADER	sFileH;
	fread( &sFileH,	sizeof(AI_FILE_HEADER),	1,					fp);
	/*
	char *szStr=CStr::GetString ();
	fread( szStr,	sizeof(char),			sFileH.iNumOfTitle,	fp);
	*/
	fseek(fp, sFileH.iNumOfTitle, SEEK_CUR);

	m_ulCheckStopAI  = sFileH.iSecond * 1000;
	m_iRateDamagedAI = sFileH.iSecondOfAttackMove;

	m_iPatternCNT = sFileH.iNumOfPattern;
	m_pPattern = new CAI_PATTERN [ m_iPatternCNT ];

	for (int iP=0; iP<sFileH.iNumOfPattern; iP++) {
		if ( !m_pPattern[ iP ].Load( fp, pSTB, iLangCol ) ) {
			::MessageBox( NULL, szFileName, "AI File ERROR", MB_OK );
			fclose(fp);
			return false;
		}
	}

	fclose(fp);
	return true;
}
#else
bool CAI_FILE::Load (char *szFileName, STBDATA *pSTB, int iLangCol)
{	
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFileName ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", szFileName );
		g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}

	AI_FILE_HEADER	sFileH;
	pFileSystem->Read( &sFileH,	sizeof(AI_FILE_HEADER) );
	
	//char Buf[255];
	//pFileSystem->Read( Buf, sFileH.iNumOfTitle );
	pFileSystem->Seek( sFileH.iNumOfTitle, FILE_POS_CUR );	

	m_ulCheckStopAI  = sFileH.iSecond * 1000;
	m_iRateDamagedAI = sFileH.iSecondOfAttackMove;

	m_iPatternCNT = sFileH.iNumOfPattern;
	m_pPattern = new CAI_PATTERN [ m_iPatternCNT ];

	for (int iP=0; iP<sFileH.iNumOfPattern; iP++) 
	{
		m_pPattern[ iP ].Load( pFileSystem, pSTB, iLangCol );
	}

	pFileSystem->CloseFile();	
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return true;
}
#endif
//-------------------------------------------------------------------------------------------------
void CAI_FILE::AI_Check (WORD wPattern, CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage)
{
	if ( wPattern < m_iPatternCNT ) {
		t_AIPARAM aiParam;

		aiParam.m_pSourCHAR = pSourCHAR;
		aiParam.m_pDestCHAR = pDestCHAR;
		aiParam.m_pFindCHAR = NULL;
		aiParam.m_pNearCHAR = NULL;
		aiParam.m_iDamage   = iDamage;

#ifdef	__SERVER
		aiParam.m_pLocalNPC  = NULL;
#endif

		m_pPattern[ wPattern ].Check( &aiParam );
	}
}

//-------------------------------------------------------------------------------------------------
void CAI_FILE::AI_WhenCREATED (CAI_OBJ *pSourCHAR)
{	
	AI_Check( 0, pSourCHAR );
}

void CAI_FILE::AI_WhenSTOP (CAI_OBJ *pSourCHAR)
{
	unsigned long lCurTime;

#ifndef	__SERVER
	lCurTime = g_GameDATA.GetGameTime();
#else
	lCurTime = ::timeGetTime();
#endif
	if ( lCurTime - pSourCHAR->Get_AICheckTIME( 0 ) < m_ulCheckStopAI	) return;
	pSourCHAR->Set_AICheckTIME( 0, lCurTime );

	AI_Check( 1, pSourCHAR );
}

void CAI_FILE::AI_WhenAttackMOVE (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR)
{	
	AI_Check( 2, pSourCHAR, pDestCHAR );
}

void CAI_FILE::AI_WhenDAMAGED (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage)
{	
	if ( pSourCHAR->Get_TARGET() ) {
		// 확률계산...
		if ( m_iRateDamagedAI < AI_SysRANDOM( 100 ) )
			return;
	}

	AI_Check( 3, pSourCHAR, pDestCHAR, iDamage );
}

void CAI_FILE::AI_WhenKILL (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage)
{
	AI_Check( 4, pSourCHAR, pDestCHAR, iDamage );
}

void CAI_FILE::AI_WhenDEAD (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage)
{	
	AI_Check( 5, pSourCHAR, pDestCHAR, iDamage );
}

//-------------------------------------------------------------------------------------------------
