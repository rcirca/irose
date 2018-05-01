#ifndef	__IO_STB_H
#define	__IO_STB_H
#include "..\Util\classHASH.h"
#ifndef __SERVER
	#include "../GameCommon/StringManager.h"
#endif
//-------------------------------------------------------------------------------------------------

class STBVALUE {
private:
	bool	m_bString;

	int		m_iValue;
	CStrVAR*m_pString;

public :
	STBVALUE ();
	~STBVALUE ();

	void	SetTYPE (bool bString)	{	m_bString=bString;		}
	bool	IsString ()				{	return m_bString;		}

	void	SetVALUE (char *szValue);
	void	SetVALUE (int iValue);

	int		GetSHORT ()		{	return GetINT();			}
	int		GetINT ()		{	return m_iValue;			}

	char   *GetSTR ()		{	return (m_bString)?m_pString->Get():NULL;		}
#ifdef	__SERVER
	int		GetStrLEN()		{	return IsString()?m_pString->BuffLength():0;	}
#endif
} ;	


class STBDATA {
private:
	classHASH< short >	m_KeyTable;
	bool	m_bLoad2;

public :
	short	m_nDataCnt;
	short	m_nColCnt;

	char  **m_ppNAME;
	char  **m_ppDESC;
	union {
		int		**m_ppDATA;
		STBVALUE**m_ppVALUE;
	} ;
	
	STBDATA ()				{	m_bLoad2=false, m_ppDATA=NULL,m_nColCnt=m_nDataCnt=0;	}
	virtual ~STBDATA ()		{	this->Free();					}

	bool	Load2 (char *szFileName, bool bHasTYPE, bool bMakeKEY);

	bool	Load (char *szFileName, bool bHasNameCol=false, bool bHasDescCol=false, bool bMakeKEY=false);
	void	Free (void);

	//	xls2stb.exe로 변경된 유니코드stb파일 읽기..
	bool	LoadWSTB (char *szFileName, int iKeyColIDX, ...);
	bool	LoadWSTB (bool bCheckQuotationMark, char *szFileName, int iKeyColIDX, ...);
	char   *GetValueSTR(short nCol, short nRow)
	{
		if ( nCol < 0 || nRow < 0 )
			return NULL;
		if ( nCol >= m_nColCnt || nRow >= m_nDataCnt )
			return NULL;
		return m_ppVALUE[ nRow ][ nCol ].GetSTR();		
	}

	short	GetRowIndex (t_HASHKEY HashKey)		
	{	
		tagHASH< short > *pNode;
		pNode = m_KeyTable.Search( HashKey );
		return ( pNode ) ? pNode->m_DATA : 0;
	}
	short	GetRowIndex (char *szName)			{	return GetRowIndex( StrToHashKey( szName ) );	}
} ;

//-------------------------------------------------------------------------------------------------
// 장착 아이템 STB 21,22Column추가( 장착 제한 조건 추가 ): 2004/1/12 - nAvy

// 모든 중분류 아이템의 공통
#ifdef __SERVER
	#define	ITEM_NAME(T,I)					g_pTblSTBs[T]->m_ppNAME[ I ]			// 아이템 이름
	#define ITEM_DESC(T,I)					g_pTblSTBs[T]->m_ppDESC[ I ]			// 아이템 설명
#else
	#define	ITEM_NAME(T,I)					CStringManager::GetSingleton().GetItemName( T, I ) 
	#define ITEM_DESC(T,I)					CStringManager::GetSingleton().GetItemDesc( T, I )// 아이템 설명
#endif

///사용제한 - 0: 모두 사용가능, 1: 상점 판매만 불가, 2: 버리기,교환만 불가, 3:상점판매,버리기,교환 불가
#define	ITEM_ANNOUNCE_TYPE(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][  2 ]
#define ITEM_USE_RESTRICTION(T,I)		g_pTblSTBs[T]->m_ppDATA[ I ][  3 ]

#define ITEM_NO_RESTRICTION				0x00
#define ITEM_DONT_SELL					0x01
#define ITEM_DONT_DROP_EXCHANGE			0x02
#define ITEM_DONT_SELL_DROP_EXCHANGE	0x03
#define	ITEM_ENABLE_KEEPING				0x04


#define	ITEM_TYPE(T,I)					g_pTblSTBs[T]->m_ppDATA[ I ][  4 ]	// 아이템 종류 : 중분류?
#define	ITEM_BASE_PRICE(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][  5 ]	// 기준가격
#define	ITEM_PRICE_RATE(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][  6 ]	// 가격 변동률
#define	ITEM_WEIGHT(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][  7 ]	// 무게
#define	ITEM_QUALITY(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][  8 ]	// 품질
#define	ITEM_ICON_NO(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][  9 ]	// 아이템 번호
#define	ITEM_FIELD_MODEL(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][ 10 ]	// 필드 모델 데이타
#define	ITEM_EQUIP_SOUND(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][ 11 ]	// 장착 효과음
#define	ITEM_MAKE_NUM(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 12 ]	// 제조 번호
#define ITEM_SKILL_LEV(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 13 ]	// 스킬 레벨
#define ITEM_PRODUCT_IDX(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][ 14 ]	// 재료 번호
#define	ITEM_MAKE_DIFFICULT(T,I)		g_pTblSTBs[T]->m_ppDATA[ I ][ 15 ]	// 제조 난이도
#define ITEM_TRADE_UNIONPOINT(T,I)		ITEM_MAKE_DIFFICULT(T,I)			// 조합상점에서 아이템 구입시 필요한 조합포인트

// 소모, 기타 아이템 공통
#define	ITEM_RATE_TYPE(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 16 ]	// 생필품 종류

// 장착 가능한 아이템의 공통
/*
#define	ITEM_JOB_TYPE_CNT				3
#define	ITEM_JOB_TYPE(T,I,C)			g_pTblSTBs[T]->m_ppDATA[ I ][ 16+C ]
#define	ITEM_JOB_TYPE1(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 16 ]	// 해당 직업1
#define	ITEM_JOB_TYPE2(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 17 ]	// 해당 직업2
#define	ITEM_JOB_TYPE3(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 18 ]	// 해당 직업3
*/
#define ITEM_EQUIP_REQUIRE_CLASS(T,I)		g_pTblSTBs[T]->m_ppDATA[ I ][ 16 ]
#define ITEM_EQUIP_REQUIRE_UNION_CNT		2
#define ITEM_EQUIP_REQUIRE_UNION(T,I,C)		g_pTblSTBs[T]->m_ppDATA[ I ][ 17 + C ]


