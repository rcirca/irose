#ifndef _CGameStateSelectSvr_
#define _CGameStateSelectSvr_
#include "cgamestate.h"

/**
* 서버 선택 State Class
*
* @Author	최종진
* @Date		2005/9/15
*/
class CGameStateSelectSvr :	public CGameState
{
public:
	CGameStateSelectSvr( int iID );
	virtual ~CGameStateSelectSvr(void);

	int Update( bool bLostFocus );
	int Enter( int iPrevStateID );
	int Leave( int iNextStateID );
	virtual void ServerDisconnected();

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
private:
	int		m_iPrevState;
};
#endif