#ifndef _CSeparate_
#define _CSeparate_

#include "../GameCommon/CObservable.h"
#include "../GameCommon/IObserver.h"
#include "../Common/CItem.h"
#include "../interface/dlgs/subclass/CSlot.h"
#include "event/CTEventSeparate.h"
#include <vector>

class CItemFragment;
class CItem;



/**
* 분리/분해시 사용되는 Data Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CSeparate : public CObservable, public IObserver
{
	CSeparate(void);
	~CSeparate(void);

public:
	static CSeparate& GetInstance();

	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void	SetItem( CItem* pItem );
	void	RemoveItem();

	int		GetRequireMp();
	int		GetRequireMoney();

	enum{
		TYPE_NONE			= 0,
		TYPE_SEPARATE		= 1,///분리 - 제밍
		TYPE_DECOMPOSITION	= 2,///분해 - 재조된 혹은 재료번호가가 있는 아이템
	};
	
	enum{
		TYPE_SKILL,
		TYPE_NPC
	};
	void Send_CRAFT_BREAKUP_REQ();
	void SetSkillSlotIdx( int iSkillSlotIdx );
	void SetNpcSvrIdx( WORD wNpcSvrIdx );
	WORD GetNpcSvrIdx();
	int	 GetType();

private:
	int	GetQuantity( tagITEM& Item, int iRequireQuantity );

private:
	CTEventSeparate			m_Event;
	CItemFragment*			m_pMaterialItem;///분리 하고자 하는 아이템
	std::vector< CItem* >	m_pItems;		///분리후 생성될 아이템들
	int						m_iType;
	int						m_iRequireMp;
	int						m_iRequireMoney;
	int						m_iSkillSlotIdx;
	WORD					m_wNpcSvrIdx;
	bool					m_bWaitServerReply;
};
#endif