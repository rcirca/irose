#include "stdAFX.h"
#include "IO_STB.h"
#include "CInventory.h"

#ifndef	__SERVER
#include "..\\Object.h"
#endif


/*
static t_EquipINDEX s_EquipIDX[] = {
	EQUIP_IDX_NULL,					// Not Used

	EQUIP_IDX_FACE_ITEM,			// ITEM_TYPE_FACE_ITEM = 1,		// 1	LIST_FACEITEM.stb	얼굴 장식	
	EQUIP_IDX_HELMET,				// ITEM_TYPE_HELMET,			// 2	LIST_CAP.stb
	EQUIP_IDX_ARMOR,				// ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
	EQUIP_IDX_GAUNTLET,				// ITEM_TYPE_GAUNTLET,			// 4	LIST_ARMS.stb
	EQUIP_IDX_BOOTS,				// ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
	EQUIP_IDX_KNAPSACK,				// ITEM_TYPE_KNAPSACK,				// 6	LIST_BACK.stb

	EQUIP_IDX_NULL,					// 장신구 : 목걸이 반지

	EQUIP_IDX_WEAPON_R,				// ITEM_TYPE_WEAPON,			// 8	LIST_WEAPON.stb		무기
	EQUIP_IDX_WEAPON_L				// ITEM_TYPE_SUBWPN,			// 9	LIST_SUBWPN.stb
} ;

//-------------------------------------------------------------------------------------------------
t_EquipINDEX CInventory::GetEquipInvIDX (tagITEM *pITEM)
{
	if ( pITEM->GetTYPE() >= 1 && pITEM->GetTYPE() < ITEM_TYPE_USE ) {
		if ( pITEM->GetTYPE() == ITEM_TYPE_JEWEL ) {
			return EQUIP_IDX_NECKLACE,
			return EQUIP_IDX_RING,
			return EQUIP_IDX_RING2,
		}

		return ::s_EquipIDX[ pITEM->GetTYPE() ];
	}

	return EQUIP_IDX_NULL;
}
*/


t_InvTYPE CInventory::m_InvTYPE[ ITEM_TYPE_MONEY ] = {
/*
enum t_InvTYPE {
	INV_WEAPON = 0,
	INV_USE,
	INV_ETC,
	IMV_EMOTION,
	MAX_INV_TYPE
} ;
*/
	MAX_INV_TYPE,	//	Not used...

	INV_WEAPON,		//	ITEM_TYPE_FACE = 1,				// 1	LIST_FACEITEM.stb	얼굴 장식	
	INV_WEAPON,		//	ITEM_TYPE_HELMET,				// 2	LIST_CAP.stb
	INV_WEAPON,		//	ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
	INV_WEAPON,		//	ITEM_TYPE_GAUNTLET,				// 4	LIST_ARMS.stb
	INV_WEAPON,		//	ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
	INV_WEAPON,		//	ITEM_TYPE_KNAPSACK,				// 6	LIST_BACK.stb
	INV_WEAPON,		//	ITEM_TYPE_JEWEL,				// 7	LIST_JEWEL.stb		장신구 : 목걸이 반지

	INV_WEAPON,		//	ITEM_TYPE_WEAPON,				// 8	LIST_WEAPON.stb		무기
	INV_WEAPON,		//	ITEM_TYPE_SUBWPN,				// 9	LIST_SUBWPN.stb

	INV_USE,		//	ITEM_TYPE_USE,					// 10	LIST_USEITEM.stb	소모

	INV_ETC,		//	ITEM_TYPE_ETC = ITEM_TYPE_GEM,	// 11						기타 : 보석
	INV_ETC,		//	ITEM_TYPE_NATURAL,				// 12	LIST_NATURAL.stb
	INV_ETC,		//	ITEM_TYPE_QUEST,				// 13	LIST_QUESTITEM.stb
	INV_RIDING,		//	ITEM_TYPE_SPECIAL,				// 14	xxx

	MAX_INV_TYPE,	//	15 Not used...	
	MAX_INV_TYPE,	//	16 Not used...
	MAX_INV_TYPE,	//	17 Not used...
	MAX_INV_TYPE,	//	18 Not used...
	MAX_INV_TYPE,	//	19 Not used...
	MAX_INV_TYPE,	//	20 Not used...
	MAX_INV_TYPE,	//	21 Not used...
	MAX_INV_TYPE,	//	22 Not used...
	MAX_INV_TYPE,	//	23 Not used...
	MAX_INV_TYPE,	//	24 Not used...
	MAX_INV_TYPE,	//	25 Not used...
	MAX_INV_TYPE,	//	26 Not used...
	MAX_INV_TYPE,	//	27 Not used...
	MAX_INV_TYPE,	//	28 Not used...
	MAX_INV_TYPE,	//	29 Not used...
	MAX_INV_TYPE,	//	31 ITEM_TYPE_MONEY
} ;

//-------------------------------------------------------------------------------------------------
void CInventory::Clear ()
{
	m_i64Money = 0;
	::ZeroMemory (m_ItemLIST,		sizeof( m_ItemLIST ) );
#ifndef	__SERVER
	::ZeroMemory (m_btIndexLIST,	sizeof( BYTE ) * INVENTORY_TOTAL_SIZE);
#endif
}

#ifndef	__SERVER
//-------------------------------------------------------------------------------------------------
//
/// ItemLIST 를 기반으로 IndexLIST 를 만든다.
void CInventory::MakeItemIndexList()
{
	int i,j,iIndexListCount = 0;

	memset( m_btIndexLIST, 0, sizeof( m_btIndexLIST ) );

	for( i =0; i < INVENTORY_TOTAL_SIZE ; i++ )
		m_btIndexLIST[ i ] = i;

	return;

	/// 각 아이템 타입에 대해
	for( i = 0 ; i < MAX_INV_TYPE ; i++ )
	{
		iIndexListCount = 0;

		for( j = 0; j < INVENTORY_PAGE_SIZE; j++ )
		{			
			
			/// 뭔가 이이템이 있다면
			if( m_ItemPAGE[ i ][ j ].m_cType != 0 )
			{				
				m_btIndexPAGE[ i ][ iIndexListCount ] = ( MAX_EQUIP_IDX + i * INVENTORY_PAGE_SIZE ) + j;
				iIndexListCount++;
			}			
		}
	}
}

/// 실제 아이템 인덱스로 참조테이블 인덱스를 구한다.
short CInventory::GetLookupIndexFromRealIndex( short nRealIndex )
{
	int i,j;

	return nRealIndex;

	/// 각 아이템 타입에 대해
	for( i = 0 ; i < MAX_INV_TYPE ; i++ )
	{
		for( j = 0; j < INVENTORY_PAGE_SIZE; j++ )
		{
			if( m_btIndexPAGE[ i ][ j ] == nRealIndex )
			{
				return ( MAX_EQUIP_IDX + i * INVENTORY_PAGE_SIZE ) + j;
			}
		}
	}
	return -1;
}


//-------------------------------------------------------------------------------------------------
/// Lookup Table 를 참조해서 실제 아이템을 얻는다.
bool CInventory::IDX_GetITEM (short nLookUpIndexNO, tagITEM &OutITEM)
{
	_ASSERT( nLookUpIndexNO >= 0 && nLookUpIndexNO < INVENTORY_TOTAL_SIZE );

	if ( m_btIndexLIST[ nLookUpIndexNO ] >0) {
		OutITEM = m_ItemLIST[ m_btIndexLIST[ nLookUpIndexNO ] ];

		if( OutITEM.GetTYPE() )
			return true;
	}
	return false;
}

/// Lookup Table 를 참조해서 실제 인덱스를 얻기위한 IDX_GetITEM 함수를 호출한다.
bool CInventory::IDX_GetITEM (short nInvTYPE, short nPageIndexNO, tagITEM &OutITEM)
{
	_ASSERT( nInvTYPE >= 0 && nInvTYPE < MAX_EQUIP_IDX );
	_ASSERT( nPageIndexNO >= 0 && nPageIndexNO < INVENTORY_PAGE_SIZE );

	short nLookUpIndexNO = ( MAX_EQUIP_IDX + nInvTYPE * INVENTORY_PAGE_SIZE ) + nPageIndexNO;

	return IDX_GetITEM( nLookUpIndexNO, OutITEM );
}

//
//-------------------------------------------------------------------------------------------------
#endif	// __SERVER


short CInventory::GetBodyPartByEquipSlot( short nEquipSlot )
{
	short nBodyPartIDX = MAX_BODY_PART;
	switch( nEquipSlot )
	{		
		case EQUIP_IDX_FACE_ITEM:
			nBodyPartIDX = BODY_PART_FACE_ITEM;
			break;
		case EQUIP_IDX_HELMET:
			nBodyPartIDX = BODY_PART_HELMET;
			break;
		case EQUIP_IDX_ARMOR:
			nBodyPartIDX = BODY_PART_ARMOR;
			break;
		case EQUIP_IDX_KNAPSACK:
			nBodyPartIDX = BODY_PART_KNAPSACK;
			break;
		case EQUIP_IDX_GAUNTLET:
			nBodyPartIDX = BODY_PART_GAUNTLET;
			break;
		case EQUIP_IDX_BOOTS:
			nBodyPartIDX = BODY_PART_BOOTS;
			break;
		case EQUIP_IDX_WEAPON_R:
			nBodyPartIDX = BODY_PART_WEAPON_R;	
			break;
		case EQUIP_IDX_WEAPON_L:
			nBodyPartIDX = BODY_PART_WEAPON_L;
			break;		
	}

	/// 장비 아이템이 아니다.
	return nBodyPartIDX;
}


