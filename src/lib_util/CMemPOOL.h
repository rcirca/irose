#ifndef	__CMemPOOL_H
#define	__CMemPOOL_H
#include <crtdbg.h>
#include "classSYNCOBJ.h"
#include "SLLIST.h"
#include "classSTR.h"
#include "classLOG.h"
//-------------------------------------------------------------------------------------------------

template <class pDataType>
class CPoolITEM {
public :
	classSLLNODE< pDataType > *m_pNODE;
	pDataType				   m_pDATA;	
} ;

template <class pDataType>
class CMemPOOL : public CCriticalSection 
{
private  :
	classSLLIST< CPoolITEM<pDataType> >	m_NODEs;

protected:
	classSLLIST< pDataType >	m_POOLs;
	classSLLIST< pDataType >	m_USEDs;

	CStrVAR		m_Name;
	int			m_iIncreaseSize;
	int			m_iTotalSize;
	
	bool CreatePool (int iPoolSize);
	void DeletePool ();

	virtual pDataType	Real_Alloc ()=0;
	virtual void		Real_Free (pDataType pData)=0;
	virtual void		InitData (pDataType pData)				{	/* nop */	}
	virtual void		FreeData (pDataType pData)				{	/* nop */	}

public :
	CMemPOOL (char *szName, int iIncreaseSize);
	virtual ~CMemPOOL ();

	pDataType	Pool_Alloc ();
	void		Pool_Free (pDataType pData);
} ;


//-------------------------------------------------------------------------------------------------
template <class pDataType>
CMemPOOL<pDataType>::CMemPOOL (char *szName, int iIncreaseSize)
{
	m_Name.Set( szName );
	m_iIncreaseSize = iIncreaseSize;
	m_iTotalSize = 0;
}

template <class pDataType>
CMemPOOL<pDataType>::~CMemPOOL ()
{
	_ASSERT( m_NODEs.GetNodeCount() == 0 );	// 상속 받은 클레스의 파괴자에서 this->DeletePool()이 호출되어야 한다.
//	_ASSERT( m_USEDs.GetNodeCount() == 0 );
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
bool CMemPOOL<pDataType>::CreatePool (int iIncSize)
{
	int iC;

	classSLLNODE< CPoolITEM<pDataType> > *pNewITEM;
	classSLLNODE< pDataType > *pNewNODE;
	for (iC=0; iC<iIncSize; iC++) {
		pNewNODE = m_POOLs.AllocNAppend ();
		pNewNODE->DATA = this->Real_Alloc ();

		pNewITEM = m_NODEs.AllocNAppend ();
		pNewITEM->DATA.m_pNODE = pNewNODE;
		pNewITEM->DATA.m_pDATA = pNewNODE->DATA;
	}
	m_iTotalSize += iIncSize;

	g_LOG.CS_ODS( 0xffff, "%s : CreatePool[ %d / %d ]\n", this->m_Name.Get(), iIncSize, m_iTotalSize );

	return ( pNewNODE && pNewNODE->DATA );
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
void CMemPOOL<pDataType>::DeletePool ()
{
	classSLLNODE< CPoolITEM<pDataType> > *pNODE;

	pNODE = m_NODEs.DeleteHead ();
	while ( pNODE ) {
		this->Real_Free( pNODE->DATA.m_pDATA );
		SAFE_DELETE( pNODE->DATA.m_pNODE );
		SAFE_DELETE( pNODE );

		pNODE = m_NODEs.DeleteHead ();
	}
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
pDataType CMemPOOL<pDataType>::Pool_Alloc ()
{
	classSLLNODE< pDataType > *pNODE;

	this->Lock ();
		pNODE = m_POOLs.DeleteHead ();
		if ( pNODE == NULL ) {
			this->CreatePool ( m_iIncreaseSize );
			pNODE = m_POOLs.DeleteHead ();
		}

		m_USEDs.AppendNode( pNODE );
	this->Unlock ();

	this->InitData( pNODE->DATA );

	return pNODE->DATA;
}

//-------------------------------------------------------------------------------------------------
template <class pDataType>
void CMemPOOL<pDataType>::Pool_Free (pDataType pData)
{
	this->FreeData (pData);

	classSLLNODE< pDataType > *pNODE;

	this->Lock ();
		pNODE = m_USEDs.DeleteHead ();

		_ASSERT( pNODE );

		pNODE->DATA = pData;
		m_POOLs.AppendNode( pNODE );
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
#endif
