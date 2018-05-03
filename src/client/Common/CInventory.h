#ifndef	__CINVENTORY_H
#define	__CINVENTORY_H
#include "CItem.h"
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#pragma pack (push, 1)
class CInventory {
public :
	static t_InvTYPE	m_InvTYPE[ ITEM_TYPE_MONEY ];

	__int64		m_i64Money;

	union {
		// m_ItemLIST[ 0 ]은 서버에서 돈아이템이다...
		tagITEM		m_ItemLIST[ INVENTORY_TOTAL_SIZE ];
		struct {
			tagITEM	m_ItemEQUIP[ MAX_EQUIP_IDX	];		// 착용 장비..
			tagITEM	m_ItemPAGE [ MAX_INV_TYPE	][ INVENTORY_PAGE_SIZE ];
			tagITEM	m_ItemSHOT [ MAX_SHOT_TYPE	];		// 설정된 소모탄..
			tagITEM	m_ItemRIDE [ MAX_RIDING_PART];		// 설정된 승용 아이템.. 
		} ;
	} ;	

#ifndef	__SERVER
	union {
		BYTE		m_btIndexLIST[ INVENTORY_TOTAL_SIZE ];
		struct {
			BYTE	m_btIndexEQUIP[ MAX_EQUIP_IDX ];
			BYTE	m_btIndexPAGE [ MAX_INV_TYPE ][ INVENTORY_PAGE_SIZE ];
		} ;
	} ;


	/// ItemLIST 를 기반으로 IndexLIST 를 만든다.
	void MakeItemIndexList();
	/// 실제 아이템 인덱스로 참조테이블 인덱스를 구한다.
	short GetLookupIndexFromRealIndex( short nRealIndex );
	short Add_CatchITEM (short nListRealNO, tagITEM &sITEM, short &nCurWeight);
	//bool	Remove( tagITEM Item );
	//bool	Append( tagITEM& Item );
	//short	FindEnableAppendDupCNTItem( tagITEM& Item );
#endif

	void Clear ();

	static short GetBodyPartToItemType( short nEquipSlot );
	static short GetBodyPartByEquipSlot( short nEquipSlot );

	bool IDX_GetITEM (short nIndexNO, tagITEM &OutITEM);
	bool IDX_GetITEM (short nInvTYPE, short nPageIndexNO, tagITEM &OutITEM);

	tagITEM LST_GetITEM (short nListNO);
	tagITEM LST_GetITEM (t_InvTYPE InvTYPE, short nPageListNO);

	bool IDX_SetITEM (short nIndexNO, short nListNO, tagITEM &sITEM);
	bool IDX_SetITEM (t_InvTYPE InvTYPE, short nPageListNO, short nListNO, tagITEM &sITEM);
	bool IDX_SetITEM (t_InvTYPE IdxInvTYPE, short nIdxPageListNO, t_InvTYPE LstInvTYPE, short nLstPageListNO, tagITEM &sITEM);

	short	AppendITEM (tagITEM &sITEM, short &nCurWEIGHT);
	short	AppendITEM (short nListNO, tagITEM &sITEM, short &nCurWEIGHT);
	void	DeleteITEM  (WORD wListRealNO);
	short	FindITEM (tagITEM &sITEM);

	void	SubtractITEM (short nLitNO, tagITEM &sITEM, short &nCurWEIGHT);
//	void	SubtractITEM (short nLitNO, int iQuantity, short &nCurWEIGHT);

	short GetWEIGHT (short nListNO);
	void  SetInventory (short nListNO, int iItemNO, int iQuantity=1);

	short GetEmptyInventory(short nInvPAGE);	
	short GetEmptyInvenSlotCount( t_InvTYPE InvType );

	static t_InvTYPE GetInvPageTYPE( tagITEM &sITEM )	{	return m_InvTYPE[ sITEM.m_cType ];	}
	
} ;
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif
