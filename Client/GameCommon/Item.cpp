#include "stdafx.h"
#include "item.h"
#include "../Object.h"
#include "../io_terrain.h"
#include "../JCommandState.h"
#include "../interface/icon/ciconitem.h"
#include "../interface/icon/ciconskill.h"
#include "../interface/dlgs/subclass/cslot.h"

#include "../interface/interfacetype.h"
#include "../util/Localizing.h"
//#include "../interface/it_mgr.h"
//#include "../interface/dlgs/MakeDlg.h"
//#include "../interface/dlgs/ChattingDlg.h"
#include "../interface/command/UICommand.h"
#include "../network/cnetwork.h"
#include "../gamecommon/skill.h"
#include "../gamecommon/StringManager.h"
#include "../gamedata/cstore.h"

#include "../common/calculation.h"
#include "../common/io_pat.h"
#include "../System/CGame.h"

#include "../misc/gameutil.h"

#include "../Country.h"
#include "tgamectrl/SplitHangul.h"
#define INFO_TEXT_WIDTH		26
enum
{
	ATTACK_SPEED_FASTEST = 0,
	ATTACK_SPEED_FAST,
	ATTACK_SPEED_NORMAL,
	ATTACK_SPEED_SLOW,
	ATTACK_SPEED_SLOWEST
};

CItem::CItem(void)
{
	m_bEnable			= true;
	m_iIndex			= 0;
	m_iIndexType		= 0;	

	m_pDefaultIcon		= NULL;
	m_strString			= "CItem";

	m_Item.Clear();
	m_pCommand			= NULL;
	m_strString			= "CItem";
	m_iUnitPrice		= 0;
}

CItem::CItem( tagITEM* pItemData )
{
	m_bEnable			= true;
	m_iIndex			= 0;
	m_iIndexType		= 0;	

	m_pDefaultIcon		= NULL;
	m_strString			= "CItem";

	m_Item.Clear();
	m_pCommand			= NULL;
	m_strString			= "CItem";
	m_iUnitPrice		= 0;
	SetItem( *pItemData );
}

CItem::~CItem(void)
{
	m_pCommand			= NULL;
}

void CItem::SetCommand( CTCommand* pCmd )
{
	m_pCommand = pCmd;
}

CTCommand*	CItem::GetCommand()
{
	return m_pCommand;
}

void CItem::SetDefaultIcon( CIcon* pIcon )
{
	assert( m_pDefaultIcon == NULL );
	m_pDefaultIcon	= pIcon;
}

void CItem::Clear()
{
	if( m_pDefaultIcon )
	{
		delete m_pDefaultIcon;
		m_pDefaultIcon = NULL;
	}
}

CIcon* CItem::CreateItemIcon()
{
	CIcon* pIcon  = new CIconItem( this );
	return pIcon;
}



tagITEM& CItem::GetItem()
{
	if( GetIndexType() == IT_INVENTORY )
		return g_pAVATAR->m_Inventory.m_ItemLIST[ m_iIndex ];

	return m_Item;
}

void CItem::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int CItem::GetIndex()
{
	return m_iIndex;
}

int  CItem::GetType()
{
	tagITEM& Item = GetItem();
	return Item.GetTYPE();
}

void CItem::SetIndexType( int iType )
{
	m_iIndexType = iType;
}

int	CItem::GetIndexType()
{
	return m_iIndexType;
}

bool CItem::IsEmpty()
{
	tagITEM& Item = GetItem();
	return Item.IsEmpty();
}

int  CItem::GetQuantity()
{
	tagITEM& Item = GetItem();

	if( Item.IsEmpty() )
		return 0;

	if( !Item.IsEnableDupCNT() )
		return 1;

	return Item.GetQuantity();
}

/// StateLocation 이 LS_NONE일경우 사용될 m_Item Set
void CItem::SetItem( tagITEM& Item )
{
	if( GetIndexType() == IT_INVENTORY )
		g_pAVATAR->m_Inventory.m_ItemLIST[ m_iIndex ] = Item;
	else
		m_Item = Item;
}

void CItem::SetLastExecTime(DWORD dwTime )
{
	m_dwLastExecTime = dwTime;
}
DWORD CItem::GetLastExecTime()
{
	return m_dwLastExecTime;
}

///*------------------------------------------------------------------------------------/
/// @return 실제 빼면 남는 개수
/// @brief 아이템의 개수를 빼는 Method, Client는 기본적으로 Subtract시 아이템 삭제를 할수 없다.
///        예외) 제조
///*------------------------------------------------------------------------------------/
//int	CItem::SubItemQuantity( int iQuantity )
//{
//	tagITEM& Item = g_pAVATAR->m_Inventory.m_ItemLIST[ m_iInvenIndex ];
//
//	if( Item.IsEnableDupCNT() )
//	{
//		if( Item.GetQuantity() > iQuantity )
//		{
//			Item.m_uiQuantity -= iQuantity;		
//			return Item.m_uiQuantity;
//		}
//		else
//		{
//			Item.m_uiQuantity = 1;
//			return 0;
//		}
//	}
//	return 0;
//}

int CItem::GetItemNo()
{
	tagITEM& Item = GetItem();
	return Item.GetItemNO();
}

bool CItem::IsEnableDupCNT()
{
	tagITEM& Item = GetItem();
	return Item.IsEnableDupCNT();
}
void CItem::Excute()
{
	if( m_pCommand && IsEnable() )
		m_pCommand->Exec( this );
}


void CItem::AddItemName( tagITEM& sItem, CInfo& Info )
{
	DWORD dwTextColor = 0;

	if( sItem.GetLife() < 1 )
		dwTextColor = g_dwRED;
	else
		dwTextColor = g_dwYELLOW;

	if( sItem.GetGrade() )
		Info.AddString( CStr::Printf( "%s (%d)",ITEM_NAME(sItem.GetTYPE(), sItem.GetItemNO()),sItem.GetGrade() ),dwTextColor , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
	else
		Info.AddString( CStr::Printf( "%s",ITEM_NAME(sItem.GetTYPE(), sItem.GetItemNO()) ),dwTextColor , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
}

void CItem::AddItemWeight( tagITEM& sItem, CInfo& Info )
{
	Info.AddString( CStr::Printf( "%s:%d", STR_WEIGHT, ITEM_WEIGHT(sItem.GetTYPE(), sItem.GetItemNO()) ) );
}

void CItem::AddItemPatUseFuelRate( tagITEM& sItem, CInfo& Info )
{
	if( PAT_ITEM_USE_FUEL_RATE( sItem.GetItemNO() ) <=0 ) return;
	Info.AddString( CStr::Printf( "%s : %d", STR_WASTE_FUEL, PAT_ITEM_USE_FUEL_RATE( sItem.GetItemNO() ) ), g_dwWHITE ); 
}

void CItem::AddItemPatAttackRange( tagITEM& sItem, CInfo& Info )
{
	if(PAT_ITEM_ATK_RANGE( sItem.GetItemNO() ) <= 0) return;
	Info.AddString( CStr::Printf( "%s: %dM", STR_ATTACK_RANGE, PAT_ITEM_ATK_RANGE( sItem.GetItemNO() ) / 100 ), g_dwWHITE );
}

void CItem::AddItemPatCartGauage( tagITEM& sItem, CInfo& Info )
{	
	if(PAT_ITEM_CART_GAUAGE(sItem.GetItemNO() ) <= 0) return;
	Info.AddString( CStr::Printf( "%s: %d", CStringManager::GetSingleton().GetAbility( AT_PATHP ), PAT_ITEM_CART_GAUAGE(sItem.GetItemNO() ) ) , g_dwWHITE ) ;
}

void CItem::AddItemPatAttackPowSpeed(tagITEM& sItem, CInfo& Info)
{
	if( GetAttackPower( sItem ) <= 0 ) return;

	std::string strAttackSpeed;
	GetAttackSpeedString( PAT_ITEM_ATK_SPD( sItem.GetItemNO() ), strAttackSpeed );
	//ATTACK POWER, ATTACK SPEED
	/*
	Info.AddString( CStr::Printf( "%s:%d %s:%s", 
		CStringManager::GetSingleton().GetAbility( AT_ATK ), 
		GetAttackPower( sItem ),
		CStringManager::GetSingleton().GetAbility( AT_ATK_SPD ),
		strAttackSpeed.c_str()) 
		);
	*/
	Info.AddString( CStr::Printf( "%s:%d", 
		CStringManager::GetSingleton().GetAbility( AT_ATK ), 
		GetAttackPower( sItem ) ) );
}

void CItem::AddItemPatDesc( tagITEM& sItem, CInfo& Info )
{
	CSplitHangul	splitHAN(  (char*)ITEM_DESC( sItem.GetTYPE(), sItem.GetItemNO() ),
								Info.GetMaxSizeString() - 2,
								CLocalizing::GetSingleton().GetCurrentCodePageNO());

	short	nCNT   = splitHAN.GetLineCount();
	for(short nI=0; nI<nCNT; nI++) 
	{
		Info.AddString( splitHAN.GetString(nI));
	}
}


void CItem::AddItemPatVer( tagITEM& sItem, CInfo& Info )
{
	Info.AddString( CStr::Printf( "%s: %d",  STR_RIDE_PART_VER, PAT_ITEM_PART_VER(sItem.GetItemNO() ) ) , g_dwWHITE ) ;
}


void CItem::AddItemPatClass( tagITEM& sItem, CInfo& Info )
{
	int iClass = ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() );	
	
	//분류,명중력
	if( iClass == TUNING_PART_WEAPON_CASTLEGEAR )
	{
		Info.AddString( CStr::Printf( "%s:%s %s:%d", 
			STR_ITEM_TYPE, 
			CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ), 
			CStringManager::GetSingleton().GetAbility( AT_HIT ), 
			(int)( ITEM_QUALITY( sItem.GetTYPE(), sItem.GetItemNO() ) * 1.2) )
			);
	}
	else
	{
		Info.AddString( CStr::Printf( "%s:%s", 
			STR_ITEM_TYPE, 
			CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ) ) );
	}
}

