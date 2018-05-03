#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include "CGameSTB.h"
#include "classSTR.h"


//-------------------------------------------------------------------------------------------------
CGameSTB::CGameSTB()
{	
	m_pMemStream = new AMemStream;

	m_pFileSystem = NULL;

	m_pOffset	= NULL;
	m_RowNAME	= NULL;

	m_iRowCount=m_iColCount=0;	
}

//-------------------------------------------------------------------------------------------------
CGameSTB::~CGameSTB()
{	
	SAFE_DELETE( m_pMemStream );

	if ( m_pFileSystem ) 
		m_pFileSystem->CloseFile();

	if ( m_RowNAME )
		delete[] m_RowNAME;

	if ( m_pOffset )
		delete[] m_pOffset;
}

//-------------------------------------------------------------------------------------------------

bool CGameSTB::Open( char *szFileName )
{
    union {
        char    cValue[ 4 ];
        long    lValue;
    } u;

	m_lFP = 0;

	m_pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( m_pFileSystem->OpenFile( szFileName ) == false )
	{
		//::MessageBox (NULL, "File open error...", szFileName, MB_OK);
		g_pCApp->ErrorBOX("File open error...", szFileName);
		(CVFSManager::GetSingleton()).ReturnToManager( m_pFileSystem );
		return false;
	}

	////////////////////////////////////////////////////////
	m_pMemStream->Alloc( m_pFileSystem->GetSize() );
	m_pFileSystem->Read( m_pMemStream->m_pStream, m_pMemStream->m_dwSize );
	////////////////////////////////////////////////////////

    m_pMemStream->Read( &u.lValue, sizeof(long) );

	int iVersion=0;
    if ( u.cValue[ 0 ] == 'S' &&
         u.cValue[ 1 ] == 'T' &&
         u.cValue[ 2 ] == 'B' ) {
        iVersion = u.cValue[ 3 ] - '0';

	    // 열, 줄 데이타 몸체가 들어있는곳...
		m_pMemStream->Read( &u.lValue, sizeof(long) );        
    }

    // 열 갯수, 줄 갯수
	m_pMemStream->ReadInt32( &m_iRowCount );
	m_pMemStream->ReadInt32( &m_iColCount );    

	m_lFP += u.lValue;

	// 줄높이.
	m_pMemStream->Seek( sizeof(int), FILE_POS_CUR );	

    switch ( iVersion ) 
	{
		case 0 :
			m_pMemStream->Seek( sizeof(int), FILE_POS_CUR );				
			break;
		case 1 :
			m_pMemStream->Seek( sizeof(short)*(m_iColCount+1), FILE_POS_CUR );				
			break;
    }
	
	// 열 이름..
	short nI, nLen;
	for (nI=0; nI<m_iColCount; nI++) 
	{		
		m_pMemStream->ReadInt16( &nLen );
		m_pMemStream->Seek( nLen, FILE_POS_CUR );
	}
	
	// 데이타 이름.
	m_RowNAME = new CStrVAR[ m_iRowCount ];
	char *pStr = CStr::GetString ();

	// skip colume title line ...
	m_pMemStream->ReadInt16( &nLen );
	m_pMemStream->Seek( nLen, FILE_POS_CUR );

	for (nI=0; nI<m_iRowCount-1; nI++) 
	{
		m_pMemStream->ReadInt16( &nLen );
		m_pMemStream->Read( pStr, sizeof(char) * nLen );
		pStr[ nLen ] = 0;
		m_RowNAME[ nI ].Set( pStr );
	}

	m_iRowCount --;
	m_iColCount --;

	m_iRowIndex = 0;
	m_iColIndex = 0;

    m_pMemStream->Seek( m_lFP, FILE_POS_SET );

	m_pOffset = new long [ m_iRowCount ];
	ZeroMemory (m_pOffset, sizeof(long) * m_iRowCount);
	m_pOffset[ 0 ] = m_pMemStream->Tell();

	return true;
}


