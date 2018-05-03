#ifndef _CFriendListItem_
#define _CFriendListItem_
#include "tgamectrl/winctrl.h"
/**
* 커뮤니티 창에서 친구 리스트에 사용되는 Item Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CFriendListItem :	public CWinCtrl
{
public:
	CFriendListItem( DWORD dwUserTag, BYTE btStatus, const char* pszName );
	virtual ~CFriendListItem(void);
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void SetSelected();
	
	void	SetStatus( BYTE btStatus );
	
	const	DWORD	GetUserTag();
	const	char*	GetName();
	const	BYTE	GetStatus();

	
protected: 
	DWORD		m_dwUserTag;
	BYTE		m_btStatus;
	std::string m_strName;
	int			m_online_image_id;
	int			m_offline_image_id;
};
#endif