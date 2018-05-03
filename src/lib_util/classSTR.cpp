/*
	$History: classSTR.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 05-06-20   Time: 7:18p
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 05-06-20   Time: 7:18p
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-05-27   Time: 8:32p
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:09a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:51a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-07-03   Time: 11:38a
 * Created in $/LIB_Util
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <crtdbg.h>
#include "classSTR.h"
#include "classHASH.h"

bool			CStr::m_bInit = false;
unsigned short	CStr::m_wStaticStrLen;
char		   *CStr::m_pStaticStr;

char  **CStr::m_ppStr;	// [	CSTRING_BUFFER_CNT ][ CSTRING_BUFFER_LEN ];
short	CStr::m_nStrBuffLEN = CSTRING_BUFFER_LEN;
short	CStr::m_nStrBuffCNT = CSTRING_BUFFER_CNT;
short	CStr::m_nStrIdx;

//-------------------------------------------------------------------------------------------------
void CStr::Init (short nStrBufferCNT, short nStrBufferLEN)
{
	if ( nStrBufferCNT <= 0 || nStrBufferLEN <= 0 )
		return;

	m_nStrIdx = 0;

	m_wStaticStrLen = m_nStrBuffLEN;// CSTRING_BUFFER_LEN;
	m_pStaticStr = new char [ m_wStaticStrLen ];

	m_nStrBuffLEN = nStrBufferLEN;
	m_ppStr = new char*[ m_nStrBuffCNT ];
	for (short nI=0; nI<m_nStrBuffCNT; nI++) 
		m_ppStr[ nI ] = new char[ m_nStrBuffLEN ];

	m_bInit = true;
}
void CStr::Free (void)
{
	m_bInit = false;

	for (short nI=0; nI<m_nStrBuffCNT; nI++)
		delete[] m_ppStr[ nI ];
	delete[] m_ppStr;

	if ( m_pStaticStr )
		delete[] m_pStaticStr;
}

//-------------------------------------------------------------------------------------------------
void CStr::SetBufferLength (short nStrBufferLEN)
{
	if ( !m_bInit ) return;
	for (short nI=0; nI<m_nStrBuffCNT; nI++) {
		delete[] m_ppStr[ nI ];
		m_ppStr[ nI ] = new char[ nStrBufferLEN ];
	}
	m_nStrBuffLEN = nStrBufferLEN;
}
void CStr::SetBufferCount (short nStrBufferCNT)
{
	if ( !m_bInit ) return;

	short nI;
	for (nI=0; nI<m_nStrBuffCNT; nI++)
		delete[] m_ppStr[ nI ];
	delete[] m_ppStr;

	m_ppStr = new char*[ nStrBufferCNT ];
	for (nI=0; nI<m_nStrBuffCNT; nI++)
		m_ppStr[ nI ] = new char[ m_nStrBuffCNT ];
}

//-------------------------------------------------------------------------------------------------
int CStr::ParamCount (char *pStr)
{
	int   iCount=0;
	char  pDelimiters[] = " \t\n";
	char *pToken;
	short nStrLen = (short)strlen( pStr );

	if ( nStrLen >= m_wStaticStrLen ) {
		delete[] m_pStaticStr;
		m_wStaticStrLen = nStrLen+1;
		m_pStaticStr = new char [ m_wStaticStrLen ];
	}

	strncpy (m_pStaticStr, pStr, strlen(pStr) );
	m_pStaticStr[ nStrLen ] = 0;

	pToken = strtok( m_pStaticStr, pDelimiters );
	while( pToken )
	{
		iCount ++;

		// Get next token: 
		pToken = strtok( NULL, pDelimiters );
	}

	return iCount;
}

//-------------------------------------------------------------------------------------------------
void CStr::DebugString (char *fmt, ...)
{
	char *pStr;

	pStr = GetString ();

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(pStr,fmt,argptr);
    va_end(argptr);

	::OutputDebugString( pStr );
}

//-------------------------------------------------------------------------------------------------
char *CStr::Printf (char *fmt, ...)
{
	char *pStr;

	pStr = GetString ();

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(pStr,fmt,argptr);
    va_end(argptr);

	return pStr;
}

//-------------------------------------------------------------------------------------------------
char *CStr::ParamStr(char *pStr, int iIndex)
{
	int   iCount=0;
	char  pDelimiters[] = " \t\n";
	char *pToken;
	short nStrLen = (short)strlen( pStr );

	if ( nStrLen >= m_wStaticStrLen ) {
		delete[] m_pStaticStr;
		m_wStaticStrLen = nStrLen+1;
		m_pStaticStr = new char [ m_wStaticStrLen ];
	}

	strncpy (m_pStaticStr, pStr, strlen(pStr) );
	m_pStaticStr[ nStrLen ] = 0;

	pToken = strtok( m_pStaticStr, pDelimiters );
	while( pToken )
	{
		if ( iCount == iIndex )
			return pToken;
		iCount ++;

		// Get next token: 
		pToken = strtok( NULL, pDelimiters );
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
char *CStr::GetTokenFirst (char *pStr, char *pDelimiters)
{
	short nStrLen = (short)strlen( pStr );

	if ( nStrLen >= m_wStaticStrLen ) {
		delete[] m_pStaticStr;
		m_wStaticStrLen = nStrLen+1;
		m_pStaticStr = new char [ m_wStaticStrLen ];
	}

	strncpy (m_pStaticStr, pStr, strlen(pStr) );
	m_pStaticStr[ nStrLen ] = 0;

	return strtok( m_pStaticStr, pDelimiters );
}

//-------------------------------------------------------------------------------------------------
char *CStr::GetTokenNext (char *pDelimiters)
{
	return strtok( NULL, pDelimiters );
}

//-------------------------------------------------------------------------------------------------
char *CStr::ReadString (FILE *fp, bool bIgnoreWhiteSpace)
{
	char cChar, *szStrOut=CStr::GetString();
	bool bGetChar=false, bInDoubleQuote=false;
	int  iReadCnt=0;
	do {
		if ( fread (&cChar, sizeof(char), 1, fp) < 1 )
			break;

		if ( cChar == '\"' ) {
			bInDoubleQuote = !bInDoubleQuote;
			continue;
		}

		if ( (cChar==' ' || cChar=='\t' || cChar==0x0D || cChar==0x0A) ) {
			if ( !bInDoubleQuote && !bIgnoreWhiteSpace ) {
				if ( bGetChar ) break;
				continue;
			}

			if ( !bGetChar ) continue;
		}

		szStrOut[ iReadCnt++ ] = cChar;
		bGetChar = true;
	} while( cChar && !feof(fp) && iReadCnt < m_nStrBuffLEN-1 );

	szStrOut[ iReadCnt ] = 0;

	return szStrOut;
}

//-------------------------------------------------------------------------------------------------
bool CStr::SaveString (char *szFileName, char *szString)
{
	FILE *fp;

	fp = fopen (szFileName, "wt");
	if ( !fp )
		return false;

	fputs (szString, fp);

	fclose (fp);
	return true;
}

//-------------------------------------------------------------------------------------------------
unsigned int CStr::GetHASH (char const * szString)
{
	return StrToHashKey( szString );
}


//-------------------------------------------------------------------------------------------------
CStrVAR::CStrVAR(WORD wStrLen, WORD wStaticStrLen)
{
	m_pStr = NULL;
	m_pStaticStr = NULL;

	this->Alloc( wStrLen, wStaticStrLen );
}

char *CStrVAR::Alloc (WORD wStrLen, WORD wStaticStrLen)
{
	this->Del ();

	if ( wStrLen > 0 ) {
		m_wStrLen = wStrLen;
		m_pStr = new char[ wStrLen ];
	}

	return m_pStr;
}

void CStrVAR::Del ()
{	
	if ( m_pStr ) {
		delete[] m_pStr; 
		m_pStr=NULL;
	}
	m_wStrLen=0;

	if ( m_pStaticStr ) {
		delete[] m_pStaticStr;
		m_pStaticStr=NULL;
	}
	m_wStaticStrLen = 0;
}

void CStrVAR::Set (char *szStrIn)
{
	this->Del ();

	if ( szStrIn ) {
		m_wStrLen = strlen( szStrIn );
		if ( m_wStrLen > 0 ) {
			m_pStr = new char[ m_wStrLen+1 ];
			_ASSERT( m_pStr );
			strncpy(m_pStr, szStrIn, m_wStrLen);
			m_pStr[ m_wStrLen ] = 0;
			return;
		}
	}
	
	m_wStrLen = 0;
}

//-------------------------------------------------------------------------------------------------
char *CStrVAR::Printf (char *fmt, ...)
{
	if ( !m_pStr )
		return NULL;

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(m_pStr,fmt,argptr);
    va_end(argptr);

	return m_pStr;
}

//-------------------------------------------------------------------------------------------------
char *CStrVAR::GetTokenFirst (char *pStr, char *pDelimiters)
{
	short nStrLen = (short)strlen( pStr );

	if ( nStrLen >= m_wStaticStrLen ) {
		if ( m_pStaticStr )
			delete[] m_pStaticStr;

		m_wStaticStrLen = nStrLen+1;
		m_pStaticStr = new char [ m_wStaticStrLen ];
	}

	strncpy (m_pStaticStr, pStr, strlen(pStr) );
	m_pStaticStr[ nStrLen ] = 0;

	return strtok( m_pStaticStr, pDelimiters );
}


//-------------------------------------------------------------------------------------------------
char *CStrVAR::GetTokenNext (char *pDelimiters)
{
	return strtok( NULL, pDelimiters );
}

//-------------------------------------------------------------------------------------------------
unsigned int CStrVAR::GetHASH (char *szString)
{
	return StrToHashKey( szString );
}

//-------------------------------------------------------------------------------------------------
char *CStrVAR::ReadString (FILE *fp, bool bIgnoreWhiteSpace)
{
	char cChar;
	bool bGetChar=false, bInDoubleQuote=false;
	int  iReadCnt=0;
	do {
		if ( fread (&cChar, sizeof(char), 1, fp) < 1 )
			break;

		if ( cChar == '\"' ) {
			bInDoubleQuote = !bInDoubleQuote;
			continue;
		}

		if ( (cChar==' ' || cChar=='\t' || cChar==0x0D || cChar==0x0A) ) {
			if ( !bInDoubleQuote && !bIgnoreWhiteSpace ) {
				if ( bGetChar ) break;
				continue;
			}

			if ( !bGetChar ) continue;
		}

		m_pStr[ iReadCnt++ ] = cChar;
		bGetChar = true;
	} while( cChar && !feof(fp) && iReadCnt < m_wStrLen-1 );

	m_pStr[ iReadCnt ] = 0;

	return m_pStr;
}

//-------------------------------------------------------------------------------------------------
bool CStrVAR::SaveString (char *szFileName, char *szString)
{
	FILE *fp;

	fp = fopen (szFileName, "wt");
	if ( !fp )
		return false;

	fputs (szString, fp);

	fclose (fp);
	return true;
}

//-------------------------------------------------------------------------------------------------
