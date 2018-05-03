#ifndef _CPARTYDLG_
#define	_CPARTYDLG_

#include "tgamectrl/tdialog.h"
#include "Net_Prototype.h"

class CPartyMember;

#include "../../GameCommon/IObserver.h"
#include <list>

typedef	list< CPartyMember* >			LIST_PARTY_MEMBER;
typedef LIST_PARTY_MEMBER::iterator		LIST_PARTY_MEMBER_ITER;


/**
* 파티시 파티정보및 파티원을 보여주는 다이얼로그
*	- 파티장은 항상 리스트의 맨앞Node로 
*	- Observable : CParty
*
* @Todo		일정시간마다 경험치를 깍는 부분 추가 필요
* @Author	최종진
* @Date		2005/9/12
*/
class CPartyDlg : public CTDialog, public IObserver
{

public:
	CPartyDlg(void);
	virtual ~CPartyDlg(void);
	virtual void Draw();
	virtual void Show();
	virtual void Hide();
	virtual void Update(POINT ptMouse);
	virtual void MoveWindow( POINT pt );
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);

	virtual void Update( CObservable* pObservable, CTObject* pObj );
	
	void			ClearMemberList();

protected:

	void			OnLButtonUp( unsigned iProcID );

	WORD			GetSelectedMemberSvrIdx();
	DWORD			GetSelectedMemberUserTag();


	void			ResetMemberPositions();										/// 파티원이 한명 빠질때마다 표시 위치들을 재 계산해준다.

	CPartyMember*	FindMember( WORD wObjSvrIdx );
	CPartyMember*	GetSelectedMember();

protected:

	enum{
		IID_BTN_CLOSE		= 10,	/// 파티창 닫기 == IID_BTN_LEAVE와 같은 처리
		IID_BTN_ENTRUST		= 11,	/// 파티장으로 위임
		IID_BTN_BAN			= 12,	/// 파티에서 추방
		IID_BTN_LEAVE		= 13,	/// 파티 떠나기
		IID_BTN_OPTION		= 14,	/// 파티 옵션창 열기
	};


	LIST_PARTY_MEMBER		m_listPartyMember;							/// 파티원들 정보 컨테이너


	int						m_iGuagePartyExpIdx;						/// 파티 경험지 게이지 그래픽 ID
	int						m_iGuagePartyExpBGIdx;						/// 파티 경험치 게이지 배경 그래픽 ID
};
#endif