#define ITEM_NEED_DATA_CNT				2
#define	ITEM_NEED_DATA_TYPE(T,I,C)		g_pTblSTBs[T]->m_ppDATA[ I ][ 19+(C*2) ]	// 아이템 종류
#define	ITEM_NEED_DATA_VALUE(T,I,C)		g_pTblSTBs[T]->m_ppDATA[ I ][ 20+(C*2) ]	// 아이템 종류
#define	ITEM_NEED_UNION_CNT				2
#define	ITEM_NEED_UNION(T,I,C)			g_pTblSTBs[T]->m_ppDATA[ I ][ 23+(C*3) ]
#define ITEM_ADD_DATA_TYPE(T,I,C)		g_pTblSTBs[T]->m_ppDATA[ I ][ 24+(C*3) ]
#define	ITEM_ADD_DATA_VALUE(T,I,C)		g_pTblSTBs[T]->m_ppDATA[ I ][ 25+(C*3) ]
#define	ITEM_DURABITY(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 29 ]	// 아이템 내구도

#define	ITEM_RARE_TYPE(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 30 ]
#define ITEM_DEFENCE(T,I)				g_pTblSTBs[T]->m_ppDATA[ I ][ 31 ]	// 방어력
#define ITEM_RESISTENCE(T,I)			g_pTblSTBs[T]->m_ppDATA[ I ][ 32 ]	// 항마력


//-------------------------------------------------------------------------------------------------
// LIST_CAP.STB
#define HELMET_NAME(I)					ITEM_NAME( ITEM_TYPE_HELMET, I );


//	@Use ITEM_xxxx
#define HELMET_HAIR_TYPE(I)				g_TblHELMET.m_ppDATA[ I ][ 33 ]

//-------------------------------------------------------------------------------------------------
// LIST_BODY.STB
#define	ARMOR_NAME(I)					ITEM_NAME( ITEM_TYPE_ARMOR, I );
//	@Use ITEM_xxxx
#define	ARMOR_MAT_TYPE(I)				g_TblARMOR.m_ppDATA[ I ][ 33 ]		// 재질 타입

//-------------------------------------------------------------------------------------------------
// LIST_FOOT.STB
#define	BOOTS_NAME(I)				ITEM_NAME( ITEM_TYPE_BOOTS, I )
//	@Use ITEM_xxxx
#define	BOOTS_MOVE_SPEED(I)				g_TblBOOTS.m_ppDATA[ I ][ 33 ]

//-------------------------------------------------------------------------------------------------
// LIST_ARMS.STB
#define	GAUNTLET_NAME(I)			ITEM_NAME( ITEM_TYPE_GAUNTLET, I )

//	@Use ITEM_xxxx

//-------------------------------------------------------------------------------------------------
// LIST_FACEITEM.STB	얼굴아이템 
#define FACEITEM_NAME(I)			ITEM_NAME( ITEM_TYPE_FACE_ITEM, I )
#define FACEITEM_DESC(I)			ITEM_DESC( ITEM_TYPE_FACE_ITEM, I )


//-------------------------------------------------------------------------------------------------
// LIST_BACK.STB		가방아이템 
#define BACKITEM_NAME(I)				ITEM_NAME( ITEM_TYPE_KNAPSACK, I )
#define BACKITEM_DESC(I)				ITEM_DESC( ITEM_TYPE_KNAPSACK, I )	


//	@Use ITEM_xxxx
#define	BACKITEM_MOVE_SPEED(I)			g_TblBACKITEM.m_ppDATA[ I ][ 33 ]


//-------------------------------------------------------------------------------------------------
// LIST_WEAPON.STB
#define	WEAPON_NAME(I)					ITEM_NAME( ITEM_TYPE_WEAPON, I )

#define	WEAPON_TYPE(I)					g_TblWEAPON.m_ppDATA[ I ][  4 ]
//	@Use ITEM_xxxx
#define	WEAPON_GEM_COUNT(I)				g_TblWEAPON.m_ppDATA[ I ][ 5+25 ]

#define	WEAPON_ATTACK_RANGE(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+28 ]
#define	WEAPON_MOTION_TYPE(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+29 ]
#define	WEAPON_ATTACK_POWER(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+30 ]
#define	WEAPON_ATTACK_SPEED(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+31 ]
#define	WEAPON_IS_MAGIC_DAMAGE(I)		g_TblWEAPON.m_ppDATA[ I ][ 5+32 ]
#define	WEAPON_BULLET_EFFECT(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+33 ]
#define	WEAPON_DEFAULT_EFFECT(I)		g_TblWEAPON.m_ppDATA[ I ][ 5+34 ]
#define	WEAPON_ATK_START_SOUND(I)		g_TblWEAPON.m_ppDATA[ I ][ 5+35 ]
#define	WEAPON_ATK_FIRE_SOUND(I)		g_TblWEAPON.m_ppDATA[ I ][ 5+36 ]
#define	WEAPON_ATK_HIT_SOUND(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+37 ]
#define	WEAPON_GEMMING_POSITION(I)		g_TblWEAPON.m_ppDATA[ I ][ 43 ]

//-------------------------------------------------------------------------------------------------
// LIST_SUBWPN.STB
#define	SUBWPN_NAME(I)					ITEM_NAME( ITEM_TYPE_SUBWPN, I )
#define	SUBWPN_TYPE(I)					g_TblSUBWPN.m_ppDATA[ I ][  4 ]
//	@Use ITEM_xxxx
#define SUBWPN_GEM_COUNT(I)				g_TblSUBWPN.m_ppDATA[ I ][ 5+25 ]

//#define SUBWPN_ATTACK_RANGE(I)			g_TblSUBWPN.m_ppDATA[ I ][ 5+28 ]
//#define SUBWPN_MOTION_TYPE(I)			g_TblSUBWPN.m_ppDATA[ I ][ 5+29 ]
//#define SUBWPN_ATTACK_POWER(I)			g_TblSUBWPN.m_ppDATA[ I ][ 5+30 ]
//#define	SUBWPN_ATTACK_SPEED(I)			g_TblSUBWPN.m_ppDATA[ I ][ 5+31 ]
//#define	SUBWPN_IS_MAGIC_DAMAGE(I)		g_TblWEAPON.m_ppDATA[ I ][ 5+32 ]
//#define	SUBWPN_BULLET_TYPE(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+33 ]
//#define SUBWPN_LIST_POSITION(I)			g_TblWEAPON.m_ppDATA[ I ][ 5+34 ]
//#define SUBWPN_RES(I)					g_TblSUBWPN.m_ppDATA[ I ][ 5+35 ]

