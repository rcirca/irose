#ifndef _CChatRoom_
#define _CChatRoom_

/**
* 커뮤니티에서의 채팅룸의 멤버 클래스
*
* @Author		최종진
* @Date			2005/9/12
*/
class CChatMember
{
public:
	CChatMember(){}
	CChatMember(WORD wServerIdx, const char* pszName );
	~CChatMember(void){}

	void SetServerIdx( WORD wServerIdx );
	void SetName( const char* pszName );

	WORD GetServerIdx();
	const char* GetName();
protected:
	WORD		m_wServerIdx;
	std::string	m_strName;
};

#include "../GameCommon/CObservable.h"
#include "Event/CTEventChatRoom.h"
/**
* 커뮤니티에서의 채팅룸에 대한 Data Class
*	- Observer : CChatRoomDlg
*
* @Author		최종진
* @Date			2005/9/12
*/
class CChatRoom : public CObservable
{
	CChatRoom(void);
	~CChatRoom(void);
public:
	static CChatRoom& GetInstance();

	enum{
		STATE_DEACTIVATED,		/// 아무 상태 아님. ui가 Open할수 없으며 Open되어 있다면 닫는다.
		//STATE_MAKE,				/// 대화방을 개설하기 위해서 창을 뛰운 상태 - 방설정만 가능하고 확인시 서버에 보내고 대기한다.(상태변경)
		STATE_WAIT_MAKE,		/// 대화방을 만들기를 서버에게 요청하고 대기상태,모든 인터페이스가 잠겨야 한다.
		STATE_WAIT_JOIN,
		STATE_ACTIVATED			/// 대화방이 개설/조인되고 대화중인 상태,, 방장일경우에만 설정가능하다.
	};

	int	GetState();
	void	SetState( int iState );

	void	JoinMember( WORD wServerIdx, const char* pszName );
	void	LeaveMember( WORD wServerIdx );
	void    LeaveMember( const char* pszName );

	void	Join();	///기존에 채팅방에 조인한다.
	void	Made( WORD wMyID ); ///내가 만들기 요청한 방이 만들어졌다.
	void	Leave();///채팅방에서 나간다.
	void	SetTitle( const char* pszTitle );
	void	SendReqMakeRoom( BYTE btRoomType, BYTE btMaxUser, char* pszTitle, char* pszPwd );
	void	SendReqJoinRoom( BYTE btRoomType, WORD wRoomID, char* pszPwd );
	void	SetMaster( WORD wServerIdx );

	const char* GetTitle();
private:

	std::list< CChatMember >	m_Members;
	CTEventChatRoom	m_Event;

	int			m_iState;
	int			m_iMasterServerIdx;
	std::string	m_strTitle;
};
#endif