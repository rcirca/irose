#ifndef	__IO_BASIC_H
#define	__IO_BASIC_H
typedef unsigned int HNODE;

#include "Common\IO_Motion.h"
#include "IO_Skeleton.h"
#include "IO_Model.h"
#include "IO_Morpher.h"
//#include "Common/DataType.h"

//-------------------------------------------------------------------------------------------------
///
/// 기본데이터 및 존데이터를 읽는다.
///
class CBasicDATA {
private :
	HNODE		m_MatOCEAN;
public:
	HNODE		m_MatOCEAN2;

public  :
	/// Avatar model 
	CModelDATA <CCharPART>		*m_pMD_CharPARTS[ 2 ][ MAX_BODY_PART ];
	/// cart model
	CModelDATA <CCharPART>		*m_pMD_CartPARTS;

	CSKELETON		m_SKELETON;
	t_HASHKEY		m_SkelMALE;
	t_HASHKEY		m_SkelFEMALE;

	t_HASHKEY		m_SkelPET[ PET_TYPE_MAX ];

	CMorpherDATA	m_MORPHER;

//	CModelDATA<CFixedPART>	m_ModelBULLET;
	CModelDATA<CFixedPART>	m_ModelSpecial;				/// 워프, 충돌등..
	CModelDATA<CFixedPART>	m_ModelEvent;				/// 이벤트 오브젝트


	CModelDATA<CFixedPART>	m_ModelCNST;
	CModelDATA<CFixedPART>	m_ModelDECO;
	CModelDATA<CFixedPART>	m_ModelFieldITEM;
	CModelDATA<CCharPART>	m_ModelMOBnNPC;

	CBasicDATA ()			{	m_MatOCEAN=NULL;	}
	~CBasicDATA ()			{	Free3DDATA ();		}

	bool Load3DDATA ();
	void Free3DDATA ();

	/// Zone type 에 따른 로딩데이터
	bool LoadZoneData( short nZoneNO );
	void FreeZoneData();

	/// Planet type 에 따른 로딩데이터
	bool LoadPlanetData( short nZoneNO ){};
	void FreePlanetData(){};


	short GetPartModelCount (short nPartIdx, bool bIsFemale);

	CMODEL<CCharPART>*Get_CharPartMODEL(short nPartIDX, short nItemNO, bool bIsFemale=false)
	{
		return m_pMD_CharPARTS[ bIsFemale ][ nPartIDX ]->GetMODEL( nItemNO );
	}

	HNODE Get_OceanMAT ()							{	return m_MatOCEAN;	}

	HNODE Get_SKELETON		(bool bFemale)			{	return m_SKELETON.Get_DATAUseKEY( (bFemale)?m_SkelFEMALE:m_SkelMALE );	}
	HNODE Get_PetSKELETON	( int iPetType )		
	{	
		return m_SKELETON.Get_DATAUseKEY( m_SkelPET[ iPetType ] );	
	}

	HNODE KEY_GetSKELETON	(unsigned int uiKEY)	{	return m_SKELETON.Get_DATAUseKEY( uiKEY );		}
	HNODE IDX_GetSKELETON	(short nIndex)			{	return m_SKELETON.Get_DATAUseIDX( nIndex );		}

	short GetDefaultITEM(short nRow, short nCol)	{	return m_SKELETON.Get_DATA( nRow, nCol );		}
} ;
extern CBasicDATA	g_DATA;

//-------------------------------------------------------------------------------------------------
#endif
