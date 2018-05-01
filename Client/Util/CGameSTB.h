#ifndef _CGAMESTB_
#define _CGAMESTB_

#include <assert.h>
#include "CFileSystem.h"

class AMemStream
{
public :
	DWORD	 m_dwSize;
	BYTE	*m_pStream;
	DWORD	 m_dwMemPtr;


	AMemStream ()
	{
		m_pStream = NULL;
		m_dwSize = 0;
		m_dwMemPtr = 0;
	}
	AMemStream (DWORD dwSize)
	{
		m_pStream = new BYTE[ dwSize ];
		m_dwSize = dwSize;
		m_dwMemPtr = 0;
	}
	~AMemStream ()
	{
		SAFE_DELETE_ARRAY( m_pStream );
	}
	bool Alloc (DWORD dwSize)
	{
		SAFE_DELETE_ARRAY( m_pStream );

		m_dwSize = dwSize;
		m_pStream = new BYTE[ dwSize ];
		return NULL != m_pStream;
	}

public:
	int			Read( void *lpBuf, unsigned int nCount ) 
	{ 
		assert( m_dwMemPtr+nCount <= m_dwSize );
		::CopyMemory( lpBuf, &m_pStream[ m_dwMemPtr ], nCount );
		m_dwMemPtr += nCount;
		return nCount;
	};
	bool		Seek( long lOff, unsigned int nFrom ) 
	{ 
		if ( !m_pStream )
			return false;

		switch( nFrom ) {
			case FILE_POS_SET	:
				m_dwMemPtr = lOff;
				break;
			case FILE_POS_CUR	:
				m_dwMemPtr += lOff;
				break;
			case FILE_POS_END	:
				m_dwMemPtr = m_dwSize;
				break;
			default:
				return false;
		} 
		if ( m_dwMemPtr >= m_dwSize )
			m_dwMemPtr = m_dwSize - 1;
		return true;
	};
	long		Tell() 
	{ 
		return m_dwMemPtr;
	};
	bool		IsEOF() 
	{ 
		return ( m_dwMemPtr >= m_dwSize );
	};


	int			ReadInt16( short* pValue ) 
	{
//		assert( m_dwMemPtr+2 <= m_dwSize ); Garnet: cannot pass this assert
		*pValue = *( (short*)(&m_pStream[ m_dwMemPtr ]) );
		m_dwMemPtr += 2;
		return 2;
	};
	int			ReadInt32( int* pValue ) 
	{ 
		assert( m_dwMemPtr+4 <= m_dwSize );
		*pValue = *( (int*)(&m_pStream[ m_dwMemPtr ]) );
		m_dwMemPtr += 4;
		return 4;
	};

	int			ReadUInt16( unsigned short* pValue ) 
	{ 
		assert( m_dwMemPtr+2 <= m_dwSize );
		*pValue = *( (WORD*)(&m_pStream[ m_dwMemPtr ]) );
		m_dwMemPtr += 2;
		return 2;
	};
	int			ReadUInt32( unsigned int* pValue ) 
	{ 
		assert( m_dwMemPtr+4 <= m_dwSize );
		*pValue = *( (UINT*)(&m_pStream[ m_dwMemPtr ]) );
		m_dwMemPtr += 4;
		return 4;
	};
} ;



class CGameSTB : public classSTB 
{
private :
	AMemStream		   *m_pMemStream;
	CFileSystem*		m_pFileSystem;	

	long				m_lFP;
	long*				m_pOffset;
	
	CStrVAR*			m_RowNAME;
	char				m_szValue[ 4096 ];
	
	int					m_iRowIndex;
	int					m_iColIndex;

public  :
	int					m_iRowCount;
	int					m_iColCount;

	CGameSTB();
	~CGameSTB();				

	bool  Open  ( char *szFileName );
	void  Close ();

	char		*GetNAME (int iRow)	{	return m_RowNAME[ iRow ].Get();		}
	unsigned int GetHASH (int iRow)	{	return m_RowNAME[ iRow ].GetHASH();	}
		
	int   GetRowCount()			{	return m_iRowCount;				}
	int   GetColumnCount()		{	return m_iColCount;				}

	bool  SetIndexPosition (int iCol, int iRow);

	int   GetInteger();
	int	  GetInteger(int iCol, int iRow);
	char *GetString();
	char *GetString(int iCol, int iRow);
} ;


#endif //_CGAMESTB_