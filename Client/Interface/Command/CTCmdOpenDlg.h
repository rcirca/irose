#ifndef _CTCMDOPENDLG_
#define _CTCMDOPENDLG_
#include "tgamectrl/tcommand.h"
#include "../dlgs/cmsgbox.h"

class CTCmdNumberInput;
class CTDialog;
class CIcon;


/// 현재 사용안하고 있다.
class CTCmdOpenDlg : public CTCommand
{
public:
	CTCmdOpenDlg(void);
	~CTCmdOpenDlg(void);

	void	SetOpenDlgID(int iID){ m_iOpenDlgID = iID; }
	void	SetInvokerDlgID(int iID ){ m_iInvokerDlgID = iID; }
	virtual bool	Exec( CTObject* pObj ){ return true; }
protected:
	int		m_iOpenDlgID;
	int		m_iInvokerDlgID;
};

struct CreateMsgBoxData{
	bool		bModal;		///Modal/Modeless
	int			iMsgType;	///중복이 불가능할경우 
	int			iButtonType;
	int			iInvokerDlgID;
	CTCommand*  pCmdOk;
	CTCommand*	pCmdCancel;
	std::string	strMsg;
	std::list<CIcon*>	m_Icons;///메세지 박스에 아이콘을 그리기 원할때 사용

	WORD		parm1;
	WORD		parm2;

	CreateMsgBoxData(){
		pCmdOk		= NULL;
		pCmdCancel	= NULL;
		bModal		= true;
		iMsgType	= 0;
		iButtonType = CMsgBox::BT_OK;
		iInvokerDlgID   = 0;
		strMsg			= "MessageBox";

		parm1			= 0;
		parm2			= 0;
	}
};

/// 메세지 박스 생성및 오픈
class CTCmdCreateMsgBox : public CTCommand
{
public:
	CTCmdCreateMsgBox( CreateMsgBoxData& Data );
	virtual ~CTCmdCreateMsgBox(void);

	virtual bool Exec( CTObject* pObj );

protected:
	CreateMsgBoxData	m_Data;

};
/// 메세지 박스 삭제
class CTCmdDeleteMsgBox : public CTCommand
{
public:
	CTCmdDeleteMsgBox( CTDialog* pDlg );
	virtual ~CTCmdDeleteMsgBox(void ){};
	virtual bool Exec( CTObject* pObj );

protected:
	CTDialog*	m_pDialog;
};
/// 1:1 대화창 생성및 오픈
class CTCmdOpenPrivateChatDlg : public CTCommand
{
public:
	CTCmdOpenPrivateChatDlg( DWORD dwUserTag , BYTE btStatus, const char* pszName );
	virtual ~CTCmdOpenPrivateChatDlg(){}
	virtual bool Exec( CTObject* pObj );

protected:
	DWORD		m_dwUserTag;
	BYTE		m_btStatus;
	std::string m_strName;
};

/// 1:1 대화창 삭제
class CTCmdClosePrivateChatDlg : public CTCommand
{
public:
	CTCmdClosePrivateChatDlg( DWORD dwUserTag );
	virtual ~CTCmdClosePrivateChatDlg(){}
	virtual bool Exec( CTObject* pObj );
protected:
	DWORD	m_dwUserTag;
};
#endif