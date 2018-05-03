#ifndef _CGAMESTATERELOGIN_
#define _CGAMESTATERELOGIN_
#include "cgamestate.h"

/**
* CGameStateMain에서 CGameStateLogin로 바뀌는 중간 State Class
*	- Next State : CGameStateLogin
*
* @Author		김주현
* @Date			2005/10/20
*/
class CGameStateReLogin :	public CGameState
{
	HANDLE		m_hThread;
public:
	CGameStateReLogin(int iID);
	~CGameStateReLogin(void);

	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );

	virtual void ServerDisconnected(){}////이 상태에서는 서버가 끊겨도 아무것도 하지 않는다.

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ){return 0;}
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ){ return 0;}

	
protected:
	static unsigned __stdcall ThreadFunc( void* pArguments );
	void Draw();

	///
	/// system 배경에 사용될 존번호
	///
	static int	m_iBackGroundZone;
};


#endif