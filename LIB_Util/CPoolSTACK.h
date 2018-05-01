#ifndef	__CPoolSTACK_H
#define	__CPoolSTACK_H
#include <crtdbg.h>
#include "SLLIST.h"
#include "classSYNCOBJ.h"
//-------------------------------------------------------------------------------------------------

template <class t_DATA>
class CPoolSTACK  : public CCriticalSection
{
private:
	union UDataNODE {
		t_DATA		 m_DATA;
		UDataNODE	*m_pNextNODE;
	} ;

	UINT					m_uiTotalDataCNT;
	UINT					m_uiIncDataCNT;
	int						m_iUsedCNT;

	classSLLIST<UDataNODE*> m_BlockLIST;
	UDataNODE			   *m_pHeadNODE;

	UDataNODE *Increase (UINT uiDataCNT)
	{
		UDataNODE *pDataBLOCK;
		
		pDataBLOCK = new UDataNODE[ uiDataCNT ];
		if ( NULL == pDataBLOCK )
			return NULL;
		
		m_BlockLIST.AllocNAppend( pDataBLOCK );
		m_uiTotalDataCNT += uiDataCNT;

		UDataNODE *pNextNODE = NULL;
		UDataNODE *pDataNODE;

		UDataNODE *pTailNODE = &pDataBLOCK[ uiDataCNT-1 ];
		pTailNODE->m_pNextNODE = NULL;
		for (int iC=uiDataCNT-1; iC>=0; iC--) {
			pDataNODE = &pDataBLOCK[ iC ];

			pDataNODE->m_pNextNODE = pNextNODE;
			pNextNODE = pDataNODE;
		}

		return pDataBLOCK;
	}

public :
	CPoolSTACK (UINT uiInitDataCNT, UINT uiIncDataCNT)
	{
		m_uiIncDataCNT = uiIncDataCNT;
		m_uiTotalDataCNT = 0;
		m_iUsedCNT = 0;

		m_pHeadNODE = this->Increase( uiInitDataCNT );
	}
	virtual ~CPoolSTACK ()
	{
		classSLLNODE< UDataNODE* > *pBlockNODE;

		pBlockNODE = m_BlockLIST.DeleteHead();
		while( pBlockNODE ) {
			delete[] pBlockNODE->DATA;
			delete   pBlockNODE;
			pBlockNODE = m_BlockLIST.DeleteHead();
		}
	}

	void*	Alloc ()
	{
		UDataNODE *pNODE;

		this->Lock ();
		{
			if ( NULL == this->m_pHeadNODE ) {
				this->m_pHeadNODE = this->Increase( m_uiIncDataCNT );
			}

			pNODE = this->m_pHeadNODE;
			this->m_pHeadNODE = this->m_pHeadNODE->m_pNextNODE;
			this->m_iUsedCNT ++;
		}
		this->Unlock ();

		return pNODE;
	}
	void	Free (void *pDATA)
	{
		UDataNODE *pNODE = reinterpret_cast<UDataNODE*>( pDATA );
		this->Lock ();
		{
			this->m_iUsedCNT --;
			_ASSERT( this->m_iUsedCNT >= 0 );

			pNODE->m_pNextNODE = m_pHeadNODE; 
			m_pHeadNODE = pNODE; 
		}
		this->Unlock ();
	}
} ;


#define	DECLARE_POOLSTACK(class_name)												\
public :																			\
	static CPoolSTACK< class_name >	m_Pool;											\
	void* operator new(size_t size)					{	return m_Pool.Alloc();	}	\
	void  operator delete(void* pData, size_t size)	{	m_Pool.Free(pData);		}		
	
#define	IMPLEMENT_POOLSTACK(class_name,I,J)		CPoolSTACK<class_name> class_name::m_Pool(I,J);

//-------------------------------------------------------------------------------------------------
#endif

