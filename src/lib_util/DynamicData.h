#ifndef	__DYNAMICDATA_H
#define __DYNAMICDATA_H
//-------------------------------------------------------------------------------------------------

template <class dType>
class CDynamicITEM {
	public :
		ULONG	m_ulTag;
		ULONG	m_ulTime;
		dType	m_DATA;

		CDynamicITEM()	{	m_iTag=-1;	m_lTime=0;	}
} ;

//---------------------------------------------------------------------------
template <class dType>
class CDynamicDATA {
    public :
		short	m_nMaxDataCount;
		short	m_nOldDataIndex;
		CDynamicITEM <dType> *m_pITEM;

		CDynamicDATA ()		{	m_nMaxDataCount=0; m_pITEM=NULL;	}
		~CDynamicDATA ()	{	_Free ();							}

		bool _Init (short nMaxDataCount);
		void _Free (void);

		short GetData (int iTag, ULONG lTime);
		void  SetData (short nIndex, int iTag, dType DATA, ULONG lTime);
} ;


//---------------------------------------------------------------------------
template <class dType>
bool CDynamicDATA <dType>::_Init (short nMaxDataCount)
{
	if ( nMaxDataCount < 1 ) return false;
	
	m_nMaxDataCount = nMaxDataCount;
	
	m_pITEM = new CDynamicITEM <dType> [ m_nMaxDataCount ];
	if ( m_pITEM ) {
		short nI;

		for (nI=0; nI<m_nMaxDataCount; nI++) {
			m_pITEM[ nI ].m_iTag  = -1;
			m_pITEM[ nI ].m_lTime = 0;
		}

		return true;
	}

	return false;
}


//---------------------------------------------------------------------------
template <class dType>
void CDynamicDATA <dType>::_Free (void)
{
	if ( m_pITEM ) 
		delete[] m_pITEM;

	m_pITEM = NULL;
	m_nMaxDataCount = 0;
}


//---------------------------------------------------------------------------
template <class dType>
void CDynamicDATA <dType>::SetData (short nIndex, int iTag, dType DATA, ULONG lTime)
{
	if ( nIndex == 0 || nIndex >= m_nMaxDataCount )
		return;

	m_pITEM[ nIndex ].m_iTag  = iTag;
	m_pITEM[ nIndex ].m_lTime = lTime;
	m_pITEM[ nIndex ].m_DATA  = DATA;
}


//---------------------------------------------------------------------------
template <class dType>
short CDynamicDATA <dType>::GetData (int iTag, ULONG lTime)
{
	short nI;

	m_nOldDataIndex = 0;
	for (nI=1; nI<m_nMaxDataCount; nI++) {
		if ( m_pITEM[ nI ].m_iTag == iTag ) {
			m_pITEM[ nI ].m_lTime = lTime;
			return nI;
		}

		if ( !m_nOldDataIndex || ( (lTime-m_pITEM[ nI ].m_lTime) > (lTime-m_pITEM[ m_nOldDataIndex ].m_lTime) ) ) {
			m_nOldDataIndex = nI;
		}
	}
	
	// 못 찾았다...
	return 0;
}


//-------------------------------------------------------------------------------------------------
#endif
