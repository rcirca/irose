/*
	$Header: /Client/Util/classQUEUE.h 3     03-09-26 1:26p Icarus $
*/
#ifndef	__CQUEUE_H
#define	__CQUEUE_H
#include "classSTR.h"
#include "DLLIST.h"
//-------------------------------------------------------------------------------------------------

#define	DEFAULT_MSGQ_SIZE	128

template <class dType>
class classQNODE {
public :
	bool	m_bAlloced;
	dType	m_DATA;

	classQNODE ()	{	m_bAlloced=false, m_DATA=NULL;	}
} ;


template <class dType>
class classQUEUE {
private :
	classDLLNODE< classQNODE<dType> >  *m_pCurNODE;
	classDLLIST < classQNODE<dType> >	m_LIST;
	int	 m_iMaxQSize;

public :
	classQUEUE (int iQSize=DEFAULT_MSGQ_SIZE)	{	m_iMaxQSize=iQSize;	}

	bool	AddItem (dType Item, bool bAlloced=false);
	void	DelFirst ();
	dType	GetFirst ();
	dType	GetNext ();
} ;

template <class dType>
bool classQUEUE<dType>::AddItem (dType Item, bool bAlloced)
{
	if ( m_LIST.GetNodeCount() >= m_iMaxQSize ) {
		this->DelFirst ();
	}

	m_pCurNODE = m_LIST.AllocNAppend ();
	if ( m_pCurNODE ) {
		m_pCurNODE->DATA.m_bAlloced = bAlloced;
		m_pCurNODE->DATA.m_DATA = Item;
		return true;
	}
	return false;
}

template <class dType>
void classQUEUE<dType>::DelFirst ()
{
	m_pCurNODE = m_LIST.GetHeadNode ();
	if ( m_pCurNODE ) {
		if ( m_pCurNODE->DATA.m_bAlloced ) {
			delete m_pCurNODE->DATA.m_DATA;
		}

		m_LIST.DeleteNFree( m_pCurNODE );
	}
}

template <class dType>
dType classQUEUE<dType>::GetFirst ()
{
	m_pCurNODE = m_LIST.GetHeadNode ();
	if ( m_pCurNODE )
		return m_pCurNODE->DATA.m_DATA;
	return NULL;
}

template <class dType>
dType classQUEUE<dType>::GetNext ()
{
	m_pCurNODE = m_LIST.GetNextNode( m_pCurNODE );
	if ( m_pCurNODE )
		return m_pCurNODE->DATA.m_DATA;
	return NULL;
}

//-------------------------------------------------------------------------------------------------
#endif