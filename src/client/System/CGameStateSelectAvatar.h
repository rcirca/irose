#ifndef _CGAMESTATESELECTAVATAR_
#define _CGAMESTATESELECTAVATAR_
#include "cgamestate.h"

/**
* 캐릭터 선택 State Class
*	- Next State : CGameStateCreateAvatar, CGameStateMoveMain, CGameStateLoginVirtual
*
* @Author	최종진	
* @Date		2005/9/15
*/
class CGameStateSelectAvatar : public CGameState
{
public:
	CGameStateSelectAvatar(int iID);
	~CGameStateSelectAvatar(void);
	
	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );
	
	virtual void ServerDisconnected();

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );

};
#endif