#define	SUBWPN_GEMMING_POSITION(I)		g_TblSUBWPN.m_ppDATA[ I ][ 34 ]

//-------------------------------------------------------------------------------------------------
// LIST_JEWEL.STB		장신구아이템 
#define	JEWEL_NAME(I)						ITEM_NAME( ITEM_TYPE_JEWEL, I)

//	@Use ITEM_xxxx

#define JEWEL_NEED_DATA_TYPE(I)				g_TblJEWELITEM.m_ppDATA[ I ][ 19 ]
#define JEWEL_NEED_DATA_VALUE(I)			g_TblJEWELITEM.m_ppDATA[ I ][ 20 ]

#define	JEWEL_NEED_UNION(I,C)				g_TblJEWELITEM.m_ppDATA[ I ][ 23+(C*3) ]
#define JEWEL_ADD_DATA_TYPE(I,C)			g_TblJEWELITEM.m_ppDATA[ I ][ 24+(C*3) ]
#define	JEWEL_ADD_DATA_VALUE(I,C)			g_TblJEWELITEM.m_ppDATA[ I ][ 25+(C*3) ]
///이상 장착 아이템
//-------------------------------------------------------------------------------------------------
// LIST_USEITEM.STB		소모아이템
#define USEITEM_NAME(I)						ITEM_NAME(ITEM_TYPE_USE, I)
#define USEITEM_DESC(I)						ITEM_DESC(ITEM_TYPE_USE, I)

#define	USEITEM_STORE_SKIN(I)				g_TblUSEITEM.m_ppDATA[ I ][  8 ]	// 개인상점 이미지 번호...

#define USEITEM_CONFILE_IDX(I)				g_TblUSEITEM.m_ppDATA[ I ][ 22 ]
//	@Use ITEM_xxxx
#define	USEITEM_NEED_DATA_TYPE(I)			g_TblUSEITEM.m_ppDATA[ I ][ 17 ]
#define	USEITEM_NEED_DATA_VALUE(I)			g_TblUSEITEM.m_ppDATA[ I ][ 18 ]
#define	USEITEM_ADD_DATA_TYPE(I)			g_TblUSEITEM.m_ppDATA[ I ][ 19 ]	// 아이템 종류
#define	USEITEM_ADD_DATA_VALUE(I)			g_TblUSEITEM.m_ppDATA[ I ][ 20 ]	// 아이템 종류

#define	USEITEM_SCROLL_LEARN_SKILL			USEITEM_ADD_DATA_VALUE				// 스킬 배우는 스크롤..
#define	USEITEM_SCROLL_USE_SKILL			USEITEM_ADD_DATA_VALUE				// 스킬 사용하는 스크롤.

#define	USEITEM_SCRIPT(I)					g_TblUSEITEM.m_ppDATA[ I ][ 21 ]
#define	USEITEM_USE_EFFECT(I)				g_TblUSEITEM.m_ppDATA[ I ][ 22 ]	// 사용 효과음
#define	USEITEM_USE_SOUND(I)				g_TblUSEITEM.m_ppDATA[ I ][ 23 ]	// 사용 효과음
#define	USEITME_STATUS_STB(I)				g_TblUSEITEM.m_ppDATA[ I ][ 24 ]	// 지속형 상태

#define USEITME_DELAYTIME_TYPE(I)			g_TblUSEITEM.m_ppDATA[ I ][ 25 ]	// 쿨타임타입
#define USEITME_DELAYTIME_TICK(I)			g_TblUSEITEM.m_ppDATA[ I ][ 26 ]	// 쿨타임틱(초단위)

#define	MAX_USEITEM_COOLTIME_TYPE			4	// 0~3





//-------------------------------------------------------------------------------------------------
// LIST_JEMITEM.STB		보석아이템
#define GEMITEM_NAME(I)						ITEM_NAME( ITEM_TYPE_GEM, I )//이름
#define GEMITEM_DESC(I)						ITEM_DESC( ITEM_TYPE_GEM, I )//설명 

//	@Use ITEM_xxxx
#define	GEMITEM_BASE_PRICE(I)				g_TblGEMITEM.m_ppDATA[ I ][  5 ]	// 기준가격
#define	GEMITEM_PRICE_RATE(I)				g_TblGEMITEM.m_ppDATA[ I ][  6 ]	// 가격 변동률

#define GEMITEM_ADD_DATA_TYPE(I,C)			g_TblGEMITEM.m_ppDATA[ I ][ 16+(C*2) ]
#define	GEMITEM_ADD_DATA_VALUE(I,C)			g_TblGEMITEM.m_ppDATA[ I ][ 17+(C*2) ]

#define GEMITEM_MARK_IMAGE(I)				g_TblGEMITEM.m_ppDATA[ I ][ 20 ]	//마크이미지 
#define GEMITEM_ATTACK_EFFECT(I)			g_TblGEMITEM.m_ppDATA[ I ][ 21 ]	//아이텝에 붙을 이미지




//-------------------------------------------------------------------------------------------------
// LIST_NATURAL.STB		원재료 
#define NATURAL_NAME(I)				ITEM_NAME( ITEM_TYPE_NATURAL, I )
#define NATURAL_DESC(I)				ITEM_DESC( ITEM_TYPE_NATURAL, I )
//	@Use ITEM_xxxx
#define NATURAL_BULLET_NO(I)		g_TblNATUAL.m_ppDATA[ I ][ 17 ]///수정 2004 / 2 /18 :nAvy g_TblNPC => g_TblNATUAL



//#ifdef __SERVER
//	#define	NPC_NAME_STR_COLUMN			0
//
//	#define	NPC_NAME(I)					g_TblNPC.m_ppVALUE[ I ][ NPC_NAME_STR_COLUMN ].GetSTR()
//	#define	NPC_DEAD_EVENT(I)			g_TblNPC.m_ppVALUE[ I ][ 41 ].GetSTR()
//
//	#define	SET_NPC_LEVEL(I,V)			g_TblNPC.m_ppVALUE[ I ][  7 ].SetVALUE(V)
//	#define SET_NPC_ATK_SPEED(I,V)		g_TblNPC.m_ppVALUE[ I ][ 14 ].SetVALUE(V)
//	#define SET_NPC_DROP_TYPE(I,V)		g_TblNPC.m_ppVALUE[ I ][ 18 ].SetVALUE(V)
//	#define	SET_NPC_DEAD_EVENT(I,V)		g_TblNPC.m_ppVALUE[ I ][ 41 ].SetVALUE(V)
//#else
/// use load2 function
	#define	NPC_NAME(I)					CStringManager::GetSingleton().GetNpcName( I )	// NPC 이름
	#define NPC_DESC(I)					g_TblNPC.m_ppVALUE[ I ][ 41 ].GetSTR()			/// 죽을때 이벤트
	#define NPC_HEIGHT(I)				g_TblNPC.m_ppVALUE[ I ][ 42 ].GetINT()			/// NPC키
