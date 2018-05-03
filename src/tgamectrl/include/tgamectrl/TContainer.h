#ifndef _TCONTAINER_
#define _TCONTAINER_

#include "TObject.h"
#include <deque>

/**
* TObject를 Item으로 가지는 Container Class
*	- Subscript는 1-Base로 동작한다.
*	- 내부적으로 아이템을 선택할 수 있다.
*
* @Author	최종진
*
* @Date		2005/8/30 
*/
class TGAMECTRL_API CTContainer : public CTObject
{
	std::deque< CTObject* >	m_listItem;
	unsigned				m_uSelectedItemIndex;

public:
	CTContainer(void);
	~CTContainer(void);

	void	Clear();
	void	AddItem( CTObject* item );

	const CTObject*	GetItem( unsigned iSubscript );
	const CTObject*	GetSelectedItem();
	
	 
	unsigned		GetSelectedItemIndex();
	void			SelectItem( unsigned iSubscript );
	
	unsigned		GetSize();
	std::deque< CTObject* >&	GetItemList();
};
#endif