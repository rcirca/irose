#pragma once

#include "cgamestate.h"


class CGameStateWarning :	public CGameState
{
private:


public:
	CGameStateWarning(int iID);
	~CGameStateWarning(void);

	int Update( bool bLostFocus );
	int Enter( int iPrevStateID );
	int Leave( int iNextStateID );
		
	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );


};
