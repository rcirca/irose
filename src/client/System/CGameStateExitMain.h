#ifndef _CGAMESTATEEXITMAIN_
#define _CGAMESTATEEXITMAIN_
#include "cgamestate.h"

/**
* CGameStateMain에서 CGameStateSelectAvatar로 바뀌는 중간 State Class
*	- Next State : CGameStateSelectAvatar
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameStateExitMain :	public CGameState
{
	HANDLE		m_hThread;
public:
	CGameStateExitMain(int iID);
	~CGameStateExitMain(void);

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