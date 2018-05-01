#ifndef _CGAMESTATENULL_
#define _CGAMESTATENULL_
#include "cgamestate.h"

class CGameStateNull : public CGameState
{
public:
	CGameStateNull(int iID);
	~CGameStateNull(void);

	virtual int Update( bool bLostFocus ){return 0;}
	virtual int Enter( int iPrevStateID ){return 0;}
	virtual int Leave( int iNextStateID ){return 0;}

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ) { return 0;}
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ) { return 0;}

protected:
	int ProcWndMsg(){ return 0; }
};
#endif   