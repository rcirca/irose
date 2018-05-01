#ifndef _CFileSystemTriggerVFS_
#define _CFileSystemTriggerVFS_

#include "CFileSystem.h"
#include "TriggerVFS.h"


class CFileSystemTriggerVFS : public CFileSystem
{
private:
	VFileHandle*			m_pFileHandle;
	VHANDLE					m_hVFile;

	std::string				m_strFileName;
	unsigned char*			m_pData;

	int						m_iSize;
public:
	void					SetVFS( VHANDLE hVFile ){ m_hVFile = hVFile; };


public:
	CFileSystemTriggerVFS(void);
	~CFileSystemTriggerVFS(void);

	virtual bool			OpenFile( const char* pFileName, int iOpenType = OPEN_READ_BIN );
	virtual void			CloseFile();

		
	virtual bool			ReadToMemory();
	virtual void			ReleaseData();		
	virtual unsigned char*	GetData(){ return m_pData; };;	

	virtual int				GetSize();
	virtual bool			IsExist( const char* pFileName );

private:
	virtual void			SetVFSHandle( VFileHandle* pFileHandle ){ m_pFileHandle = pFileHandle; };



public:
	virtual	int			Read( void *lpBuf, unsigned int nCount );
	virtual void		Write( const void *lpBuf, unsigned int nCount );
	virtual bool		Seek( long lOff, unsigned int nFrom );
	virtual long		Tell();
	virtual bool		IsEOF();

	virtual int			ReadStringByNullLength();
	virtual int			ReadStringByNull( char* lpBuf );
	virtual void		WriteStringByNull( const char* pStr );

	virtual int			ReadPascalStringLength();
	virtual int			ReadPascalString( char* lpBuf, int iBufferLength );
	virtual void		WritePascalString( const char* pStr );


	// Specific read method
	virtual int			ReadFloat( float* pValue );
	virtual int			ReadFloat2( float* lpBuf );
	virtual int			ReadFloat2( float* x, float* y );
	virtual int			ReadFloat3( float* lpBuf );
	virtual int			ReadFloat3( float* x, float* y, float* z );
	virtual int			ReadFloat4( float* lpBuf );
	virtual int			ReadFloat4( float* x, float* y, float* z, float* w );

	virtual int			ReadChar( char* pValue );
	virtual int			ReadByte( unsigned char* pValue );
	virtual int			ReadBool( bool* pValue );

	virtual int			ReadInt16( short* pValue );
	virtual int			ReadInt32( int* pValue );
	virtual int			ReadInt64( __int64* pValue );

	virtual int			ReadUInt16( unsigned short* pValue );
	virtual int			ReadUInt32( unsigned int* pValue );
	virtual int			ReadUInt64( unsigned __int64* pValue );

	// Specific write method
	virtual void		WriteFloat( float* pValue, const char* strValueName = NULL );
	virtual void		WriteFloat2( float* lpBuf, const char* strValueName = NULL );
	virtual void		WriteFloat2( float* x,float* y, const char* strValueName = NULL );
	virtual void		WriteFloat3( float* lpBuf, const char* strValueName = NULL );
	virtual void		WriteFloat3( float* x, float* y, float* z, const char* strValueName = NULL );
	virtual void		WriteFloat4( float* lpBuf, const char* strValueName = NULL );
	virtual void		WriteFloat4( float* x, float* y, float* z, float* w, const char* strValueName = NULL );

	virtual void		WriteChar( char* pValue, const char* strValueName = NULL );
	virtual void		WriteByte( unsigned char* pByte, const char* strValueName = NULL );
	virtual void		WriteBool( bool* pValue, const char* strValueName = NULL );

	virtual void		WriteInt16( short* pValue, const char* strValueName = NULL );
	virtual void		WriteInt32( int* pValue, const char* strValueName = NULL );
	virtual void		WriteInt64( __int64* pValue, const char* strValueName = NULL );

	virtual void		WriteUInt16( unsigned short* pValue, const char* strValueName = NULL );
	virtual void		WriteUInt32( unsigned int* pValue, const char* strValueName = NULL );
	virtual void		WriteUInt64( unsigned __int64* pValue, const char* strValueName = NULL );
};



#endif //_CFileSystemTriggerVFS_