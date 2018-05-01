#ifndef	__CDATAPOOL_H
#define	__CDATAPOOL_H
#include "crtdbg.h"
#include "SLLIST.h"
#include "classSYNCOBJ.h"
#include "classSTR.h"
#include "classLOG.h"
//-------------------------------------------------------------------------------------------------

template <class t_DATA>
class CDataPOOL : public CCriticalSection
{
protected :
	struct CDataNODE : public t_DATA
	{
		bool		 m_POOL_bUsed;
		CDataNODE	*m_POOL_pNextNODE;
		CDataNODE ()
		{
			m_POOL_bUsed = false;
			m_POOL_pNextNODE = NULL;
		}
	} ;

	UINT					m_uiTotalDataCNT;
	UINT					m_uiIncDataCNT;
	int						m_iUsedCNT;

	CStrVAR					m_PoolName;
	classSLLIST<CDataNODE*> m_BlockLIST;
	CDataNODE			   *m_pHeadNODE;
	CDataNODE			   *m_pTailNODE;

	CDataNODE *Create (UINT uiDataCNT)
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
		m_pTailNODE->m_POOL_pNextNODE = NULL;
		for (int iC=uiDataCNT-1; iC>=0; iC--) {
			pDataNODE = &pDataBLOCK[ iC ];

			pDataNODE->m_POOL_pNextNODE = pNextNODE;
			pNextNODE = pDataNODE;
		}

		g_LOG.CS_ODS( 0xffff, ">>>> Pool[ %s ] Increase %d data total: %d\n", m_PoolName.Get(), uiDataCNT, m_uiTotalDataCNT );


		return pDataBLOCK;
	}

public :
	char *GetPoolNAME()		{	return	m_PoolName.Get();	}
	int	  GetUsedCNT ()		{	return	m_iUsedCNT;			}

	
	CDataPOOL(char *szName, UINT uiInitDataCNT, UINT uiIncDataCNT) : CCriticalSection( 4000 )
	{
		m_PoolName.Set( szName );

		m_uiIncDataCNT = uiIncDataCNT;
		m_uiTotalDataCNT = 0;
		m_iUsedCNT = 0;

		m_pTailNODE = NULL;
		m_pHeadNODE = this->Create( uiInitDataCNT );
	}
	virtual ~CDataPOOL ()
	{
		classSLLNODE< CDataNODE* > *pBlockNODE;

		pBlockNODE = m_BlockLIST.DeleteHead();
		while( pBlockNODE ) {
			SAFE_DELETE_ARRAY( pBlockNODE->DATA );
			SAFE_DELETE( pBlockNODE );
			pBlockNODE = m_BlockLIST.DeleteHead();
		}
	}

	t_DATA*	Pool_Alloc ()
	{
		t_DATA *pDATA;

		this->Lock ();
		{
			if ( this->m_pHeadNODE ) {
				if ( NULL == this->m_pHeadNODE->m_POOL_pNextNODE ) {
					this->m_pHeadNODE->m_POOL_pNextNODE = this->Create( m_uiIncDataCNT );
				}

				this->m_pHeadNODE->m_POOL_bUsed = true;
				pDATA = reinterpret_cast<t_DATA*>( this->m_pHeadNODE );
				this->m_pHeadNODE = this->m_pHeadNODE->m_POOL_pNextNODE;
				this->m_iUsedCNT ++;
			} else {
				// Out of memory ...
				pDATA = NULL;
			}
		}
		this->Unlock ();

		return pDATA;
	}
	void	Pool_Free (t_DATA *pDATA)
	{
		CDataNODE *pDataNODE = reinterpret_cast<CDataNODE*>( pDATA );
		this->Lock ();
		{
			// 두번 풀리는거 체크...
			if ( pDataNODE->m_POOL_bUsed ) {
				pDataNODE->m_POOL_bUsed = false;

				this->m_iUsedCNT --;
				_ASSERT( this->m_iUsedCNT >= 0 );

				this->m_pTailNODE->m_POOL_pNextNODE = pDataNODE;
				this->m_pTailNODE = pDataNODE;
				this->m_pTailNODE->m_POOL_pNextNODE = NULL;

				if ( NULL == this->m_pHeadNODE ) {
					this->m_pHeadNODE = this->m_pTailNODE;
				}
			} else {
				//g_LOG.CS_ODS( 0xffff, ">>>> ERROR:: Pool[ %s ] Duplicated free, UsedCNT: %d\n", m_PoolName.Get(), this->m_iUsedCNT );
			}
		}
		this->Unlock ();
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif