#ifndef	__CPoolQUEUE_H
#define	__CPoolQUEUE_H
#include <crtdbg.h>
#include "SLLIST.h"
#include "classSYNCOBJ.h"
//-------------------------------------------------------------------------------------------------

template <class t_DATA>
class CPoolQUEUE : public CCriticalSection
{
protected :
	struct CDataNODE
	{
		t_DATA		 m_DATA;
		bool		 m_bUSED;
		CDataNODE	*m_pNextNODE;
		CDataNODE ()
		{
			m_bUSED = false;
		}
	} ;

	UINT					m_uiTotalDataCNT;
	UINT					m_uiIncDataCNT;
	int						m_iUsedCNT;

	classSLLIST<CDataNODE*> m_BlockLIST;
	CDataNODE			   *m_pHeadNODE;
	CDataNODE			   *m_pTailNODE;

	CDataNODE *Increase (UINT uiDataCNT)
	{
		CDataNODE *pDataBLOCK;
		
		pDataBLOCK = new CDataNODE[ uiDataCNT ];
		if ( NULL == pDataBLOCK )
			return NULL;
		
		m_BlockLIST.AllocNAppend( pDataBLOCK );
		m_uiTotalDataCNT += uiDataCNT;

		CDataNODE *pNextNODE = NULL;
		CDataNODE *pDataNODE;

		m_pTailNODE = &pDataBLOCK[ uiDataCNT-1 ];
		m_pTailNODE->m_pNextNODE = NULL;
		for (int iC=uiDataCNT-1; iC>=0; iC--) {
			pDataNODE = &pDataBLOCK[ iC ];

			pDataNODE->m_pNextNODE = pNextNODE;
			pNextNODE = pDataNODE;
		}

		g_LOG.CS_ODS( 0xffff, ">>>> Pool[ %s ] Increase %d data total: %d\n", m_PoolName.Get(), uiDataCNT, m_uiTotalDataCNT );


		return pDataBLOCK;
	}

public :
	int	  GetUsedCNT ()		{	return	m_iUsedCNT;			}

	CPoolQUEUE(UINT uiInitDataCNT, UINT uiIncDataCNT) : CCriticalSection( 4000 )
	{
		m_uiIncDataCNT = uiIncDataCNT;
		m_uiTotalDataCNT = 0;
		m_iUsedCNT = 0;

		m_pHeadNODE = this->Increase( uiInitDataCNT );
	}
	virtual ~CPoolQUEUE ()
	{
		classSLLNODE< CDataNODE* > *pBlockNODE;

		pBlockNODE = m_BlockLIST.DeleteHead();
		while( pBlockNODE ) {
			SAFE_DELETE_ARRAY( pBlockNODE->DATA );
			SAFE_DELETE( pBlockNODE );
			pBlockNODE = m_BlockLIST.DeleteHead();
		}
	}

	t_DATA*	Alloc ()
	{
		t_DATA *pDATA;

		this->Lock ();
		{
			if ( NULL == this->m_pHeadNODE ) {
				this->m_pHeadNODE = this->Increase( m_uiIncDataCNT );
			}

			this->m_pHeadNODE->m_bUSED = true;
			pDATA = reinterpret_cast<t_DATA*>( this->m_pHeadNODE );
			this->m_pHeadNODE = this->m_pHeadNODE->m_pNextNODE;
			this->m_iUsedCNT ++;
		}
		this->Unlock ();

		return pDATA;
	}
	void	Free (void *pDATA)
	{
		CDataNODE *pDataNODE = reinterpret_cast<CDataNODE*>( pDATA );
		this->Lock ();
		{
			// 두번 풀리는거 체크...
			_ASSERT( pDataNODE->m_bUSED );

			if ( pDataNODE->m_bUSED ) {
				pDataNODE->m_bUSED = false;

				this->m_iUsedCNT --;
				_ASSERT( this->m_iUsedCNT >= 0 );

				this->m_pTailNODE->m_pNextNODE = pDataNODE;
				this->m_pTailNODE = pDataNODE;
				this->m_pTailNODE->m_pNextNODE = NULL;

				if ( NULL == this->m_pHeadNODE ) {
					this->m_pHeadNODE = this->m_pTailNODE;
				}
			} 
		}
		this->Unlock ();
	}
} ;

#define	DECLARE_POOLQUEUE(class_name)												\
public :																			\
	static CPoolQUEUE< class_name >	m_Pool;											\
	void* operator new(size_t size)					{	return m_Pool.Alloc();	}	\
	void  operator delete(void* pData, size_t size)	{	m_Pool.Free(pData);		}		
	
#define	IMPLEMENT_POOLSTACK(class_name,I,J)		CPoolQUEUE<class_name> class_name::m_Pool(I,J);

//-------------------------------------------------------------------------------------------------
#endif