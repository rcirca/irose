#ifndef _CGameStateLoginVirtual_
#define _CGameStateLoginVirtual_
#include "cgamestate.h"

/**
* CGameStateSelectAvatar에서 CGameStateSelectSvr로 이동하기 위한 중간 State Class
*	- 로그인서버에 다시 접속해야하기 때문에 아이디와 패스워드를 보관하였다가 자동으로 서버선택창까지 진행되는 구조이다
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameStateLoginVirtual :	public CGameState
{
public:
	CGameStateLoginVirtual( int iID );
	virtual ~CGameStateLoginVirtual(void);
	virtual int	Update(	bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );
	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void ServerDisconnected();
	virtual void WorldServerDisconnected();
	virtual void AcceptedConnectLoginSvr();
};
#endif