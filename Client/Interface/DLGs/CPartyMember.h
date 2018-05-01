#ifndef _CPARTYMEMBER_
#define _CPARTYMEMBER_

#include "Net_Prototype.h"
#include "../Icon/CIcon.h"
#include <list>

/**
* 파티창( CPartyDlg)에서 각 파티원들의 상태를 저장하고 그리는 Class
*
* @Author		최종진
* @Date			2005/9/12
*/
class CPartyMember 
{
public:
	CPartyMember(void);
	CPartyMember( DWORD dwObjectTag, WORD wObjSvrIdx, const char* pszName );
	virtual ~CPartyMember(void);


	void		Draw();
	unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	void		Update( POINT ptMouse );


	void		SetSelected( bool b ){ m_bSelected = b; }
	bool		IsSelected(){ return m_bSelected; }
	void		SetPosition( POINT pt );
	int			GetHeight();

	WORD		GetObjSvrIdx();
	void		SetObjSvrIdx( WORD wObjSvrIdx );

	DWORD		GetObjectTag();

	const char* GetName(){ return m_stName.c_str(); }


protected:
	bool	IsInside(POINT pt);
	bool	AddStatusIcon( DWORD dwFlag );					/// 상태 아이콘 추가
	void	ResetStatusIconsPosition();						/// 상태 아이콘들의 위치를 재정렬한다.

protected:
	/// Member의 정보
	WORD				m_wObjSvrIdx;						/// 파티원의 서버 인덱스
	DWORD				m_dwObjectTag;						/// 파티원의 서버 태그( 서버에서 유일한 ID , 서버인덱스는 유일하지 않을수 있다 )

	std::string			m_stName;							/// 이름
	bool				m_bLeader;							/// 파티장인가?

	RECT				m_rcThis;							/// 인터페이스 영역
	int					m_iWidth;
	int					m_iHeight;
	bool				m_bSelected;						/// 선택되어 졌는가?

	std::list<CIconStatus>	m_listStatusIcon;				/// 상태 아이콘 리스트

	int					m_iGuageIdx;						/// 체력 게이지 그래픽 ID
	int					m_iGuageBGIdx;						/// 체력 게이지 배경 그래픽 ID

};
#endif