#ifndef	__CSTRMAP_H
#define	__CSTRMAP_H
#include "classSTR.h"
// ------------------------------------------------------------------------------------------------

#define DEFAULT_TABLE_ENTRIES 	 256

template <typename T>
class CStrMAP {
	public :
		typedef	unsigned long	KEY;
		struct tagNODE {
			KEY			m_KEY;
			CStrVAR	    m_NAME;
			tagNODE	   *m_NEXT;
			T			m_VALUE;
		} ;

	private :
		size_t			m_iNodeCount;

		tagNODE		  **m_pTABLE;
		int				m_iTableCount;

		unsigned long	m_ulPos;
		tagNODE		   *m_pHEAD;
		tagNODE		   *m_pNODE;

	public  :
		CStrMAP (int iEntryCount=DEFAULT_TABLE_ENTRIES);
		~CStrMAP ();

		KEY		MakeKey (const char *szKeyName)
		{
			return ::StrToHashKey ( szKeyName );
		}

		int		GetTableCount ()				{	return m_iTableCount;	}
		tagNODE*GetEntryNode (int iTableIDX)	{	return m_pTABLE[ iTableIDX ]->m_NEXT;	}

		int		GetNodeCount (void)				{	return m_iNodeCount;		}
		int     GetEntryCount(int iEntryIDX);
		bool	GetFirst (KEY* pHashKey, T* pHashData);

		KEY		Insert (const char *szKeyName, T HashData )	
		{	
			KEY key = MakeKey( szKeyName );
			m_ulPos = (key & 0xFFFF) % m_iTableCount;

			m_pNODE = new tagNODE;
			if ( NULL == m_pNODE )
				return 0;

			m_pNODE->m_NAME.Set( szKeyName );
			m_pNODE->m_DATA  = HashData;
			m_pNODE->m_ulKEY = key;
			m_pNODE->m_NEXT  = m_pTABLE[ m_ulPos ]->m_NEXT;

			m_pTABLE[ m_ulPos ]->m_NEXT = m_pNODE;

			m_iNodeCount ++;
			return true;
		}

		bool	Delete (const char *szKeyName)				
		{	
			KEY key = MakeKey( szKeyName );
			m_ulPos = (key & 0xFFFF) % m_iTableCount;

			m_pHEAD = m_pTABLE[ m_ulPos ];
			m_pNODE = m_pHEAD->m_NEXT;
			while ( m_pNODE ) {
				if ( m_pNODE->m_ulKEY == ulHashKey ) {
					if ( 0 == strcmpi( m_pNODE->m_NAME.Get(), szKeyName ) ) {
						m_pHEAD->m_NEXT = m_pNODE->m_NEXT;
						delete m_pNODE;

						m_iNodeCount --;
						return true;
					}
				}

				m_pHEAD = m_pNODE;
				m_pNODE = m_pNODE->m_NEXT;
			}

			return false;
		}

		T		Search (const char *szKeyName)				
		{
			KEY key = MakeKey( szKeyName );

			m_ulPos = (key & 0xFFFF) % m_iTableCount;
			m_pNODE = m_pTABLE[ m_ulPos ]->m_NEXT;
			while ( m_pNODE ) {
				if ( m_pNODE->m_ulKEY == ulHashKey ) {
					if ( 0 == strcmpi( m_pNODE->m_NAME.Get(), szKeyName ) )
						return m_pNODE->m_DATA;
				}

				m_pNODE = m_pNODE->m_NEXT;
			}

			return NULL;
		}

		bool	Delete (KEY ulHashKey, T HashData);
		T		Search (KEY ulHashKey, T HashData);
} ;


template <typename T>
CStrMAP<T>::CStrMAP (int iEntryCount)
{
	m_iTableCount = iEntryCount;
	m_pTABLE = new tagNODE* [ m_iTableCount ];

	for (int iL=0; iL<m_iTableCount; iL++) {
		m_pTABLE[ iL ] = new tagNODE;
		m_pTABLE[ iL ]->m_NEXT = NULL;
	}

	m_iNodeCount = 0;
}


template <typename T>
CStrMAP<T>::~CStrMAP ()
{
	for (int iL=0; iL<m_iTableCount; iL++) {
		m_pNODE = m_pTABLE[ iL ]->m_NEXT;
		while( m_pNODE ) {
			m_pHEAD = m_pNODE;
			m_pNODE = m_pNODE->m_NEXT;

			delete m_pHEAD;
		}

		delete m_pTABLE[ iL ];
	}

	delete[] m_pTABLE;
    m_iTableCount = 0;
}


template <typename T>
bool CStrMAP<T>::GetFirst (KEY* pHashKey, T* pHashData)
{
	for (int iL=0; iL<m_iTableCount; iL++)
		if ( m_pTABLE[ iL ]->m_NEXT ) {
			*pHashData = m_pTABLE[ iL ]->m_NEXT->m_DATA;
			*pHashKey  = m_pTABLE[ iL ]->m_NEXT->m_ulKEY;
			return true;
		}

	return false;
}


template <typename T>
int CStrMAP<T>::GetEntryCount(int iEntryIDX)
{
    int iCnt=0;

    m_pNODE = m_pTABLE[ iEntryIDX ]->m_NEXT;
    while( m_pNODE ) {
		m_pNODE = m_pNODE->m_NEXT;
        iCnt ++;
    }

    return iCnt;
}


template <typename T>
bool CStrMAP<T>::Delete (KEY ulHashKey, T HashData)
{
	m_ulPos = (ulHashKey & 0xFFFF) % m_iTableCount;

	m_pHEAD = m_pTABLE[ m_ulPos ];
	m_pNODE = m_pHEAD->m_NEXT;
	while ( m_pNODE ) {
		if ( m_pNODE->m_ulKEY == ulHashKey ) {
			if ( m_pNODE->m_DATA == HashData ) {
				m_pHEAD->m_NEXT = m_pNODE->m_NEXT;
				delete m_pNODE;

				m_iNodeCount --;
				return true;
			}
		}

		m_pHEAD = m_pNODE;
		m_pNODE = m_pNODE->m_NEXT;
	}

	return false;
}


template <typename T>
T CStrMAP<T>::Search (KEY ulHashKey, T HashData)
{
	m_ulPos = (ulHashKey & 0xFFFF) % m_iTableCount;

	m_pNODE = m_pTALBE[ m_ulPos ]->m_NEXT;
	while ( m_pNODE ) {
		i ( m_pNODE->m_DATA == HashData ) {
			return m_pNODE->m_DATA;
		}
		m_pNODE = m_pNODE->m_NEXT;
	}

	return NULL;
}

// ------------------------------------------------------------------------------------------------
#endif
