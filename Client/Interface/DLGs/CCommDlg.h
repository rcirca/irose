#ifndef _CCommDlg_
#define _CCommDlg_
#include "TDialog.h"
class CZListBox;
class CFriendListItem;

/**
* Community 를 위한 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CCommDlg : public CTDialog
{
public:
	CCommDlg( int iDlgType );
	virtual ~CCommDlg(void);
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show();

	///친구목록 관련
	void AddFriend( DWORD dwUserTag, BYTE btStatus, const char* pszName );
	void ClearFriendList();
	void ChangeFriendStatus( DWORD dwUserTag, BYTE btStatus );

	void RemoveFriend( DWORD dwUserTag );

	CFriendListItem* FindFriend( DWORD dwUserTag );
	CFriendListItem* FindFriendByName( const char* pszName );

	///우편함목록 관련
	void ClearMemoList();
	void AddMemo( int iRowid, const char* pszFromName, const char* pszMemo , DWORD dwRecvTime );
	void DeleteMemo( int iIndex );

	/// 대화방목록 관련
	void ClearChatRoomList();
	void AddChatRoom( const char* pszTitle, BYTE btRoomType, short nRoomID, BYTE btUserCount );


	/// 클라이언트에 저장되어 있던 쪽지를 가져오기 위하여 필요한 Method(  Sqlite 사용중 )
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);

private:
	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	CZListBox*	GetZListBox( int iTab, int iListID );

	/// 클라이언트에 저장되어 있던 쪽지 가져오기( Sqlite 사용중 )
	void		LoadMemoFromDB();
private:

	enum{
		IID_BTN_CLOSE			= 10,
		IID_BTN_ICONIZE			= 11,
		IID_TABBEDPANE			= 20,
		IID_TABBUTTON_FRIEND	= 25,
		IID_ZLIST_FRIEND		= 26,
		IID_BTN_REMOVEFRIEND	= 24,
		IID_BTN_ADDFRIEND		= 29,
		IID_TABBUTTON_CHATROOM	= 35,
		IID_ZLIST_CHATROOM		= 36,
		IID_BTN_CREATECHATROOM	= 39,
		IID_TABBUTTON_MAILBOXM	= 45,
		IID_ZLIST_MAILBOX		= 46,
	};

	enum{
		TAB_FRIEND   = 2,
		TAB_CHATROOM = 1,
		TAB_MAILBOX  = 0
	};
};
#endif