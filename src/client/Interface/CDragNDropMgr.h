#ifndef _CDragNDropMgr_
#define _CDragNDropMgr_

#include "../gamecommon/iobserver.h"

class CObservable;
class CDragItem;
class CTObject;

/**
* 드래그N드랍 관리자
*	: 드래그 시작, 드래그시 Draw, 드래그중인가?, 마우스버튼 업시 드랍처리를 한다.
*		- 드래그N드랍은 CIcon만 되며 CSlot, CIconDialog, CMenuDlg에서 시작된다.
*		- CDragItem은 드랍될수 있는 Target및 드랍됬을때의 TCommand를 가지고 있다가 해당 Target에 드랍되었을때 TCommand를 실행한다.
*		- Target은 Ground, All, 각 인터페이스 다이얼로그이다
*
*
* @Author		최종진
* @Date			2005/9/5
*
*/

class CDragNDropMgr : public IObserver
{
	CDragNDropMgr(void);
public:
	~CDragNDropMgr(void);

	static CDragNDropMgr&	GetInstance();

	virtual void Update( CObservable* pObservable, CTObject* pObj );	/// 드래그중인 아이콘의 Data가 변경시 처리( 스킬, 아이템등 )

	void Draw( POINT ptMouse );				/// 드래그 중인 CIcon을 그린다.
	void DragStart( CDragItem* pItem );		/// 드래그 시작
	void DragEnd( int iTargetType );		/// 드랍
	bool IsDraging();						/// 드래그 중인가?

private:
	CDragItem*	m_pItem;					/// 드래그 중인 CIcon 의 포인터 임시 저장
};
#endif