#ifndef _CCursor_
#define _CCursor_
#include "../CInfo.h"
class CCursorState;
class CInfo;



/**
* 게임내 커서 Class
* - 그리는 방식에 따라 3가지 상태로 구분되어 진다.
*
* @Author		최종진
*
* @Date			2005/9/6
*/
class CCursor
{
	CCursor(void);
	~CCursor(void);

public:
	static CCursor& GetInstance();
	void Initialize();
	void Finalize();

	void	Draw( POINT ptMouse );
	void	ChangeNextState();			/// 다음 상태로 바꾼다.
	int		SetCursorType( int iType, CInfo* pMouseInfo = NULL );
	int		GetCurrCursorType();		/// 현재 커서 State를 리턴한다.
	int		RefreshCursor();	
	void	ReloadCursor();				/// 

	CInfo&	GetMouseInfo();

	enum{
		STATE_DX,						/// DirectX Cursor	-> 윈도우 커서를 변경하여 사용한다( 디폴트 )
		STATE_SPRITE,					/// Sprite Cursor(매프레임그린다)
		STATE_WINDOW,					/// Window Default Cursor
		STATE_MAX
	};

	enum eCURSOR_TYPE
	{
		CURSOR_DEFAULT = 0,
		CURSOR_ATTACK,
		CURSOR_ITEM_INVEN,
		CURSOR_ITEM_PICK,
		CURSOR_LEFT,
		CURSOR_RIGHT,
		CURSOR_NPC,
		CURSOR_USER,
		CURSOR_WHEEL,
		CURSOR_NOUI,
		CURSOR_REPAIR,
		CURSOR_APPRAISAL,
		CURSOR_MAX,
	};

private:
	CCursorState*	m_pCurrState;
	CCursorState*	m_pCursorState[ STATE_MAX ];
	CInfo			m_MouseInfo;
	int				m_iType;
};
#endif