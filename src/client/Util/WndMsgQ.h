/*
	$Header: /Client/Util/WndMsgQ.h 5     04-06-07 4:18p Jeddli $
*/
#ifndef	__WNDMSGQ_H
#define	__WNDMSGQ_H
//-------------------------------------------------------------------------------------------------

struct tagWNDMSG {
	UINT	uiMsg;
	WPARAM	wParam;
	LPARAM	lParam;

	void operator = ( tagWNDMSG& inMsg )
	{
		this->uiMsg = inMsg.uiMsg;
		this->wParam = inMsg.wParam;
		this->lParam = inMsg.lParam;
	};
} ;

//dd
class CWndMsgQ
{
private :
	short		m_nQHead;
	short		m_nQTail;
	short		m_nQCount;
	short		m_nMaxQSize;
	tagWNDMSG  *m_pQueue;

protected :
	void		Add_WndMSG2Q	(UINT uiMsg, WPARAM wParam, LPARAM lParam);

public  :
	tagWNDMSG	m_sCurMsg;

	CWndMsgQ()		{	m_pQueue = NULL;	_Init( 32 );	}
	~CWndMsgQ()		{	_Free();		}
	bool	_Init (short nMaxQSize=32);
	void	_Free (void);

			void Del_AllMSG	(void);
			void Del_CurMSG (void)		{	m_sCurMsg.uiMsg = 0;	}
			bool Get_WndMSG	(void);
	virtual bool Add_WndMSG	(UINT uiMsg, WPARAM wParam, LPARAM lParam)=0{ *(int*)0 = 10; };
} ;

//-------------------------------------------------------------------------------------------------
#endif