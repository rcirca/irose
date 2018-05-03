/*
	$History: classSTB.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Netggun      Date: 04-08-16   Time: 11:01a
 * Updated in $/Client/Util
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-04-02   Time: 3:30p
 * Updated in $/Client/Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-11-21   Time: 11:01a
 * Created in $/Client/Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-07-03   Time: 11:38a
 * Created in $/LIB_Util
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 03-05-20   Time: 9:34a
 * Updated in $/Client/Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-05-20   Time: 9:22a
 * Created in $/Client/Util
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 03-03-17   Time: 7:55p
 * Updated in $/Client/Util
*/

#include <windows.h>
#include <stdlib.h>
#include "classSTB.h"
#include "classSTR.h"

//-------------------------------------------------------------------------------------------------
classSTB::classSTB()
{	
	m_fp		= NULL;	
	m_pOffset	= NULL;
	m_RowNAME	= NULL;

	m_iRowCount	=m_iColCount=0;	
	m_pDataOFF	= NULL;
}

//-------------------------------------------------------------------------------------------------
classSTB::~classSTB()
{	
	if ( m_fp ) 
		fclose(m_fp);

	if ( m_RowNAME )
		delete[] m_RowNAME;

	if ( m_pOffset )
		delete[] m_pOffset;

	if ( m_ppData )
		delete[] m_pDataOFF;
}

