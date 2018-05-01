#ifndef	__CDLLIST_H
#define	__CDLLIST_H
// ------------------------------------------------------------------------------------------------

template <typename T>
class CDLList {
	public :
		struct tagNODE {
			tagNODE	   *m_pPREV;
			tagNODE	   *m_pNEXT;
			T			m_VALUE;
			tagNODE *GetNext ()		{	return m_pNEXT;	}
			tagNODE *GetPrev ()		{	return m_pPREV;	}
		} ;
	private :
        int			 m_iSIZE;
        tagNODE		*m_pHEAD;
        tagNODE		*m_pTAIL;

    public :
        CDLList ()		{	this->Init ();	}

		void	 Init (void);
		bool	 IsEmpty (void)							{ return ( 0 == m_iSIZE);					}
        int		 GetNodeCount(void) 					{ return m_iSIZE;							}
		void	 ResetNode   (void)						{ m_pHEAD = m_pTAIL = NULL; m_iSIZE = 0;	}
        tagNODE *GetHeadNode (void)						{ return m_pHEAD;							}
        tagNODE *GetTailNode (void)						{ return m_pTAIL;							}
        tagNODE *GetNextNode (tagNODE *pCurrentNode)	{ return pCurrentNode->GetNext();			}
        tagNODE *GetPrevNode (tagNODE *pCurrentNode)	{ return pCurrentNode->GetPrev();			}

        tagNODE *AllocNAppend (T DATA)
		{
			tagNODE* pNewNode = new tagNODE;
			if ( pNewNode ) {
				AppendNode (pNewNode);
				pNewNode->m_VALUE = DATA;
				return pNewNode;
			}
			return NULL;
		}
        tagNODE *AllocNAppend (void)
		{
			tagNODE* pNewNode = new tagNODE;
			if ( pNewNode ) {
				AppendNode (pNewNode);
				return pNewNode;
			}
			return NULL;
		}

        void DeleteNFree (tagNODE *pNodeToDelete);

        void AppendNode		  (tagNODE *pNodeToAppend);
        void DeleteNode		  (tagNODE *pNodeToDelete);
		void ReplaceNode	  (tagNODE *TargetNode, tagNODE *pNodeToReplace);
		void InsertNodeToPrev (tagNODE *TargetNode, tagNODE *pNodeToInsert);
		void InsertNodeToNext (tagNODE *TargetNode, tagNODE *pNodeToInsert);

		void AppendNodeList (CDLList<T> *pList);

		void ClearList ();
		void ClearListNFree ();
		/*
		class iterator {
			private :
				tagNODE	*m_pNODE;

			public :
				iterator( tagNODE *pNODE ) : m_pNODE(pNODE)		{	}
				iterator( iterator &it ) : m_pNODE(it.m_pNODE)	{	}

				T &		  operator * ()
				{
					return (*m_pNODE).m_VALUE;
				}
				iterator& operator++ ()
				{
					m_pNODE = m_pNODE->m_pNEXT;
					return *this;
				}
				iterator  operator++(int)
				{
					iterator tmp = *this;
					++*this;
					return tmp;
				}
				iterator& operator-- ()
				{
					m_pNODE = m_pNODE->m_pPREV;
					return *this;
				}
				iterator  operator--(int)
				{
					iterator tmp = *this;
					--*this;
					return tmp;
				}
		} ;
		iterator	begin()	const	{	return iterator( m_pHEAD->m_pNEXT );	}
		iterator	end()	const	{	return iterator( m_pHEAD );				}
		size_t		size()	const	{	return m_iSIZE;							}
		bool		empty()	const	{	return ( 0 == size() );					}
		*/
} ;


//---------------------------------------------------------------------------
template <typename T>
void CDLList<T>::Init (void)
{
	m_pHEAD = NULL;
	m_pTAIL = NULL;
	m_iSIZE = 0;
}

//---------------------------------------------------------------------------
template <typename T>
void CDLList<T>::AppendNodeList (CDLList<T> *pList)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = pList->GetHeadNode ();
		m_pTAIL = pList->GetTailNode ();
		m_iSIZE = pList->GetNodeCount ();
    } else {
		tagNODE *pHEAD = pList->GetHeadNode ();
		if ( pHEAD ) {
			pHEAD->m_pPREV   = m_pTAIL;
		    m_pTAIL->m_pNEXT = pHEAD;

			m_pTAIL = pList->GetTailNode ();
			m_iSIZE += pList->GetNodeCount ();
		}
    }
}

