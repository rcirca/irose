#ifndef _CClan_
#define _CClan_
#include "../gamecommon/cobservable.h"
#include "event/CTEventClan.h"
#include "Interface/SlotContainer/ClanSkillSlot.h"

struct S_ClanMember{
	std::string		m_strName;
	int				m_iClass;
};

struct tagClanSKILL;

/**
* 클랜정보를 가지고 있는 데이타 Class
*	- 싱글톤
*	- Observer : CClanDlg
*
* @Author		최종진
* @Date			2005/9/15
*/

class CClan : public CObservable
{
	CClan(void);
	~CClan(void);
public:
	static CClan& GetInstance();

	void	Clear();

	void	SetClanNo( int iNo );
	void	SetName( const char* pszName );
	void	SetSlogan( const char* pszSlogan );
	void	SetMarkCenter( short nID );
	void	SetMarkBack( short nID );
	void	SetLevel( short nLevel );
	void	SetPoint( int iPoint );
	void	SetAlly( int iIndex, int iClanID, const char* pszName );
	void	SetStoreRate( int iRate );
	void	SetMoney( __int64 i64Money );
	void	SetNotice( const char* pszNotice );
	void	SetClass( int iClass );
	void	SetMemberLevel( const char* pszName, short nLev );
	void	SetMemberJob( const char* pszName, short nLev );
	void    SetSkill( tagClanSKILL* skilldata, int count );

	int			GetClanNo();
	const char* GetName();
	const char* GetSlogan();
	short		GetMarkCenter();
	short		GetMarkBack();
	short		GetLevel();
	int			GetPoint();
	int			GetAllyID( int iIndex );
	const char* GetAllyName( int iIndex );
	int			GetStoreRate();
	__int64		GetMoney();
	const char*	GetNotice();
	int			GetClass();
	int			GetMemberCount();
	int			GetMemberMaxCount();

	void		AddMember( const char* pszName, int iClass ,int iClanPoint, int iChannelNo ,short nLevel , short nJob );
	void		RemoveMember( const char* pszName );
	void		ChangeClass( const char* pszName, int iClass );

	void		ChangeConnectStatus( const char* pszName, int iChannelNo );
	void		RequestMemberList();
	//void		RequestClanInfo();
	void		ClearMemberlist();

	bool		IsValidClassUp( int iMemberClass );
	bool		IsValidJoinMember();

	int			GetMyClanPoint();
	CClanSkillSlot* GetClanSkillSlot();//					{ return &m_ClanSkillSlot; }



	void		SetClanMarkRegTime( SYSTEMTIME& time );
	SYSTEMTIME& GetClanMarkRegTime();

	enum{
		CLAN_MASTER			= 6,
		CLAN_SUB_MASTER		= 5,
		CLAN_COMMANDER		= 4,
		CLAN_LANDER			= 3,
		CLAN_SENIOR			= 2,
		CLAN_JUNIOR			= 1,
		CLAN_PENALTY		= 0

	};
private:
	int	GetMaxClassCount( int iClanLevel, int iClass );
	int GetCurrClassCount( int iClass );
private:
	int				m_iClanNo;
	std::string		m_strName;
	std::string		m_strSlogan;
	short			m_nMarkCenter;
	short			m_nMarkBack;
	short			m_nLevel;
	int				m_iPoint;
	int				m_iClass;		///클랜에서의 나의 직위

	int				m_iAlly1;
	std::string		m_strAlly1;

	int				m_iAlly2;
	std::string		m_strAlly2;

	int				m_iAlly3;
	std::string		m_strAlly3;


	std::string		m_strNotice;

	int				m_iStoreRate;
	__int64			m_i64Money;

	int				m_iMyClanPoint;

	CTEventClan		m_Event;
	std::list< S_ClanMember > m_Members;


	CClanSkillSlot	m_ClanSkillSlot;
	SYSTEMTIME		m_ClanMarkRegTime;
};
#endif