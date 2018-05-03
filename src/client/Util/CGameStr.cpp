#include "stdafx.h"
#include "CGameStr.h"


char *CGameStr::ReadString ( CFileSystem* pFileSystem, bool bIgnoreWhiteSpace)
{
	char cChar, *szStrOut=CStr::GetString();
	bool bGetChar=false, bInDoubleQuote=false;
	int  iReadCnt=0;

	do {
		if ( pFileSystem->ReadChar( &cChar ) < 1 )
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
	} while( cChar && !( pFileSystem->IsEOF() ) && iReadCnt < GetBufferLength()-1 );

	szStrOut[ iReadCnt ] = 0;

	return szStrOut;
}