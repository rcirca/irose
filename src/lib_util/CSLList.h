#ifndef CSLLIST_H
#define CSLLIST_H
// ------------------------------------------------------------------------------------------------

template <typename T>
class CSLList {
	public :
		struct tagNODE {
			tagNODE		*m_pNEXT;
			T			 m_VALUE;
			tagNODE *GetNext ()		{	return m_pNEXT;	}
		} ;
    private :
        size_t		 m_iSIZE;
        tagNODE		*m_pHEAD;
        tagNODE		*m_pTAIL;

    public :
        CSLList ()		{ this->Init();	}

		void	Init (void);
		bool	IsEmpty (void)				{ return ( 0 == m_iSIZE);	}
        size_t	GetNodeCount(void) 			{ return m_iSIZE;			}

        tagNODE	*AllocNAppend (T DATA)
		{
			tagNODE* pNewNode = new tagNODE(NULL);
			if ( pNewNode ) {
				AppendNode (pNewNode);
				pNewNode->DATA = DATA;
				return pNewNode;
			}
			_ASSERT( pNewNode );
			return NULL;
		}

        tagNODE	*AllocNAppend (void)
		{
			tagNODE* pNewNode = new tagNODE(NULL);
			if ( pNewNode ) {
				AppendNode (pNewNode);
				return pNewNode;
			}
			_ASSERT( pNewNode );
			return NULL;
		}

        bool     DeleteHeadNFree (void);
        void	 AppendNode (tagNODE*NodeToAppend);

        tagNODE	*DeleteHead (void)
		{
			tagNODE*pDelNode;

			if ( m_pHEAD == NULL ) return NULL;

			pDelNode = m_pHEAD;

			m_pHEAD = m_pHEAD->m_pNEXT;
			if ( m_pHEAD == NULL )
				m_pTAIL = NULL;

			m_iSIZE --;

			return pDelNode;
		}

        tagNODE	*GetHeadNode (void)					{	return m_pHEAD;				}
		tagNODE	*GetTailNode (void)					{	return m_pTAIL;				}
        tagNODE	*GetNextNode (tagNODE*CurrentNode)	{	return pCurNode->m_pNEXT;	}

		void AppendNodeList (CSLList<T> *pList);

		void ClearList ();
		void ClearListNFree ();
} ;


//---------------------------------------------------------------------------
template <typename T>
void CSLList<T>::Init (void)
{
	m_pHEAD = NULL;
	m_pTAIL = NULL;
	m_iSIZE = 0;
}

//---------------------------------------------------------------------------
template <typename T>
void CSLList<T>::AppendNodeList (CSLList<T> *pList)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = pList->GetHeadNode ();
		m_pTAIL = pList->GetTailNode ();
		m_iSIZE = pList->GetNodeCount ();
    } else {
		tagNODE*pHEAD = pList->GetHeadNode ();
		if ( pHEAD ) {
		    m_pTAIL->m_pNEXT = pHEAD;

			m_pTAIL = pList->GetTailNode ();
			m_iSIZE += pList->GetNodeCount ();
		}
    }
}

//---------------------------------------------------------------------------

template <typename T>
bool CSLList <T>::DeleteHeadNFree (void)
{
    tagNODE*pNode;

	pNode = DeleteHead ();
	if ( pNode ) {
        delete pNode;
	    pNode = NULL;
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

template <typename T>
void  CSLList <T>::AppendNode(tagNODE*NodeToAdd)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = m_pTAIL	= NodeToAdd;
        NodeToAdd->m_pNEXT	= NULL;
    } else {
        NodeToAdd->m_pNEXT	= NULL;
        m_pTAIL->m_pNEXT	= NodeToAdd;

        m_pTAIL = NodeToAdd;
    }

    m_iSIZE ++;
} ;

//---------------------------------------------------------------------------

template <typename T>
void  CSLList <T>::ClearList ()
{
    tagNODE*pNode;
	pNode = DeleteHead ();
	while( pNode ) 
		pNode = DeleteHead ();
}

//---------------------------------------------------------------------------

template <typename T>
void  CSLList <T>::ClearListNFree ()
{
	while( DeleteHeadNFree () )
		;
}

// ------------------------------------------------------------------------------------------------
#endif

