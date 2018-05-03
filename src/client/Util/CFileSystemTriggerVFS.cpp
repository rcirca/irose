#include "stdafx.h"
#include "CFileSystemTriggerVFS.h"
#include <assert.h>



CFileSystemTriggerVFS::CFileSystemTriggerVFS(void) : m_pFileHandle( NULL ), m_hVFile( NULL )
{
	m_pData = NULL;
	m_iSize = 0;
};

CFileSystemTriggerVFS::~CFileSystemTriggerVFS(void)
{
}


bool CFileSystemTriggerVFS::OpenFile( const char* fname, int iMode )
{
	if( m_hVFile == NULL )
		return false;

	VFileHandle* pFileHandle = ::VOpenFile( fname, m_hVFile );
	
	/// File open error
	if( pFileHandle == NULL )
	{
		m_strFileName = std::string( "" );
		return false;
	}

	SetVFSHandle( pFileHandle );

	m_strFileName = std::string( fname );	

	if( m_pData != NULL )
	{
		delete m_pData;
		m_pData = NULL;
	}

	m_iSize = 0;

	return true;
}

void CFileSystemTriggerVFS::CloseFile()
{
	if( m_pFileHandle )
	{
		::VCloseFile( m_pFileHandle );
		m_pFileHandle = NULL;
	}

	m_strFileName = std::string( "" );
}