void CItem::AddItemPatEquipRequireJob( tagITEM& sItem, CInfo& Info )
{
	int iRequireClass = PAT_ITEM_EQUIP_REQUIRE_CLASS( sItem.GetItemNO() );

	if( iRequireClass )
	{
		const char* pszTemp = CStr::Printf("[%s: %s]",STR_EQUIP_REQUIRE_JOB, CLASS_NAME(iRequireClass));
		DWORD  color = g_dwRED;

		int iJob = g_pAVATAR->Get_JOB();

		if( iJob )
		{
			for( int i = 0; i < CLASS_INCLUDE_JOB_CNT; ++i )
			{
				if( iJob == CLASS_INCLUDE_JOB(iRequireClass,i) )
				{
					color = g_dwGREEN;
					break;
				}
			}
		}
		Info.AddString( pszTemp, color );
	}
}

void CItem::GetToolTipRidePart_BattleCart( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	tagITEM& sItem = GetItem();

	assert( !sItem.IsEmpty() );
	short nItemType = sItem.GetTYPE();
	short nItemIdx  = sItem.GetItemNO();

	//1 아이템 명칭
	AddItemName( sItem, ToolTip );

	//2 아이템 분류
	AddItemPatClass( sItem, ToolTip );

	//3, 4 ITEM_DURABITY : 수명, 내구도
	AddItemLifeDuration( sItem, ToolTip );

	//5 ITEM_RESISTENCE : 연료소비율
	AddItemPatUseFuelRate( sItem, ToolTip );		

	//6, 7 공격력, 공격속도
	AddItemPatAttackPowSpeed( sItem, ToolTip );

	//8 부품버전
	AddItemPatVer( sItem, ToolTip );

	//9 공격 거리
	AddItemPatAttackRange( sItem, ToolTip );

	//10 이동속도
	AddItemMoveSpeed( sItem, ToolTip );

	//11 카트 게이지.
	AddItemPatCartGauage( sItem, ToolTip );

	if( !(INFO_STATUS_DETAIL & dwType) ) 
	{
		return;
	}

	//12, 13 ITEM_ADD_DATA_TYPE,  ITEM_ADD_DATA_VALUE
	//14, 15
	AddItemPatAddAbility( sItem, ToolTip );

	//16,17 ITEM_NEED_DATA_TYPE, ITEM_NEED_DATA_VALUE		
	AddItemEquipRequireCondition( sItem, ToolTip );		

	//18 해당 직업군
	AddItemPatEquipRequireJob( sItem, ToolTip );	
	
	//19 가격 : 다른 아이템과 호환성때문에 맨 마직막에 넣음.

	//20 ITEM_WEIGHT 무게
	AddItemWeight( sItem, ToolTip );
	
	// desc : 문서에는 없지만 필요할것 같아 넣음.
	AddItemPatDesc( sItem, ToolTip );		
	
	return;
}

