#ifndef _CParty_
#define _CParty_
#include "../gamecommon/CObservable.h"
#include "net_prototype.h"
#include "Event/CTEventParty.h"
struct PartyMember{
	tag_PARTY_MEMBER	m_Info;
	std::string			m_strName;
	DWORD				m_dwDisconnectedTime;///비정상 종료시 접속이 끊여진 사용자
};

/**
* 파티시 파티 Data를 가지고 있는 Class
*	- Observer : CPartyDlg
*
* @Author		최종진
* @Date			2005/9/15
*/
class CParty : public CObservable
{
	CParty(void);
	~CParty(void);
public:
	static CParty& GetInstance();

	bool	JoinMember( tag_PARTY_MEMBER& memberinfo, const char* pszName );
	bool	LeaveMember( DWORD dwObjectTAG );
	bool	BanMember( DWORD dwObjectTag );
	void    MemberDisconnected( DWORD dwUserTag );
	bool	ChangeLeaderByObjSvrIdx( WORD wObjSvrIdx );
	bool	ChangeLeaderByTag( DWORD dwObjectTag );
	void	SetLeaderObjSvrIdx( WORD wObjSvrIdx );
	void	SetLeaderObjectTag( DWORD dwObjectTag );

	bool	ChangeMemberInfoByUserTag( tag_PARTY_MEMBER& MemberInfo );

	bool	HasParty();						
	bool	IsPartyFull();					
	bool	IsPartyLeader();				
	bool	IsPartyMember(WORD wObjSvrIdx);	


	int		GetLevel();
	int		GetCurrExp();
	int		GetMaxExp();
	int		GetMemberCount();
	
	bool	GetMemberInfoByObjSvrIdx(WORD wObjSvrIdx, PartyMember& member );
	bool	GetMemberInfoByTag(DWORD dwObjectTag, PartyMember& member );

	WORD	GetLeaderSvrIndex();
	DWORD	GetLeaderObjectTAG();


	void	SetLevel( int iLevel );
	void	SetExp( int iExp );
	void	LevelUp();

	void	Make();///내가 파티를 만들었을경우
	void	Join();///파티에 참가했을경우
	void	Leave();///파티를 떠날때
	void	Destory();
	void	Update();

	bool	IsValidJoinParty();
	void	SendChangePartyRule( BYTE btPartyRule );
	void	RecvPartyRule( BYTE btPartyRule );
private:
	int		GetNeedExp4Levelup ( int iLevel );	
	int		GetAutoDecreaseExp( int iPartyLv, int iCountPartyMember );

private:
	BYTE	m_btPartyRule;
	bool	m_bHasParty;
	int		m_iLevel;
	int		m_iCurrExp;
	WORD	m_wLeaderObjSvrIdx;
	DWORD	m_dwLeaderObjectTAG;


	DWORD					m_dwExpLastUpdateTime;


	std::list< PartyMember >	m_Members;
	CTEventParty	m_Event;
};
#endif