#ifndef	__CMemBUFF_H
#define	__CMemBUFF_H
#include <crtdbg.h>
#include "classSYNCOBJ.h"
#include "classSTR.h"
#include "classLOG.h"
//-------------------------------------------------------------------------------------------------

template <class pDataType>
class CMemBUFF : private CCriticalSection 
{
private :
	pDataType  *m_ppData;

protected :
	CStrVAR		m_Name;

	pDataType  *m_ppBuff;

	int			m_iBuffSize;

	int			m_iAllocIDX;
	int			m_iFreeIDX;

	int			m_iUsedCNT;

	bool InitBUFF ();
	void FreeBUFF ();

	virtual pDataType	Real_Alloc () = 0;
	virtual	void		Real_Free( pDataType pData ) = 0;

public :
	CMemBUFF (char *szName, int iBuffSize);
	virtual ~CMemBUFF ();

	virtual void		InitData (pDataType pData)=0;//				{	/* nop */	}
	virtual void		FreeData (pDataType pData)=0;//				{	/* nop */	}

	int			GetUsedCNT ()	{	return m_iUsedCNT;	}

	pDataType	Buff_Alloc ();
	void		Buff_Free (pDataType pData);
} ;

//-------------------------------------------------------------------------------------------------
template <class pDataType>
CMemBUFF<pDataType>::CMemBUFF (char *szName, int iBuffSize)
{
	m_Name.Set( szName );
	m_iBuffSize = iBuffSize;

	m_iAllocIDX = 0;
	m_iFreeIDX  = 0;

	m_iUsedCNT = 0;

	m_ppData = NULL;
	m_ppBuff = NULL;
}

template <class pDataType>
CMemBUFF<pDataType>::~CMemBUFF ()
{
	_ASSERT( 0 == m_iBuffSize );
	SAFE_DELETE_ARRAY( m_ppData );
	SAFE_DELETE_ARRAY( m_ppBuff );
}

template <class pDataType>
bool CMemBUFF<pDataType>::InitBUFF ()
{
	m_ppData = new pDataType [ m_iBuffSize ];
	m_ppBuff = new pDataType [ m_iBuffSize ];

	if ( !m_ppData || !m_ppBuff )
		return false;

	::ZeroMemory( m_ppData,	sizeof( pDataType ) * m_iBuffSize );
	::ZeroMemory( m_ppBuff,	sizeof( pDataType ) * m_iBuffSize );

	for (int iC=0; iC<m_iBuffSize; iC++) {
		m_ppData[ iC ] = this->Real_Alloc ();
		m_ppBuff[ iC ] = m_ppData[ iC ];

		_ASSERT( m_ppBuff[ iC ] );
	}

	return true;
}

template <class pDataType>
void CMemBUFF<pDataType>::FreeBUFF ()
{
	for (int iC=0; iC<m_iBuffSize; iC++) {
		this->Real_Free( m_ppData[ iC ] );
	}
	m_iBuffSize = 0;
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
pDataType CMemBUFF<pDataType>::Buff_Alloc ()
{
	int iNextAllocIDX = ( m_iAllocIDX+1 ) % m_iBuffSize;

	if ( iNextAllocIDX == m_iFreeIDX ) {
		// Out of buffer 
		return NULL;
	}

	pDataType pData = m_ppBuff[ m_iAllocIDX ];
	m_ppBuff[ m_iAllocIDX ] = NULL;
	m_iAllocIDX = iNextAllocIDX;

	m_iUsedCNT ++;

	this->InitData( pData );
	
	return pData;
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
void CMemBUFF<pDataType>::Buff_Free (pDataType pData)
{
	this->FreeData (pData);

	m_ppBuff[ m_iFreeIDX ] = pData;
	m_iUsedCNT --;

	m_iFreeIDX = ( m_iFreeIDX+1 ) % m_iBuffSize;
}

//-------------------------------------------------------------------------------------------------
#endif