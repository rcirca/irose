#ifndef _CManufacture_
#define _CManufacture_
#include "CRequireMaterial.h"
#include "../gamecommon/cobservable.h"
#include "../gamecommon/iobserver.h"
#include "../common/citem.h"

#include <utility>
#include <string>
#include <map>
#include <iomanip>



const int g_iMaxCountMaterial = 4;

class CItem;
class CItemFragment;
class CTEventManufacture;

/**
* 제조 관련 Data Class
*	- 싱글톤
*	- Observer		: CMakeDLg
*	- Observable	: CItemSlot
*
* @Warning		일본 상용화 이벤트 건으로 2단계부터 제조가 가능한 보석에 대한 코드 추가
* @Author		최종진
* @Date			2005/9/15
*/
class CManufacture : public CObservable, public IObserver
{
	CManufacture(void);
public:
	static CManufacture& GetInstance();
	~CManufacture(void);

	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void	Clear();
	void	SetSkillSlotIndex( int iIndex );
	int		GetSkillSlotIndex();
	int		GetSkillIndex();



	int		GetCosumeMP();
	int		GetSuccessRate();
	int		GetSuccessPoint( int iIndex );
	void	SetSuccessPoint( int iIndex , int iValue );

	CRequireMaterial&	GetRequireMaterial( int iIndex );


	bool	IsRequireMaterialItem( int iInvenIdx );			///현재 제조를 위해서 필요한 아이템인가? 제조창에 올려져 있지않은 것도 체크
	void    SubItemsAfterRecvResult( int iSlotNum );		///결과를 받은후 재료슬롯에서 재료들을 빼는 Method
	int		GetMaterialIndex( CItem* pItem );

	void	SetMakeClass( int iClass );
	void	SetMakeItem( tagITEM& Item );
	void	SetMaterialItem( CItem* pItem );
	void	RemoveMaterialItem( CItem* pItem );
	int     GetMaterialCount();
	CItem*  GetMakeItem();

	const std::list<tagITEM>& GetMakableItems();			
	const std::list<int>& GetMakableClasses();				

	int		IsValidSendMakeItemReq();

	int		GetMakeType();

	enum MAKE_TYPE{
		SWORD_TYPE  = 11,		/// 검
		BLUNT_TYPE,				/// 둔기
		BOW_TYPE,				/// 활
		PISTOL_TYPE,			/// 총
		MAGICWEAPON_TYPE,		/// 매직무기
		SUBEQUIP_TYPE,			/// 보조장비
		BATTLEARMOR_TYPE,		/// 전투복장
		MAGICARMOR_TYPE,		/// 매직복장
		NORMALARMOR_TYPE,		/// 일반복장
		CART_TYPE		= 21,	/// 카드
		CASTLEGEAR_TYPE,		/// 캐슬기어
		ALCHEMY_TYPE,			/// 연금술
		ACCESSORY_TYPE	= 31,   /// 장신구
		ART_TYPE,				/// 예술품
		GEM_TYPE		= 35,	/// 보석
		INDUSTRY_TYPE	= 41,	/// 공산품

	};


private:

	void	AddMaterialItem( CItem* pItem , int index , int iQuantity);
	int		GetEstimateProbability(int fItemDif, int fItemQuality, int fMatQuality, int iNumMat, short nWorldProd);
	short	GetRandShort( short iMin, short iMax );
	int		GetSuccessPoint( int iSlot, int iItemDif, int iItemQuality , int iNumMat );



	/// 스킬인덱스와 레벨로 제조가 가능한 아이템 종류를 STB에서 로드한다.
	bool	LoadMakableClasses( int iSkillNo, int iSkillLv );

	/// 유저가 선택한 아이템 종류와 스킬인덱스, 레벨로 제조 가능한 아이템들을 STB에서 로드한다.
	bool	LoadMakableItem( int iClass, int iSkillNo, int iSkillLv );

	/// STB에서 아이템 종류를 로드하는 Method
	int		LoadClassesFromSTB( t_eITEM itemType, int iMakeNo, int iSkillLv );
	/// STB에서 아이템을 로드하는 Method
	int		LoadItemsFromSTB( t_eITEM itemType, int iClass, int iMakeNo, int iSkillLv );


	/// 보석의 경우 정해진 규칙에 따라 로드하는 방식이 다른 아이템들과 다르다
	int		LoadGemClassesFromSTB( t_eITEM itemType, int iMakeNo, int iSkillLv );
	int		LoadGemItemsFromSTB( t_eITEM itemType, int iLineNo, int iMakeNo, int iSkillLv );


	bool	IsLoadedClass( int iClass );
	///*----------------------------------------------------------------------------------*/

	//CItem*	CreateItem( tagITEM& Item );

private:

	/// 0. 제조할수 있는 아이템의 리스트 - UI에서 보관( ItemType과 ItemNo )
	int		m_iSkillSlotIndex;
	int		m_iSkillIndex;
	int		m_iSkillLevel;
	
	/// 4. 성공확률						 - 1,3번이 바뀔때마다 수정됨
	int		m_iSuccessRate;

	/// 5. 소모MP						 - SetSkillSlotIndex에서 바뀐다( 스킬번호에 따라 바뀐다. )
	int		m_iConsumeMP;

	/// 1. 제조할(선택된) 아이템		 - UI에서 선택시 바뀐다.
	CItem*			m_pMakeItem;

	/// 2. 제조에 필요한 아이템 리스트   - 1번이 변경될때마다. 바뀐다.
	CRequireMaterial			m_RequireMaterials[g_iMaxCountMaterial];

	/// 3. 인벤토리에서 옮겨진 아이템 리스트 - 유저 입력에 따라 동작
	CItemFragment*	m_pFragmentItems[g_iMaxCountMaterial];

	///성공점( 최소:0 ~ 최대:1000 )
	int		m_iSuccessPoints[g_iMaxCountMaterial];

	///예상확률계산에 사용되는 변수들
	int		m_iItemDif;
	int		m_iItemQuality;
	int		m_iNumMat;
	
	short	m_nPRO_POINT[g_iMaxCountMaterial];

	
	std::list< int >			m_MakableClasses;
	std::list< tagITEM >		m_MakableItems;///현재 스킬로 제조할수 있는 아이템들의 리스트

	CTEventManufacture*	m_pEvent;

	int		m_iMakeType;
};
#endif