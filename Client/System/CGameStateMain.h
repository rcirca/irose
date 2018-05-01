#ifndef _CGAMESTATEMAIN_
#define _CGAMESTATEMAIN_
#include "cgamestate.h"

/**
* 캐릭터 선택후 실제 게임중인 상태
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameStateMain : public CGameState
{
public:
	CGameStateMain(int iID);
	~CGameStateMain(void);

	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );
	virtual void ServerDisconnected();
	virtual int ProcWndMsgInstant( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	bool RightClickedOnChar;

protected:

	bool On_WM_CHAR (WPARAM wParam, LPARAM lParam);
	bool On_WM_KEYDOWN (WPARAM wParam, LPARAM lParam);
	bool On_WM_LBUTTONDOWN (WPARAM wParam, LPARAM lParam);
	bool On_WM_RBUTTONDOWN (WPARAM wParam, LPARAM lParam);
	bool On_WM_LBUTTONDBCLICK (WPARAM wParam, LPARAM lParam);	
	bool On_WM_MOUSEWHEEL (WPARAM wParam, LPARAM lParam);

	void UpdateCheckFrame();
	void Render_GameMENU();
	/// 현재 마우스가 어떤 위치에 있는가?
	int	CheckMouseRegion( int x, int y );

	/// 매프레임 뭔가 해야되는 것들
	void UpdateCameraPositionByMouse();///마우스의 상태및 위치로 카메라 이동처리
	void Pick_POSITION(/*LPARAM	lParam*/);

private:
	DWORD		m_dwEnterTime;

	tPOINT16	m_PosRButtonClick;
	D3DXVECTOR3	m_PosPICK;
	tPOINT16	m_ScreenPOS;
	bool		m_bPickedPOS;
	int			m_iPickedOBJ;
};
#endif