//-------------------------------------------------------------------------------------------------
void CGameSTB::Close ()
{
	m_pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( m_pFileSystem );

	if ( m_pOffset ) {
		delete[] m_pOffset;
		m_pOffset = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
bool CGameSTB::SetIndexPosition (int iCol, int iRow)
{
	if ( iRow >= m_iRowCount ||
	     iCol >= m_iColCount ) 
		return false;

	if ( m_iColIndex == iCol && m_iRowIndex == iRow )
		return true;

	int   iX;
	short nStrLen;

	if ( !m_pOffset[ iRow ] ) 
	{
		// Back Move Row & Make Table Index to iROW..
		int iY;

		for (iY=iRow-1; m_pOffset[ iY ]==0; iY--) ;

		m_pMemStream->Seek( m_pOffset[ iY ], FILE_POS_SET );		
		for (  ;iY <= iRow; iY++) 
		{
			m_pOffset[ iY ] = m_pMemStream->Tell();

			for (iX=0; iX<m_iColCount; iX++) 
			{
				m_pMemStream->ReadInt16( &nStrLen );
				if ( nStrLen ) 
					m_pMemStream->Seek( nStrLen, FILE_POS_CUR );
			}
		}
	} 
	m_pMemStream->Seek( m_pOffset[ iRow ], FILE_POS_SET );

	// Move Column ...
	for (iX=0; iX < iCol; iX++) 
	{
		m_pMemStream->ReadInt16( &nStrLen );
		if ( nStrLen ) 
			m_pMemStream->Seek( nStrLen, FILE_POS_CUR );
	}

	m_iColIndex = iCol;
	m_iRowIndex = iRow;

	return true;
}


//-------------------------------------------------------------------------------------------------
int  CGameSTB::GetInteger(int iCol, int iRow)
{
	if ( !SetIndexPosition(iCol, iRow) )
		return 0;

	return GetInteger();
}


//-------------------------------------------------------------------------------------------------
char *CGameSTB::GetString(int iCol, int iRow)
{
	if ( !SetIndexPosition(iCol, iRow) ) 
		return NULL;

	return GetString();
}


//-------------------------------------------------------------------------------------------------
int  CGameSTB::GetInteger()
{
	if( m_pMemStream->IsEOF() )
		return 0;

	short nStrLen;
	int   iValue;

	if ( m_iColIndex >= m_iColCount ) {
		if ( m_iRowIndex >= m_iRowCount )
			return 0;

		m_iRowIndex ++;
		m_iColIndex = 0;
	}
	
	if ( m_iColIndex++ == 0 )
		m_pOffset[ m_iRowIndex ] = m_pMemStream->Tell();


	m_pMemStream->ReadInt16( &nStrLen );
	if ( nStrLen ) 
	{
		char *pStr;

		pStr = new char [ nStrLen+1 ];
		
		m_pMemStream->Read( pStr, nStrLen * sizeof(char) );
		pStr[ nStrLen ] = '\0';

		iValue = atoi ( pStr );

		delete[] pStr;

		return iValue;
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
char *CGameSTB::GetString()
{
	short nStrLen;

	if ( m_iColIndex >= m_iColCount ) {
		if ( m_iRowIndex >= m_iRowCount ) 
			return NULL;

		m_iRowIndex ++;
		m_iColIndex = 0;
	}
	
	if ( m_iColIndex++ == 0 )
		m_pOffset[ m_iRowIndex ] = m_pMemStream->Tell();

	
	m_pMemStream->ReadInt16( &nStrLen );
	if ( nStrLen && nStrLen < sizeof( m_szValue )  )
	{
		m_pMemStream->Read( m_szValue, nStrLen * sizeof(char) );
		m_szValue[ nStrLen ] = '\0';

		return m_szValue;
	}

	return NULL;
}


//-------------------------------------------------------------------------------------------------
