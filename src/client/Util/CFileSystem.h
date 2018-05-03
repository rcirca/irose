#ifndef _CFILESYSTEM_
#define _CFILESYSTEM_




enum FILESYSTEM_TYPE
{
	FILE_SYSTEM_NORMAL = 1,
	FILE_SYSTEM_TEXT_FOR_TEST = 2,
	FILE_SYSTEM_ZIP = 3,
	FILE_SYSTEM_PACK = 4,
};

enum FILEOPEN_MODE
{
	OPEN_CREATE_BIN = 1,		//"wb"
	OPEN_READ_BIN = 2,			//"rb"
	OPEN_APPEND_BIN = 3,		//"ab"

	OPEN_CREATE_TEXT = 4,		//"wt"
	OPEN_READ_TEXT = 5,			//"rt"
	OPEN_APPEND_TEXT = 6,		//"at"
};

enum FILE_POSITION
{
	FILE_POS_SET,
	FILE_POS_CUR,
	FILE_POS_END,
};

/// application file system for VFS dll.
class CFileSystem
{
private:
	

public:
	CFileSystem(void){};
	virtual ~CFileSystem(void){};
	
	virtual bool			OpenFile( const char* pFileName, int iOpenType = OPEN_READ_BIN ) = 0{ *(int*)0 = 10; };
	virtual void			CloseFile() = 0{ *(int*)0 = 10; };


	virtual bool			ReadToMemory() = 0{ *(int*)0 = 10; };
	virtual void			ReleaseData() = 0{ *(int*)0 = 10; };		
	virtual unsigned char*	GetData() = 0{ *(int*)0 = 10; };	

	virtual int				GetSize() = 0{ *(int*)0 = 10; };
	virtual bool			IsExist( const char* pFileName ) = 0{ *(int*)0 = 10; };


public:
	virtual	int			Read( void *lpBuf, unsigned int nCount ) = 0{ *(int*)0 = 10; };
	virtual void		Write( const void *lpBuf, unsigned int nCount ) = 0{ *(int*)0 = 10; };
	virtual bool		Seek( long lOff, unsigned int nFrom ) = 0{ *(int*)0 = 10; };
	virtual long		Tell() = 0{ *(int*)0 = 10; };
	virtual bool		IsEOF() = 0{ *(int*)0 = 10; };

	virtual int			ReadStringByNullLength() = 0{ *(int*)0 = 10; };
	virtual int			ReadStringByNull( char* lpBuf ) = 0{ *(int*)0 = 10; };
	virtual void		WriteStringByNull( const char* pStr ) = 0{ *(int*)0 = 10; };

	virtual int			ReadPascalStringLength() = 0{ *(int*)0 = 10; };
	virtual int			ReadPascalString( char* lpBuf, int iBufferLength ) = 0{ *(int*)0 = 10; };
	virtual void		WritePascalString( const char* pStr ) = 0{ *(int*)0 = 10; };


	// Specific read method	
	virtual int			ReadFloat( float* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat2( float* lpBuf ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat2( float* x, float* y ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat3( float* lpBuf ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat3( float* x, float* y, float* z ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat4( float* lpBuf ) = 0{ *(int*)0 = 10; };
	virtual int			ReadFloat4( float* x, float* y, float* z, float* w ) = 0{ *(int*)0 = 10; };

	virtual int			ReadChar( char* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadByte( unsigned char* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadBool( bool* pValue ) = 0{ *(int*)0 = 10; };

	virtual int			ReadInt16( short* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadInt32( int* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadInt64( __int64* pValue ) = 0{ *(int*)0 = 10; };

	virtual int			ReadUInt16( unsigned short* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadUInt32( unsigned int* pValue ) = 0{ *(int*)0 = 10; };
	virtual int			ReadUInt64( unsigned __int64* pValue ) = 0{ *(int*)0 = 10; };

	// Specific write method
	virtual void		WriteFloat( float* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat2( float* lpBuf, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat2( float* x,float* y, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat3( float* lpBuf, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat3( float* x, float* y, float* z, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat4( float* lpBuf, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteFloat4( float* x, float* y, float* z, float* w, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };

	virtual void		WriteChar( char* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteByte( unsigned char* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteBool( bool* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };

	virtual void		WriteInt16( short* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteInt32( int* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteInt64( __int64* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };

	virtual void		WriteUInt16( unsigned short* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteUInt32( unsigned int* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };
	virtual void		WriteUInt64( unsigned __int64* pValue, const char* strValueName = NULL ) = 0{ *(int*)0 = 10; };

};


#endif //_CFILESYSTEM_