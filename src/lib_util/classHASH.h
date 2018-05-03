#ifndef	__CLASS_HASH
#define	__CLASS_HASH
//-------------------------------------------------------------------------------------------------

typedef	unsigned long	t_HASHKEY;

t_HASHKEY StrToHashKey (const char* pString);

//-------------------------------------------------------------------------------------------------

#define DEFAULT_HASH_ENTRIES 	 128
// #define DEFAULT_HASH_ENTRIES (1024*8)

template <class t_HASHDATA>
struct tagHASH {
	t_HASHDATA		m_DATA;
	t_HASHKEY		m_ulKEY;
	tagHASH		   *m_NEXT;
} ;

template <class t_HASHDATA>
class classHASH {
private :
	int					 m_iRegCount;

	tagHASH<t_HASHDATA>**m_pTABLE;
	int					 m_iEntryCount;

public  :
	classHASH (int iEntryCount=DEFAULT_HASH_ENTRIES);
	~classHASH ();

	int			GetTableCount ()	{	return m_iEntryCount;	}
	tagHASH<t_HASHDATA> *GetEntryNode (int iTableIDX)	{	return m_pTABLE[ iTableIDX ]->m_NEXT;	}

	int			GetCount (void)		{	return m_iRegCount;		}
    int         GetEntryCount(int iEntryIDX);
	bool		GetFirst (t_HASHKEY* pHashKey, t_HASHDATA* pHashData);

	bool		Insert (t_HASHKEY ulHashKey, t_HASHDATA HashData);

	bool				 Delete (t_HASHKEY ulHashKey, t_HASHDATA HashData);
	tagHASH<t_HASHDATA> *Search (t_HASHKEY ulHashKey, t_HASHDATA HashData);

	t_HASHDATA			 Delete (t_HASHKEY ulHashKey);
	tagHASH<t_HASHDATA> *Search (t_HASHKEY ulHashKey);

	tagHASH<t_HASHDATA> *SearchContinue (tagHASH<t_HASHDATA> *pStartNODE, t_HASHKEY ulHashKey);
} ;


template <class t_HASHDATA>
classHASH<t_HASHDATA>::classHASH (int iEntryCount)
{
	m_iEntryCount = iEntryCount;
	m_pTABLE = new tagHASH<t_HASHDATA>* [ m_iEntryCount ];

	for (int iL=0; iL<m_iEntryCount; iL++) {
		m_pTABLE[ iL ] = new tagHASH<t_HASHDATA>;
		m_pTABLE[ iL ]->m_NEXT = NULL;
	}

	m_iRegCount = 0;
}

template <class t_HASHDATA>
classHASH<t_HASHDATA>::~classHASH ()
{
	tagHASH<t_HASHDATA> *pHEAD, *pNODE;

	for (int iL=0; iL<m_iEntryCount; iL++) {
		pNODE = m_pTABLE[ iL ]->m_NEXT;
		while( pNODE ) {
			pHEAD = pNODE;
			pNODE = pNODE->m_NEXT;

			delete pHEAD;
		}

		delete m_pTABLE[ iL ];
	}

	delete[] m_pTABLE;
    m_iEntryCount = 0;
}


template <class t_HASHDATA>
bool classHASH<t_HASHDATA>::GetFirst (t_HASHKEY* pHashKey, t_HASHDATA* pHashData)
{
	for (int iL=0; iL<m_iEntryCount; iL++)
		if ( m_pTABLE[ iL ]->m_NEXT ) {
			*pHashData = m_pTABLE[ iL ]->m_NEXT->m_DATA;
			*pHashKey  = m_pTABLE[ iL ]->m_NEXT->m_ulKEY;
			return true;
		}

	return false;
}


template <class t_HASHDATA>
bool classHASH<t_HASHDATA>::Insert (t_HASHKEY ulHashKey, t_HASHDATA HashData)
{
	tagHASH<t_HASHDATA> *pNODE;
	unsigned long ulPos = (ulHashKey & 0xFFFF) % m_iEntryCount;

	pNODE = new tagHASH<t_HASHDATA>;
	if ( NULL == pNODE )
		return false;

	pNODE->m_DATA  = HashData;
	pNODE->m_ulKEY = ulHashKey;
	pNODE->m_NEXT  = m_pTABLE[ ulPos ]->m_NEXT;

	m_pTABLE[ ulPos ]->m_NEXT = pNODE;

	m_iRegCount ++;
	return true;
}