//-------------------------------------------------------------------------------------------------
/// Real table 에서 아이템을 얻는다.
tagITEM CInventory::LST_GetITEM (short nListNO)
{
	_ASSERT( nListNO >= 0 && nListNO < INVENTORY_TOTAL_SIZE );

	return m_ItemLIST[ nListNO ];
}

/// Real table 에서 아이템타입과, 페이지 번호로( LST_GetITEM( real list no ) 호출 )아이템을 얻는다.
tagITEM CInventory::LST_GetITEM (t_InvTYPE InvTYPE, short nPageListNO)
{
	_ASSERT( InvTYPE >= 0 && InvTYPE < MAX_EQUIP_IDX );
	_ASSERT( nPageListNO >= 0 && nPageListNO < INVENTORY_PAGE_SIZE );

	short nListNO = ( MAX_EQUIP_IDX + InvTYPE * INVENTORY_PAGE_SIZE ) + nPageListNO;

	return LST_GetITEM( nListNO );
}

//-------------------------------------------------------------------------------------------------
/// Real table과 Lookup table 에 각자의 인덱스로 아이템 등록.
bool CInventory::IDX_SetITEM ( short nIndexNO, short nListNO, tagITEM &sITEM )
{
#ifndef	__SERVER
	m_btIndexLIST[ nIndexNO ] = nListNO;
#endif

	m_ItemLIST[ nListNO ]	  = sITEM;
	return true;
}

bool CInventory::IDX_SetITEM (t_InvTYPE InvTYPE, short nPageListNO, short nTotListNO, tagITEM &sITEM)
{
	_ASSERT( InvTYPE >= 0 && InvTYPE < MAX_EQUIP_IDX );
	_ASSERT( nPageListNO >= 0 && nPageListNO < INVENTORY_PAGE_SIZE );
	_ASSERT( nTotListNO >= 0 && nTotListNO < INVENTORY_TOTAL_SIZE );

	short nIndexNO = ( MAX_EQUIP_IDX + InvTYPE * INVENTORY_PAGE_SIZE ) + nPageListNO;

	return IDX_SetITEM( nIndexNO, nTotListNO, sITEM );
}
bool CInventory::IDX_SetITEM (t_InvTYPE IdxInvTYPE, short nIdxPageListNO, t_InvTYPE LstInvTYPE, short nLstPageListNO, tagITEM &sITEM)
{
	_ASSERT( IdxInvTYPE >= 0 && IdxInvTYPE < MAX_EQUIP_IDX );
	_ASSERT( nIdxPageListNO >= 0 && nIdxPageListNO < INVENTORY_PAGE_SIZE );

	_ASSERT( LstInvTYPE >= 0 && LstInvTYPE < MAX_EQUIP_IDX );
	_ASSERT( nLstPageListNO >= 0 && nLstPageListNO < INVENTORY_TOTAL_SIZE );

	short nIndexNO = ( MAX_EQUIP_IDX + IdxInvTYPE * INVENTORY_PAGE_SIZE ) + nIdxPageListNO;
	short nListNO  = ( MAX_EQUIP_IDX + LstInvTYPE * INVENTORY_PAGE_SIZE ) + nLstPageListNO;

	return IDX_SetITEM( nIndexNO, nListNO, sITEM );
}

//-------------------------------------------------------------------------------------------------
short CInventory::GetWEIGHT (short nListNO)
{
	tagITEM *pITEM = &this->m_ItemLIST[ nListNO ];

	if ( 0 == pITEM->GetTYPE() ) {
		return 0;
	}

	if ( pITEM->IsEnableDupCNT() ) {
		return ITEM_WEIGHT( pITEM->m_cType, pITEM->m_nItemNo ) * pITEM->GetQuantity();
	}

	return ITEM_WEIGHT( pITEM->m_cType, pITEM->m_nItemNo );
}

//-------------------------------------------------------------------------------------------------
// iItemNO == 03005 (무사수련복) 
void CInventory::SetInventory (short nListNO, int iItem, int iQuantity)
{
	if ( 0 == iItem )
		return;

	tagITEM sITEM;
	sITEM.Init( iItem, iQuantity );
/*
	::ZeroMemory( &sITEM, sizeof(sITEM) );
	sITEM.m_cType    = (char)(iItemNO / 1000);
	sITEM.m_nItemNo  = iItemNO % 1000;

	t_InvTYPE InvTYPE = m_InvTYPE[ sITEM.m_cType ];
	if ( INV_WEAPON == InvTYPE ) {
		sITEM.m_cResmelt = 0;
		sITEM.m_cQuality = ITEM_QUALITY( sITEM.m_cType, sITEM.m_nItemNo );
	} else {
		sITEM.m_iQuantity = iQuantity;
	}
*/
	m_ItemLIST[ nListNO ] = sITEM;
}


//-------------------------------------------------------------------------------------------------
/// Server function
short CInventory::AppendITEM (tagITEM &sITEM, short &nCurWeight)
{
	_ASSERT( sITEM.GetTYPE() );

	if ( sITEM.IsEmpty() ) {
		return -1;
	}

	if ( ITEM_TYPE_MONEY == sITEM.m_cType ) {
		// 돈이다..
		m_i64Money += sITEM.m_uiMoney;
		return 0;
	}

	t_InvTYPE InvTYPE = m_InvTYPE[ sITEM.m_cType ];
	if ( sITEM.IsEnableDupCNT() ) {
		for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++) {
			// 같은 기타 아이템이라도 아이템 타입이 틀린것들로 구성됨...
			if ( this->m_ItemPAGE[ InvTYPE ][ nI ].GetHEADER() == sITEM.GetHEADER() ) {
				if ( this->m_ItemPAGE[ InvTYPE ][ nI ].GetQuantity() + sITEM.GetQuantity() <= MAX_DUP_ITEM_QUANTITY ) {
					// 더했을 경우 최대 갯수가 넘어 가면 새 스롯에다 할당.
					nCurWeight += ( ITEM_WEIGHT( sITEM.m_cType, sITEM.m_nItemNo ) * sITEM.GetQuantity() );

					this->m_ItemPAGE[ InvTYPE ][ nI ].m_uiQuantity += sITEM.GetQuantity();
					sITEM.m_uiQuantity = this->m_ItemPAGE[ InvTYPE ][ nI ].GetQuantity();

					_ASSERT( INVENTORY_TOTAL_SIZE > nI+MAX_EQUIP_IDX+(InvTYPE*INVENTORY_PAGE_SIZE) );

					return nI+MAX_EQUIP_IDX+(InvTYPE*INVENTORY_PAGE_SIZE);
				}
			}
		}
	}

	// 중복될수 없는 장비아이템, PAT아이템 이나 보유한 아이템중에 같은 아이템이 없는 경우...빈슬롯 할당...
	short nInvIDX = GetEmptyInventory( InvTYPE );
	if ( nInvIDX >= 0 ) {
		AppendITEM ( nInvIDX, sITEM, nCurWeight );
		return nInvIDX;
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER	// 서버에선 사용 안함.
/// Client fucntion
short CInventory::Add_CatchITEM (short nListRealNO, tagITEM &sITEM, short &nCurWeight)
{
	if ( ITEM_TYPE_MONEY == sITEM.m_cType ) {
		// 돈아이템일 경우 현재 돈과 더한다..
		m_i64Money += sITEM.m_iMoney;
		return 0;
	}

	if ( nListRealNO >= INVENTORY_TOTAL_SIZE ) {
		return -1;
	}

	if ( m_ItemLIST[ nListRealNO ].m_dwITEM ) {
		nCurWeight -= this->GetWEIGHT( nListRealNO );
	}

	if ( m_ItemLIST[ nListRealNO ].GetTYPE() == sITEM.GetTYPE()  ) {
		// 더함...
		m_ItemLIST[ nListRealNO ].m_iQuantity += sITEM.m_iQuantity;
	} else {
		// 교체...
		m_ItemLIST[ nListRealNO ] = sITEM;
	}
	nCurWeight += this->GetWEIGHT( nListRealNO );

	/// 추가된 아이템에 대한 Loopup table 갱신
	MakeItemIndexList();	
	g_pAVATAR->m_HotICONS.UpdateHotICON();

	return nListRealNO;
}
#endif


//-------------------------------------------------------------------------------------------------
/// Client & Server function
/// Real table 에 아이템을 추가하고, Lookup table 갱신
short CInventory::AppendITEM (short nListRealNO, tagITEM &sITEM, short &nCurWeight)
{
	_ASSERT( sITEM.GetTYPE() );

	if ( sITEM.IsEmpty() ) {
		return -1;
	}

	if ( ITEM_TYPE_MONEY == sITEM.m_cType ) {
		// 돈아이템일 경우 현재 돈과 더한다..
		m_i64Money += sITEM.m_uiMoney;
		return 0;
	}

	if ( nListRealNO >= INVENTORY_TOTAL_SIZE ) {
		return -1;
	}

	// 돈을 제외한 아이템은 서버에서 받은 아이템으로 교체...
	if ( m_ItemLIST[ nListRealNO ].m_dwITEM ) {
		nCurWeight -= this->GetWEIGHT( nListRealNO );
	}

	m_ItemLIST[ nListRealNO ] = sITEM;
	nCurWeight += this->GetWEIGHT( nListRealNO );


#ifndef	__SERVER	// 서버에선 사용 안함.
	/// 추가된 아이템에 대한 Loopup table 갱신
	MakeItemIndexList();	
	g_pAVATAR->m_HotICONS.UpdateHotICON();
#endif

	return nListRealNO;
}
	

//-------------------------------------------------------------------------------------------------
/// Real Index 로 아이템을 슬롯에서 비운다.
void CInventory::DeleteITEM(WORD wListRealNO)
{
	m_ItemLIST[ wListRealNO ].Clear();

#ifndef	__SERVER	// 서버에선 사용 안함.
	short nLookUpIndex = GetLookupIndexFromRealIndex( wListRealNO );
	if( nLookUpIndex < 0 )
	{
		//_ASSERT( 0 && "nLookUpIndex < 0" );
		return;
	}

	m_btIndexLIST[ nLookUpIndex ] = 0; 

#endif	
}

//-------------------------------------------------------------------------------------------------
// 인벤토리에서 sITEM을 뺀후 nCurWEIGHT를 갱신한다.
void CInventory::SubtractITEM (short nListNO, tagITEM &sITEM, short &nCurWEIGHT)
{
	if ( ITEM_TYPE_MONEY == sITEM.m_cType ) {
		m_i64Money -= sITEM.m_uiMoney;
		return;
	}

	if ( m_ItemLIST[ nListNO ].GetTYPE() ) {
		nCurWEIGHT -= m_ItemLIST[ nListNO ].Subtract( sITEM );
	}
}
/*
// iQuantity갯수 만큼 제거 한다.
void CInventory::SubtractITEM (short nListNO, int iQuantity, short &nCurWEIGHT)
{
	tagITEM SubITEM = m_ItemLIST[ nListNO ];

	if ( SubITEM.IsEnableDupCNT() ) {
		// 중복된 갯수를 갖는 아이템이다.
		SubITEM.m_iQuantity = iQuantity;
	}
	
	nCurWEIGHT -= m_ItemLIST[ nListNO ].Subtract( SubITEM );
}
*/

short CInventory::FindITEM (tagITEM &sITEM)
{
	t_InvTYPE InvTYPE = m_InvTYPE[ sITEM.m_cType ];

	for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++) {
		if ( this->m_ItemPAGE[ InvTYPE ][ nI ].GetHEADER() == sITEM.GetHEADER() ) {
			return nI+MAX_EQUIP_IDX+(InvTYPE*INVENTORY_PAGE_SIZE);
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
/// @bug m_dwITEM == 0 으로만 이 슬롯이 비었다고 할수 있는가?
short CInventory::GetEmptyInventory(short nInvPAGE)
{
	_ASSERT( nInvPAGE >= INV_WEAPON && nInvPAGE < MAX_INV_TYPE );

	for (short nI=0 ; nI < INVENTORY_PAGE_SIZE ; nI++ ) 
	{	
		if( m_ItemLIST[ MAX_EQUIP_IDX + ( nInvPAGE * INVENTORY_PAGE_SIZE ) + nI ].m_cType == 0 ) 
		{
			_ASSERT( INVENTORY_TOTAL_SIZE > nI + MAX_EQUIP_IDX + ( nInvPAGE * INVENTORY_PAGE_SIZE ) );

			return nI + MAX_EQUIP_IDX + ( nInvPAGE * INVENTORY_PAGE_SIZE );
		}
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
short CInventory::GetEmptyInvenSlotCount( t_InvTYPE InvType )
{
	short nCount = 0;

	_ASSERT( InvType >= INV_WEAPON && InvType < MAX_INV_TYPE );

	for( short nl =  0; nl < INVENTORY_PAGE_SIZE; ++nl )
	{
		if( m_ItemPAGE [ InvType  ][ nl ].GetTYPE() == 0 )
			++nCount;
	}
	return nCount;
}

//-------------------------------------------------------------------------------------------------
