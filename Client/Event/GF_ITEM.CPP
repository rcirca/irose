/*
	$Header: /Client/Event/GF_ITEM.CPP 15    04-10-25 4:33p Jeddli $
*/

#include "stdAFX.h"
#include "Quest_FUNC.h"
#include "TGameCtrl.h"
#include "OBJECT.h"  /* <=== 이 파일을 인클루드 시켜 사용할 것 */
#include "IO_Basic.h"
#include "..\\interface\\it_mgr.h"


//---------------------------------[ 아바타 소지 현금 관련 ]------------------------------------
/* 아바타 소지현금 가져오기 */
int		GF_checkUserMoney ( void )
{
	return (int)::g_pAVATAR->Get_MONEY ();
}

/* 아바타 소지현금 가져오기 */
void	GF_addUserMoney ( int iAmount )
{
	::g_pAVATAR->Add_MONEY (iAmount);
}

/* 아바타 소지현금 가져오기 */
void	GF_takeUserMoney ( int iAmount )
{
	::g_pAVATAR->Add_MONEY (-iAmount);
}

//------------------------------------[ 아바타 아이템 관련 ]---------------------------------------

/* 아바타 아이템 장착/해제하기 bWear : 1(장착), 0(해제) , 인벤토리와 상관 없음 */
void GF_setEquipedItem ( int nPartIDX, int iItemIDX, int bWear )
{
	/*	BODY_PART_FACE = 0,
	BODY_PART_HAIR,
	BODY_PART_HELMET,

	BODY_PART_ARMOR,
	BODY_PART_GAUNTLET,
	BODY_PART_BOOTS,

	BODY_PART_GOGGLE,
	BODY_PART_KNAPSACK,

	BODY_PART_WEAPON_R,
	BODY_PART_WEAPON_L,

	MAX_BODY_PART */

	if(nPartIDX >= MAX_BODY_PART) { return; }
	
	if(bWear)	{ g_pAVATAR->SetPartITEM (nPartIDX, iItemIDX); }
	else		{ g_pAVATAR->SetPartITEM (nPartIDX, 0);}

	g_pAVATAR->Update ();
}

//====================================================================================
// 인벤토리에 아이템 추가 / 제거 / 갯수 알아내기
//====================================================================================
int GF_getIDXOfInvItem ( int nItemNo, int nItemType )
{
	tagITEM		Item;

	CInventory *pInv = ::g_pAVATAR->GetInventory ();
	if(!pInv) { return -1; }

	for(int i = MAX_EQUIP_IDX; i < INVENTORY_TOTAL_SIZE; i++)
	{
		pInv->IDX_GetITEM (i, Item);
		if(Item.GetItemNO () == nItemNo && Item.GetTYPE () == nItemType ) { return i; }
	}

	return -1;
}

int GF_getIDXOfEquipItem ( int nItemNo, int nItemType )
{
	tagITEM		Item;

	CInventory *pInv = ::g_pAVATAR->GetInventory ();
	if(!pInv) { return -1; }

	for(int i = 0 ; i < MAX_EQUIP_IDX; i++)
	{
		pInv->IDX_GetITEM (i, Item);
		if(Item.GetItemNO () == nItemNo && Item.GetTYPE () == nItemType ) { return i; }
	}

	return -1;
}

int GF_checkNumOfInvItem ( int nItemNo, int nItemType )
{
	tagITEM		Item;

	CInventory *pInv = ::g_pAVATAR->GetInventory ();
	if(!pInv) { return -1; }

	int iIDX = ::GF_getIDXOfInvItem (nItemNo, nItemType);
	if(iIDX < 0) { return 0; }
	switch (nItemType)
	{
	case ITEM_TYPE_FACE_ITEM:
	case ITEM_TYPE_HELMET:
	case ITEM_TYPE_ARMOR:
	case ITEM_TYPE_GAUNTLET:
	case ITEM_TYPE_BOOTS:
	case ITEM_TYPE_KNAPSACK:
	case ITEM_TYPE_JEWEL:
	case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_SUBWPN:
		return 1;
	}

	pInv->IDX_GetITEM (iIDX, Item);
	
	return Item.GetQuantity ();
}