//-------------------------------------------------------------------------------------------------
bool classSTB::Open(char *szFileName, long lFilePtr)
{
    union {
        char    cValue[ 4 ];
        long    lValue;
    } u;

    m_fp = fopen (szFileName, "rb");
	if ( m_fp == NULL ) {
		::MessageBox (NULL, "File open error...", szFileName, MB_OK);
		return false;
	}

	if ( lFilePtr >= 0 ) {
		m_lFP = lFilePtr + sizeof(long);	// GetFIlePos...
	} else
		m_lFP = 0;

	fseek (m_fp, m_lFP, SEEK_SET);

    fread( &u.lValue, 1, sizeof(long), m_fp);

	int iVersion=0;
    if ( u.cValue[ 0 ] == 'S' &&
         u.cValue[ 1 ] == 'T' &&
         u.cValue[ 2 ] == 'B' ) {
        iVersion = u.cValue[ 3 ] - '0';

	    // 열, 줄 데이타 몸체가 들어있는곳...
        fread (&u.lValue, 1, sizeof(long), m_fp);
    }

    // 열 갯수, 줄 갯수
    fread( &m_iRowCount, 1, sizeof(int), m_fp);
    fread( &m_iColCount, 1, sizeof(int), m_fp);

	m_lFP += u.lValue;

	// 줄높이.
	fseek( m_fp, sizeof(int),	SEEK_CUR);

    switch ( iVersion ) {
		case 0 :
			fseek( m_fp, sizeof(int),	SEEK_CUR);
			break;
		case 1 :
			fseek( m_fp, sizeof(short)*(m_iColCount+1),	SEEK_CUR);
			break;
    }
	
	// 열 이름..
	short nI, nLen;
	for (nI=0; nI<m_iColCount; nI++) {
		fread( &nLen,	sizeof(short),	1,	m_fp);
		fseek( m_fp,	nLen, SEEK_CUR);
	}
	
	// 데이타 이름.
	m_RowNAME = new CStrVAR[ m_iRowCount ];
	char *pStr = CStr::GetString ();

	// skip colume title line ...
	fread( &nLen,	sizeof(short),	1,		m_fp);
	fseek( m_fp,	nLen,	SEEK_CUR);

	for (nI=0; nI<m_iRowCount-1; nI++) {
		fread( &nLen,	sizeof(short),	1,		m_fp);
		fread( pStr,	sizeof(char),	nLen,	m_fp);
		pStr[ nLen ] = 0;
		m_RowNAME[ nI ].Set( pStr );
	}

	m_iRowCount --;
	m_iColCount --;

	m_iRowIndex = 0;
	m_iColIndex = 0;

    fseek (m_fp, m_lFP, SEEK_SET);
/*
	m_pOffset = new long [ m_iRowCount ];
	::ZeroMemory (m_pOffset, sizeof(long) * m_iRowCount);
	m_pOffset[ 0 ] = ftell(m_fp);
*/
	m_pDataOFF = new tagSTB [ m_iRowCount * m_iColCount ];
	::ZeroMemory (m_pDataOFF, sizeof(tagSTB) * m_iRowCount * m_iColCount);

	short nStrLen;
	for (int iR=0; iR<m_iRowCount; iR++) {
		for (int iC=0; iC<m_iColCount; iC++) {
			fread (&m_pDataOFF[ iR*m_iColCount + iC ].m_nStrLen, 1, sizeof(short), m_fp);
			m_pDataOFF[ iR*m_iColCount + iC ].m_lFilePOS = ftell( m_fp );

			if ( m_pDataOFF[ iR*m_iColCount + iC ].m_nStrLen ) 
				fseek (m_fp, m_pDataOFF[ iR*m_iColCount + iC ].m_nStrLen, SEEK_CUR);
			
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
void classSTB::Close ()
{
	if ( m_fp ) {
		fclose(m_fp);
		m_fp = NULL;
	}

	if ( m_pOffset ) {
		delete[] m_pOffset;
		m_pOffset = NULL;
	}

	if ( m_pDataOFF ) {
		delete[] m_pDataOFF;
		m_pDataOFF = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
bool classSTB::SetIndexPosition (int iCol, int iRow)
{
	if ( iRow >= m_iRowCount ||
	     iCol >= m_iColCount ) 
		return false;

	if ( m_iColIndex == iCol && m_iRowIndex == iRow )
		return true;

	int   iX;
	short nStrLen;

	if ( !m_pOffset[ iRow ] ) {
		// Back Move Row & Make Table Index to iROW..
		int iY;

		for (iY=iRow-1; m_pOffset[ iY ]==0; iY--) ;

		fseek (m_fp, m_pOffset[ iY ], SEEK_SET);
		for (  ;iY <= iRow; iY++) {
			m_pOffset[ iY ] = ftell (m_fp);

			for (iX=0; iX<m_iColCount; iX++) {
				fread (&nStrLen, 1, sizeof(short), m_fp);
				if ( nStrLen ) 
					fseek (m_fp, nStrLen, SEEK_CUR);
			}
		}
	} 
	fseek (m_fp, m_pOffset[ iRow ], SEEK_SET);

	// Move Column ...
	for (iX=0; iX < iCol; iX++) {
		fread (&nStrLen, 1, sizeof(short), m_fp);
		if ( nStrLen ) 
			fseek (m_fp, nStrLen, SEEK_CUR);
	}

	m_iColIndex = iCol;
	m_iRowIndex = iRow;

	return true;
}


//-------------------------------------------------------------------------------------------------
int  classSTB::GetInteger(int iCol, int iRow)
{
	if ( m_pDataOFF[ iRow*m_iColCount+iCol ].m_nStrLen ) {
		fseek( m_fp, m_pDataOFF[ iRow*m_iColCount+iCol ].m_lFilePOS, SEEK_SET );
		fread (m_szValue, nStrLen, sizeof(char), m_fp);
		m_szValue[ nStrLen ] = '\0';

		return atoi( m_szValue );
	}
	return 0;

	/*
	if ( !SetIndexPosition(iCol, iRow) )
		return 0;

	return GetInteger();
	*/
}


//-------------------------------------------------------------------------------------------------
char *classSTB::GetString(int iCol, int iRow)
{
	if ( m_pDataOFF[ iRow*m_iColCount+iCol ].m_nStrLen ) {
		fseek( m_fp, m_pDataOFF[ iRow*m_iColCount+iCol ].m_lFilePOS, SEEK_SET );
		fread (m_szValue, nStrLen, sizeof(char), m_fp);
		m_szValue[ nStrLen ] = '\0';

		return m_szValue;
	}
	return NULL;
	/*
	if ( !SetIndexPosition(iCol, iRow) ) 
		return NULL;

	return GetString();
	*/
}


//-------------------------------------------------------------------------------------------------
int  classSTB::GetInteger()
{
	if ( feof (m_fp ) )
		return 0;

	short nStrLen;

	if ( m_iColIndex >= m_iColCount ) {
		if ( m_iRowIndex >= m_iRowCount )
			return 0;

		m_iRowIndex ++;
		m_iColIndex = 0;
	}
	
	if ( m_iColIndex++ == 0 )
		m_pOffset[ m_iRowIndex ] = ftell(m_fp);

	fread (&nStrLen, 1, sizeof(short), m_fp);
	if ( nStrLen && nStrLen < MAX_PATH ) {
		fread (m_szValue, nStrLen, sizeof(char), m_fp);
		m_szValue[ nStrLen ] = '\0';

		return atoi ( m_szValue );
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
char *classSTB::GetString()
{
	if ( feof (m_fp ) )
		return NULL;

	short nStrLen;

	if ( m_iColIndex >= m_iColCount ) {
		if ( m_iRowIndex >= m_iRowCount ) 
			return NULL;

		m_iRowIndex ++;
		m_iColIndex = 0;
	}
	
	if ( m_iColIndex++ == 0 )
		m_pOffset[ m_iRowIndex ] = ftell(m_fp);
	
	fread (&nStrLen, 1, sizeof(short), m_fp);
	if ( nStrLen && nStrLen < MAX_PATH ) {
		fread (m_szValue, nStrLen, sizeof(char), m_fp);
		m_szValue[ nStrLen ] = '\0';

		return m_szValue;
	}

	return NULL;
}


//-------------------------------------------------------------------------------------------------