//#endif
#define	NPC_WALK_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 2 ].GetINT()
#define NPC_RUN_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 3 ].GetINT()
#define	NPC_SCALE(I)				g_TblNPC.m_ppVALUE[ I ][ 4 ].GetINT()
#define	NPC_R_WEAPON(I)				g_TblNPC.m_ppVALUE[ I ][ 5 ].GetINT()
#define NPC_L_WEAPON(I)				g_TblNPC.m_ppVALUE[ I ][ 6 ].GetINT()
#define	NPC_LEVEL(I)				g_TblNPC.m_ppVALUE[ I ][ 7 ].GetINT()
#define NPC_HP(I)					g_TblNPC.m_ppVALUE[ I ][ 8 ].GetINT()
#define NPC_ATK(I)					g_TblNPC.m_ppVALUE[ I ][ 9 ].GetINT()
#define NPC_HIT(I)					g_TblNPC.m_ppVALUE[ I ][ 10].GetINT()
#define NPC_DEF(I)					g_TblNPC.m_ppVALUE[ I ][ 11 ].GetINT()
#define NPC_RES(I)					g_TblNPC.m_ppVALUE[ I ][ 12 ].GetINT()
#define NPC_AVOID(I)				g_TblNPC.m_ppVALUE[ I ][ 13 ].GetINT()
#define NPC_ATK_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 14 ].GetINT()
#define NPC_IS_MAGIC_DAMAGE(I)		g_TblNPC.m_ppVALUE[ I ][ 15 ].GetINT()
#define NPC_AI_TYPE(I)				g_TblNPC.m_ppVALUE[ I ][ 16 ].GetINT()
#define NPC_GIVE_EXP(I)				g_TblNPC.m_ppVALUE[ I ][ 17 ].GetINT()
#define NPC_DROP_TYPE(I)			g_TblNPC.m_ppVALUE[ I ][ 18 ].GetINT()
#define NPC_MARK_NO(I)				NPC_DROP_TYPE(I)			///미니맵에 표시되는 NPC IMAGE NO

#define NPC_DROP_MONEY(I)			g_TblNPC.m_ppVALUE[ I ][ 19 ].GetINT()
#define NPC_DROP_ITEM(I)			g_TblNPC.m_ppVALUE[ I ][ 20 ].GetINT()

#define NPC_UNION_NO(I)				NPC_DROP_ITEM(I)			///조합상점에서 해당 조합의 번호

#define	NPC_NEED_SUMMON_CNT(I)		g_TblNPC.m_ppVALUE[ I ][ 21 ].GetINT()		// 소환시 필요한 소환능력치
#define NPC_SELL_TAB(I,T)			g_TblNPC.m_ppVALUE[ I ][ 21+T ].GetINT()
#define NPC_SELL_TAB0(I)			g_TblNPC.m_ppVALUE[ I ][ 21 ].GetINT()
#define NPC_SELL_TAB1(I)			g_TblNPC.m_ppVALUE[ I ][ 22 ].GetINT()
#define NPC_SELL_TAB2(I)			g_TblNPC.m_ppVALUE[ I ][ 23 ].GetINT()
#define NPC_SELL_TAB3(I)			g_TblNPC.m_ppVALUE[ I ][ 24 ].GetINT()
#define NPC_CAN_TARGET(I)			g_TblNPC.m_ppVALUE[ I ][ 25 ].GetINT()
#define	NPC_ATK_RANGE(I)			g_TblNPC.m_ppVALUE[ I ][ 26 ].GetINT()
#define	NPC_TYPE(I)					g_TblNPC.m_ppVALUE[ I ][ 27 ].GetINT()
#define	NPC_HIT_MATERIAL_TYPE(I)	g_TblNPC.m_ppVALUE[ I ][ 28 ].GetINT()

/// NPC일경우( NPC_TYPE==999 )일경우에는 대화시 얼굴 이미지 인덱스이고 소환몹일경우에는 HP 계산식구분에 사용된다.
/// 2005/8/26 최종진
#define	NPC_FACE_ICON(I)			g_TblNPC.m_ppVALUE[ I ][ 29 ].GetINT()
#define	NPC_SUMMONMOB_TYPE(I)		g_TblNPC.m_ppVALUE[ I ][ 29 ].GetINT()

#define	NPC_NORMAL_EFFECT_SOUND(I)	g_TblNPC.m_ppVALUE[ I ][ 30 ].GetINT()
#define	NPC_ATTACK_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 31 ].GetINT()		// 공격 시작 사운드
#define	NPC_HITTED_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 32 ].GetINT()		// 맞았을때 내는 소리
#define	NPC_HAND_HIT_EFFECT(I)		g_TblNPC.m_ppVALUE[ I ][ 33 ].GetINT()		//	맨손 타격효과

#define	NPC_DEAD_EFFECT(I)			g_TblNPC.m_ppVALUE[ I ][ 34 ].GetINT()
#define NPC_DIE_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 35 ].GetINT()		//	죽을때 효과음

#define NPC_QUEST_TYPE(I)			g_TblNPC.m_ppVALUE[ I ][ 38 ].GetINT()		//	퀘스트 타입
#define NPC_GLOW_COLOR(I)			g_TblNPC.m_ppVALUE[ I ][ 39 ].GetINT()		//	퀘스트 타입
#define NPC_GLOW_COLOR(I)			g_TblNPC.m_ppVALUE[ I ][ 39 ].GetINT()		//	퀘스트 타입

#define	NPC_STRING_ID_COLOUM		40
#define NPC_STRING_ID(I)			g_TblNPC.m_ppVALUE[ I ][ NPC_STRING_ID_COLOUM ].GetSTR()		//	스트링 아이디


#define	NPC_CREATE_EFFECT(I)		g_TblNPC.m_ppVALUE[ I ][ 44 ].GetINT()		// 생성시 효과.
#define NPC_CREATE_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 45 ].GetINT()		// 생성시 효과음.


//////////////////////////////////////////////////////////////////////////



//-------------------------------------------------------------------------------------------------

#define	FILE_MOTION(WEAPON,ACTION)	g_TblAniTYPE.m_ppDATA[ ACTION ][ WEAPON ]

