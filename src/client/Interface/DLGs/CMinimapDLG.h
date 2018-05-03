#ifndef _CMINIMAPDLG_
#define _CMINIMAPDLG_


#include "tgamectrl/tdialog.h"
#include "tgamectrl/singlelinestring.h"
class CTAnimation;

/**
* 미니맵 표시를 위한 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMinimapDLG : public CTDialog
{


private:

	/// 미니맵의 존의 몇번째 맵부터 존재하는가? 그래야만 현재 출력위치를 결정할수 있다..
	DWORD				m_dwMinimapStartX;			/// 미니맵의 LEFT-TOP과 일치시킬 맵의 번호
	DWORD				m_dwMinimapStartY;

	float				m_fMinMinimapWorldPosX;		/// 미니맵의 시작좌표의  월드 좌표( 가로 )
	float				m_fMaxMinimapWorldPosY;		/// 미니맵의 시작좌표의  월드 좌표( 세로 )
	
	float				m_fMaxMinimapWorldPosX;		/// 미니맵 Texture의 Size로 이동가능한 최대 월드 X좌표를 구한 값( 실제 이동가능 월드 좌표와 틀릴수 있다:미니맵이 잘못 들어간 경우)
	float				m_fMinMinimapWorldPosY;		/// 미니맵 Texture의 Size로 이동가능한 최소 월드 Y좌표를 구한 값

	float				m_fCurrentScale;			/// Zoom시에 확대할 비율

	float				m_fMinimapCenterWorldPosX;	/// 현재 보여지는 미니맵의 중간의 실제 월드 좌표
	float				m_fMinimapCenterWorldPosY;			

	bool				m_bExploreMode;				/// 마우스 드래그에 따라 미니맵의 중심이 변경되는 Mode(Draw에서 false조건 체크)
	bool				m_bShowMiniMap;				/// Expand( 실제 미니맵을 보여주는가? Dialog는 항상 보여지는 상태이다 )

	HNODE				m_hMiniMap;
	HNODE				m_hArrow;

	float				m_fPrevUserPosX;			/// Explore Mode를 종료하기 위하여 임시적으로 내 아바타의 위치를 저장해두는 변수
	float				m_fPrevUserPosY;
	POINT				m_ptLButtonDown;			/// LButtonDown시 마우스 위치
	bool				m_bDragStart;				/// 미니맵위에서 드래그가 시작되었는가?




	int					m_iAvatarImageID;
	int					m_iPartyImageID;


	struct  PartyMemberMarkInfo{
		RECT rcDraw;
		int	 iIconNo;
	};
	struct	NpcMarkInfo{
		RECT		rcDraw;
		std::string	strName;
		int			iIconNo;
	};

	struct S_IndicatorCoordinates{
		int		m_zoneno;
		float	m_x;
		float	m_y;
	};

	struct S_IndicatorNpc{
		int	m_iNpcNo;
		int m_auto_remove;
	};



	/**미니맵에서 npc&PartyMember마크를 그리기위하여 임시로 저장해두는 리스트
	*	ObjMGR에서 가져온 리스트에서 일단 아바타들은 그리고 NPC들은 임시 저장해두었다가
	*	젤위로 그리기 위하여 잠시 보관해둔다.
	*	std::map<int, NpcMarkInfo>			
	*		-m_listNpcMarkInfo; - 중복된 NPC 번호를 가진 NPC의 경우 안그려지는 상황 발생
	*		-모든 NPC는 맵에서 로딩해서 그리는것으로 한다.
	*/
	std::list<PartyMemberMarkInfo>		m_listPartyMemberMarkInfo;


	bool	m_bExpand;															///현재 크게 보이는가?
	bool	m_bMinimize;														///현재 캡션만 보이는가?

	CSinglelineString	m_zonename;												/// 존네임 스트링

	/// 스크립트로 미니맵위에 특별한 표시를 추가시 저장 컨테이너
	std::multimap< int ,S_IndicatorNpc >			m_indicators_npc;			/// NPC( 중복 지정할수 있다 )
	std::map< int ,S_IndicatorCoordinates >			m_indicators_coordinates;	/// 좌표

	CTAnimation*	m_indicator;												///표시에 사용될 CTAnimation
public:
	CMinimapDLG();
	virtual ~CMinimapDLG();

	virtual unsigned int	Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void			Draw();
	virtual void			Update( POINT ptMouse );
	virtual void			Show();



	bool	SetMinimap( const char* pFname, const char* pCursorName, DWORD dwMapStartX, DWORD dwMapStartY );
	void	FreeMinimap();
	
	void	AddIndicatorNpc( int npcno , int auto_remove );
	void	AddIndicatorCoordinates( int index, int zoneno, float x, float y );
	void	RemoveIndicatorNpc( int npcno );
	void	RemoveIndicatorCoordinates( int index );
	int		GetIndicatorNpc( int npcno );
	void	RemoveAutodeleteIndicatorNpc( int npcno );
	void	CalculateDisplayPos();												///게임에서 사용되는 맵의 시작위치와 미니맵의 시작위치를 맞춘다.

	void	ToggleShowMinimap();
	void	ToggleZoomMinimap();

	bool	IsShowMinimap(){ return m_bShowMiniMap; }
	bool	IsZoomMinimap(){ return((m_fCurrentScale == 1)? false: true); }

	void	SetAvataInfoPos(int y);													//  아바타인포창 확대,축소시 위치 변경

	void	RefreshDlg();
	void	SetInterfacePos_After();

	enum{
		IID_PANE_BIG				= 50,
		IID_CAPTION_BIG				= 51,
		IID_BTN_NORMAL				= 52,
		IID_BTN_MINIMIZE_BIG		= 53,
		IID_PANE_BIG_CHILDPANE		= 60,

		IID_PANE_SMALL				= 100,
		IID_CAPTION_SMALL			= 101,
		IID_BTN_EXPAND				= 102,
		IID_BTN_MINIMIZE_SMALL		= 103,
		IID_PANE_SMALL_CHILDPANE	= 110
	};

protected:
	bool	On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	bool	On_LButtonDN( unsigned iProcID, WPARAM wParam, LPARAM lParam );

	
	float	fGetWorldDistancePerPixel();										/// 미니맵과 월드 거리와의 비율

	void	DrawAVTPosition();													/// 다른 아바타,NPC등을 표시
	bool	IsInsideMinimap( float x, float y , int width, int height );		/// 미니맵Dialog내부의 실제 미니맵위인가?

	void	DrawPartyMember();													/// 파티 멤버 출력
	void	DrawNpcFromMap();													/// 맵에 찍혀힌 NPC들 출력( 서버에서 받은 CObjNPC이 아닌 맵에서 직접 로드한 )

	void	DrawCoordinatesIndicator();											/// 특정한 표시가 추가된 좌표 Draw

	void	Expand();															/// 전체 보이기
	void	Reduct();															/// 캡션만 보이기

	void	ToggleShowMinimapSmall();											/// 최소화
	void	ToggleShowMinimapBig();												/// 최대화

	

	CWinCtrl* FindPaneChild( unsigned uiPaneID, unsigned uiChildID );


};



#endif //_CMINIMAPDLG_