void CItem::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	tagITEM& sItem = GetItem();

	assert( !sItem.IsEmpty() );
	short nItemType = sItem.GetTYPE();
	short nItemIdx  = sItem.GetItemNO();

	bool bIsDetail;

	if(INFO_STATUS_DETAIL & dwType) 
		bIsDetail = true;
	else 
		bIsDetail = false;


	DWORD dwTextColor = 0;

	//if( CGame::GetInstance().IsRepairMode() && dwDialogType == DLG_TYPE_ITEM && sItem.IsEquipITEM() )
	//{
	//	ToolTip.AddString( ITEM_NAME( sItem.GetTYPE(), sItem.GetItemNO() ) );
	//	ToolTip.AddString( CStr::Printf("현재수명: %d", sItem.GetLife() / 10 ) ) );
	//	ToolTip.AddString( CStr::Printf("품질등급: %d", sItem.GetGrade() ) );
	//	ToolTip.AddString( CStr::Printf("수리비용: %d", sItem.GetRepairPrice() ) );
	//	return;
	//}
	switch(nItemType)
	{
	case ITEM_TYPE_WEAPON:
		{
			//이름 
			if( sItem.GetGrade() )
				ToolTip.AddString( CStr::Printf( "%s (%d)",(char*)ITEM_NAME(nItemType,nItemIdx),sItem.GetGrade() ), 
									GetItemNameColor( sItem.GetTYPE(), sItem.GetItemNO() ),
									g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]
									);
			else
				ToolTip.AddString( CStr::Printf( "%s",ITEM_NAME(nItemType,nItemIdx) ) , 
									GetItemNameColor( sItem.GetTYPE(), sItem.GetItemNO() ),
									g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]
									);

			//분류,명중력
			std::string strTemp;
			strTemp = CStr::Printf( "%s:%s %s:", 
				STR_ITEM_TYPE, 
				CStringManager::GetSingleton().GetItemType( ITEM_TYPE( nItemType, nItemIdx ) ),
				CStringManager::GetSingleton().GetAbility( AT_HIT )
			);

			if( sItem.GetGrade() <= 0)
				strTemp.append( CStr::Printf("%d", sItem.GetHitRate() ) );
			else
				strTemp.append( CStr::Printf("%d", sItem.GetHitRate() + ITEMGRADE_HIT( sItem.GetGrade() )) );

			ToolTip.AddString( strTemp.c_str() );

			AddItemLifeDuration( sItem, ToolTip );



			std::string strAttackSpeed;
			GetAttackSpeedString( WEAPON_ATTACK_SPEED( nItemIdx ), strAttackSpeed );
			//공격력,공격속도
			ToolTip.AddString( CStr::Printf( "%s:%d  %s:%s", 
									CStringManager::GetSingleton().GetAbility( AT_ATK ), 
									GetAttackPower( sItem ) + ITEMGRADE_ATK( sItem.GetGrade() ),
									CStringManager::GetSingleton().GetAbility( AT_ATK_SPD ),
									strAttackSpeed.c_str()) 
							);
			//공격거리 
			ToolTip.AddString( CStr::Printf( "%s%dM", STR_ATTACK_RANGE, WEAPON_ATTACK_RANGE( nItemIdx ) / 100 ), g_dwWHITE );


			if(bIsDetail) 
			{
				AddItemSpecialJob( sItem, ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );

				AddItemOption( sItem, ToolTip );


				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_SUBWPN:
		{
			//방패일경우
			if( ITEM_TYPE(nItemType,nItemIdx) == 261 )
			{
				AddItemDefaultTopTextDefenceItem( sItem , ToolTip );
				AddItemLifeDuration( sItem, ToolTip );
				AddItemDefence( sItem, ToolTip );
			}
			else
			{
				AddItemDefaultTopText( sItem ,ToolTip );				
				AddItemLifeDuration( sItem, ToolTip );
			}
			


			if(bIsDetail) 
			{
				AddItemSpecialJob( sItem , ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_ARMOR:
		{
			AddItemDefaultTopTextDefenceItem( sItem, ToolTip );
			AddItemLifeDuration( sItem, ToolTip );			
			AddItemDefence( sItem, ToolTip );

			if( bIsDetail )
			{
				///재질
				AddItemSpecialJob( sItem, ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_GAUNTLET:///list_arm
	case ITEM_TYPE_HELMET:///list_cap
	case ITEM_TYPE_FACE_ITEM:
		{
			if( nItemType == ITEM_TYPE_FACE_ITEM )
				AddItemDefaultTopText( sItem, ToolTip );
			else
				AddItemDefaultTopTextDefenceItem( sItem, ToolTip );

			AddItemLifeDuration( sItem, ToolTip );
			AddItemDefence( sItem, ToolTip );


			if( bIsDetail )
			{
				AddItemSpecialJob( sItem , ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}

			break;
		}
	case ITEM_TYPE_KNAPSACK:
		{

			AddItemDefaultTopTextDefenceItem( sItem, ToolTip );
			AddItemLifeDuration( sItem, ToolTip );			
			AddItemDefence( sItem , ToolTip );
			AddItemMoveSpeed( sItem, ToolTip );

			if( bIsDetail )
			{
				AddItemSpecialJob( sItem , ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_BOOTS:
		{
			AddItemDefaultTopTextDefenceItem( sItem, ToolTip );
			AddItemLifeDuration( sItem, ToolTip );
			AddItemDefence( sItem , ToolTip );
			AddItemMoveSpeed( sItem, ToolTip );

			if( bIsDetail )
			{
				AddItemSpecialJob( sItem , ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );
				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_JEWEL:
		{
			AddItemDefaultTopText( sItem, ToolTip );
			if( bIsDetail )
			{
				AddItemSpecialJob( sItem , ToolTip );
				AddItemSpecialUnion( sItem, ToolTip );
				AddItemOption( sItem, ToolTip );
			
				AddItemEquipRequireJob( sItem, ToolTip );
				AddItemEquipRequireUnion( sItem, ToolTip );
				AddItemEquipRequireCondition( sItem, ToolTip );

				AddItemDefaultBottomText( sItem, ToolTip );
			}
			break;
		}
	case ITEM_TYPE_NATURAL:
		{
			AddItemDefaultTopText( sItem, ToolTip );
			if( bIsDetail )
				AddItemDefaultBottomText( sItem, ToolTip );

			break;
		}
	case ITEM_TYPE_GEM:
		{
			AddItemDefaultTopText( sItem, ToolTip );

			for( int i = 0; i < 2 ; i++ )
			{
				/// 소속조건없이 올라가는 능력치가 있다면 
				if( GEMITEM_ADD_DATA_TYPE( nItemIdx, i ) != 0 )					
				{
					if( GEMITEM_ADD_DATA_TYPE( nItemIdx, i ) == AT_SAVE_MP )
					{
						ToolTip.AddString( CStr::Printf( "[%s %d%%]", 
							CStringManager::GetSingleton().GetAbility( GEMITEM_ADD_DATA_TYPE( nItemIdx, i ) ), 
							GEMITEM_ADD_DATA_VALUE( nItemIdx, i ) ),

							g_dwBlueToolTip, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
					}
					else
					{
						ToolTip.AddString( CStr::Printf( "[%s %d]", 
							CStringManager::GetSingleton().GetAbility( GEMITEM_ADD_DATA_TYPE( nItemIdx, i ) ), 
							GEMITEM_ADD_DATA_VALUE( nItemIdx, i ) ),

							g_dwBlueToolTip, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
					}
				}
			}

			if( bIsDetail )
				AddItemDefaultBottomText( sItem, ToolTip );

			break;
		}
	case ITEM_TYPE_USE:
		{
			AddItemDefaultTopText( sItem, ToolTip );

			switch( ITEM_TYPE( nItemType, nItemIdx ) )
			{
			case USE_ITEM_FUEL:
				{
					ToolTip.AddString( CStr::Printf("%s : %d", 
						CStringManager::GetSingleton().GetItemType( USE_ITEM_FUEL ),
						USEITEM_ADD_DATA_VALUE( nItemIdx) ) );

					if( bIsDetail )
						AddItemDefaultBottomText( sItem, ToolTip );
				}
				break;
			case USE_ITEM_SKILL_LEARN:			///MAGIC ITEM
				{
					int iSkillNo = USEITEM_SCROLL_USE_SKILL( nItemIdx );
					if( iSkillNo )
					{
						CIconSkill::AddSkillRequireJob( iSkillNo, ToolTip );
						CIconSkill::AddSkillRequireAbility( iSkillNo, ToolTip );
						CIconSkill::AddSkillRequireSkill( iSkillNo, ToolTip );
						CIconSkill::AddSkillRequireSkillPoint2Learn( iSkillNo, ToolTip );
					}
					if( bIsDetail )
						AddItemDefaultBottomText( sItem, ToolTip );

					break;
				}
			case USE_ITEM_SKILL_DOING:
				{
					int iSkillNo = USEITEM_SCROLL_USE_SKILL( nItemIdx );
					///워프스킬사용 매직아이템일경우
					if( iSkillNo )
					{
						DWORD   dwColor;
						if( SKILL_TYPE( iSkillNo ) == SKILL_TYPE_18 )
						{
				
							///<-MP 체크 
							int	iRequireMp = (int)(g_pAVATAR->GetCur_WEIGHT() * 0.05);

							if( g_pAVATAR->Get_AbilityValue( AT_MP ) >= iRequireMp )
								dwColor = g_dwGREEN;
							else
								dwColor = g_dwRED;
							ToolTip.AddString( CStr::Printf( "[%s:%s %d]", 
												STR_CONSUME_ABILITY, 
												CStringManager::GetSingleton().GetAbility( AT_MP ), 
												iRequireMp )
												, 
												dwColor );
							///->
						}
						else///그외 스킬사용 매직아이템일경우
						{
							///소모능력치와 소모 수치를 표시한다.
							int iUseValue;

							for( int iUseProperty = 0; iUseProperty < SKILL_USE_PROPERTY_CNT; ++iUseProperty )
							{
								if( SKILL_USE_PROPERTY( iSkillNo, iUseProperty ) )
								{
									iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, iUseProperty );

									if( g_pAVATAR->Get_AbilityValue( SKILL_USE_PROPERTY( iSkillNo, iUseProperty ) ) >= iUseValue )
										dwColor = g_dwGREEN;
									else
										dwColor = g_dwRED;

									ToolTip.AddString( CStr::Printf( "[%s:%s %d]", 
										STR_CONSUME_ABILITY, 
										CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY( iSkillNo, iUseProperty ) ), 
										iUseValue )
										,
										dwColor );
								}
							}
						}

						AddItemUseNeedData( sItem, ToolTip );
						if( bIsDetail )
							AddItemDefaultBottomText( sItem, ToolTip );
					}
					break;
				}
			case USE_ITEM_MAINTAIN_ITEM:///시간제 쿠폰 아이템
				if( USEITEM_ADD_DATA_TYPE( nItemIdx ) )
					ToolTip.AddString( CStr::Printf( "[%s %d %s]", CStringManager::GetSingleton().GetAbility( USEITEM_ADD_DATA_TYPE( nItemIdx ) ), USEITEM_ADD_DATA_VALUE( nItemIdx ) / 144, STR_DAY) );

				if( bIsDetail )
				{
					AddItemUseNeedData( sItem, ToolTip );
					AddItemDefaultBottomText( sItem, ToolTip );
				}
				break;
			default:
				{
					/// 기능
					if( USEITEM_ADD_DATA_TYPE( nItemIdx ) )
					{
						if( USEITEM_ADD_DATA_TYPE( nItemIdx ) == AT_SAVE_MP )					
							ToolTip.AddString( CStr::Printf( "[%s %d%%]", CStringManager::GetSingleton().GetAbility( USEITEM_ADD_DATA_TYPE( nItemIdx ) ), USEITEM_ADD_DATA_VALUE( nItemIdx ) ) );
						else
							ToolTip.AddString( CStr::Printf( "[%s %d]", CStringManager::GetSingleton().GetAbility( USEITEM_ADD_DATA_TYPE( nItemIdx ) ), USEITEM_ADD_DATA_VALUE( nItemIdx ) ) );
					}

					if( bIsDetail )
					{
						AddItemUseNeedData( sItem, ToolTip );
						AddItemDefaultBottomText( sItem, ToolTip );
					}
					break;
				}
			}
			break;
		}
	case ITEM_TYPE_RIDE_PART:
		{	

#ifdef _GBC
			GetToolTipRidePart_BattleCart(ToolTip, dwDialogType, dwType);
			break;
#endif

			int iClass = ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() );
			if( (iClass % 100) / 10 == 5 ) ///WEAPON
			{
				if( sItem.GetLife() < 1 )
					dwTextColor = g_dwRED;
				else
					dwTextColor = g_dwYELLOW;

				if( sItem.GetGrade() )
					ToolTip.AddString( CStr::Printf( "%s (%d)",ITEM_NAME(nItemType,nItemIdx),sItem.GetGrade() ),dwTextColor , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
				else
					ToolTip.AddString( CStr::Printf( "%s",ITEM_NAME(nItemType,nItemIdx) ),dwTextColor , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );

				//분류,명중력
				if( iClass == TUNING_PART_WEAPON_CASTLEGEAR )
				{
					ToolTip.AddString( CStr::Printf( "%s:%s %s:%d", 
						STR_ITEM_TYPE, 
						CStringManager::GetSingleton().GetItemType( ITEM_TYPE( nItemType, nItemIdx ) ), 
						CStringManager::GetSingleton().GetAbility( AT_HIT ), 
						(int)( ITEM_QUALITY( nItemType, nItemIdx ) * 1.2) )
					);
				}
				else
				{
					ToolTip.AddString( CStr::Printf( "%s:%s", 
						STR_ITEM_TYPE, 
						CStringManager::GetSingleton().GetItemType( ITEM_TYPE( nItemType, nItemIdx ) )  ));
				}

				AddItemLifeDuration( sItem, ToolTip );

				if( iClass == TUNING_PART_WEAPON_CASTLEGEAR )
				{
					std::string strAttackSpeed;
					GetAttackSpeedString( PAT_ITEM_ATK_SPD( nItemIdx ), strAttackSpeed );
					//ATTACK POWER, ATTACK SPEED
					ToolTip.AddString( CStr::Printf( "%s:%d %s:%s", 
											CStringManager::GetSingleton().GetAbility( AT_ATK ), 
											GetAttackPower( sItem ),
											CStringManager::GetSingleton().GetAbility( AT_ATK_SPD ),
											strAttackSpeed.c_str()) 
									);
					//ATTACK RANGE
					ToolTip.AddString( CStr::Printf( "%s: %dM", STR_ATTACK_RANGE, PAT_ITEM_ATK_RANGE( nItemIdx ) / 100 ), g_dwWHITE );
					//WASTE FUEL
					ToolTip.AddString( CStr::Printf( "%s : %d", STR_WASTE_FUEL, PAT_ITEM_USE_FUEL_RATE( nItemIdx ) ), g_dwWHITE );
				}

				AddItemEquipRequireCondition( sItem, ToolTip );
			}
			else
			{
				AddItemDefaultTopTextDefenceItem( sItem, ToolTip );
				AddItemLifeDuration( sItem, ToolTip );		

				switch( (iClass % 100) / 10  )  
				{
				case 2:///ENGINE
					{
						ToolTip.AddString( CStr::Printf("%s:%d%%", 
								CStringManager::GetSingleton().GetAbility( AT_SPEED ), PAT_ITEM_MOV_SPD( sItem.GetItemNO() ) ) );
					}
					break;
				case 1:///BODY
					{
						if( PAT_ITEM_USE_FUEL_RATE( nItemIdx ) )
							ToolTip.AddString( CStr::Printf( "%s:%d", STR_WASTE_FUEL, PAT_ITEM_USE_FUEL_RATE( nItemIdx ) ), g_dwWHITE );
					}
					break;
				default:
					break;
				}

				AddItemEquipRequireCondition( sItem, ToolTip );
			}
			AddItemPatAddAbility( sItem, ToolTip );
			
			ToolTip.AddString( CStr::Printf( "%s: %d",STR_RIDE_PART_VER,PAT_ITEM_PART_VER( sItem.GetItemNO() ) ) );

			///AddItemDefaultBottomText( sItem, ToolTip );2004/7/30 - 현재 설명을 보이려고 할때 에러남.
			break;
		}
	default:
		break;
	}	

	// 가격을 표시하나?
	if( (dwType & INFO_ADD_PRICE) || 
		(dwType & INFO_ADD_PRICE_SELL) || 
		(dwType & INFO_ADD_PRICE_STORAGE_FEE) ||
		(dwType & INFO_ADD_PRICE_PRIVATESTORE ) || 
		(dwType & INFO_ADD_PRICE_AVATARSTORE_BUYTAB ) ||
		(dwType & INFO_ADD_PRICE_AVATARSTORE_SELLTAB ) ||
		(dwType & INFO_ADD_PRICE_REPAIR ) ||
		(dwType & INFO_ADD_PRICE_APPRAISAL )
		)
	{
		AddItemPrice( sItem, dwDialogType, dwType, ToolTip );
	}

}

void CItem::SubQuantity( int iQuantity )
{
	tagITEM* pItem = &( g_pAVATAR->m_Inventory.m_ItemLIST[ m_iIndex ] );
	if( pItem->IsEmpty() )
	{
		assert( 0 && "빈 아이템을 빼려고 합니다@CItem::SubQuantity:" );	
		return;
	}
	if( !pItem->IsEnableDupCNT() )
	{
		assert( 0 && "개수가 없는 아이템의 개수를 빼려고 합니다 @CItem::SubQuantity:" );	
		return;
	}

	if( (int)pItem->GetQuantity() <= iQuantity )
	{
		pItem->m_uiQuantity = 1;
		LogString( LOG_NORMAL, "Underflow @CItem::SubQuantity" );
		return;
	}

	pItem->m_uiQuantity = pItem->GetQuantity() - iQuantity;
}

void CItem::SetUnitPrice( int iPrice )
{
	m_iUnitPrice = iPrice;
}
int CItem::GetUnitPrice()
{
	return m_iUnitPrice;
}

///*---------------------------------------------------------------------------------------------------------------*/
/// 이하 툴팁용 Method
void CItem::AddItemDefaultTopTextDefenceItem( tagITEM& sItem ,CInfo& Info )
{
	//이름 
//	DWORD dwTextColor;
//	if( sItem.GetLife() < 1 )
//		dwTextColor = g_dwRED;
//	else
//		dwTextColor = g_dwYELLOW;

	if( sItem.GetGrade() )
		//Info.SetTitle( CStr::Printf("%s (%d)",ITEM_NAME(sItem.GetTYPE(), sItem.GetItemNO() ), sItem.GetGrade() )  );
		Info.AddString( CStr::Printf("%s (%d)",ITEM_NAME(sItem.GetTYPE(), sItem.GetItemNO() ), sItem.GetGrade() ), 
						GetItemNameColor( sItem.GetTYPE(), sItem.GetItemNO() ),
						g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]
						);
	else
		Info.AddString( (char*)ITEM_NAME(sItem.GetTYPE(), sItem.GetItemNO() ) , 
						GetItemNameColor( sItem.GetTYPE(), sItem.GetItemNO() ),
						g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]
						);

	//분류,회피력


	std::string stdTemp;
	//stdTemp.append( STR_ITEM_TYPE );
	//stdTemp.append( CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ) );
	//stdTemp.append( CStringManager::GetSingleton().GetAbility( AT_AVOID ) );

	if( sItem.GetTYPE() != ITEM_TYPE_RIDE_PART )
	{
		stdTemp = CStr::Printf("%s:%s %s:",
				STR_ITEM_TYPE,
				CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ),
				CStringManager::GetSingleton().GetAbility( AT_AVOID ) );

		if( sItem.GetGrade() )
			stdTemp.append( CStr::Printf("%d", sItem.GetAvoidRate() + ITEMGRADE_AVOID( sItem.GetGrade() ) ));
		else
			stdTemp.append( CStr::Printf("%d", sItem.GetAvoidRate() ));
	}
	else
	{
		stdTemp = CStr::Printf("%s:%s",
				STR_ITEM_TYPE,
				CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ) );
	}

	Info.AddString( stdTemp.c_str() );
}

void CItem::AddItemDefaultTopText( tagITEM& sItem, CInfo& Info )
{
	///이름
	//DWORD dwTextColor;
	//if( sItem.IsEquipITEM() && sItem.GetLife() < 1 )
	//	dwTextColor = g_dwRED;
	//else
	//	dwTextColor = g_dwYELLOW;
	//Info.SetTitle( (char*)ITEM_NAME( sItem.GetTYPE(), sItem.GetItemNO() ) );
	//Info.SetTitle( (char*)(ITEM_NAME( sItem.GetTYPE(), sItem.GetItemNO() )) );
	Info.AddString( (char*)(ITEM_NAME( sItem.GetTYPE(), sItem.GetItemNO() ) ), GetItemNameColor( sItem.GetTYPE(), sItem.GetItemNO()) , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]);
	///분류,무게
	char* pszBuf = CStr::Printf( "%s:%s %s:%d",
		STR_ITEM_TYPE,
		CStringManager::GetSingleton().GetItemType( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) ),
		STR_QUALITY,
		ITEM_QUALITY(sItem.GetTYPE(), sItem.GetItemNO()));

	Info.AddString( pszBuf ,g_dwWHITE, g_GameDATA.m_hFONT[ FONT_NORMAL ]);
}

void CItem::AddItemDefaultBottomText( tagITEM& sItem, CInfo& Info )
{
	// 무게 				
	char* pszBuf = CStr::Printf("%s:%d",  STR_WEIGHT, ITEM_WEIGHT( sItem.GetTYPE(), sItem.GetItemNO() ) );

	Info.AddString( pszBuf );
	
	///볼드라서 
	CSplitHangul	splitHAN(  (char*)ITEM_DESC( sItem.GetTYPE(), sItem.GetItemNO() ), INFO_TEXT_WIDTH - 2, CLocalizing::GetSingleton().GetCurrentCodePageNO());
	short	nCNT   = splitHAN.GetLineCount();
	for(short nI=0;nI<nCNT;nI++) 
		Info.AddString( splitHAN.GetString(nI));

}
void CItem::AddItemEquipRequireCondition( tagITEM& sItem, CInfo& Info )
{
	DWORD color = g_dwRED;
	char* pszBuf = NULL;

	switch( sItem.GetTYPE() )
	{
	case ITEM_TYPE_USE:
		break;
	case ITEM_TYPE_RIDE_PART:
		if( PAT_ITEM_NEED_ABILITY_IDX( sItem.GetItemNO()) )
		{
			pszBuf = CStr::Printf( "[%s:%s %d]",
				STR_EQUIP_CONDITION,
				CStringManager::GetSingleton().GetAbility( PAT_ITEM_NEED_ABILITY_IDX(  sItem.GetItemNO()) ),
				PAT_ITEM_NEED_ABILITY_VALUE( sItem.GetItemNO()) );

			if( g_pAVATAR->Get_AbilityValue( PAT_ITEM_NEED_ABILITY_IDX( sItem.GetItemNO() ) ) >= PAT_ITEM_NEED_ABILITY_VALUE( sItem.GetItemNO() ) )
				color = g_dwGREEN;
			else
				color = g_dwRED;

			Info.AddString( pszBuf, color );
		}
		break;
	default:
		{
			for( int i = 0; i < ITEM_NEED_DATA_CNT; ++i )
			{
				if( ITEM_NEED_DATA_TYPE( sItem.GetTYPE(), sItem.GetItemNO(), i) )
				{
					pszBuf = CStr::Printf( "[%s:%s %d]",
						STR_EQUIP_CONDITION,
						CStringManager::GetSingleton().GetAbility( ITEM_NEED_DATA_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ,i )),
						ITEM_NEED_DATA_VALUE( sItem.GetTYPE(), sItem.GetItemNO() ,i ) );

					if( g_pAVATAR->Get_AbilityValue( ITEM_NEED_DATA_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ,i ) ) >= ITEM_NEED_DATA_VALUE( sItem.GetTYPE(), sItem.GetItemNO() ,i ) )
						color = g_dwGREEN;
					else
						color = g_dwRED;

					Info.AddString( pszBuf, color );
				}
			}
			break;
		}
	}
}

void CItem::AddItemEquipRequireUnion( tagITEM& sItem, CInfo& Info )
{
	std::string		strTemp("[");
	strTemp.append( CStringManager::GetSingleton().GetAbility(AT_UNION) );
	strTemp.append(":");

	int iItemType = sItem.GetTYPE();
	int iItemNo	  = sItem.GetItemNO();

	DWORD	dwColor		= g_dwRED;
	int		iCheckCount = 0;
	bool	bCorrect	= false;


	for( int i = 0; i < ITEM_EQUIP_REQUIRE_UNION_CNT; i++ )
	{
		if( 0 != ITEM_EQUIP_REQUIRE_UNION( sItem.GetTYPE(), sItem.GetItemNO(), i ))
		{
			++iCheckCount;
			strTemp.append(" ");
			strTemp.append( UNION_NAME( ITEM_EQUIP_REQUIRE_UNION( sItem.GetTYPE(), sItem.GetItemNO(), i )));

			if( ITEM_EQUIP_REQUIRE_UNION( sItem.GetTYPE(), sItem.GetItemNO(), i ) == g_pAVATAR->GetCur_UNION() )
			{
				bCorrect = true;
				break;
			}
		}
	}

	strTemp.append("]");

	if( iCheckCount )
	{
		if( bCorrect )
			dwColor = g_dwGREEN;
		
		Info.AddString( strTemp.c_str(), dwColor );
	}
}

void CItem::AddItemEquipRequireJob( tagITEM& sItem, CInfo& Info )
{
//	aJobIndex[0] = ITEM_JOB_TYPE1( nItemType, nItemIdx );
//	aJobIndex[1] = ITEM_JOB_TYPE2( nItemType, nItemIdx );
//	aJobIndex[2] = ITEM_JOB_TYPE3( nItemType, nItemIdx );


	int iRequireClass = ITEM_EQUIP_REQUIRE_CLASS( sItem.GetTYPE(), sItem.GetItemNO() );

	if( iRequireClass )
	{
		const char* pszTemp = CStr::Printf("[%s: %s]",STR_EQUIP_REQUIRE_JOB, CLASS_NAME(iRequireClass));
		DWORD  color = g_dwRED;

		int iJob = g_pAVATAR->Get_JOB();

		if( iJob )
		{
			for( int i = 0; i < CLASS_INCLUDE_JOB_CNT; ++i )
			{
				if( iJob == CLASS_INCLUDE_JOB(iRequireClass,i) )
				{
					color = g_dwGREEN;
					break;
				}
			}
		}
		Info.AddString( pszTemp, color );
	}
}

void CItem::AddItemSpecialJob( tagITEM& sItem, CInfo& Info )
{
	char* pszBuf;
	int iJobIndex = ITEM_NEED_UNION( sItem.GetTYPE(), sItem.GetItemNO(), 0 );
	if( iJobIndex )
	{
		pszBuf = CStr::Printf( "[%s %s]", CStringManager::GetSingleton().GetJobName( iJobIndex), GetAddDataValueString( sItem, 0 ) );
		Info.AddString( pszBuf, g_dwBlueToolTip, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
	}
	else
	{
		if( ITEM_ADD_DATA_VALUE( sItem.GetTYPE(), sItem.GetItemNO(), 0 ) )
		{
			pszBuf = CStr::Printf( "[%s]", GetAddDataValueString( sItem, 0 ) );
			Info.AddString( pszBuf, g_dwBlueToolTip, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
		}
	}
}
void CItem::AddItemSpecialUnion( tagITEM& sItem, CInfo& Info )
{
	char* pszBuf;

	int iJobIndex = ITEM_NEED_UNION( sItem.GetTYPE(), sItem.GetItemNO(), 1 );
	if( iJobIndex )
	{
		pszBuf = CStr::Printf( "[%s %s]", CStringManager::GetSingleton().GetJobName(iJobIndex), GetAddDataValueString( sItem, 1 ) );

		Info.AddString( pszBuf, g_dwBlueToolTip, g_GameDATA.m_hFONT[FONT_NORMAL_BOLD] );
	}
	else
	{
		if( ITEM_ADD_DATA_VALUE( sItem.GetTYPE(), sItem.GetItemNO(), 1 ) )
		{
			pszBuf = CStr::Printf( "[%s]", GetAddDataValueString( sItem, 1 ) );	
			Info.AddString( pszBuf, g_dwBlueToolTip, g_GameDATA.m_hFONT[FONT_NORMAL_BOLD]);
		}
	}
}


void CItem::AddItemDefence( tagITEM& sItem, CInfo& Info )
{
	short nItemType = sItem.GetTYPE();
	short nItemIdx  = sItem.GetItemNO();

	std::string stdTemp;
	stdTemp.append( CStringManager::GetSingleton().GetAbility( AT_DEF ) );
	if( sItem.GetGrade() )
		stdTemp.append(CStr::Printf(":%d ", GetDefence( sItem ) + ITEMGRADE_DEF(sItem.GetGrade()) ) );
	else
		stdTemp.append(CStr::Printf(":%d ", GetDefence( sItem ) ) );


	stdTemp.append( CStringManager::GetSingleton().GetAbility( AT_RES ) );
	if( sItem.GetGrade() )
		stdTemp.append( CStr::Printf(":%d", ITEM_RESISTENCE(nItemType, nItemIdx) + ITEMGRADE_RES( sItem.GetGrade() ) ) );
	else
		stdTemp.append( CStr::Printf(":%d", ITEM_RESISTENCE(nItemType, nItemIdx) ) )  ;


	Info.AddString( stdTemp.c_str() );
}
void CItem::AddItemUseNeedData( tagITEM& sItem, CInfo& Info )
{
	short nItemIdx = sItem.GetItemNO();

	int iNeedDataType  = USEITEM_NEED_DATA_TYPE( nItemIdx );
	int iNeedDataValue = USEITEM_NEED_DATA_VALUE(  nItemIdx );
	DWORD dwColor = g_dwRED;
	if( iNeedDataType )
	{
		if( iNeedDataType == AT_CURRENT_PLANET )
		{
			int iPlanetNo = iNeedDataValue;

			if( iPlanetNo == ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )
				dwColor = g_dwWHITE;

			CStringManager& StringMgr = CStringManager::GetSingleton();
			char* pszBuf = CStr::Printf( "%s : %s", StringMgr.GetAbility(iNeedDataType), StringMgr.GetPlanetName( iPlanetNo ));
			Info.AddString( pszBuf, dwColor );
		}
		else
		{
			if( g_pAVATAR->Get_AbilityValue( iNeedDataType ) >= iNeedDataValue )
				dwColor = g_dwGREEN;
			
			Info.AddString( CStr::Printf( "[%s:%s %d]", 
				STR_USE_REQUIRE, 
				CStringManager::GetSingleton().GetAbility( iNeedDataType ), 
				iNeedDataValue ),
				dwColor );
		}
	}

}
void CItem::AddItemMoveSpeed( tagITEM& sItem, CInfo& Info )
{
	short nItemType = sItem.GetTYPE();
	short nItemIdx	= sItem.GetItemNO();

	int iDefaultMoveSpeed = 50;
	int iItemMoveSpeed    = 0;
	char* pszBuf;

	switch( nItemType )
	{
	case ITEM_TYPE_KNAPSACK:
		{
			if( BACKITEM_MOVE_SPEED( nItemIdx ) )
			{
				pszBuf = CStr::Printf( "[%s %d]",STR_MOVE_SPEED,BACKITEM_MOVE_SPEED( nItemIdx )  );
				Info.AddString( pszBuf );
			}
			break;
		}
	case ITEM_TYPE_BOOTS:
		pszBuf = CStr::Printf("[%s %d]",STR_MOVE_SPEED,BOOTS_MOVE_SPEED( nItemIdx ) - iDefaultMoveSpeed );
		Info.AddString( pszBuf );
		break;

	case ITEM_TYPE_RIDE_PART:

		if( PAT_ITEM_MOV_SPD( sItem.GetItemNO() ) > 0 )
		{
			Info.AddString( CStr::Printf("%s:%d%%", 
				CStringManager::GetSingleton().GetAbility( AT_SPEED ), PAT_ITEM_MOV_SPD( sItem.GetItemNO() ) ) );		
		}
		break;

	default:
		Info.AddString( "INVALID ITEM TYPE:MOVE SPEED" );
		break;
	}
}
void CItem::AddItemOption( tagITEM& sItem, CInfo& Info )
{
	//무기에 제밍된 보석혹은 옵션에 따른 무기의 효과상승 표시
	char* pszBuf = NULL;
	short nOption = sItem.GetOption();

	if( nOption != 0 )
	{
		if( nOption <= 300 && sItem.IsAppraisal() == false )
		{
			Info.AddString(STR_REQUIRE_IDENTIFY,g_dwRED );
		}
		else
		{
			if( nOption > 300 )///제밍된경우
				Info.AddString( ITEM_NAME( ITEM_TYPE_GEM, nOption ), g_dwYELLOW );

			for( int j = 0; j < 2 ; j++ )
			{
				short nAttribute = GEMITEM_ADD_DATA_TYPE( nOption, j );
				if( nAttribute != 0 )
				{
					if( nAttribute == AT_SAVE_MP )
						pszBuf = CStr::Printf( "[%s %d%%]", CStringManager::GetSingleton().GetAbility( nAttribute ), GetOption( sItem, nOption, j ) );
					else
						pszBuf = CStr::Printf( "[%s %d]", CStringManager::GetSingleton().GetAbility( nAttribute ), GetOption( sItem, nOption, j ) );
					if( nOption > 300 )
						Info.AddString( pszBuf, g_dwYELLOW );
					else
						Info.AddString( pszBuf, g_dwBlueToolTip );
				}
			}						
		}
	}
}


void CItem::AddItemPrice(tagITEM& sItem, DWORD dwDlgType, DWORD dwType, CInfo& Info )
{
	char* pszBuf = NULL;
	int iPrice = 0;
	const int money_buffer_size = 64;
	char money_buffer[money_buffer_size];

	if( dwType & INFO_ADD_PRICE_STORAGE_FEE )
	{
		unsigned short ItemType				= sItem.GetTYPE();
		unsigned short ItemNo				= sItem.GetItemNO();
		if( sItem.IsEnableKEEPING() )///보관료 표시
		{
			int	iCount = 1;

			int iStorageFee = CCal::Get_StorageFEE( ITEM_BASE_PRICE( sItem.GetTYPE(), sItem.GetItemNO() ),
				ITEM_PRICE_RATE( sItem.GetTYPE(), sItem.GetItemNO()) ,iCount );
			
			CGameUtil::ConvertMoney2String( iStorageFee, money_buffer, money_buffer_size );
			pszBuf = CStr::Printf( "%s:%s",STR_BANK_STORAGE_FEE,money_buffer );
		}
		else///보관할수 없는 아이템 
		{
			pszBuf = STR_BANK_CANT_STORE;
		}
	}
	else if( dwType & INFO_ADD_PRICE )
	{
		if( dwDlgType == DLG_TYPE_ITEM )
		{
			unsigned short ItemType				= sItem.GetTYPE();
			unsigned short ItemNo				= sItem.GetItemNO();
			unsigned short ItemUseRestriction	= ITEM_USE_RESTRICTION( ItemType , ItemNo );

			if( sItem.IsEnableSELL() )
			{
				iPrice  = g_pTerrain->m_Economy.Get_ItemSellPRICE(sItem, g_pAVATAR->GetSellSkillVALUE());
				CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
				pszBuf = CStr::Printf("%s %s",STR_PRICE, money_buffer);
			}
			else
			{
				pszBuf = STR_CANT_SELL;
			}
		}
		else
		{
			iPrice  = g_pTerrain->m_Economy.Get_ItemSellPRICE(sItem, g_pAVATAR->GetSellSkillVALUE());

			CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
			pszBuf = CStr::Printf( "%s %s",STR_PRICE, money_buffer );
		}
	}
	else if( dwType & INFO_ADD_PRICE_SELL )
	{
		if( CStore::GetInstance().IsUnionStore() )
		{
			iPrice = ITEM_TRADE_UNIONPOINT( sItem.GetTYPE(), sItem.GetItemNO() );
			pszBuf = CStr::Printf( "%s:%d",STR_UNIONPOINT, iPrice );
		}
		else
		{
			iPrice  = g_pTerrain->m_Economy.Get_ItemBuyPRICE(sItem.GetTYPE(),sItem.GetItemNO(), g_pAVATAR->GetBuySkillVALUE());
			CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
			pszBuf = CStr::Printf( "%s:%s",STR_SELL_PRICE,money_buffer );
		}
	}
	else if( dwType & INFO_ADD_PRICE_PRIVATESTORE )
	{
		iPrice = GetUnitPrice();
		CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
		pszBuf = CStr::Printf( "%s:%s",STR_UNIT_PRICE, money_buffer );
	}
	else if( dwType & INFO_ADD_PRICE_AVATARSTORE_BUYTAB )
	{
		iPrice = GetUnitPrice();

		CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
		pszBuf = CStr::Printf( "%s:%s",STR_UNIT_PRICE, money_buffer );
		Info.AddString( pszBuf, g_dwYELLOW );

		iPrice  = g_pTerrain->m_Economy.Get_ItemSellPRICE(sItem, g_pAVATAR->GetSellSkillVALUE());
		CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
		pszBuf = CStr::Printf( "%s:%s", STR_NPC_TRADE_PRICE, money_buffer );
	}
	else if( dwType & INFO_ADD_PRICE_AVATARSTORE_SELLTAB )
	{
		iPrice = GetUnitPrice();
		CGameUtil::ConvertMoney2String( iPrice, money_buffer, money_buffer_size );
		pszBuf = CStr::Printf( "%s:%s",STR_UNIT_PRICE, money_buffer );
	}
	else if( dwType & INFO_ADD_PRICE_REPAIR )
	{
		__int64 iRepairPrice = 0;
		if( (sItem.GetTYPE() == ITEM_TYPE_RIDE_PART || sItem.IsEquipITEM() ) && 
			sItem.GetLife() < 1000 && 
			sItem.GetDurability() > 0 )
		{
			__int64 i64BasePrice    = ITEM_BASE_PRICE( sItem.GetTYPE(), sItem.GetItemNO() );
			iRepairPrice  = ( i64BasePrice + 1000 ) * ( sItem.GetDurability() + 10 ) * ( 1100 - sItem.GetLife() ) / 400000;
		}

		CGameUtil::ConvertMoney2String( iRepairPrice, money_buffer, money_buffer_size );


		if( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) != TUNING_PART_ENGINE_CART &&
			ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) != TUNING_PART_ENGINE_CASTLEGEAR )
		{			
			pszBuf = CStr::Printf("%s:%s",STR_REPAIR_PRICE, money_buffer );			
		}
		else
		{
			pszBuf = STR_CANT_REFAIR_PAT_ENGINE;
		}
		
	}
	else if( (dwType & INFO_ADD_PRICE_APPRAISAL) && sItem.IsEnableAppraisal() )
	{
		CGameUtil::ConvertMoney2String( sItem.GetAppraisalCost(), money_buffer, money_buffer_size );
		pszBuf = CStr::Printf("%s:%s",STR_APPRAISAL_COST, money_buffer );
	}

	DWORD color = D3DCOLOR_ARGB( 255,255,255,255 );
	switch( dwDlgType )
	{
	case DLG_TYPE_ITEM:
		color = g_dwVIOLET;
		break;
	case DLG_TYPE_STORE:
		color = g_dwYELLOW;
		break;
	case DLG_TYPE_AVATARSTORE:
	case DLG_TYPE_PRIVATESTORE:
		color  = g_dwYELLOW;
	default:
		break;
	}

	Info.AddString( pszBuf, color );
}

void CItem::AddItemLifeDuration( tagITEM& sItem, CInfo& Info )
{
	int iLife = sItem.GetLife() / 10;
	if( sItem.GetLife() % 10 )
		iLife += 1;
	
	char* pszLife = NULL;

	if( ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) != TUNING_PART_ENGINE_CART &&
		ITEM_TYPE( sItem.GetTYPE(), sItem.GetItemNO() ) != TUNING_PART_ENGINE_CASTLEGEAR )
	{
		pszLife = STR_LIFE;
	}
	else
	{
		pszLife = STR_FUEL;
	}

	Info.AddString( CStr::Printf("%s:%3d%%   %s:%3d", pszLife, iLife, STR_DURABILITY, sItem.GetDurability() ));

}

void CItem::GetAttackSpeedString( short nSpeed, std::string& strMsg )
{
	if( nSpeed < 0 )
	{
		strMsg = "스피드 < 0";
		return;
	}

	if( nSpeed == 12 )
		strMsg = STR_NORMAL;
	else if( nSpeed < 12 )
	{
		strMsg = STR_FASTER;
		strMsg.append(" +");
		strMsg.append( CStr::Printf("%d", 12 - nSpeed ) );
	}
	else///nSpeed > 12
	{
		strMsg = STR_SLOWER;
		strMsg.append(" -");
		strMsg.append( CStr::Printf("%d", nSpeed - 12 ) );
	}
}

short CItem::GetAttackSpeedType( short nSpeed )	// 어텍 스피드 타입( 매우빠름, 빠름... )
{
	if( nSpeed <= 9 )
		return ATTACK_SPEED_FASTEST;

	if( nSpeed <= 11 )
		return ATTACK_SPEED_FAST;

	if( nSpeed <= 13 )
		return ATTACK_SPEED_NORMAL;

	if( nSpeed <= 15 )
		return ATTACK_SPEED_SLOW;

	return ATTACK_SPEED_SLOWEST;
}

bool CItem::IsEnable()
{
	return m_bEnable;
}

void CItem::SetEnable()
{
	m_bEnable = true;
}

void CItem::SetLife( short nLife )
{
	g_pAVATAR->m_Inventory.m_ItemLIST[m_iIndex].m_nLife = nLife;;
	if( nLife <= 0 )
		g_pAVATAR->UpdateAbility();
}

int CItem::GetOption( tagITEM& sItem, short nOption, short nCol )
{
	if( sItem.GetLife() < 1 )
		return 0;

	return GEMITEM_ADD_DATA_VALUE( nOption, nCol );
}

const char* CItem::GetAddDataValueString( tagITEM& sItem , int iColumn )
{
	int iAddValue = ITEM_ADD_DATA_VALUE( sItem.GetTYPE(), sItem.GetItemNO() , iColumn );
	int iAddType  = ITEM_ADD_DATA_TYPE( sItem.GetTYPE(), sItem.GetItemNO(), iColumn );
	if( iAddValue == 0 || iAddType <= 0 )
		return NULL;

	char* pszFormat = NULL;
	
	if( iAddType == AT_SAVE_MP )
		pszFormat = "%s %d%%";
	else
		pszFormat = "%s %d";

	return CStr::Printf( pszFormat,
		CStringManager::GetSingleton().GetAbility( iAddType ), iAddValue );
}

int	CItem::GetDefence( tagITEM& sItem )
{
	if( sItem.GetLife() < 1 )
		return 0;
	return ITEM_DEFENCE( sItem.GetTYPE(), sItem.GetItemNO() );
}

int CItem::GetAttackPower( tagITEM& sItem )
{
	if( sItem.GetLife() < 1 )
		return 0;

	if( sItem.GetTYPE() == ITEM_TYPE_RIDE_PART )
		return PAT_ITEM_ATK_POW( sItem.GetItemNO() );

	return WEAPON_ATTACK_POWER( sItem.GetItemNO() );
}
//*---------------------------------------------------------------------------------------*/
/// @brief 상점에서 파는 아이템일경우에는 항상 false가 될수 없다.
//*---------------------------------------------------------------------------------------*/
void CItem::SetDisable()
{
	if( m_iIndexType != IT_STORE )
		m_bEnable = false;
}

const char*	CItem::GetName()
{
	tagITEM& Item = GetItem();
	return ITEM_NAME( Item.GetTYPE(), Item.GetItemNO() );
}

void CItem::AddItemPatAddAbility( tagITEM& sItem, CInfo& Info )
{
	for( int i = 0 ; i < PAT_ITEM_ADD_ABILITY_CNT; ++i )
	{
		if( PAT_ITEM_ADD_ABILITY_IDX( sItem.GetItemNO(), i ) )
		{
			Info.AddString( CStr::Printf( "[%s %d]", 			
				CStringManager::GetSingleton().GetAbility( PAT_ITEM_ADD_ABILITY_IDX( sItem.GetItemNO() ,i ) ),
				PAT_ITEM_ADD_ABILITY_VALUE( sItem.GetItemNO() ,i ) ),
				g_dwBlueToolTip, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] );
		}
	}
}

#ifdef FRAROSE
int CItem::GetItemRareType( int type, int no )
{
	switch( type )
	{
	case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_ARMOR:
	case ITEM_TYPE_HELMET:
	case ITEM_TYPE_GAUNTLET:
	case ITEM_TYPE_BOOTS:
		return ( g_pTblSTBs[type]->m_ppDATA[ no ][ g_pTblSTBs[type]->m_nColCnt - 1 ] );
	default:
		return 0;
	}
	return 0;
}

DWORD CItem::GetItemNameColor( int type, int no )
{
	int iRareType = GetItemRareType( type, no );
	
	DWORD color = D3DCOLOR_ARGB( 255, 0xFF, 0xFF, 0);//노란색

	if( iRareType == 21 )
		color = D3DCOLOR_ARGB( 255, 255, 128, 255 );///분홍색
	else if( iRareType >= 1 && iRareType <= 20 )
		color = D3DCOLOR_ARGB( 0xFF, 0, 0xFF, 0xFF);///하늘색
	else if( iRareType == 22 )
		color = D3DCOLOR_ARGB( 255, 179, 0, 0 );		///Red- Prefix 21
	else if( iRareType == 23 )
		color = D3DCOLOR_ARGB( 255, 107, 158, 101 );	///Green- Prefix 1
	else if( iRareType == 24 )
		color = D3DCOLOR_ARGB( 255, 80, 119, 186 );		///Blue- Prefix 2
	else if( iRareType == 25 )
		color = D3DCOLOR_ARGB( 255, 179, 179, 0 );		///Gold- Prefix 3
	else if( iRareType == 26 )
		color = D3DCOLOR_ARGB( 255, 255, 85, 0 );		///Orange- Prefix 4
	else if( iRareType == 27 )
		color = D3DCOLOR_ARGB( 255, 178, 87, 101 );		///Pink- Prefix 5
	else if( iRareType == 28 )
		color = D3DCOLOR_ARGB( 255, 255, 255, 0 );		///Yellow- Prefix 6
	else if( iRareType == 29 )
		color = D3DCOLOR_ARGB( 255, 193, 199, 255 );	///C1C7FF
	else if( iRareType == 30 )
		color = D3DCOLOR_ARGB( 255, 255, 84, 58 );		///FF543A
	return color;
}
#else
int CItem::GetItemRareType( int type, int no )
{
	switch( type )
	{
	case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_ARMOR:
	case ITEM_TYPE_HELMET:
	case ITEM_TYPE_GAUNTLET:
	case ITEM_TYPE_BOOTS:
		return ( g_pTblSTBs[type]->m_ppDATA[ no ][ g_pTblSTBs[type]->m_nColCnt - 1 ] );
	default:
		return 0;
	}
	return 0;
}

DWORD CItem::GetItemNameColor( int type, int no )
{
	int iRareType = GetItemRareType( type, no );
	
	DWORD color = D3DCOLOR_ARGB( 255, 0xFF, 0xFF, 0);//노란색

	if( iRareType == 21 )
		color = D3DCOLOR_ARGB( 255, 255, 128, 255 );///분홍색
	else if( iRareType >= 1 && iRareType <= 20 )
		color = D3DCOLOR_ARGB( 0xFF, 0, 0xFF, 0xFF);///하늘색

	return color;
}
#endif
///*----------------------------------------------------------------------------------/
CItemFragment::CItemFragment( CItem* pItem )
{
	assert( pItem );
	m_pItem = pItem;
	m_iQuantity = 1;
	m_pItem->SetDisable();
}

CItemFragment::~CItemFragment()
{
	assert( m_pItem );
	m_pItem->SetEnable();
}

CIcon* CItemFragment::CreateItemIcon()
{
	CIcon* pIcon  = new CIconItem( this );
	return pIcon;
}

int	CItemFragment::GetQuantity()
{
	return m_iQuantity;
}

void CItemFragment::SetQuantity( int iQuantity )
{
	m_iQuantity = iQuantity;
}

CItem*	CItemFragment::GetInnerCItem()
{
	assert( m_pItem );
	return m_pItem;
}

void	CItemFragment::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	assert( m_pItem );
	m_pItem->GetToolTip( ToolTip, dwDialogType, dwType );
}

tagITEM& CItemFragment::GetItem()
{
	assert( m_pItem );
	return m_pItem->GetItem();
}
int CItemFragment::GetIndex()
{
	assert( m_pItem );
	return m_pItem->GetIndex();
}
//*----------------------------------------------------------------------------------*/
/// @brief CItemFragment는 항상 Enable이다.
//*----------------------------------------------------------------------------------*/
bool CItemFragment::IsEnable()
{
	return true;
}

void CItemFragment::SetUnitPrice( int iUnitPrice )
{
	m_pItem->SetUnitPrice( iUnitPrice );
}

int CItemFragment::GetUnitPrice()
{
	return m_pItem->GetUnitPrice();
}