//-------------------------------------------------------------------------------------------------
#define EFFECT_NAME(I)				(NULL)		// g_TblEFFECT.m_ppDATA[ I ][ 0 ]
#define	EFFECT_TYPE(I)				g_TblEFFECT.m_ppDATA[ I ][ 1 ]
#define	EFFECT_POINT_CNT			4
#define	EFFECT_POINT(I,PNO)			g_TblEFFECT.m_ppDATA[ I ][ 2+PNO ]
#define	EFFECT_POINT1(I)			g_TblEFFECT.m_ppDATA[ I ][ 2 ]
#define	EFFECT_POINT2(I)			g_TblEFFECT.m_ppDATA[ I ][ 3 ]
#define	EFFECT_POINT3(I)			g_TblEFFECT.m_ppDATA[ I ][ 4 ]
#define	EFFECT_POINT4(I)			g_TblEFFECT.m_ppDATA[ I ][ 5 ]
#define EFFECT_TRAIL_NORMAL(I)		g_TblEFFECT.m_ppDATA[ I ][ 6 ]
#define EFFECT_TRAIL_CRITICAL(I)	g_TblEFFECT.m_ppDATA[ I ][ 7 ]
#define EFFECT_TRAIL_DURATION(I)	g_TblEFFECT.m_ppDATA[ I ][ 8 ]
#define EFFECT_HITTED_NORMAL(I)		g_TblEFFECT.m_ppDATA[ I ][ 9 ]
#define EFFECT_HITTED_CRITICAL(I)	g_TblEFFECT.m_ppDATA[ I ][ 10 ]
#define EFFECT_BULLET_NORMAL(I)		g_TblEFFECT.m_ppDATA[ I ][ 11 ]
#define EFFECT_BULLET_CRITICAL(I)	g_TblEFFECT.m_ppDATA[ I ][ 12 ]
#define	EFFECT_BULLET_MOVETYPE(I)	g_TblEFFECT.m_ppDATA[ I ][ 13 ]
#define	EFFECT_BULLET_TYPE(I)		g_TblEFFECT.m_ppDATA[ I ][ 14 ]
#define EFFECT_BULLET_SPEED(I)		g_TblEFFECT.m_ppDATA[ I ][ 15 ]
#define	EFFECT_FIRE_SND_IDX(I)		g_TblEFFECT.m_ppDATA[ I ][ 16 ]
#define	EFFFCT_HIT_SND_IDX(I)		g_TblEFFECT.m_ppDATA[ I ][ 17 ]

//-------------------------------------------------------------------------------------------------

#define	DROPITEM_ITEMNO(I,NO)		g_TblDropITEM.m_ppDATA[ I ][ 1+NO ]	// NO >= 1
#define	DROPITEM_ITEM1(I)			g_TblDropITEM.m_ppDATA[ I ][ 1 ]

/*
	Hit sound
*/
#define	HIT_SOUND(T, I)				g_TblHitSound.m_ppDATA[ T ][ I ]

//-------------------------------------------------------------------------------------------------
// LIST_PRODUCT.STB			: 아이템 제조 재료
#define	PRODUCT_TYPE(I)					g_TblPRODUCT.m_ppDATA[ I ][ 0 ]
#define	PRODUCT_RAW_MATERIAL(I)			g_TblPRODUCT.m_ppDATA[ I ][ 1 ]			// 원료 종류 번호

#define	PRODUCT_NEED_ITEM_NO(P,C)		g_TblPRODUCT.m_ppDATA[ P ][ 2+(C)*2 ]	// 재료 아이템 번호
#define	PRODUCT_NEED_ITEM_CNT(P,C)		g_TblPRODUCT.m_ppDATA[ P ][ 3+(C)*2 ]	// 팰요 갯수

/*
	..
	..
	..
*/
#define JEWEL_ADD_JOB
#define	JEWEL_ICON_NO(I)					g_TblJEWELITEM.m_ppDATA[ I ][ 9 ]
#define	JEWEL_QUALITY(I)					g_TblJEWELITEM.m_ppDATA[ I ][ 8 ]



// 퀘스트아이템 

// 상점 
#ifdef __SERVER
	#define STORE_NAME(I)					g_TblStore.m_ppNAME[ I ]
	#define STORE_TAB_ICON(I)				g_TblStore.m_ppDATA[ I ][ 1 ]
	#define STORE_ITEM(I,T)					g_TblStore.m_ppDATA[ I ][ 2+T ]
#else
	#define STORE_NAME(I)					CStringManager::GetSingleton().GetStoreTabName( I )
	#define STORE_ITEM(I,T)					g_TblStore.m_ppVALUE[ I ][ 2+T ].GetINT()
#endif

// 워프
#define	TELEPORT_NAME(I)					g_TblWARP.m_ppVALUE[ I ][ 0 ].GetSTR()
#define	TELEPORT_ZONE(I)					g_TblWARP.m_ppVALUE[ I ][ 1 ].GetINT()
#define TELEPORT_EVENT_POS(I)				g_TblWARP.m_ppVALUE[ I ][ 2 ].GetSTR()

// EVENT
#define	EVENT_NAME(I)						g_TblEVENT.m_ppVALUE[ I ][ 0 ].GetSTR()
#define EVENT_TYPE(I)						g_TblEVENT.m_ppVALUE[ I ][ 1 ].GetSTR()
#define	EVENT_DESC(I)						g_TblEVENT.m_ppVALUE[ I ][ 2 ].GetSTR()
#define EVENT_FILENAME(I)					g_TblEVENT.m_ppVALUE[ I ][ 3 ].GetSTR()


#define	SKY_MESH(S)							g_TblSKY.m_ppVALUE[ S ][ 0 ].GetSTR()
#define SKY_TEXTURE_CNT						4
#define	SKY_TEXTURE(S,T)					g_TblSKY.m_ppVALUE[ S ][ 1 + T ].GetSTR()
#define SKY_LIGHTMAP_BLEND(I)				g_TblSKY.m_ppVALUE[ I ][ 5 ].GetINT()
#define SKY_AMBIENT_CNT						4
#define SKY_AMBIENT( S, T )					g_TblSKY.m_ppVALUE[ S ][ 6 + T ].GetINT()
#define SKY_CHAR_LIGHT_CNT					4
#define SKY_CHAR_AMBIENT( S, T )			g_TblSKY.m_ppVALUE[ S ][ 10 + (2*T) ].GetINT()
#define SKY_CHAR_DIFFUSE( S, T )			g_TblSKY.m_ppVALUE[ S ][ 10 + (2*T) + 1 ].GetINT()

