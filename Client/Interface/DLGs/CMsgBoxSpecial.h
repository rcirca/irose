#ifndef _CMsgBoxSpecial_
#define _CMsgBoxSpecial_
#include "tmsgbox.h"

/**
* 서버와의 종료 대기를 표시하기 위한 다이얼로그
*	- 일반 메세지 박스와 구분하기 위하여 별도의 클래스로 정의하였다
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMsgBoxSpecial :	public CTMsgBox
{
public:
	CMsgBoxSpecial(void);
	virtual ~CMsgBoxSpecial(void);

	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Hide();

	void SetCommand( CTCommand* pCmdOk, CTCommand* pCmdCancel );
	void SetInvokerDlgID( unsigned int iDlgID );
	enum{
		IID_BTN_OK		= 255,
		IID_BTN_CANCEL	= 256
	};
protected:
	void SendCommand( unsigned int iDlgID, CTCommand* pCmd );
	void ClearCommand();

protected:
	CTCommand*			m_pCmdOk;			/// Ok Btn이 눌렸을때 처리할 Command
	CTCommand*			m_pCmdCancel;		/// Cancel Btn이 눌렸을때 처리할 Command
	unsigned int		m_iInvokerDlgID;	/// Command 를 처리할 TDialogID
};
#endif