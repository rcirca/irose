/**
 * \ingroup SHO_GS
 * \file	Calculation.h
 * \brief	전투,스킬 사용시 계산, 클라이언트, 서버 두 계산식은 항상 일치해야함
 */
#ifndef	__CALCULATION_H
#define	__CALCULATION_H
#include "CItem.h"

#ifdef	__SERVER
	class	CObjCHAR;
	typedef	CObjCHAR	CObjUSER;
#else
	class	CObjUSER;
	class	CObjCHAR;
#endif
class CObjMOB;

//-------------------------------------------------------------------------------------------------
class CUserDATA;
/**
 * \ingroup SHO_GS_LIB
 * \class	CCal
 * \author	wookSang.Jo
 * \brief	클라이언트와 공유되는 전투및 일반 계산 공식 설정 클래스
 */
class CCal {
private:
	static int  Get_SuccessRATE(CObjCHAR *pATK, CObjCHAR *pDEF );
	static WORD Get_BasicDAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, WORD wHitCNT, int iSuc);
	static WORD Get_MagicDAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, WORD wHitCNT, int iSuc);

	static int  Get_WeaponSkillDAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, short nSkillIDX, int iSuccess);
	static int  Get_MagicSkillDAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, short nSkillIDX, int iSuccess);

	static int Get_CriSuccessRATE( CObjCHAR *pATK );

public :
	static bool Get_DropITEM (int iLevelDiff, CObjMOB *pMobCHAR, tagITEM &sITEM, int iZoneNO, int iDropRate, int iCharm);
	static int	Get_EXP (CObjCHAR *pAtkCHAR, CObjCHAR *pDefCHAR, int iGiveDamage);

	static WORD Get_DAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, WORD wHitCNT);
	static WORD Get_SkillDAMAGE (CObjCHAR *pATK, CObjCHAR *pDEF, short nSkillIDX, WORD wHitCNT);
	static int	Get_StorageFEE (int iBasePrice, int iPriceRate, UINT uiDupCnt)
	{
		return (int)( ( iBasePrice * 5 / 1000 + 1 ) * ( iPriceRate+1 ) ) * uiDupCnt;
	}

	static int  Get_RewardVALUE( BYTE btEquation, int S_REWARD, CUserDATA *pUSER, short nDupCNT );

	static int  Get_SkillAdjustVALUE( CObjUSER *pCHAR, short nSkillIDX, short nCol, int iSpellerINT );

	static int	GetMoney_WhenAPPRAISAL ( int iBasePrice, int iCurDurability )
	{
		return (int)( ( iBasePrice+10000 ) * ( iCurDurability + 50 ) / 10000.f );
	}

	// 장비에서 보석 분리시 필요한 mp & money
	// param :
	//		short ORI_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	//		short GEM_QUAL = ITEM_QUALITY( ITEM_TYPE_GEM,	  pInITEM->GetGemNO() );
	static int  GetMP_WhenBreakupGEM ( short ORI_QUAL, short GEM_QUAL )
	{
		return ( (ORI_QUAL/2) + GEM_QUAL );
	}
	static int  GetMONEY_WhenBreakupGEM ( short ORI_QUAL, short GEM_QUAL )
	{
		return ( ORI_QUAL*5 + 50 );
	}

	// 장비를 재료로 분해시 필요한 mp & money
	// param : ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	static int  GetMP_WhenBreakupMAT ( short ORI_QUAL )
	{
		return ( ORI_QUAL + 30 );
	}
	static int  GetMONEY_WhenBreakupMAT ( short ORI_QUAL )
	{
		return ( ORI_QUAL*10 + 20 );
	}

	// 장비의 등급을 올릴때 필요한 mp & money
	// param :
	//		pInITEM->GetGrade ()
	//		ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	static int  GetMP_WhenUpgradeEQUIP ( short nCurGrade, short ITEM_QUAL )
	{
		return (int)( (nCurGrade+4) * (ITEM_QUAL+20)*0.9f );
	}
	static int  GetMONEY_WhenUpgradeEQUIP( short nCurGrade, short ITEM_QUAL )
	{
		return (int)( (nCurGrade) * (nCurGrade+1) * ITEM_QUAL * (ITEM_QUAL+20) * 0.2f );
	}

	// 현재 파티 레벨에서 업되기 위해 얻어야할 경험치양
	static int  Get_PartyLevelUpNeedEXP( int iCurPartyLEV )
	{
		return ( (iCurPartyLEV+7) * (iCurPartyLEV+10)*5 ) + 200;
	}

	static int Get_NeedRawEXP (int iLevel);
} ;

//-------------------------------------------------------------------------------------------------
#endif
 