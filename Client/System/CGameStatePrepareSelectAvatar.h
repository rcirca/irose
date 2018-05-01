#ifndef _CGAMESTATEPREPARESELECTAVATAR_
#define _CGAMESTATEPREPARESELECTAVATAR_
#include "cgamestate.h"



/**
* CGameStateSelectAvatar에 필요한 전처리를 하는 State Class
*	- Next State : CGameStateSelectAvatar
* @Author	최종진	
* @Date		2005/9/15
*/
class CGameStatePrepareSelectAvatar : public CGameState
{
	HANDLE		m_hThread;
public:
	CGameStatePrepareSelectAvatar(int iID);
	~CGameStatePrepareSelectAvatar(void);

	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ){ return 0; }
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ){ return 0; }



protected:
	static unsigned __stdcall ThreadFunc( void* pArguments );
	void		Draw();

};
#endif