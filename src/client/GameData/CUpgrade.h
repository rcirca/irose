#ifndef _CUpgrade_
#define _CUpgrade_
#include <vector>
#include "../gamecommon/CObservable.h"
#include "../gamecommon/IObserver.h"
#include "Net_Prototype.h"
#include "event/CTEventUpgrade.h"
class CItem;
class CItemFragment;
class CRequireMaterial;
/**
* 제련용 Data Class
*
* @Author		최종진
* @Date			2005/9/15
*/

class CUpgrade : public CObservable, public IObserver
{
	CUpgrade(void);
	~CUpgrade(void);
public:
	static CUpgrade& GetInstance();
	virtual void Update( CObservable* pObservable, CTObject* pObj );


	bool    SetItem( CItem* pItem );

	
	void	RemoveTargetItem();
	void	RemoveMaterialItem( CItem* pItem );

	void	SetNpcSvrIdx( int iNpcSvrIdx );
	void	SetSkillSlotIdx( int iSkillSlotIdx );
	void	SetUseItemInvenIdx( int iUseItemInvenIdx );


	bool	Send_CRAFT_UPGRADE_REQ();

	int		GetRequireMp();
	int		GetSuccessProb();
	const char*	GetRequireMaterialName( int iIndex );
	int		GetRequireMaterialCount( int iIndex );
	const char* GetTargetItemName();

	CItemFragment* GetTargetItem();

	bool   SendPacketUpgrade();

	enum{
		ALL_OK,
		NOTENOUGH_MP,		/// MP 부족.
		NOTENOUGH_MATERIAL, /// 재료아이템의 개수 부족.
		INVALID_MATERIAL,   /// 필요한 재료아이템이 없다.
		INVALID_TARGETITEM, /// 제련할 아이템이 없거나 잘못된 아이템이다.
		NOTENOUGH_MONEY,
		INVALID_TYPE,
	};

	void ClearResultItemSet();
	void AddResultItemSet( tag_SET_INVITEM& Item );
	void SetResult( BYTE btResult );

	void SetResultSuccessProb( int iSuccessProb );
	int	 GetResultSuccessProb();

	BYTE GetResult();
	void ApplyResultItemSet();

	int		GetRequireMoney();
	int		GetType();
	WORD	GetNpcSvrIdx();
	enum{
		TYPE_NONE,
		TYPE_NPC,
		TYPE_SKILL
	};

private:
	int		IsValidUpgradeCondition();
	void	SetTargetItem( CItem* pItem );
	void	SetMaterialItem( CItem* pItem );

	int		ComputeRequireMp();
	int		ComputeSuccessProb();


private:
	CItemFragment*					m_pTargetItem;			/// 제련하고자 하는 타겟 아이템
	CItemFragment*					m_pMaterialItems[3];       /// 유저가 인벤토리에서 올려놓은 재료아이템리스트
	CRequireMaterial*				m_pRequireMaterialItems[3];/// 제련에 요구되는 아이템리스트
	
	std::list<tag_SET_INVITEM>		m_ResultItemSet;
	BYTE							m_btResult;
	int								m_iResultSuccessProb;

	CTEventUpgrade					m_Event;

	int								m_iNpcSvrIdx;
	int								m_iSkillSlotIdx;
	int								m_iUseItemInvenIdx;

	int								m_iRequireMoney;
	int								m_iRequireMp;
	int								m_iSuccessProb;

};
#endif