#define SKY_CHANGE_DELAY_CNT				4
#define SKY_CHANGE_DELAY(I,T)				g_TblSKY.m_ppDATA[ I ][ 18 + T ]



// ZONE LIST
#ifdef __SERVER
#define	ZONE_NAME(I)						g_TblZONE.m_ppVALUE[ I ][ 0 ].GetSTR()
#define ZONE_DESC(I)
#else
#define	ZONE_NAME(I)						CStringManager::GetSingleton().GetZoneName( I )	// Zone 이름
#define ZONE_DESC(I)						CStringManager::GetSingleton().GetZoneDesc( I )	// Zone 설명
#endif

#define	ZONE_FILE(I)						g_TblZONE.m_ppVALUE[ I ][ 1 ].GetSTR()
#define	ZONE_START_POS(I)					g_TblZONE.m_ppVALUE[ I ][ 2 ].GetSTR()
#define	ZONE_REVIVE_POS(I)					g_TblZONE.m_ppVALUE[ I ][ 3 ].GetSTR()

#define	ZONE_IS_UNDERGROUND(I)				g_TblZONE.m_ppVALUE[ I ][ 4 ].GetINT()

#define	ZONE_BG_MUSIC_DAY(I)				g_TblZONE.m_ppVALUE[ I ][ 5 ].GetSTR()
#define	ZONE_BG_MUSIC_NIGHT(I)				g_TblZONE.m_ppVALUE[ I ][ 6 ].GetSTR()

#define	ZONE_BG_IMAGE(I)					g_TblZONE.m_ppVALUE[ I ][ 7 ].GetINT()
#define ZONE_MINIMAP_NAME(I)				g_TblZONE.m_ppVALUE[ I ][ 8 ].GetSTR()
#define ZONE_MINIMAP_STARTX(I)				g_TblZONE.m_ppVALUE[ I ][ 9 ].GetINT()
#define ZONE_MINIMAP_STARTY(I)				g_TblZONE.m_ppVALUE[ I ][ 10 ].GetINT()
#define ZONE_OBJECT_TABLE(I)				g_TblZONE.m_ppVALUE[ I ][ 11 ].GetSTR()
#define ZONE_CNST_TABLE(I)					g_TblZONE.m_ppVALUE[ I ][ 12 ].GetSTR()
#define ZONE_DAY_CYCLE(I)					g_TblZONE.m_ppVALUE[ I ][ 13 ].GetINT()
#define ZONE_MORNING_TIME(I)				g_TblZONE.m_ppVALUE[ I ][ 14 ].GetINT()
#define ZONE_DAY_TIME(I)					g_TblZONE.m_ppVALUE[ I ][ 15 ].GetINT()
#define ZONE_EVENING_TIME(I)				g_TblZONE.m_ppVALUE[ I ][ 16 ].GetINT()
#define ZONE_NIGHT_TIME(I)					g_TblZONE.m_ppVALUE[ I ][ 17 ].GetINT()
#define ZONE_PVP_STATE(I)					g_TblZONE.m_ppVALUE[ I ][ 18 ].GetINT()
#define ZONE_PLANET_NO(I)					g_TblZONE.m_ppVALUE[ I ][ 19 ].GetINT()
#define ZONE_TYPE(I)						g_TblZONE.m_ppVALUE[ I ][ 20 ].GetINT()
#define ZONE_CAMERA_TYPE(I)					g_TblZONE.m_ppVALUE[ I ][ 21 ].GetINT()			/// 카메라에서의 시작위치.
#define	ZONE_JOIN_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 22 ].GetSTR()			/// 존에 조인시 실행할 트리거
#define	ZONE_KILL_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 23 ].GetSTR()			/// PK 모드 on시 사용자를 죽였을때 실행될 트리거
#define	ZONE_DEAD_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 24 ].GetSTR()			/// PK 모드 on시 죽었을때 실행될 트리거
#define	ZONE_SECTOR_SIZE(I)					g_TblZONE.m_ppVALUE[ I ][ 25 ].GetINT()			/// 서버에서 사용하는 값
#define ZONE_STRING_ID(I)					g_TblZONE.m_ppVALUE[ I ][ 26 ].GetSTR()			/// 스트링 아이디
#define ZONE_WEATHER_TYPE(I)				g_TblZONE.m_ppVALUE[ I ][ 27 ].GetINT()			/// 날씨타입

#define	ZONE_PARTY_EXP_A(I)					g_TblZONE.m_ppVALUE[ I ][ 28 ].GetINT()
#define	ZONE_PARTY_EXP_B(I)					g_TblZONE.m_ppVALUE[ I ][ 29 ].GetINT()
//
//#define	ZONE_OCEAN_TEXTURE_DIRECTORY(I)		g_TblZONE.m_ppVALUE[ I ][ 30 ].GetINT()
//#define	ZONE_OCEAN_FRAME(I)					g_TblZONE.m_ppVALUE[ I ][ 31 ].GetINT()


#define	ZONE_RIDING_REFUSE_FLAG(I)			g_TblZONE.m_ppVALUE[ I ][ 30 ].GetINT()			// 탑승 거부 플래그
#define	ZONE_REVIVE_ZONENO(I)				g_TblZONE.m_ppVALUE[ I ][ 31 ].GetINT()			// 사망시 부활존 번호
#define	ZONE_REVIVE_X_POS(I)				g_TblZONE.m_ppVALUE[ I ][ 32 ].GetINT()			// 사망시 부활존 좌표
#define	ZONE_REVIVE_Y_POS(I)				g_TblZONE.m_ppVALUE[ I ][ 33 ].GetINT()			// 사망시 부활존 좌표



// INIT AVATAR
#define	AVATAR_STR(I)						g_TblAVATAR.m_ppDATA[ I ][ 0 ]
#define	AVATAR_DEX(I)						g_TblAVATAR.m_ppDATA[ I ][ 1 ]
#define	AVATAR_INT(I)						g_TblAVATAR.m_ppDATA[ I ][ 2 ]
#define	AVATAR_CON(I)						g_TblAVATAR.m_ppDATA[ I ][ 3 ]
#define	AVATAR_CHARM(I)						g_TblAVATAR.m_ppDATA[ I ][ 4 ]
#define	AVATAR_SENSE(I)						g_TblAVATAR.m_ppDATA[ I ][ 5 ]

#define AVATAR_FACEITEM(I)					g_TblAVATAR.m_ppDATA[ I ][ 6 ]
#define AVATAR_HELMET(I)					g_TblAVATAR.m_ppDATA[ I ][ 7 ]
#define AVATAR_ARMOR(I)						g_TblAVATAR.m_ppDATA[ I ][ 8 ]
#define AVATAR_GAUNTLET(I)					g_TblAVATAR.m_ppDATA[ I ][ 9 ]
#define AVATAR_BOOTS(I)						g_TblAVATAR.m_ppDATA[ I ][ 10 ]
#define AVATAR_BACKITEM(I)					g_TblAVATAR.m_ppDATA[ I ][ 11 ]
#define AVATAR_WEAPON(I)					g_TblAVATAR.m_ppDATA[ I ][ 12 ]
#define AVATAR_SUBWPN(I)					g_TblAVATAR.m_ppDATA[ I ][ 13 ]

#define AVATAR_ITEM_WEAPON(I,J)				g_TblAVATAR.m_ppDATA[ I ][ 14+J ]
#define AVATAR_ITEM_USE(I,J)				g_TblAVATAR.m_ppDATA[ I ][ 24+(J)*2 ]
#define AVATAR_ITEM_USECNT(I,J)				g_TblAVATAR.m_ppDATA[ I ][ 25+(J)*2 ]
#define AVATAR_ITEM_ETC(I,J)				g_TblAVATAR.m_ppDATA[ I ][ 34+(J)*2 ]
#define AVATAR_ITEM_ETCCNT(I,J)				g_TblAVATAR.m_ppDATA[ I ][ 35+(J)*2 ]

#define	AVATAR_MONEY(I)						g_TblAVATAR.m_ppDATA[ I ][ 44 ]
#define	AVATAR_ZONE(I)						g_TblAVATAR.m_ppDATA[ I ][ 45 ]

/// Option - 해상도
#define RESOLUTION_DEFAULT					0
#define RESOLUTION_NAME(I)					g_TblResolution.m_ppVALUE[ I ][ 0 ].GetSTR();
#define RESOLUTION_WIDTH(I)					g_TblResolution.m_ppVALUE[ I ][ 1 ].GetINT();
#define RESOLUTION_HEIGHT(I)				g_TblResolution.m_ppVALUE[ I ][ 2 ].GetINT();
#define RESOLUTION_DEPTH(I)					g_TblResolution.m_ppVALUE[ I ][ 3 ].GetINT();

#define FILTER_FOULWORD(I)					g_TblFoulWord.m_ppVALUE[ i ][ 0 ].GetSTR();
#define FILTER_CHANGEWORD(I)				g_TblFoulWord.m_ppVALUE[ i ][ 1 ].GetSTR();



/// 캐릭터 상태변경 정보
#ifdef __SERVER
	#define STATE_NAME(I)					g_TblSTATE.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define STATE_NAME(I)					CStringManager::GetSingleton().GetStatusName( I )
#endif
//#define STATE_DESC(I)						g_TblSTATE.m_ppVALUE[ I ][ 18 ].GetSTR()

#define STATE_TYPE(I)						g_TblSTATE.m_ppVALUE[ I ][ 1 ].GetINT()		/// 유형
#define STATE_CAN_DUPLICATED(I)				g_TblSTATE.m_ppVALUE[ I ][ 2 ].GetINT()		/// 중복 여부
#define STATE_PRIFITS_LOSSES(I)				g_TblSTATE.m_ppVALUE[ I ][ 3 ].GetINT()		/// 유리 불리
#define STATE_APPLY_ARG(I)					g_TblSTATE.m_ppVALUE[ I ][ 4 ].GetINT()		/// 적용 인자

/// 적용 능력
#define STATE_APPLY_ABILITY_CNT				2
#define STATE_APPLY_ING_STB(I,T)			g_TblSTATE.m_ppVALUE[ I ][ 5 + (T)*2 ].GetINT()			/// 적용 능력
#define STATE_APPLY_ABILITY_VALUE(I,T)		g_TblSTATE.m_ppVALUE[ I ][ 5 + (T)*2 + 1 ].GetINT()		/// 적용 능력 수치

#define STATE_SYMBOL(I)						g_TblSTATE.m_ppVALUE[ I ][ 9  ].GetINT()	/// 상태 심볼
#define STATE_STEP_EFFECT(I)				g_TblSTATE.m_ppVALUE[ I ][ 10 ].GetINT()	/// 단계 효과
#define STATE_STEP_SOUND(I)					g_TblSTATE.m_ppVALUE[ I ][ 11 ].GetINT()	/// 단계 사운드

/// 컨터롤
#define STATE_CONTROL_CNT					3
#define STATE_CONTROL(I,T)					g_TblSTATE.m_ppVALUE[ I ][ 12 +(T) ].GetINT()	/// 컨터롤

#define STATE_SHOTDOWN_EFFECT(I)			g_TblSTATE.m_ppVALUE[ I ][ 15 ].GetINT()	/// 종료 효과
#define STATE_SHOTDOWN_SOUND(I)				g_TblSTATE.m_ppVALUE[ I ][ 16 ].GetINT()	/// 종료 사운드

/// STB 인덱스를 상태 번호로 봤을때 유리, 불리 정보
#define STATE_PRIFITS_LOSSES_BY_STATE(I)	g_TblSTATE.m_ppVALUE[ I ][ 17 ].GetINT()	/// 종료 사운드

/// 해지 설정시 출력할 메세지
#ifdef __SERVER
	#define STATE_SETTING_STRING(I)				g_TblSTATE.m_ppVALUE[ I ][ 18 ].GetSTR()	/// 설정 스트링
	#define STATE_DELETE_STRING(I)				g_TblSTATE.m_ppVALUE[ I ][ 19 ].GetSTR()	/// 종료 스트링
#else
	#define STATE_SETTING_STRING(I)				CStringManager::GetSingleton().GetStatusStartMsg( I )
	#define STATE_DELETE_STRING(I)				CStringManager::GetSingleton().GetStatusEndMsg( I )
#endif

#define STATE_STRING_ID(I)					g_TblSTATE.m_ppVALUE[ I ][ 20 ].GetSTR()	/// 종료 스트링


///
///	시야정보...
///

