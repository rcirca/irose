#ifndef _LOGIN_
#define _LOGIN_


#include "CExternalUI.h"
#include "../../../TGameCtrl/TDialog.h"

const int				MAX_ID_LENGTH = 		20;
const int				MAX_PASSWORD_LENGTH =	17;	

#define	TCP_LSV_PORT	29000
#define TCP_LSV_IP		"211.234.107.200"

///
/// Login Manager class
///
struct t_PACKET;


/**
* 로그인시 사용되는 인터페이스
*
* @Warning		일본의 경우 파트너사 선택콤보박스가 한개 더 있다
* @Author		최종진
* @Date			2005/9/6
*/
class CLogin : public CTDialog
{
public:
	CREATE_RTTI( CLogin, CExternalUI, CExternalUI );

private:
	///
	///	Observer class
	///
	CExternalUIManager*		m_pEUIManager;
	
	char					m_strID[32];
	char					m_strPassword[32];

public:
	CLogin();
	virtual ~CLogin();

	/// set observer class
	void SetEUIObserver( CExternalUIManager* pObserver );


	virtual UINT	Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    Create( const char* szIDD );
	virtual void	Show();
	virtual void	Hide();
	virtual void	Draw();
	virtual void    MsgBoxProc(  unsigned int iMsgType, short int iButton );
	/// set login user ID	
	/// @param strID user ID
	/// @return is valid ID	
	bool SetID( const char* strID );

	/// set login user Password
	/// @param strID user Password
	/// @return is valid Password
	bool SetPassword( const char* strPassword );

	const char*	GetID(){ return m_strID; }
	const char* GetPassword(){ return m_strPassword; }


	/// send login request	
	void SendLoginReq( );

	enum{
		IID_EDIT_ID		= 2,
		IID_EDIT_PWD	= 3,
		IID_BTN_OK		= 4,
		IID_BTN_CANCEL	= 5,
		IID_CHECKBOX_SAVE_LASTCONECTID = 10,
		IID_COMBOBOX_ROUTE = 20							/// 일본 파트너사 구분을 위한 콤보박스

	};
	enum{
		MSG_TYPE_EXIT,
		MSG_TYPE_NORMAL
	};
	
	bool ConnectLoginServer();
protected:

	void OnLButtonUp( unsigned iProcID );
	void OnLButtonDown( unsigned iProcID );

	HNODE				m_hCopyRight;					/// 저작권 Texture Node
	HNODE				m_hUserGrade;					/// 사용자 등급 Texture Node

	int					m_iHeightCopyRight;
	int					m_iWidthUserGrade;

};


#endif //_LOGIN_