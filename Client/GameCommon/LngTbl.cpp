#include "stdafx.h"
#include "LngTbl.h"
#include <stdio.h>
#include "StringManager.h"
#include "Util\\VFSManager.h"
#include "..\Util\localizing.h"

const char * _LtbPath [] = { LANGSTB_AI_NAME , LANGSTB_QST_NAME , LANGSTB_CON_NAME };



// 지울때는 NULL 을 szStr에 전달.
void tagStrTblDATA::Set(const WCHAR *szStr, short nLen)
{
	if ( m_pStr ) {
		delete[] m_pStr;
		m_pStr = NULL;
		m_nStrLEN = 0;
	}

	if ( szStr ) {
		m_pStr = new WCHAR[ nLen + 1 ];
		wcsncpy ( m_pStr, szStr, nLen );
		m_pStr[ nLen ] = L'\0';
		m_nStrLEN = nLen + 1;
	}
}

void tagStrTblDATA::Clear (void)
{
	Set (NULL, 0);
}

tagStrTblDATA::tagStrTblDATA () 
{
	m_pStr = NULL;
	m_lFilePOS = 0;
	m_nStrLEN = 0;
}

tagStrTblDATA::~tagStrTblDATA ()
{
	if ( m_pStr ) {
		delete[] m_pStr;
		m_pStr = NULL;
	}
}


AStringTable::AStringTable ()
{
	m_pFS = NULL;
	m_ppFilePOS = NULL;
	m_nTmpStrLen = 1024;
	m_nMbcsTmpStrLen = 0;
	m_szMbcBuffer = NULL;
	
	m_pTmpStr = new WCHAR[ m_nTmpStrLen ];

	m_pData = NULL;
}

AStringTable::~AStringTable ()
{
	this->Clear ();
	this->Close ();
	if(m_pTmpStr)
	{
		delete [] m_pTmpStr;
		m_pTmpStr = NULL;
	}
	if(m_szMbcBuffer)
	{
		delete [] m_szMbcBuffer;
		m_szMbcBuffer = NULL;
		m_nMbcsTmpStrLen = 0;
	}
}


void AStringTable::Clear ()
{

	if(m_pData)
	{
		delete [] m_pData;
		m_pData = NULL;
	}

	if ( m_ppFilePOS ) {
		for (int iR=0; iR<m_iRowCNT; iR++) 
		{
			delete[] m_ppFilePOS[iR];
			m_ppFilePOS[iR] = NULL;
		}

		delete[] m_ppFilePOS;
		m_ppFilePOS = NULL;
	}
	m_iRowCNT = m_iColCNT = 0;
}

bool AStringTable::Open (const char *szFileName)
{
	if ( m_pFS )
	{
		m_pFS->CloseFile ();
		(CVFSManager::GetSingleton()).ReturnToManager( m_pFS );
		m_pFS = NULL;
	}

	// m_FP = fopen( szFileName, "rb" );
	m_pFS = (CVFSManager::GetSingleton()).GetFileSystem();
	if( m_pFS->OpenFile( szFileName ) == false )	
		return false;

	m_pFS->ReadInt32 ( &m_iColCNT);
	m_pFS->ReadInt32 ( &m_iRowCNT);


	m_ppFilePOS = new tagStrTblDATA* [ m_iRowCNT ];
	
	int iDataPos = sizeof (int) * 2 + m_iColCNT * m_iRowCNT * (sizeof (long) + sizeof (short));

	int iR, iC;
	for (iR=0; iR<m_iRowCNT; iR++) 
	{
		m_ppFilePOS[ iR ] = new tagStrTblDATA[ m_iColCNT ];
		for (iC=0; iC<m_iColCNT; iC++) 
		{
			m_pFS->Read (&m_ppFilePOS[iR][iC].m_lFilePOS,	sizeof(long));

			if(m_ppFilePOS[iR][iC].m_lFilePOS > 0)
				m_ppFilePOS[iR][iC].m_lFilePOS -= iDataPos;

			m_pFS->ReadInt16 (&m_ppFilePOS[iR][iC].m_nStrLEN);
		}
	}

	return true;
}

void  AStringTable::Close ()
{
	//if ( m_FP ) {
	//	fclose( m_FP );
	//	m_FP = NULL;
	//}
	if ( m_pFS )
	{
		m_pFS->CloseFile ();
		(CVFSManager::GetSingleton()).ReturnToManager( m_pFS );
		m_pFS = NULL;
	}
}




short AStringTable::GetLength(WORD wColIDX, WORD wRowIDX)
{
	if ( wRowIDX >= m_iRowCNT || wColIDX >= m_iColCNT )
		return -1;
	return m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN;
}

bool AStringTable::Load (const char *szFileName)
{
	if (this->Open(szFileName) )
	{
		long lDataPOS = sizeof (int) * 2 + m_iColCNT * m_iRowCNT * (sizeof (long) + sizeof (short));
		long lReadSize = 0;
		if(m_pFS->Seek ( 0, FILE_POS_END) == true)
		{
			long lEndPOS = m_pFS->Tell ();
			lReadSize = lEndPOS - lDataPOS;

			if(m_pFS->Seek ( lDataPOS, FILE_POS_SET) == true)
			{
				m_pData = (WCHAR *)new char[ lReadSize ];
				if(m_pData)
				{
					m_pFS->Read ((void *)m_pData, sizeof (char) * lReadSize);
					Close ();

					return true;
				}
			}
		}
	}

	return false;
}

