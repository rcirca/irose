#ifndef _APPMSGQ_
#define _APPMSGQ_


#include "Util/WndMsgQ.h"
#include <deque>


/*struct tagWNDMSG 
{
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
*/

class CAppMsgQ
{
private:
	std::deque< tagWNDMSG >		m_KeyboardMsgQ;
	std::deque< tagWNDMSG >		m_MouseMsgQ;


	bool					AddMsgToMouseQ( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	bool					AddMsgToKeyboardQ( UINT uiMsg, WPARAM wParam, LPARAM lParam );

public:
	CAppMsgQ(void);
	~CAppMsgQ(void);


	///
	///	Add app msg to suitable queue
	///


	bool					AddMsgToQ( UINT uiMsg, WPARAM wParam, LPARAM lParam );


	///
	/// get specific msg from queue
	///
	bool					GetMouseMsgFromQ( tagWNDMSG& WndMsg );
	bool					GetKeyboardMsgFromQ( tagWNDMSG& WndMsg );
	void					Clear();
};


#endif //_APPMSGQ_