#ifndef _CClanMemberItem_
#define _CClanMemberItem_
#include "winctrl.h"
#include "../../CInfo.h"

/**
* 클랜창에서 클랜맴버 List에 사용되는 item class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CClanMemberItem :	public CWinCtrl
{
public:
	CClanMemberItem( const char* pszName, int iClass , int iClanPoint, int iChannelNo ,short nLevel, short nJob);
	virtual ~CClanMemberItem(void);

	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void SetSelected();
	virtual void Update( POINT ptMouse );

	void	SetLevel( short nLevel );
	void	SetJob( short nJob );
	short	GetLevel();
	short	GetJob();
	void	SetClass( int iClass );
	int		GetClass();
	int		GetClanPoint();
	int		GetChannelNo();
	void	SetConnectStatus( int iChannelNo );
	const char* GetName();

	enum{///CONNECT STATUS
		CS_OFFLINE,
		CS_ONLINE,
	};

protected:

	int				m_iOnlineMarkImageID;
	int				m_iOfflineMarkImageID;
	int				m_iClass;
	int				m_iClanPoint;
	int				m_iChannelNo;
	short			m_nLevel;
	short			m_nJob;

	std::string		m_strJob;
	std::string		m_strClass;
	std::string		m_strName;
	std::string		m_strChannel;
	
	CInfo			m_Info;
};
#endif