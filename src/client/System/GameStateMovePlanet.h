#ifndef _GAME_STATE_MOVE_PLANET_
#define _GAME_STATE_MOVE_PLANET_


#include "cgamestate.h"


enum enumPLANET
{
	PLANET_JUNON = 1,
	PLANET_LUNAR = 2,
};


/**
* 행성간 이동 연출을 위한 State Class
*
* @Author	최종진
* @Date		2005/9/15
*/
class CGameStateMovePlanet :	public CGameState
{
public:
	static int		m_iTargetPlanet;
	tPOINT16		m_PosRButtonClick;


public:
	CGameStateMovePlanet( int iStateID );

	CGameStateMovePlanet(void);
	~CGameStateMovePlanet(void);

	/*override*/int Update( bool bLostFocus );
	/*override*/int Enter( int iPrevStateID );
	/*override*/int Leave( int iNextStateID );

	/*override*/int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	/*override*/int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );

	/*override*/bool On_WM_MOUSEWHEEL (WPARAM wParam, LPARAM lParam);


	void Render_GameMENU();
};

#endif //_GAME_STATE_MOVE_PLANET_