// 파일에서 읽지 않고 메모리에서 가져옴.
WCHAR * AStringTable::GetWString (WORD wColIDX, WORD wRowIDX)
{
	if ( wRowIDX >= m_iRowCNT || wColIDX >= m_iColCNT )
		return NULL;
	if ( m_ppFilePOS[ wRowIDX ][ wColIDX ].m_nStrLEN <= 0)
		return NULL;

	const char * pStrPOS = (char *)m_pData + m_ppFilePOS[ wRowIDX ][ wColIDX ].m_lFilePOS;
	return (WCHAR *)pStrPOS;
}

char * AStringTable::GetMbcsString (WORD wColIDX, WORD wRowIDX)
{
	WCHAR * szWStr = GetWString (wColIDX, wRowIDX);
	if(szWStr)
	{
		short nWCharLngth = (short)(wcslen (szWStr) * sizeof (WCHAR) + 2);
		if(m_nMbcsTmpStrLen < nWCharLngth)
		{
			delete [] m_szMbcBuffer ;
			m_nMbcsTmpStrLen = nWCharLngth;
			m_szMbcBuffer = new char[ m_nMbcsTmpStrLen ];
		}

		int iWrtCNT = WideCharToMultiByte(	CP_ACP,
								0, 
								szWStr, 
								-1, 
								m_szMbcBuffer, 
								m_nMbcsTmpStrLen, 
								0, 
								0 );
		m_szMbcBuffer[ iWrtCNT ] = '\0';

		return m_szMbcBuffer;
	}

	return NULL;
}

int AStringTable::GetRowCNT (void)
{
	return m_iRowCNT;
}

int AStringTable::GetClmnCNT (void)
{
	return m_iColCNT;
}

int AStringTable::FindEmptyClmn (int iClmnIDX)
{
	for(int i = 0; i < m_iRowCNT; i++)
	{
		if(m_ppFilePOS[ i ][ iClmnIDX ].m_nStrLEN == 0)
			return i;
	}

	return -1;
}

int AStringTable::FindEmptyRow (void)
{
	for(int i = 0; i < m_iRowCNT; i++)
	{
		if(m_ppFilePOS[ i ][ 0 ].m_nStrLEN == 0)
		{
			bool bAllEmpty = true;
			for(int c = 1; c < m_iColCNT; c++)
			{
				if(m_ppFilePOS[ i ][ c ].m_nStrLEN != 0)
				{
					bAllEmpty = false;
					break;
				}
			}

			if(bAllEmpty)
				return i;
		}
	}

	return -1;
}

// CStringManager::GetSingleton().GetFontNameByCharSet( CLocalizing::GetSingleton().GetCurrentCharSet() );


CLtbManager::CLtbManager ()
{
	m_LngType	= LANGUAGE_KOR;
	m_pAI		= NULL;
	m_pQuest	= NULL;
	m_pEvent	= NULL;
}

CLtbManager::~CLtbManager ()
{
	if(m_pAI)
	{
		delete m_pAI;
		m_pAI = NULL;
	}
	if(m_pQuest)
	{
		delete m_pQuest;
		m_pQuest = NULL;
	}
	if(m_pEvent)
	{
		delete m_pEvent;
		m_pEvent = NULL;
	}
}

bool CLtbManager::Load ( int iLngType )
{
	m_LngType = (LANGUAGE_TYPE)iLngType;
	
	/*
	m_pAI = new AStringTable ();
	if(!m_pAI)
		return false;
	if(!m_pAI->Load (_LtbPath[ LTB_AI ]))
		return false;
	
	m_pQuest = new AStringTable ();
	if(!m_pQuest)
		return false;
	if(!m_pQuest->Load (_LtbPath[ LTB_QUEST ]))
		return false;
	*/

	m_pEvent = new AStringTable ();
	if(!m_pEvent)
	{
		assert (false);
		return false;
	}
	if(!m_pEvent->Load (_LtbPath[ LTB_EVENT ]))
	{
		assert (false);
		return false;
	}

	/*const char * pMSG = NULL;
	for(int r = 0; r < m_pEvent->GetRowCNT (); r++)
	{
		for(int c = 0; c < m_pEvent->GetClmnCNT (); c++)
		{
			pMSG = m_pEvent->GetMbcsString (r, c);
		}
	}*/

	return true;
}

//const char * CLtbManager::GetAiString (int StrID)
//{
//	if(m_pAI)
//		return m_pAI->GetMbcsString (m_LngType + 1, StrID);
//
//	return NULL;
//}
//
//const char * CLtbManager::GetQuestString (int StrID)
//{
//	if(m_pQuest)
//		return m_pQuest->GetMbcsString (m_LngType + 1, StrID);
//
//	return NULL;
//}
//
const char * CLtbManager::GetEventString (int StrID)
{
	if(m_pEvent)
		return m_pEvent->GetMbcsString (m_LngType + 1, StrID);

	return NULL;
}

CLtbManager g_LngTBL;