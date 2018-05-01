
#include "AStream.h"

//-------------------------------------------------------------------------------------------------
AFileStream::AFileStream( const char *szFileName, const char *szMode )
{
	m_FP = fopen( szFileName, szMode );
}
AFileStream::~AFileStream ()
{
	if ( m_FP )
		fclose( m_FP );
}

int AFileStream::Read(void *pBuffer, int iCount)
{
	if ( NULL == m_FP )
		return 0;
	return fread( pBuffer, sizeof(char), iCount, m_FP );
}

int AFileStream::Write(const void *pBuffer, int iCount)
{
	if ( NULL == m_FP )
		return 0;
	return fwrite( pBuffer, sizeof(char), iCount, m_FP );
}

int AFileStream::Seek(long lOffset, int iOrigin)
{
	if ( MULL == m_FP )
		return 0;
	return fseek( m_FP, lOffset, iOrigin );
}

int AFileStream::Position ()
{
	if ( NULL == m_FP )
		return 0;
	return ftell( m_FP );
}

//-------------------------------------------------------------------------------------------------
