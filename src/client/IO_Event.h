/*
	$Header: /Client/IO_Event.h 12    05-01-07 2:20p Jeddli $
*/
#ifndef	__IO_EVENT_H
#define	__IO_EVENT_H
#include "Event\CEvent.h"
//-------------------------------------------------------------------------------------------------

enum EVENT_DLG_TYPE
{
	EVENT_DLG_NPC = 0,
	EVENT_DLG_WARP,
	EVENT_DLG_EVENT_OBJECT,
};



/// Event list
class CEventLIST : public CFileLIST< CEvent* > {
private:
	bool	Load_FILE (tagFileDATA< CEvent* > *pDATA);
	void	Free_FILE (tagFileDATA< CEvent* > *pDATA);

	t_HASHKEY Add_EVENT (char *szFileName);

public :
	CEventLIST ();

	void	Clear ();	  
	void	Del_EVENT (t_HASHKEY HashKEY);
	bool	Run_EVENT( int iOwnerObjIDX, char *szQuestFILE, short nEventIDX, int iEventDlgType = EVENT_DLG_NPC );
	bool	Run_EVENT( int iOwnerObjIDX, short nQuestIDX, short nEventIDX, int iEventDlgType = EVENT_DLG_NPC );
} ;
extern CEventLIST *g_pEventLIST;

//-------------------------------------------------------------------------------------------------
#endif
