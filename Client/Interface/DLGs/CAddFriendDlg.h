#ifndef _AddFriendDlg_
#define _AddFriendDlg_
#include "TDialog.h"
/**
* 친구추가시 이름 입력 다이얼로그
* 
* @Author		최종진
* @Date			2005/9/12
*/
class CAddFriendDlg : public CTDialog
{
public:
	CAddFriendDlg( int iDlgType );
	virtual ~CAddFriendDlg(void);

	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
protected:
	enum{
		IID_BTN_CLOSE	= 10,
		IID_BTN_CONFIRM = 11,
		IID_EDITBOX		= 20
	};
protected:
	bool SendReqAddFriend();				/// 서버로 친구 추가 요청
};
#endif