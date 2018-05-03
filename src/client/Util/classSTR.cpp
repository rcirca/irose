/*
	$History: classSTR.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-10-14   Time: 5:57p
 * Created in $/Client/Util
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
	m_nStrBuffCNT = nStrBufferLEN;
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
	/*
	#define BITS_PER_BYTE 8
	#define ROL(x,n) (((x)<<(n)) | ((x)>>((sizeof(unsigned int)*BITS_PER_BYTE)-(n))))

    unsigned int i;
    unsigned int pos;
    unsigned int hash;

    if ( !szString)
    	return 0;

    for (hash=0,pos=0,i=0; i<strlen(szString); i++) {
        hash ^= ROL(((unsigned int)tolower((int)szString[i])),
	        	    (pos%(sizeof(unsigned int)*BITS_PER_BYTE)));

    	pos += BITS_PER_BYTE-1;
    }

    return hash;
	*/
	return StrToHashKey( szString );
}

//-------------------------------------------------------------------------------------------------
void CStrVAR::Del ()
{	
	if( m_pStr ) {
		delete[] m_pStr; 
		m_pStr=NULL;
		m_nStrLen=0;
	}
}

void CStrVAR::Set (char *szStrIn)
{
	this->Del ();

	if ( szStrIn ) {
		m_nStrLen = strlen( szStrIn );
		if ( m_nStrLen > 0 ) {
			m_pStr = new char[ m_nStrLen+1 ];
			_ASSERT( m_pStr );
			strncpy(m_pStr, szStrIn, m_nStrLen);
			m_pStr[ m_nStrLen ] = 0;
			return;
		}
	}
	
	m_nStrLen = 0;
}

//-------------------------------------------------------------------------------------------------