template <class t_HASHDATA>
t_HASHDATA classHASH<t_HASHDATA>::Delete (t_HASHKEY ulHashKey)
{
	tagHASH<t_HASHDATA> *pHEAD, *pNODE;

	unsigned long ulPos = (ulHashKey & 0xFFFF) % m_iEntryCount;

	pHEAD = m_pTABLE[ ulPos ];
	pNODE = pHEAD->m_NEXT;
	while ( pNODE ) {
		if ( pNODE->m_ulKEY == ulHashKey ) {
			pHEAD->m_NEXT = pNODE->m_NEXT;
			t_HASHDATA hashDATA = pNODE->m_DATA;

			delete pNODE;

			m_iRegCount --;
			return hashDATA;
		}

		pHEAD = pNODE;
		pNODE = pNODE->m_NEXT;
	}

	return NULL;
}


template <class t_HASHDATA>
int classHASH<t_HASHDATA>::GetEntryCount(int iEntryIDX)
{
    int iCnt=0;

    pNODE = m_pTABLE[ iEntryIDX ]->m_NEXT;
    while( pNODE ) {
		pNODE = pNODE->m_NEXT;
        iCnt ++;
    }

    return iCnt;
}


template <class t_HASHDATA>
tagHASH<t_HASHDATA> *classHASH<t_HASHDATA>::SearchContinue (tagHASH<t_HASHDATA> *pStartNODE, t_HASHKEY ulHashKey)
{
	tagHASH<t_HASHDATA> *pNODE;

	pNODE = pStartNODE->m_NEXT;
	while ( pNODE ) {
        if ( pNODE->m_ulKEY == ulHashKey ) {
    		return pNODE;
        }

		pNODE = pNODE->m_NEXT;
	}

	return NULL;
}


template <class t_HASHDATA>
tagHASH<t_HASHDATA> *classHASH<t_HASHDATA>::Search (t_HASHKEY ulHashKey)
{
	tagHASH<t_HASHDATA> *pNODE;

	unsigned long ulPos = (ulHashKey & 0xFFFF) % m_iEntryCount;

	pNODE = m_pTABLE[ ulPos ]->m_NEXT;
	while ( pNODE ) {
		if ( pNODE->m_ulKEY == ulHashKey )
			return pNODE;

		pNODE = pNODE->m_NEXT;
	}

	return NULL;
}


template <class t_HASHDATA>
bool classHASH<t_HASHDATA>::Delete (t_HASHKEY ulHashKey, t_HASHDATA HashData)
{
	tagHASH<t_HASHDATA> *pHEAD, *pNODE;

	unsigned long ulPos = (ulHashKey & 0xFFFF) % m_iEntryCount;

	pHEAD = m_pTABLE[ ulPos ];
	pNODE = pHEAD->m_NEXT;
	while ( pNODE ) {
		if ( pNODE->m_ulKEY == ulHashKey ) {
			if ( pNODE->m_DATA == HashData ) {
				pHEAD->m_NEXT = pNODE->m_NEXT;
				delete pNODE;

				m_iRegCount --;
				return true;
			}
		}

		pHEAD = pNODE;
		pNODE = pNODE->m_NEXT;
	}

	return false;
}


template <class t_HASHDATA>
tagHASH<t_HASHDATA> *classHASH<t_HASHDATA>::Search (t_HASHKEY ulHashKey, t_HASHDATA HashData)
{
	tagHASH<t_HASHDATA> *pNODE;

	unsigned long ulPos = (ulHashKey & 0xFFFF) % m_iEntryCount;

	pNODE = m_pTALBE[ ulPos ]->m_NEXT;
	while ( pNODE ) {
		i ( pNODE->m_DATA == HashData ) {
			return pNODE;
		}
		pNODE = pNODE->m_NEXT;
	}

	return NULL;
}


//-------------------------------------------------------------------------------------------------
#endif