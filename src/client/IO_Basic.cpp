
#include "stdAFX.h"
#include "util\classSTB.h"
#include "IO_Basic.h"
#include "Common\\IO_STB.h"
#include "Game.h"

CBasicDATA	g_DATA;

//-------------------------------------------------------------------------------------------------
short CBasicDATA::GetPartModelCount (short nPartIdx, bool bIsFemale)
{
	if ( nPartIdx < 0 || nPartIdx >= MAX_BODY_PART )
		return 0;

	if ( m_pMD_CharPARTS[ bIsFemale ][ nPartIdx ] ) {
		return m_pMD_CharPARTS[ bIsFemale ][ nPartIdx ]->GetModelCNT ();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
bool CBasicDATA::Load3DDATA ()
{	
	m_ModelSpecial.Load		( "3DDATA\\Special\\LIST_DECO_SPECIAL.ZSC" );
	m_ModelEvent.Load		( "3DDATA\\Special\\Event_Object.ZSC" );


	m_ModelFieldITEM.Load	( "3DDATA\\ITEM\\LIST_FieldITEM.ZSC" );
	m_ModelMOBnNPC.Load		( "3DDATA\\npc\\PART_NPC.ZSC" );

	m_MORPHER.Load			( "3DDATA\\STB\\LIST_MORPH_OBJECT.STB" );

	short nS;
	for (nS=0; nS<2; nS++)	
		for (short nI=0; nI<MAX_BODY_PART; nI++) 
		{
			if ( nS && nI > BODY_PART_BOOTS )	// 공통 장비는 0번만 로딩.
				continue;

			m_pMD_CharPARTS[ nS ][ nI ] = new CModelDATA <CCharPART>;
		}

	// 남자
	m_pMD_CharPARTS[ 0 ][ BODY_PART_FACE	]->Load( "3Ddata\\avatar\\LIST_mFACE.ZSC",	BONE_IDX_HEAD );
	m_pMD_CharPARTS[ 0 ][ BODY_PART_HAIR	]->Load( "3Ddata\\avatar\\LIST_mHAIR.ZSC",	BONE_IDX_HEAD );		// link to bone
	m_pMD_CharPARTS[ 0 ][ BODY_PART_HELMET	]->Load( "3Ddata\\avatar\\LIST_mCAP.ZSC",	-1,				DUMMY_IDX_CAP	);

	m_pMD_CharPARTS[ 0 ][ BODY_PART_ARMOR	]->Load( "3Ddata\\avatar\\LIST_mBODY.ZSC" );		// skinning
	m_pMD_CharPARTS[ 0 ][ BODY_PART_GAUNTLET]->Load( "3Ddata\\avatar\\LIST_mARMS.ZSC" );		// skinning
	m_pMD_CharPARTS[ 0 ][ BODY_PART_BOOTS	]->Load( "3Ddata\\avatar\\LIST_mFOOT.ZSC" );		// skinning

	// 여자
	m_pMD_CharPARTS[ 1 ][ BODY_PART_FACE	]->Load( "3Ddata\\avatar\\LIST_wFACE.ZSC",	BONE_IDX_HEAD );
	m_pMD_CharPARTS[ 1 ][ BODY_PART_HAIR	]->Load( "3Ddata\\avatar\\LIST_wHAIR.ZSC",	BONE_IDX_HEAD );		// link to bone
	m_pMD_CharPARTS[ 1 ][ BODY_PART_HELMET	]->Load( "3Ddata\\avatar\\LIST_wCAP.ZSC",	-1,				DUMMY_IDX_CAP	);

	m_pMD_CharPARTS[ 1 ][ BODY_PART_ARMOR	]->Load( "3Ddata\\avatar\\LIST_wBODY.ZSC" );		// skinning
	m_pMD_CharPARTS[ 1 ][ BODY_PART_GAUNTLET]->Load( "3Ddata\\avatar\\LIST_wARMS.ZSC" );		// skinning
	m_pMD_CharPARTS[ 1 ][ BODY_PART_BOOTS	]->Load( "3Ddata\\avatar\\LIST_wFOOT.ZSC" );		// skinning


	m_pMD_CharPARTS[ 0 ][ BODY_PART_FACE_ITEM ]->Load( "3Ddata\\avatar\\LIST_FACEIEM.ZSC",-1,				DUMMY_IDX_MOUSE	);
	m_pMD_CharPARTS[ 1 ][ BODY_PART_FACE_ITEM ] = m_pMD_CharPARTS[ 0 ][ BODY_PART_FACE_ITEM ];

	m_pMD_CharPARTS[ 0 ][ BODY_PART_KNAPSACK]->Load( "3Ddata\\avatar\\LIST_BACK.ZSC",	-1,				DUMMY_IDX_BACK	);
	m_pMD_CharPARTS[ 1 ][ BODY_PART_KNAPSACK] = m_pMD_CharPARTS[ 0 ][ BODY_PART_KNAPSACK	];

	// right weapon ...
	m_pMD_CharPARTS[ 0 ][ BODY_PART_WEAPON_R]->Load( "3Ddata\\weapon\\LIST_weapon.ZSC" );
	m_pMD_CharPARTS[ 1 ][ BODY_PART_WEAPON_R] = m_pMD_CharPARTS[ 0 ][ BODY_PART_WEAPON_R	];

	// left weapon ...
	m_pMD_CharPARTS[ 0 ][ BODY_PART_WEAPON_L]->Load( "3Ddata\\weapon\\LIST_subwpn.ZSC" );
	m_pMD_CharPARTS[ 1 ][ BODY_PART_WEAPON_L] = m_pMD_CharPARTS[ 0 ][ BODY_PART_WEAPON_L	];	



	///
	/// Avatar parts models
	///	
	m_pMD_CartPARTS = new CModelDATA <CCharPART>;
	
	m_pMD_CartPARTS->Load( "3DData\\Pat\\LIST_PAT.ZSC" );	



	m_SkelMALE   = m_SKELETON.Add_SKELETON ( "3DDATA\\avatar\\male.zmd" );
	m_SkelFEMALE = m_SKELETON.Add_SKELETON ( "3DDATA\\avatar\\female.zmd" );

	///
	///	for pet bones..
	///
    m_SkelPET[ PET_TYPE_CART01 ] = m_SKELETON.Add_SKELETON ( "3DDATA\\Pat\\Cart\\Cart01.zmd" );
	m_SkelPET[ PET_TYPE_CASTLE_GEAR01 ] = m_SKELETON.Add_SKELETON ( "3DDATA\\Pat\\CastleGear\\CastleGear02\\CastleGear02.ZMD" );

#ifdef FRAROSE
	for(int i = 0; i < 20; i++)
		m_SkelPET[ PET_TYPE_MOUNT01 + i ] = m_SKELETON.Add_SKELETON ( CStr::Printf("3DDATA\\Pat\\Mount\\Mount%02i\\Mount%02i.ZMD", i, i) );
#endif

	m_MatOCEAN = ::loadOceanMaterial( "mat_ocean", g_GameDATA.m_hShader_ocean, "3DDATA\\JUNON\\Water\\ocean01_01.dds" );
	if ( m_MatOCEAN ) {
		::setMaterialBlendType( m_MatOCEAN, 1 );	// 1 : lighten

		for (nS=2; nS<=25; nS++)
			::pushTexture( m_MatOCEAN, CStr::Printf("3DDATA\\JUNON\\Water\\ocean01_%02d.dds", nS) );
	}

	return true;
}
void CBasicDATA::Free3DDATA ()
{
	if ( m_MatOCEAN ) {
		::unloadMaterial ( m_MatOCEAN );
		m_MatOCEAN = NULL;
	}

	m_MORPHER.Free ();

	
	m_ModelFieldITEM.Free ();
	m_ModelMOBnNPC.Free ();

	m_ModelSpecial.Free();
	m_ModelEvent.Free();


	for (short nS=0; nS<2; nS++)
		for (short nI=0; nI<MAX_BODY_PART; nI++) 
		{
			if ( nS && nI > BODY_PART_BOOTS )	// 공통 장비는 0번만 로딩.
				continue;

			SAFE_DELETE ( m_pMD_CharPARTS[ nS ][ nI ] );
		}


	SAFE_DELETE ( m_pMD_CartPARTS );		

	m_SKELETON.Free ();
}


/// Zone type 에 따른 로딩데이터
bool CBasicDATA::LoadZoneData( short nZoneNO )
{
	if( m_ModelCNST.Load( ZONE_CNST_TABLE( nZoneNO ) ) == false )
		return false;

	if( m_ModelDECO.Load( ZONE_OBJECT_TABLE( nZoneNO ) ) == false )
		return false;

	return true;
}

void CBasicDATA::FreeZoneData()
{
	m_ModelDECO.Free ();
	m_ModelCNST.Free ();
}

//-------------------------------------------------------------------------------------------------
