#ifndef _CMSGBOX_
#define _CMSGBOX_
#include "tgamectrl/tmsgbox.h"
#include "subclass/cslot.h"
class CTCommand;
class CIcon;

/**
* 메세지 박스용 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMsgBox :	public CTMsgBox
{
public:
	CMsgBox(void);
	virtual ~CMsgBox(void);

	virtual void Draw();
	virtual void Hide();
	virtual void Update( POINT ptMouse );
	virtual void MoveWindow( POINT pt );
	virtual void SetString( const char* szString );
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);


	void SetCommand( CTCommand* pCmdOk, CTCommand* pCmdCancel );
	void SetInvokerDlgID( unsigned int iDlgID );
	void SetIcons( std::list<CIcon*>& Icons );

	///중복되어서 열린 MsgBox들을 같은 Type별로 모두 닫고 싶을때 Type을 지정하고 OpenMsgBox에서 그 Type을 인자로 넘겨준다.
	enum{
		MSGTYPE_NONE,
		MSGTYPE_RECV_TRADE_REQ,
		MSGTYPE_RECV_PARTY_REQ,
		MSGTYPE_RECV_CLANJOIN_REQ,
		MSGTYPE_RECV_CART_RIDE_REQ,	//2인승 카트
	};
	
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
	std::vector<CSlot>	m_Slots;			/// 메세지 박스에 아이콘이 보여질때 사용되는 슬롯 컨테이너
};
#endif