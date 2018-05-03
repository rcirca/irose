/*
	$Header: /7HeartsOnline/Client/Util/SLLIST.h 2     04-05-27 8:32p Icarus $
*/
#ifndef SLLISTH
#define SLLISTH

//---------------------------------------------------------------------------
template <class dType>
class classSLLNODE {
    private :

    public :
        dType           DATA;

        classSLLNODE   *m_pNEXT;


        classSLLNODE (int   nArg1) { m_pNEXT = NULL; }
        classSLLNODE (dType pArg1) { DATA    = pArg1; m_pNEXT = NULL; }
        classSLLNODE ()            { m_pNEXT = NULL; }
} ;

template <class dType>
class classSLLIST {
    private :
        int                iNodeCount;
        classSLLNODE <dType> *m_pHEAD;
        classSLLNODE <dType> *m_pTAIL;

    public :
        classSLLIST ()		{ this->Init();	}

		void				  Init (void);
		bool				  IsEmpty (void)				{	return !iNodeCount;	}
        int					  GetNodeCount (void)			{	return  iNodeCount;	}

        classSLLNODE <dType> *AllocNAppend (dType DATA);
        classSLLNODE <dType> *AllocNAppend (void);
        bool                  DeleteHeadNFree (void);

        void				  AppendNode (classSLLNODE <dType> *NodeToAppend);
        classSLLNODE <dType> *DeleteHead (void);
        classSLLNODE <dType> *GetHeadNode (void)	{	return m_pHEAD;		}
		classSLLNODE <dType> *GetTailNode (void)	{	return m_pTAIL;		}
        classSLLNODE <dType> *GetNextNode (classSLLNODE <dType> *CurrentNode);

		void AppendNodeList (classSLLIST<dType> *pList);

		void ClearList ();
		void ClearListNFree ();
} ;


//---------------------------------------------------------------------------
template <class dType>
void classSLLIST<dType>::Init (void)
{
	m_pHEAD = NULL;
	m_pTAIL = NULL;
	iNodeCount = 0;
}

//---------------------------------------------------------------------------
template <class dType>
void classSLLIST<dType>::AppendNodeList (classSLLIST<dType> *pList)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = pList->GetHeadNode ();
		m_pTAIL = pList->GetTailNode ();
		iNodeCount = pList->GetNodeCount ();
    } else {
		classSLLNODE <dType> *pHEAD = pList->GetHeadNode ();
		if ( pHEAD ) {
		    m_pTAIL->m_pNEXT = pHEAD;

			m_pTAIL = pList->GetTailNode ();
			iNodeCount += pList->GetNodeCount ();
		}
    }
}

//---------------------------------------------------------------------------

template <class dType>
classSLLNODE <dType> *classSLLIST <dType>::AllocNAppend (dType DATA)
{
    classSLLNODE <dType>* pNewNode;

    pNewNode = new classSLLNODE <dType> (NULL);
    if ( pNewNode ) {
        AppendNode (pNewNode);
		pNewNode->DATA = DATA;
		return pNewNode;
    }
	_ASSERT( pNewNode );
	return NULL;
}

template <class dType>
classSLLNODE <dType> *classSLLIST <dType>::AllocNAppend (void)
{
    classSLLNODE <dType>* pNewNode;

    pNewNode = new classSLLNODE <dType> (NULL);
    if ( pNewNode ) {
        AppendNode (pNewNode);
		return pNewNode;
    }
	_ASSERT( pNewNode );
	return NULL;
}

template <class dType>
bool classSLLIST <dType>::DeleteHeadNFree (void)
{
    classSLLNODE <dType> *pNode;

	pNode = DeleteHead ();
	if ( pNode ) {
        delete pNode;
	    pNode = NULL;
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

template <class dType>
void  classSLLIST <dType>::AppendNode(classSLLNODE <dType> *NodeToAdd)
{
    if ( m_pHEAD == NULL ) {
   	    m_pHEAD = m_pTAIL	= NodeToAdd;
        NodeToAdd->m_pNEXT	= NULL;
    } else {
        NodeToAdd->m_pNEXT	= NULL;
        m_pTAIL->m_pNEXT	= NodeToAdd;

        m_pTAIL = NodeToAdd;
    }

    iNodeCount ++;
} ;

//---------------------------------------------------------------------------

// This dosn't DELETE the node, it just removes it from the list
template <class dType>
classSLLNODE <dType> *classSLLIST <dType>::DeleteHead(void)
{
    classSLLNODE <dType> *pDelNode;

    if ( m_pHEAD == NULL ) return NULL;

    pDelNode = m_pHEAD;

    m_pHEAD = m_pHEAD->m_pNEXT;
    if ( m_pHEAD == NULL )
        m_pTAIL = NULL;

    iNodeCount --;

    return pDelNode;
} ;

//---------------------------------------------------------------------------

template <class dType>
classSLLNODE <dType> *classSLLIST <dType>::GetNextNode (classSLLNODE <dType> *pCurNode)
{
    if ( pCurNode == NULL ) return NULL;

    return pCurNode->m_pNEXT;
}

//---------------------------------------------------------------------------

template <class dType>
void  classSLLIST <dType>::ClearList ()
{
    classSLLNODE <dType> *pNode;
	pNode = DeleteHead ();
	while( pNode ) 
		pNode = DeleteHead ();
}

//---------------------------------------------------------------------------

template <class dType>
void  classSLLIST <dType>::ClearListNFree ()
{
	while( DeleteHeadNFree () )
		;
}

//---------------------------------------------------------------------------
#endif

