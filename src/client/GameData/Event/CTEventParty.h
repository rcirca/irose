#ifndef _CTEventParty_
#define _CTEventParty_
#include "ctevent.h"
//*---------------------------------------------------------//
/// 파티관련 변경을 알리는 Event( CParty, CPartyDlg에서 사용)
//*---------------------------------------------------------//
class CTEventParty : public CTEvent
{
public:
	CTEventParty(void);
	virtual ~CTEventParty(void);
	enum{
		EID_NONE,
		EID_JOIN_MEMBER,				/// 리스트에 추가
		EID_LEAVE_MEMBER,				/// 리스트에서 삭제
		EID_CHANGE_LEADER,				/// 방장 마크표시 변경, Entrust,Ban Button Enable/Disable
		EID_CHANGE_OBJECT_SERVERINDEX,	/// OBJECT SERVER INDEX 변경
		EID_BAN_MEMBER,					/// 내가 아닌 다름 멤버가 파티에서 추방 당했을경우
		EID_CLEAR,						/// 파티 종료
		EID_INIT,						/// 파티 시작
		EID_MEMBER_DISCONNECTED,			/// 파티원의 비정상종료
		EID_CHANGE_RULE,
	};
	void	SetObjSvrIdx( WORD iObjSvrIdx );
	WORD	GetObjSvrIdx();


	void	SetName( const char* pszName );
	const char* GetName();

	void	SetObjectTag( DWORD dwObjectTag );
	DWORD   GetObjectTag();

	void	SetRule( BYTE btRule );
	BYTE	GetRule();
protected:
	WORD				m_wObjSvrIdx;				///EID_CHANGE_OBJECT_SERVERINDEX시에는 기존		ObjSvrIndx
	std::string			m_strName;
	DWORD				m_dwObjectTag;
	BYTE				m_btRule;
};
#endif