//---------------------------------------------------------------------------
template <typename T>
void CDLList <T>::DeleteNFree (tagNODE *pNodeToDelete)
{
    if ( pNodeToDelete ) {
        DeleteNode (pNodeToDelete);
        delete pNodeToDelete;
    }

    pNodeToDelete = NULL;
}


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::AppendNode(tagNODE *pNodeToAdd)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = m_pTAIL = pNodeToAdd;
        pNodeToAdd->m_pNEXT = NULL;
        pNodeToAdd->m_pPREV = NULL;
    } else {
        pNodeToAdd->m_pNEXT = NULL;
		pNodeToAdd->m_pPREV = m_pTAIL;
        m_pTAIL->m_pNEXT = pNodeToAdd;

        m_pTAIL = pNodeToAdd;
    }

    m_iSIZE ++;
} ;

//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::DeleteNode(tagNODE *pNodeToDel)
{
    if ( pNodeToDel == NULL ) return;

    if ( pNodeToDel == m_pHEAD ) {
        m_pHEAD = m_pHEAD->m_pNEXT;

        if ( m_pHEAD != NULL )
            m_pHEAD->m_pPREV = NULL;
        else
            m_pTAIL = NULL;
    } else
    if ( pNodeToDel == m_pTAIL ) {
        m_pTAIL = m_pTAIL->m_pPREV;
        m_pTAIL->m_pNEXT = NULL;
    } else {
        pNodeToDel->m_pPREV->m_pNEXT = pNodeToDel->m_pNEXT;
        pNodeToDel->m_pNEXT->m_pPREV = pNodeToDel->m_pPREV;
    }

//    NodeToDel->m_pPREV = NodeToDel->m_pNEXT = NULL;
    m_iSIZE --;
} ;


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::ReplaceNode (tagNODE *pTargetNode, tagNODE *pNodeToReplace)
{
	if ( TargetNode == NULL ) return;

	pNodeToReplace->m_pPREV = pTargetNode->m_pPREV;
	pNodeToReplace->m_pNEXT = pTargetNode->m_pNEXT;

	if ( pTargetNode == m_pHEAD ) 
		m_pHEAD = pNodeToReplace;
	else
		pTargetNode->m_pPREV->m_pNEXT = pNodeToReplace;

	if ( pTargetNode == m_pTAIL ) 
		m_pTAIL = NodeToReplace;
	else
		pTargetNode->m_pNEXT->m_pPREV = pNodeToReplace;
}


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::InsertNodeToPrev (tagNODE *pTargetNode, tagNODE *pNodeToIns)
{
	if ( pTargetNode == NULL ) return;

    if ( pTargetNode == m_pHEAD ) {
        pNodeToIns->m_pPREV	= NULL;
        pNodeToIns->m_pNEXT	= m_pHEAD;
        m_pHEAD->m_pPREV	= pNodeToIns;
        m_pHEAD				= pNodeToIns;
    } else {
        pNodeToIns->m_pPREV	= pTargetNode->m_pPREV;
        pNodeToIns->m_pNEXT	= pTargetNode;
        pTargetNode->m_pPREV->m_pNEXT = pNodeToIns;
        pTargetNode->m_pPREV = pNodeToIns;
    }

    m_iSIZE ++;
}


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::InsertNodeToNext (tagNODE *TargetNode, tagNODE *NodeToIns)
{
	if ( pTargetNode == NULL ) return;

    if ( pTargetNode == m_pTAIL ) {
        pNodeToIns->m_pPREV	= m_pTAIL;
        pNodeToIns->m_pNEXT	= NULL;
        m_pTAIL->m_pNEXT	= pNodeToIns;
        m_pTAIL = NodeToIns;
    } else {
        pNodeToIns->m_pPREV	= pTargetNode;
        pNodeToIns->m_pNEXT	= pTargetNode->m_pNEXT;
        pTargetNode->m_pNEXT->m_pPREV = pNodeToIns;
        pTargetNode->m_pNEXT = pNodeToIns;
    }

    m_iSIZE ++;
}


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::ClearList ()
{
	tagNODE *pNode;

	pNode = GetHeadNode ();
	while( pNode ) {
		DeleteNode (pNode);
		pNode = GetHeadNode ();
	}
}


//---------------------------------------------------------------------------

template <typename T>
void  CDLList <T>::ClearListNFree ()
{
	tagNODE *pNode;

	pNode = GetHeadNode ();
	while( pNode ) {
		DeleteNFree (pNode);
		pNode = GetHeadNode ();
	}
}



// ------------------------------------------------------------------------------------------------
#endif