void CFileSystemTriggerVFS::ReleaseData()
{
	if( m_pData != NULL )
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

////////////////////////////////////////////////////////////
// Overload function
int CFileSystemTriggerVFS::Read( void *lpBuf, unsigned int nCount )
{
	assert( lpBuf && "Read failed from FileStream, output buffer is null" );
	assert( m_pFileHandle && "Read failed from FileStream, file description is null" );
	if( m_pFileHandle == NULL || lpBuf == NULL )
	{				
		return 0;	
	}

	memset( lpBuf, 0, sizeof( char ) * nCount );

	return ::vfread( lpBuf, 1, nCount, m_pFileHandle );
}

void CFileSystemTriggerVFS::Write( const void *lpBuf, unsigned int nCount )
{
	return;
	/*
	assert( lpBuf && "Write failed from FileStream, input buffer is null" );
	assert( m_pFileHandle && "Write failed from FileStream, file description is null" );

	if( m_pFileHandle != NULL && lpBuf != NULL )
		::vfwrite( lpBuf, 1, nCount, m_pFileHandle );
	*/
}

bool CFileSystemTriggerVFS::Seek( long lOff, unsigned int nFrom )
{
	assert( m_pFileHandle && "Seek failed from FileStream, file description is null" );
	if( m_pFileHandle == NULL )
		return false;

	int iResult = 1;

	switch( nFrom )
	{
		case FILE_POS_SET: iResult = ::vfseek( m_pFileHandle, lOff, SEEK_SET ); break;
		case FILE_POS_CUR: iResult = ::vfseek( m_pFileHandle, lOff, SEEK_CUR ); break;
		case FILE_POS_END: iResult = ::vfseek( m_pFileHandle, lOff, SEEK_END ); break;
	}

	if( iResult )
		return false;

	return true;
}

long CFileSystemTriggerVFS::Tell()
{
	return vftell( m_pFileHandle );
}

bool CFileSystemTriggerVFS::IsEOF()
{
	return vfeof( m_pFileHandle );
}


int CFileSystemTriggerVFS::ReadStringByNullLength()
{
	assert( ( m_pFileHandle != NULL ) && "Read string failed from FileStream, file description is null" );

	if( m_pFileHandle == NULL )
		return 0;

	long lFilePtr = this->Tell();

	char cOneChar;
	Read( &cOneChar, sizeof( char ) );				

	int j = 0;
	while( cOneChar != '\0' ) 
	{
		j++;
		Read( &cOneChar, sizeof( char ) );
	}

	this->Seek( lFilePtr, FILE_POS_SET );

	return j;
}

int CFileSystemTriggerVFS::ReadStringByNull( char* lpBuf )
{
	assert( ( lpBuf != NULL ) && "Read string failed from FileStream, Output buffer is null" );
	assert( ( m_pFileHandle != NULL ) && "Read string failed from FileStream, file description is null" );

	char cOneChar;
	Read( &cOneChar, sizeof( char ) );				

	int j = 0;
	while( cOneChar != '\0' ) 
	{
		lpBuf[j++] = cOneChar;
		Read( &cOneChar, sizeof( char ) );
	}
	lpBuf[j] = '\0';	

	return j;
}

void CFileSystemTriggerVFS::WriteStringByNull( const char* pStr )
{		
	assert( ( pStr != NULL ) && "Write string to FileStream failed , Input string is null" );
	assert( m_pFileHandle && "Write string to FileStream failed, file description is null" );

	if( pStr == NULL )
		return;
	
	int j = 0;
	while( pStr[ j ] != '\0' ) 
	{			
		Write( &( pStr[ j ] ), sizeof( char ) );
		j++;
	}
	Write( &( pStr[ j ] ), sizeof( char ) );		
}


int	CFileSystemTriggerVFS::ReadPascalStringLength()
{	
	assert( m_pFileHandle && "Write string to FileStream failed, file description is null" );

	if( m_pFileHandle == NULL )
		return 0;

	long lFilePtr = this->Tell();

	int		iLength;
	BYTE	btLength;
	ReadByte( &btLength );

	/// 한바이트를 사용하는가? 두바이트를 사용하는가?

	/// 두바이트를 사용한다.
	if( btLength & 0x80 )
	{
		BYTE btSecondByte = 0;
		ReadByte( &btSecondByte );
		
		int iSecondLength = btSecondByte;
		int iFirstLength = btLength;

		iLength = ( iSecondLength << 7 ) | ( iFirstLength - 0x80 );
	}else
	{
		iLength = btLength;
	}

	this->Seek( lFilePtr, FILE_POS_SET );

	return iLength;
}

int CFileSystemTriggerVFS::ReadPascalString( char* lpBuf, int iBufferLength )
{
	assert( ( lpBuf != NULL ) && "Write string to FileStream failed , Input string is null" );
	assert( m_pFileHandle && "Write string to FileStream failed, file description is null" );

	if( lpBuf == NULL )
		return 0;

	int		iLength;
	BYTE	btLength;
	ReadByte( &btLength );

	/// 한바이트를 사용하는가? 두바이트를 사용하는가?

	/// 두바이트를 사용한다.
	if( btLength & 0x80 )
	{
		BYTE btSecondByte = 0;
		ReadByte( &btSecondByte );

		int iSecondLength = btSecondByte;
		int iFirstLength = btLength;

		iLength = ( iSecondLength << 7 ) | ( iFirstLength - 0x80 );
	}else
	{
		iLength = btLength;
	}

	if( iLength > iBufferLength )
	{
		assert( 0 && "Input buffer is not enough to fill data that readed from file( Pascal string )" );
		return 0;
	}

	Read( lpBuf, iLength );
	lpBuf[ iLength ] = '\0';

	return iLength;
}

void CFileSystemTriggerVFS::WritePascalString( const char* pStr )
{
	assert( ( pStr != NULL ) && "Write string to FileStream failed , Input string is null" );
	assert( m_pFileHandle && "Write string to FileStream failed, file description is null" );

	if( pStr == NULL )
		return;

	short nLength = strlen( pStr );

	/// 두바이트로 길이를 표현한다.
	if( nLength > 0x7F )
	{
		BYTE btFirstLength = ( nLength & 0x1111111 ) | 0x10000000;
		WriteByte( &btFirstLength, "First length" );

		BYTE btSecondLength = nLength >> 7;
		WriteByte( &btSecondLength, "Second Length" );
	}

	Write( pStr, nLength );
}

////////////////////////////////////////////////////////////

bool CFileSystemTriggerVFS::ReadToMemory( )
{		
	if( m_pFileHandle == NULL )
	{
		m_strFileName = std::string( "" );
		return false;
	}

	ReleaseData();

	m_iSize = GetSize();
	if( m_iSize == 0 )
	{
		CloseFile();
		return false;
	}

	m_pData = new unsigned char[ m_iSize + 1 ];		// so 0 size file will be saved
	memset( m_pData, 0, m_iSize + 1 );
	Read( m_pData, m_iSize );
	
	return true;
}

int CFileSystemTriggerVFS::GetSize()
{
	if( m_pFileHandle == NULL || m_strFileName.empty() )
		return 0;

	if( m_iSize )
		return m_iSize;

	return ::vfgetsize( m_pFileHandle );
}

bool CFileSystemTriggerVFS::IsExist( const char* pFileName )
{
	return ::VFileExists( m_hVFile, pFileName );		
}


///////////////////////////////////////////////////////////////////////

// Specific read method
int	CFileSystemTriggerVFS::ReadFloat( float* pValue )
{
	return Read( pValue, sizeof( float ) );
}

int	CFileSystemTriggerVFS::ReadFloat2( float* lpBuf )
{
	return Read( lpBuf, sizeof( float ) * 2 );
}

int	CFileSystemTriggerVFS::ReadFloat2( float* x, float* y )
{
	int iSize = 0;
	iSize += Read( x, sizeof( float ) );
	iSize += Read( y, sizeof( float ) );

	return iSize;
}

int	CFileSystemTriggerVFS::ReadFloat3( float* lpBuf )
{
	return Read( lpBuf, sizeof( float ) * 3 );
}

int	CFileSystemTriggerVFS::ReadFloat3( float* x, float* y, float* z )
{
	int iSize = 0;
	iSize += Read( x, sizeof( float ) );
	iSize += Read( y, sizeof( float ) );
	iSize += Read( z, sizeof( float ) );

	return iSize;
}

int	CFileSystemTriggerVFS::ReadFloat4( float* lpBuf )
{
	return Read( lpBuf, sizeof( float ) * 4 );
}

int	CFileSystemTriggerVFS::ReadFloat4( float* x, float* y, float* z, float* w )
{
	int iSize = 0;
	iSize += Read( x, sizeof( float ) );
	iSize += Read( y, sizeof( float ) );
	iSize += Read( z, sizeof( float ) );
	iSize += Read( w, sizeof( float ) );

	return iSize;
}



int	CFileSystemTriggerVFS::ReadChar( char* pValue )
{
	return Read( pValue, sizeof( char ) );
}

int	CFileSystemTriggerVFS::ReadByte( unsigned char* pValue )
{
	return Read( pValue, sizeof( unsigned char ) );
}

int	CFileSystemTriggerVFS::ReadBool( bool* pValue )
{
	return Read( pValue, sizeof( bool ) );
}




int	CFileSystemTriggerVFS::ReadInt16( short* pValue )
{
	return Read( pValue, sizeof( short ) );
}

int	CFileSystemTriggerVFS::ReadInt32( int* pValue )
{
	return Read( pValue, sizeof( int ) );
}

int	CFileSystemTriggerVFS::ReadInt64( __int64* pValue )
{
	return Read( pValue, sizeof( __int64 ) );
}


int	CFileSystemTriggerVFS::ReadUInt16( unsigned short* pValue )
{
	return Read( pValue, sizeof( unsigned short ) );
}

int	CFileSystemTriggerVFS::ReadUInt32( unsigned int* pValue )
{
	return Read( pValue, sizeof( unsigned int ) );
}

int	CFileSystemTriggerVFS::ReadUInt64( unsigned __int64* pValue )
{
	return Read( pValue, sizeof( unsigned __int64 ) );
}



// Specific write method
void CFileSystemTriggerVFS::WriteFloat( float* pValue, const char* strValueName )
{
	Write( pValue, sizeof( float ) );
}

void CFileSystemTriggerVFS::WriteFloat2( float* lpBuf, const char* strValueName )
{
	Write( lpBuf, sizeof( float ) * 2 );
}

void CFileSystemTriggerVFS::WriteFloat2( float* x,float* y, const char* strValueName )
{
	Write( x, sizeof( float ) );
	Write( y, sizeof( float ) );
}

void CFileSystemTriggerVFS::WriteFloat3( float* lpBuf, const char* strValueName )
{
	Write( lpBuf, sizeof( float ) * 3 );
}

void CFileSystemTriggerVFS::WriteFloat3( float* x, float* y, float* z, const char* strValueName )
{
	Write( x, sizeof( float ) );
	Write( y, sizeof( float ) );
	Write( z, sizeof( float ) );
}

void CFileSystemTriggerVFS::WriteFloat4( float* lpBuf, const char* strValueName )
{
	Write( lpBuf, sizeof( float ) * 4 );
}

void CFileSystemTriggerVFS::WriteFloat4( float* x, float* y, float* z, float* w, const char* strValueName )
{
	Write( x, sizeof( float ) );
	Write( y, sizeof( float ) );
	Write( z, sizeof( float ) );
	Write( w, sizeof( float ) );
}



void CFileSystemTriggerVFS::WriteChar( char* pValue, const char* strValueName )
{
	Write( pValue, sizeof( char ) );
}

void CFileSystemTriggerVFS::WriteByte( unsigned char* pValue, const char* strValueName )
{
	Write( pValue, sizeof( unsigned char ) );
}

void CFileSystemTriggerVFS::WriteBool( bool* pValue, const char* strValueName )
{
	Write( pValue, sizeof( bool ) );
}




void CFileSystemTriggerVFS::WriteInt16( short* pValue, const char* strValueName )
{
	Write( pValue, sizeof( short ) );
}

void CFileSystemTriggerVFS::WriteInt32( int* pValue, const char* strValueName )
{
	Write( pValue, sizeof( int ) );
}

void CFileSystemTriggerVFS::WriteInt64( __int64* pValue, const char* strValueName )
{
	Write( pValue, sizeof( __int64 ) );
}


void CFileSystemTriggerVFS::WriteUInt16( unsigned short* pValue, const char* strValueName )
{
	Write( pValue, sizeof( unsigned short ) );
}

void CFileSystemTriggerVFS::WriteUInt32( unsigned int* pValue, const char* strValueName )
{
	Write( pValue, sizeof( unsigned int ) );
}

void CFileSystemTriggerVFS::WriteUInt64( unsigned __int64* pValue, const char* strValueName )
{
	Write( pValue, sizeof( unsigned __int64 ) );
}
