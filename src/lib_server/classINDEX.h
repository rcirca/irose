#ifndef	__CIndexARRAY_H
#define	__CIndexARRAY_H
//-------------------------------------------------------------------------------------------------


template <class DataType>
class CIndexARRAY {
protected :
	DataType		   *m_pINDEX;
	unsigned int		m_uiBuffSize;
	unsigned int		m_uiLastSlot;
	int					m_iUsedSlot;

	virtual void Lock ()	{	/*nop*/	}
	virtual void Unlock ()	{	/*nop*/	}

	unsigned int	GetEmptySlot ();

public  :
	CIndexARRAY (char *szName, unsigned int uiBuffSize);
    virtual ~CIndexARRAY ();

	void	 ClearAll ();

	int		 GetEmptySlotCNT ()		{	return ( m_uiBuffSize-m_iUsedSlot );	}
	int		 GetUsedSlotCNT()		{	return m_iUsedSlot;					}

	unsigned int	AddData (DataType dtValue);
	void			DelData (unsigned int uiSlotNo);
	DataType		GetData (unsigned int uiSlotNo);
} ;


//-------------------------------------------------------------------------------------------------
template <class DataType>
CIndexARRAY<DataType>::CIndexARRAY (char *szName, unsigned int uiBuffSize)
{
	m_uiBuffSize = uiBuffSize;
	m_uiLastSlot = 1;
	m_iUsedSlot = 0;

	m_pINDEX = new DataType [ uiBuffSize ];
	if ( NULL == m_pINDEX ) {
		g_LOG.CS_ODS(0xffff, "ERROR :: Out of memory - create index buffer[ %d ] failed .. \n", uiBuffSize);
		return;
	}

	for (unsigned int iL=0; iL<uiBuffSize; iL++)
		m_pINDEX[ iL ] = NULL;

	g_LOG.CS_ODS(0xffff, "Create [%s] index buffer :: %d \n", szName, uiBuffSize);
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
CIndexARRAY<DataType>::~CIndexARRAY ()
{
	delete[] m_pINDEX;
    m_pINDEX = NULL;
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
void CIndexARRAY<DataType>::ClearAll ()
{
	this->Lock ();
	::ZeroMemory( m_pINDEX, sizeof(DataType) * m_uiBuffSize );
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
unsigned int CIndexARRAY<DataType>::GetEmptySlot ()
{
	unsigned int iL;

	for (iL=m_uiLastSlot; iL<m_uiBuffSize; iL++) {
		if ( NULL == m_pINDEX[ iL ] ) {
			m_uiLastSlot = iL+1;
			return iL;
		}
	}

	for (iL=1; iL<=m_uiLastSlot; iL++) {
		if ( NULL == m_pINDEX[ iL ] ) {
			m_uiLastSlot = iL+1;
			return iL;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
unsigned int CIndexARRAY<DataType>::AddData (DataType dtValue)
{
	this->Lock ();

	unsigned int uiSlotNo = GetEmptySlot ();
	if ( uiSlotNo ) {
		m_pINDEX[ uiSlotNo ] = dtValue;
		m_iUsedSlot ++; 
	}

	this->Unlock ();

	return uiSlotNo;
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
void CIndexARRAY<DataType>::DelData (unsigned int uiSlotNo)
{
	this->Lock ();

	if ( uiSlotNo ) {
		m_pINDEX[ uiSlotNo ] = NULL;
		m_iUsedSlot --; 
	}

	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
DataType CIndexARRAY<DataType>::GetData (unsigned int uiSlotNo)
{
	if ( uiSlotNo >= m_uiBuffSize )
		return NULL;
	return m_pINDEX[ uiSlotNo ];
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
template <class DataType>
class CSyncIdxARRAY : public CIndexARRAY<DataType> {
protected :
	CRITICAL_SECTION	m_csIndex;

	void Lock ()	{	::EnterCriticalSection( &m_csIndex );	}
	void Unlock ()	{	::LeaveCriticalSection( &m_csIndex );	}

public  :
	CSyncIdxARRAY (char *szName, int iBuffSize);
    virtual ~CSyncIdxARRAY ();
} ;

//-------------------------------------------------------------------------------------------------
template <class DataType>
CSyncIdxARRAY<DataType>::CSyncIdxARRAY (char *szName, int iBuffSize) : CIndexARRAY<DataType>(szName, iBuffSize)
{
    ::InitializeCriticalSection( &m_csIndex );
}

//-------------------------------------------------------------------------------------------------
template <class DataType>
CSyncIdxARRAY<DataType>::~CSyncIdxARRAY ()
{
    ::DeleteCriticalSection( &m_csIndex );
}

//-------------------------------------------------------------------------------------------------
#endif