#ifndef _CIconItem_
#define _CIconItem_
#include "cicon.h"

struct tagITEM;
class CItem;


/**
* Item Icon Class
*	- CTRL + MOUSE LBUTTONDOWN 시  위시리스트에 추가하는 기능 ~ CIconItem::Process
*
*
* @Warning		소켓이미지 Texture의 load및 unload는 레퍼런스 카운팅으로 처리한다.
* @Author		최종진
* @Date			2005/9/6
*/
class CIconItem : public CIcon
{
public:
	CIconItem(void);
	CIconItem( tagITEM* pItemData );
	CIconItem( CItem* pItem );
	virtual ~CIconItem(void);

	virtual void		Draw();
	virtual void		Update( POINT ptMouse );
	virtual void		ExecuteCommand();
	virtual void		GetToolTip( CInfo& strToolTip,  DWORD dwDialogType, DWORD dwType );
	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual CIcon*		Clone();
	virtual void		Clear();
	
	virtual int			GetQuantity();
	virtual tagITEM&	GetItem();
	virtual bool		IsEnable();
	virtual const char*	GetName();
	virtual int			GetIndex();


	CItem*				GetCItem();
	int					GetItemNo();

	bool				IsEmpty();



protected:
	CItem*				m_pItem;							/// 아이콘이 참조할 실제아이템에 대한 포인터 < CItem이 있는 경우 : 예) 인벤토리의 아이템 >
	bool				m_bDeleteItemData;					/// 실제 아이템이 없을경우 사용할 아이템데이타
	static	HNODE		m_hSocketTexture;					/// 소켓을 가진 아이템의 경우 위에 표시하기 위한 Texture의 노드
	static  int			m_iSocketTextureRefCount;			/// 레퍼런스 카운팅을 통해서 0일경우 삭제한다.
};

#endif