#define CAMERA_NAME(I)						g_TblCamera.m_ppVALUE[ I ][ 0 ].GetSTR()
#define CAMERA_ZSC_FILE(I)					g_TblCamera.m_ppVALUE[ I ][ 1 ].GetSTR()
#define CAMERA_FOV(I)						g_TblCamera.m_ppVALUE[ I ][ 2 ].GetINT()
#define CAMERA_ASPECT_RATIO(I)				g_TblCamera.m_ppVALUE[ I ][ 3 ].GetSTR()
#define CAMERA_MAX_RANGE(I)					g_TblCamera.m_ppVALUE[ I ][ 4 ].GetINT()
#define CAMERA_NEAR_PLANE(I)				g_TblCamera.m_ppVALUE[ I ][ 5 ].GetINT()
#define CAMERA_FAR_PLANE(I)					g_TblCamera.m_ppVALUE[ I ][ 6 ].GetINT()
#define CAMERA_NEAR_ALPHA_FOG(I)			g_TblCamera.m_ppVALUE[ I ][ 7 ].GetINT()
#define CAMERA_FAR_ALPHA_FOG(I)				g_TblCamera.m_ppVALUE[ I ][ 8 ].GetINT()
#define CAMERA_NEAR_FOG(I)					g_TblCamera.m_ppVALUE[ I ][ 9 ].GetINT()
#define CAMERA_FAR_FOG(I)					g_TblCamera.m_ppVALUE[ I ][ 10 ].GetINT()


///
/// Lod 관련 거리 정보
///
#define LOD_APPEAR_MIN(I)					g_TblRangeSet.m_ppDATA[ I ][ 1 ]
#define LOD_APPEAR_MAX(I)					g_TblRangeSet.m_ppDATA[ I ][ 2 ]


///
/// 조합...( LIST_UNION.STB )
///
#ifdef __SERVER
	#define UNION_NAME(I)						g_TblUnion.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define UNION_NAME(I)						CStringManager::GetSingleton().GetUnionName( I )
#endif
#define UNION_COLOR(I)						g_TblUnion.m_ppVALUE[ I ][ 1 ].GetINT()
#define UNION_MARK(I)						g_TblUnion.m_ppVALUE[ I ][ 2 ].GetINT()
#define UNION_HOSTILE_CNT					6
#define UNION_HOSTILE(I,C)					g_TblUnion.m_ppVALUE[ I ][ 3 + C ].GetINT();
#define UNION_STRING_ID(I)					g_TblUnion.m_ppVALUE[ I ][ 11 ].GetSTR()


///
/// 직업군( LIST_CLASS.STB )
///
#ifdef __SERVER
	#define CLASS_NAME(I)						g_TblClass.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define CLASS_NAME(I)						CStringManager::GetSingleton().GetClassName(I)
#endif
#define CLASS_INCLUDE_JOB_CNT				8///해당 Class에 포함되는 직업들의 총 갯수
#define CLASS_INCLUDE_JOB(I,C)				g_TblClass.m_ppVALUE[ I ][ 1 + C ].GetINT()	
#define CLASS_STRING_ID(L)					g_TblClass.m_ppVALUE[ L ][ 11 ].GetSTR()

// *--------------------------------------------------------------------------------* //


///
/// 발자국 소리( LIST_STEPSOUND.STB )
///
#define STEPSOUND( ZONETYPE, TILENO )		g_TblStepSound.m_ppDATA[ TILENO ][ ZONETYPE ]





///
/// 장비 아이템 등급별 적용 수치...( LIST_GRADE.STB )
///
#define	ITEMGRADE_ATK(G)					g_TblItemGRADE.m_ppDATA[ G ][ 0 ]
#define	ITEMGRADE_HIT(G)					g_TblItemGRADE.m_ppDATA[ G ][ 1 ]
#define	ITEMGRADE_DEF(G)					g_TblItemGRADE.m_ppDATA[ G ][ 2 ]
#define	ITEMGRADE_RES(G)					g_TblItemGRADE.m_ppDATA[ G ][ 3 ]
#define	ITEMGRADE_AVOID(G)					g_TblItemGRADE.m_ppDATA[ G ][ 4 ]
#define	ITEMGRADE_GLOW_COLOR(G)				g_TblItemGRADE.m_ppDATA[ G ][ 5 ]








extern STBDATA	g_TblHAIR;			//머리
extern STBDATA	g_TblFACE;			//얼굴

extern STBDATA	g_TblARMOR;			//갑옷 아이템
extern STBDATA	g_TblGAUNTLET;		//장갑 아이템 
extern STBDATA	g_TblBOOTS;			//신발 아이템 
extern STBDATA	g_TblHELMET;		//헬멧 아이템 
extern STBDATA	g_TblWEAPON;		//무기 아이템
extern STBDATA	g_TblSUBWPN;		//보조무기 아이템
extern STBDATA  g_TblFACEITEM;		//얼굴 아이템 
extern STBDATA  g_TblBACKITEM;		//등에 다는 아이템 
extern STBDATA  g_TblJEWELITEM;		//장신구 아이템 
extern STBDATA  g_TblGEMITEM;		//보석 아이템 
extern STBDATA  g_TblNATUAL;		//원재료 
extern STBDATA	g_TblUSEITEM;		//소모 아이템 


extern STBDATA  g_TblEFFECT;		//효과 
extern STBDATA	g_TblNPC;			//NPC
extern STBDATA	g_TblDropITEM;		//드롭 아이템
extern STBDATA	g_TblAniTYPE;		//anitype
extern STBDATA  g_TblPRODUCT;		//생산물	

extern STBDATA  g_TblQUESTITEM;		//퀘스트 아이템 
extern STBDATA  g_TblStore;			//상점 

extern STBDATA	g_TblWARP;			// 워프 !
extern STBDATA	g_TblEVENT;			// 이벤트

extern STBDATA	g_TblSKY;

extern STBDATA	g_TblZONE;

extern STBDATA *g_pTblSTBs[ ITEM_TYPE_RIDE_PART+1 ];



extern STBDATA	g_TblString;		// String table
extern STBDATA	g_TblHitSound;		// 재질에 따른 타격시의 사운드

extern STBDATA	g_TblAVATAR;		/// 아바타 초기 설정 데이타..
extern STBDATA  g_TblResolution;	/// 옵션-해상도
extern STBDATA	g_TblFoulWord;		/// 채팅시 Filtering될 단어들의 리스트 



extern STBDATA  g_TblSTATE;			/// 캐릭터의 상태를 변경하는 정보.

extern STBDATA  g_TblCamera;		/// 시야관련 정보..
extern STBDATA  g_TblRangeSet;		/// LOD 거리 관련 정보..

extern STBDATA  g_TblUnion;
extern STBDATA	g_TblClass;
extern STBDATA  g_TblStepSound;		/// 발자국소리..

extern STBDATA	g_TblItemGRADE;		/// 장비 아이템별 등급 적용 수치

#ifndef __SERVER
	extern STBDATA      g_TblHELP; 
	extern STBDATA		g_TblBadNames;
#endif
//-------------------------------------------------------------------------------------------------
#endif