void GF_giveEquipItemIntoInv ( int cQuality, int cResmelt, int cJamming, int bAppraisal, int cOption, int nItemNo, int cType )
{
	assert( 0 && "GF_giveEquipItemIntoINV" );

	//tagITEM		Item;

	///*
	//CInventory *pInv = ::g_pAVATAR->GetInventory ();
	//if(!pInv) { return; }
	//*/
	//Item.m_cDurability 	= cQuality;
	//Item.m_cGrade	 	= cResmelt;
	//
	//Item.m_cJamming 	= cJamming;
	//Item.m_bAppraisal 	= bAppraisal;
	//Item.m_cOption 		= cOption;

	//Item.m_nItemNo		= nItemNo;
	//Item.m_cType		= cType;
	///*
	//int iOldIDX = ::GF_getIDXOfInvItem (Item.m_nItemNo, Item.m_cType);

	//pInv->AppendITEM (Item);
	//*/

	//short nPageNo	= g_itMGR.GetItemInventoryTAB(Item);
	//short nEmptyInv = g_pAVATAR->GetEmptyInventory(nPageNo);
	//if(nEmptyInv >= 0) 
	//{ g_pAVATAR->Add_ITEM(nEmptyInv, Item); }
}

/* 인벤에서 아이템 갯수 감소시키기 */
void GF_takeItemFromInv ( int nNum /* +2이면 2개 제거 */, int nItemNo, int cType )
{
	int	iIDX = 0;
	CInventory *pInv = ::g_pAVATAR->GetInventory ();

	if(!pInv) { return ; }

	switch (nNum)
	{
	case 0:
		iIDX = ::GF_getIDXOfInvItem (nItemNo, cType);

		g_pAVATAR->ClearITEM (iIDX);
		break;

	default:
		if(nNum < 0) { break; }
		iIDX = GF_getIDXOfInvItem (nItemNo, cType);
		tagITEM		Item;
		pInv->IDX_GetITEM (iIDX, Item);
		Item.m_uiQuantity -= nNum;
		g_pAVATAR->ClearITEM (iIDX);
		if ( Item.m_uiQuantity > 0 )
			g_pAVATAR->Add_ITEM (iIDX, Item);
	}
}

void GF_giveUsableItemIntoInv (int iQuantity, int iItemNo, int iItemType )
{
	tagITEM		Item;

	ZeroMemory (&Item, sizeof (tagITEM));

	if ( iQuantity < 1 ) iQuantity = 1;
	Item.m_uiQuantity	= iQuantity;
	Item.m_nItemNo_1	= iItemNo;
	Item.m_cType_1		= iItemType;

	short nPageNo		= g_itMGR.GetItemInventoryTAB(Item);
	short nEmptyInv		= g_pAVATAR->GetEmptyInventory(nPageNo);

	if(nEmptyInv >= 0) 	{ g_pAVATAR->Add_ITEM (nEmptyInv, Item); }
}

/* 인벤토리 <===> 장착 */
void GF_putoffItem ( int iPartIDX, int iType )
{
	tagITEM		Item;
	ZeroMemory (&Item, sizeof (tagITEM)); 

	CInventory *pInv = ::g_pAVATAR->GetInventory ();
	if(!pInv) { return ; }

	Item.m_nItemNo	= ::g_pAVATAR->GetPartITEM (iPartIDX);
	Item.m_cType	= iType;

	// 장비장착창(인벤내에 있음)에서 없앰
	short wListNO = GF_getIDXOfEquipItem ( Item.m_nItemNo, Item.m_cType );
	g_pAVATAR->ClearITEM (wListNO);
	// 아바타몸에서 제거
	GF_setEquipedItem (iPartIDX, 0, 0);

	GF_giveEquipItemIntoInv (0, 0 , 0 , 0 , 0 , Item.m_nItemNo, Item.m_cType);

	g_pAVATAR->Update ();
}

void GF_givePartItem (short wListNo, int nItemNO, int nItemType)
{
	CInventory *pInv = ::g_pAVATAR->GetInventory ();
	if(!pInv) { return ; }

	tagITEM		Item;

	Item.Clear();

	Item.m_nItemNo		= nItemNO;
	Item.m_cType		= nItemType;

	g_pAVATAR->Add_ITEM (wListNo, Item);
}

void GF_putonItem ( int iPartIDX, int iItemNo, int iItemType)
{
	if(GF_checkNumOfInvItem (iItemNo, iItemType) > 0)
	{
		GF_setEquipedItem (iPartIDX, iItemNo, 1);
		::GF_takeItemFromInv (0, iItemNo, iItemType);
		GF_givePartItem (iPartIDX, iItemNo, iItemType); 

		g_pAVATAR->Update ();
	}
}
