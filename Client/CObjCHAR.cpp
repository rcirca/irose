/*
	$Header: /Client/CObjCHAR.cpp 652   05-10-20 2:46p Gioend $  
*/
#include "stdAFX.h"
#include "Game.h"
#include "OBJECT.h"
#include "BULLET.h"
#include "IO_Terrain.h"
#include "IO_Event.h"
#include "CViewMSG.h"
#include "Network\CNetwork.h"
#include "common\Calculation.h"
#include "Game_FUNC.h"
#include "Interface/it_Mgr.h"
#include "Interface/CUIMediator.h"
#include "Interface/TypeResource.h"
#include "Interface/Dlgs/ChattingDlg.h"
#include "Interface/CClanMarkUserDefined.h"
#include "GameCommon/Skill.h"
#include "Event/Quest_FUNC.h"
#include "CCamera.h"
#include "Game_FUNC.h"

#include "GameProc/ChangeVisibility.h"   
#include "CObjCART.h"
#include "CObjCHAR_Collision.h"

#include "GameCommon/Item.h"
#include "GameData/CParty.h"
#include "Common/IO_Pat.h"
#include "Misc/GameUtil.h"
#include "CommandFilter.h"
#include "../GameProc/DelayedExp.h"
#include "../Country.h"
#include "../common/CInventory.h"
#include "system/System_Func.h"


extern CCamera * g_pCamera;


extern CAI_OBJ *AI_FindFirstOBJ( CAI_OBJ *pBaseOBJ, int iDistance );
extern CAI_OBJ *AI_FindNextOBJ ();


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iDistance 거리
/// @brief  : AI 동작을 위해 첫번재 오브젝트를 찾는다..
//--------------------------------------------------------------------------------

CAI_OBJ* CObjCHAR::AI_FindFirstOBJ( int iDistance )
{
	return ::AI_FindFirstOBJ( this, iDistance );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  :AI_FindFirstOBJ 호출후 호출하여 리스트상의 다음 오브젝트를 꺼집어 냄
//--------------------------------------------------------------------------------

CAI_OBJ* CObjCHAR::AI_FindNextOBJ ()
{
	return ::AI_FindNextOBJ ();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param wSrvDIST 서버에서 계산되어 날아온 목표위치와 현재위치와의 2차원적 거리. 단위 cm
/// @param PosGOTO 서버에서 날아온 이동할 목표위치. 현재로서 Z 값은 의미없다.
/// @brief  : 이동 속도 보정. 매 프레임마다 갱신됨. 서버와 클라이언트의 속도차가 나는 경우에,
///           강제로 속도를 증가시킨다. 혹은 너무 많은 차이가 날 때에, 강제로 이동시킨다.
///           
//--------------------------------------------------------------------------------

void CObjCHAR::Adj_MoveSPEED ( WORD wSrvDIST, const D3DVECTOR& PosGOTO )
{	
	int iClientDIST; // 클라이언트에서의 현재-목표 위치 차이. 단위 CM
	float fCurSpeed, fNewSpeed, fNeedTime;

	fCurSpeed = this->Get_DefaultSPEED(); // 식에 의해 계산된 현재 기본 속도

	if ( 0 == fCurSpeed ) { // 속도가 0이라면, 현재 위치와 PosGOTO 가 동일해야 하나?
		m_fAdjustSPEED = 0;
		
		//assert(m_PosCUR.x == PosGOTO.x);
		//assert(m_PosCUR.y == PosGOTO.y);

		return;
	}

	fNeedTime = float(wSrvDIST) / fCurSpeed;

	// 클라이언트의 현재-목표 거리를 계산한다.
	iClientDIST = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)PosGOTO.x, (int)PosGOTO.y);

	assert(iClientDIST >= 0);

	if ( 0 == iClientDIST ) { // 목표 지점에 이미 도달했다면,
		// 클라이언트는 이동하지 않아도 된다.
		m_fAdjustSPEED = 0;	 
		return;
	}
	/// 서버에서 가야할 거리가 클라이언트에서 가야할 거리보다 크다면
	else if( iClientDIST <= wSrvDIST )
	{
		/// 현재 속도를 그대로 유지해도 된다.
		/// 클라이언트가 먼저가서 기다리면 되기 때문.
		m_fAdjustSPEED = fCurSpeed;
		return;
	}
	else // wSrvDIST < iClientDIST. 클라이언트에서의 거리가 서버에서의 거리보다 먼 경우, 속도 증가.
	{
		fNewSpeed = float(iClientDIST) / fNeedTime; // 제 시간안에 도달하기 위한 새로운 속도 계산

		/// 주체가 나일경우는 그냥 빨린 달린다.( 점프없이 )
		if( this->IsA( OBJ_USER ) == false )
		{
			int iDiffDistance = iClientDIST - wSrvDIST; // 서버거리와 클라이언트 거리 차 계산. 단위 : cm

			float fNeedTimeDiff = float(iDiffDistance) / fNewSpeed; // 거리 차를 극복하기 위해 걸리는 시간

			if (fNeedTimeDiff > 1.0f) 
			{ 
				// 거리차를 극복하기 위해, 1초 이상 지연된다면(너무 느리다.)
				fNewSpeed = fCurSpeed; // 강제로 그 변위만큼 이동

				D3DXVECTOR3 vDir = (D3DXVECTOR3)PosGOTO - m_PosCUR;
				float fRatio = (float)iDiffDistance / (float)iClientDIST;

				D3DXVECTOR3 vPosCur = m_PosCUR + ( vDir * fRatio );
				vPosCur.z = m_PosCUR.z;

				this->ResetCUR_POS( vPosCur );
			}
		}
	}

	m_fAdjustSPEED = fCurSpeed;
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 공격속도 조정..
//--------------------------------------------------------------------------------

void CObjCHAR::Adj_AniSPEED ( float fAniSpeed )
{
	::setAnimatableSpeed( this->m_hNodeMODEL, fAniSpeed  );		
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iServerTarget 오브젝트 서버인덱스
/// @param tPOINTF &PosFROM 
/// @param tPOINTF &PosGOTO
/// @brief  : 공격명령 설정
//--------------------------------------------------------------------------------

void CObjCHAR::SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO)
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjAttackCommand* pObjCommand = (CObjAttackCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_ATTACK );
		pObjCommand->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosGOTO );

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandAttack( iServerTarget, wSrvDIST, PosGOTO );
		return;
	}

	//--------------------------------------------------------------------------------
	/// If this char is under pet mode, pass the command to pet.
	int iPetMode = this->GetPetMode();
	if( iPetMode >= 0 )
	{	
		/// Pet mode 일 경우에는..
		if( this->CanAttackPetMode() )
		{
			SetCMD_PET_ATTACK( iServerTarget, wSrvDIST, PosGOTO );
		}
//박지호::펫모드일 경우 아바타도 공격속성을 설정 하도록 한다. 
#ifndef _GBC
		return;
#endif 
	}
	
	/// 서버에서 받은 공격 패킷 처리...
	this->Adj_MoveSPEED( wSrvDIST, PosGOTO );	

	this->m_PosGOTO = PosGOTO;

	CObjAI::SetCMD_ATTACK( iServerTarget );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : Constructor
//--------------------------------------------------------------------------------

CObjCHAR::CObjCHAR () : m_EndurancePack( this ), m_ChangeActionMode( this ), m_ObjVibration( this, 200 )
{
#ifdef	__VIRTUAL_SERVER
	m_pRegenPOINT = NULL;
	m_pRegenListNODE = NULL;
#endif

	m_fScale = 1.0f;

	m_fHeightOfGround = 0.0f;
	m_hNodeMODEL = NULL;

	m_ppBoneEFFECT = NULL;
	m_hR_TRAIL = m_hL_TRAIL = NULL;

	for (short nP=0; nP<MAX_BODY_PART; nP++) {
		m_phPartVIS[ nP ] = NULL;
		m_pppEFFECT[ nP ] = NULL;
		m_nEffectPointCNT[ nP ] = 0;
	}	

	::ZeroMemory( &m_DeadDAMAGE, sizeof( tagDAMAGE ) );
	m_lDeadTIME = 0;

	m_DamageList.reserve( 10 );

	m_bProcEffectedSkill = false;
	m_iEffectedSkillIndex = 0;
	m_EffectedSkillList.clear();

	m_fAltitude = 0.0;

	m_iHP	 = 1;
	m_iMP	 = 1;
	m_iMaxHP = 1;
	m_iMaxMP = 1;

	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;
	m_dwFrameElapsedTime		= 0;
	
	m_bDead = false;

	m_pChangeVisibility			= NULL;
	m_bStopDead					= false;


	m_bIsVisible				= true;

	m_hNodeGround				= NULL;

	m_pCollision				= new CObjCHAR_Collision;				/// will be deleted in DeletCHAR()

	memset( &m_SummonMob, 0 , sizeof( gsv_MOB_CHAR ) );
	m_bHaveSummonedMob			= false;							/// 소환해야될 몹이 있는가?

	m_bUseResetPosZ				= false;
	m_fResetPosZ				= 0;
	m_fModelSPEED				= 0;

	m_iLastCastingSkill			= 0;

	m_dwClanID					= 0;
	m_wClanMarkBack				= 0;
	m_wClanMarkCenter			= 0;

	m_bFrameING					= 0;
	m_ClanMarkUserDefined		= 0;


	m_ReviseHP					= 0;
	m_ReviseMP					= 0; 

//-------------------------------------------------------------------------------
	//조성현
	m_bDisguise					= false;


//--------------------------------------------------------------------------------
	///박지호
	//카트 변수들 초기화
	m_iPetType					= -1;
	m_pObjCART					= NULL; 
	m_pRideUser					= NULL;

	m_bUseCartSkill				= FALSE;
	m_IsRideUser				= FALSE;
	
	m_saveSpeed					= 0.0f;

	m_iPetType					= -1;
	m_iRideIDX					= 0;
	m_skCartIDX					= 0;
		
	//아로아 상태변수 초기화
	m_IsAroa					= 0;
	m_IsCartVA					= 0;
//--------------------------------------------------------------------------------
	m_AruaAddMoveSpeed			= 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : Destructor
//--------------------------------------------------------------------------------

CObjCHAR::~CObjCHAR ()
{
	// 엔진에 등록된 HNODE들 삭제.
	this->DeleteCHAR ();

	ClearExternalEffect();
	m_EndurancePack.ClearEntityPack();
	SAFE_DELETE( m_pCollision );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : 회복을 위한 타이머 리셋
//--------------------------------------------------------------------------------
void CObjCHAR::ClearTimer()
{ 
	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;
	m_dwFrameElapsedTime		= 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : 죽거나, 파괴시에 모든 내부의 리스트 정리
//--------------------------------------------------------------------------------

void CObjCHAR::ClearAllEntityList()
{
	/// proc all effect of skill
	ProcEffectedSkill();

	/// clear all damage list
	ClearAllDamage();

	/// drop field item list
	DropFieldItemFromList();

	/// clear all command
	ClearAllCommand();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : 외부에서 등록시킨 이펙트 리스트들 정리
//--------------------------------------------------------------------------------

void CObjCHAR::ClearExternalEffect()
{
	classDLLNODE< CEffect* > *pNode;
	pNode = m_ExternalEffectLIST.GetHeadNode ();
	while( pNode ) 
	{			
		/// 이펙트만 지우고 이펙트의 부모처리는 안한다. 왜냐? 내가 부모니까..
		g_pEffectLIST->Del_EFFECT( pNode->DATA, false );

		m_ExternalEffectLIST.DeleteNFree( pNode );
		pNode = m_ExternalEffectLIST.GetHeadNode();
	}

	m_ExternalEffectLIST.ClearList();

	DeleteWeatherEffect();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param CEffect* pEffect 지울 이펙트
/// @brief  : 외부 이펙트 등록
//--------------------------------------------------------------------------------

void CObjCHAR::AddExternalEffect(CEffect* pEffect)
{
	m_ExternalEffectLIST.AllocNAppend( pEffect );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect* pEffect 지울 이펙트
/// @brief  : 외부에서 등록된 이펙트 지움
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteExternalEffect( CEffect* pEffect )
{
	if( pEffect == NULL )
		return;

	classDLLNODE< CEffect* > *pNode;
	pNode = m_ExternalEffectLIST.GetHeadNode ();
	while( pNode ) 
	{		
		if ( pNode->DATA == pEffect )
		{
			m_ExternalEffectLIST.DeleteNFree( pNode );
			return;
		}
		pNode = m_ExternalEffectLIST.GetNextNode( pNode );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  D3DVECTOR &PosSCR 얻어올 스크린좌표( 아웃풋 )
/// @brief  : 현재 캐릭터 위치의 스크린 좌표를 얻어옴
//--------------------------------------------------------------------------------

void CObjCHAR::GetScreenPOS ( D3DVECTOR &PosSCR)
{	
	// 모델의 좌표에 키를 더한 위치를 이름출력 위치로 설정
	::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + m_fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  HNODE hLinkNODE 링크할 대상의 노드 핸들
/// @param  short nDummyIDX 링크할 더미 인덱스
/// @brief  : 입력받은 노드를 입력받은 더미인덱스에 해당하는 더미에 링크시킨다.
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkDummy ( HNODE hLinkNODE, short nDummyIDX )
{	
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	if ( iDummyCnt >= nDummyIDX )
		return ( 0 != ::linkDummy( m_hNodeMODEL, hLinkNODE, nDummyIDX) );

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  HNODE hLinkNODE 링크할 대상의 노드 핸들
/// @brief  : 입력받은 노드를 마지막 더미에 링크 시킴
//--------------------------------------------------------------------------------

bool CObjCHAR::Link2LastDummy(HNODE hLinkNODE)
{
/*
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	// 마지막 더미에...
	if ( iDummyCnt > 0 )
		m_iLastDummyIDX = iDummyCnt - 1;
*/
	return ( 0 != ::linkDummy( m_hNodeMODEL, hLinkNODE, m_iLastDummyIDX) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 현재 캐릭터를 씬에 넣는다. ( 모든 하위 오브젝트 포함 )
//--------------------------------------------------------------------------------

void CObjCHAR::InsertToScene (void)
{
	if ( m_bIsVisible || !m_hNodeMODEL || !m_pCharMODEL )
		return;

	m_bIsVisible = true;

	::insertToScene( m_hNodeMODEL );		// CObjCHAR::InsertToScene

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의InsertToScene제거
	//----------------------------------------------------------------------------------------------------
	//for (short nP=0; nP<MAX_BODY_PART; nP++) 
	//{
	//	CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );

	//	if ( pCharPART  ) 
	//	{
	//		short nI;

	//		if ( m_pppEFFECT[ nP ] ) 
	//		{
	//			for (nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
	//			{
	//				if ( m_pppEFFECT[ nP ][ nI ] ) 
	//				{
	//					m_pppEFFECT[ nP ][ nI ]->InsertToScene ();
	//				}
	//			}
	//		}

	//		for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
	//		{
	//			if ( m_phPartVIS[ nP ][ nI ] ) 
	//			{
	//				::insertToScene( m_phPartVIS[ nP ][ nI ] );		// CObjCHAR::InsertToScene
	//			}
	//		}
	//	}
	//}

	//
	//// 뼈대 효과.
	//if ( m_ppBoneEFFECT ) 
	//{
 	//	for (nP=0; nP<m_pCharMODEL->GetBoneEffectCNT(); nP++)
	//		m_ppBoneEFFECT[ nP ]->InsertToScene ();
	//}

	//// 검잔상 효과.
	//for (nP=0; nP<2; nP++) 
	//{
	//	if ( m_hTRAIL[ nP ] )
	//		::insertToScene( m_hTRAIL[ nP ] );
	//}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bIncludeEFFECT 이펙트가 포함되었는가?
/// @brief  : 씬에서 제거한다. ( 하위 오브젝트 포함 )
//--------------------------------------------------------------------------------

void CObjCHAR::RemoveFromScene (bool bIncludeEFFECT)
{
	if ( !m_bIsVisible )
		return;

	m_bIsVisible = false;
	::removeFromScene ( m_hNodeMODEL );								// CObjCHAR::RemoveFromScene

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의removeFromScene제거
	//----------------------------------------------------------------------------------------------------
	//for (short nP=0; nP<MAX_BODY_PART; nP++) 
	//{
	//	CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );

	//	if ( pCharPART  ) 
	//	{
	//		short nI;
	//		if ( bIncludeEFFECT && m_pppEFFECT[ nP ] ) 
	//		{
	//			for (nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
	//				if ( m_pppEFFECT[ nP ][ nI ] ) {
	//					m_pppEFFECT[ nP ][ nI ]->RemoveFromScene ();
	//				}
	//		}

	//		for (nI=0; nI<pCharPART->m_nPartCNT; nI++) {
	//			if ( m_phPartVIS[ nP ][ nI ] ) {
	//				::removeFromScene( m_phPartVIS[ nP ][ nI ] );	// CObjCHAR::RemoveFromScene
	//			}
	//		}
	//	}
	//}

	//if ( bIncludeEFFECT ) 
	//{
	//	// 뼈대 효과.
	//	if ( m_ppBoneEFFECT ) 
	//	{
	//		for (nP=0; nP<m_pCharMODEL->GetBoneEffectCNT(); nP++)
	//		{
	//			if( m_ppBoneEFFECT[ nP ] )
	//				m_ppBoneEFFECT[ nP ]->RemoveFromScene ();
	//		}
	//	}

	//	for (nP=0; nP<2; nP++) {
	//		if ( m_hTRAIL[ nP ] )
	//			::removeFromScene( m_hTRAIL[ nP ] );
	//	}
	//}	
}

/*override*/D3DXVECTOR3& CObjCHAR::Get_CurPOS ()
{
	m_PosCUR.z = ::getPositionZ(m_hNodeMODEL);

	return m_PosCUR;
}

/*override*/ void CObjCHAR::SetEffectedSkillFlag( bool bResult )
{ 
	m_bProcEffectedSkill = bResult; 
	///Log_String( LOG_NORMAL, "SetEffectedSkillFlag[ %d ]\n", bResult );
}

/*override*/ bool CObjCHAR::bCanActionActiveSkill()
{ 
	///Log_String( LOG_NORMAL, "bCanActionActiveSkill[ %d ]\n", m_bProcEffectedSkill );
	return m_bProcEffectedSkill; 
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect *pEffect 링크할 이펙트
/// @param  short nPartIDX	 링크할 캐릭터 파트
/// @param  short nPointIDX  링크할 캐릭터 파트의 포인터 인덱스
/// @brief  : 이펙트를 캐릭터의 특정 파트에 속한 특정 포인터오브젝트에 링크한다.
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkEffectToPOINT (CEffect *pEffect, short nPartIDX, short nPointIDX)
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );
	
	if ( pCharPART && 
		( !pCharPART->m_pDummyPoints || nPointIDX >= pCharPART->m_nDummyPointCNT ) ) 
	{		
		_ASSERT( FALSE ); 

		return false;
	}

	pEffect->Transform( pCharPART->m_pDummyPoints[ nPointIDX ].m_Transform );
	pEffect->LinkNODE ( m_phPartVIS[ nPartIDX ][ pCharPART->m_pDummyPoints[ nPointIDX ].m_nParent ] );

	return true;
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect *pEffect 링크할 이펙트
/// @param  int iPointNO  링크할 더미 포인터 인덱스
/// @brief  : 이펙트를 캐릭터의 특정 더미에 링크
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkEffectToDUMMY (CEffect *pEffect, short nDummyIDX )
{
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	if ( iDummyCnt < nDummyIDX )
		return false;	
	
	pEffect->LinkDUMMY ( this->GetZMODEL(), nDummyIDX );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CMODEL<CCharPART> *pCharPART 캐릭터의 특정파트( 오른손이나, 왼손 무기가 되겠지? )
/// @param  nPartIDX 바디 파트 인덱스( 오른손 무기, 왼손무기 의 인덱스만 들어와야된다 )
/// @param  bool bLinkBONE		본에 링크 할것인가?
/// @param  int iColorNO		검잔상 칼라 번호
/// @param  int iDuration		지속시간?
/// @param  int iBaseDummyIDX	오른손, 왼손 구분을 위한 베이스 본 인덱스
/// @brief  : 양손에 무기를 들수 있으므로 두개설정 가능하다.
//--------------------------------------------------------------------------------

void CObjCHAR::LoadTRAIL (CMODEL<CCharPART> *pCharPART, short nPartIDX, bool bLinkBONE, int iColorNO, int iDuration, int iBaseDummyIDX )
{
	if ( pCharPART->m_nDummyPointCNT < 3 )
		return;

	short nTrailIDX = nPartIDX - BODY_PART_WEAPON_R;
	_ASSERT( nTrailIDX >= 0 && nTrailIDX < 2 );

	m_hTRAIL[ nTrailIDX ] = ::loadTrail ( NULL,	// ZSTRING pName, 
		 10,									// float fDistancePerPoint,
		 iDuration,								// int iDurationInMilliSecond,
		 1,										// int bUseCurve,
		 g_GameDATA.m_TrailTexture.Get(),		// ZSTRING pTexturePath, 
		 g_dwCOLOR[ iColorNO ],					// D3DCOLOR,
		 //0, 0, 0,								// float fSP_X, float fSP_Y, float fSP_Z,
		 pCharPART->m_pDummyPoints[ 0 + iBaseDummyIDX ].m_Transform,
		 //0, 0, 100 );							// float fEP_X, float fEP_Y, float fEP_Z );
		 pCharPART->m_pDummyPoints[ 1 + iBaseDummyIDX ].m_Transform );

	if ( m_hTRAIL[ nTrailIDX ] ) 
	{
		::controlTrail ( m_hTRAIL[ nTrailIDX ], 0 );	// stop !!

		//----------------------------------------------------------------------------------------------------	
		/// @brief CHILD의InsertToScene제거
		//----------------------------------------------------------------------------------------------------
		//if ( m_bIsVisible )		// 현재 보이면..
		//	::insertToScene( m_hTRAIL[ nTrailIDX ] );
	}

	if ( bLinkBONE ) 
	{
		::linkDummy ( m_hNodeMODEL, m_hTRAIL[ nTrailIDX ], nTrailIDX+DUMMY_IDX_R_HAND );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  nPartIDX 바디 파트 인덱스( 오른손 무기, 왼손무기 의 인덱스만 들어와야된다 )
/// @brief  : 검잔상 해제
//--------------------------------------------------------------------------------

void CObjCHAR::UnloadTRAIL (short nPartIDX)
{
	short nTrailIDX = nPartIDX - BODY_PART_WEAPON_R;

	_ASSERT( nTrailIDX >= 0 && nTrailIDX < 2 );

	if ( m_hTRAIL[ nTrailIDX ] ) {
		::controlTrail( m_hTRAIL[ nTrailIDX ], 0 );	// stop !!
		::unlinkNode  ( m_hTRAIL[ nTrailIDX ] );
		::unloadTrail ( m_hTRAIL[ nTrailIDX ] );
		m_hTRAIL[ nTrailIDX ] = NULL;
	}
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 캐릭터에 붙는 효과( 모델제작상 캐릭터 툴에서 제작된)를 링크
//--------------------------------------------------------------------------------

void CObjCHAR::Link_EFFECT (void)
{
	short nP;

	for (nP=0; nP<MAX_BODY_PART; nP++) 
	{
		if ( NULL == m_pppEFFECT[ nP ] )
			continue;

		CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );
		_ASSERT( pCharPART ) ;

		for (short nI=0; nI<pCharPART->m_nDummyPointCNT; nI++) 
		{
			if ( NULL != m_pppEFFECT[ nP ][ nI ] )
				m_pppEFFECT[ nP ][ nI ]->LinkNODE( m_phPartVIS[ nP ][ pCharPART->m_pDummyPoints[ nI ].m_nParent ] );
		}
	}

	for (nP=0; nP<2; nP++) 
	{
		if ( m_hTRAIL[ nP ] ) 
		{
			::linkDummy ( m_hNodeMODEL, m_hTRAIL[ nP ], nP+DUMMY_IDX_R_HAND );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 캐릭터에 붙는 효과( 모델제작상 캐릭터 툴에서 제작된)를 언 링크
//--------------------------------------------------------------------------------

void CObjCHAR::Unlink_EFFECT (void)
{
	short nP;

	for (nP=0; nP<2; nP++) {
		if ( m_hTRAIL[ nP ] ) {
			::unlinkNode ( m_hTRAIL[ nP ] );
		}
	}
	for (nP=0; nP<MAX_BODY_PART; nP++) 
	{
		if ( NULL == m_pppEFFECT[ nP ] )
			continue;

		// 아이템이 바뀌면서 이펙트가 먼저 생성된다.
		// set part model이 호출되기전에 pCharPART가 바뀐 아이템으로
		// 설정되지 않아 뻑~
		for (short nI=0; nI<m_nEffectPointCNT[ nI ]; nI++) 
		{
			if ( NULL != m_pppEFFECT[ nP ][ nI ] )
				m_pppEFFECT[ nP ][ nI ]->UnlinkNODE ();
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CMODEL<CCharPART> *pCharPART 이펙트를 추가할 캐릭터의 파트
/// @param  short nPartIDX	   파트 인덱스
/// @param  short nPointIDX	   파트의 포인터 인덱스
/// @param  t_HASHKEY HashEffectFILE  이펙트파일의 해쉬키
/// @param  bool bLinkNODE			  링크할것인가?
/// @brief  : 캐릭터에 붙는 효과( 모델제작상 캐릭터 툴에서 제작된)를 추가
///	@bug    : 04/4/28 g_pEffectLIST->Add_EFFECT(.., false ) 였는데.. g_pEffectLIST->Del_EFFECT 로 삭제를 의뢰했다..
/// @bug    : 04/4/28 링크할때 Rotation은 안하네?
//--------------------------------------------------------------------------------

void CObjCHAR::Add_EFFECT( CMODEL<CCharPART> *pCharPART, short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE, bool bLinkNODE )
{
	if ( NULL == pCharPART )
		return;

	if ( nPointIDX >= pCharPART->m_nDummyPointCNT )
		return;

	if ( m_pppEFFECT[ nPartIDX ] ) 
	{
		// 기존 포인트의 효과 삭제...
		///g_pEffectLIST->Del_EFFECT( m_pppEFFECT[ nPartIDX ][ nPointIDX ] );
		SAFE_DELETE( m_pppEFFECT[ nPartIDX ][ nPointIDX ] );
		m_pppEFFECT[ nPartIDX ][ nPointIDX ] = NULL;
	} else 
	{
		m_nEffectPointCNT[ nPartIDX ] = pCharPART->m_nDummyPointCNT;
		m_pppEFFECT[ nPartIDX ] = new CEffect*[ pCharPART->m_nDummyPointCNT ];
		for (short nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
			m_pppEFFECT[ nPartIDX ][ nI ] = NULL;
	}

	m_pppEFFECT[ nPartIDX ][ nPointIDX ] = g_pEffectLIST->Add_EFFECT( HashEffectFILE );
	if ( m_pppEFFECT[ nPartIDX ][ nPointIDX ] ) 
	{
#ifdef	_DEBUG
		if ( !pCharPART->m_pDummyPoints ||
		      nPointIDX >= pCharPART->m_nDummyPointCNT ) 
		{
			_ASSERT( FALSE );
		}
#endif

		m_pppEFFECT[ nPartIDX ][ nPointIDX ]->Transform( pCharPART->m_pDummyPoints[ nPointIDX ].m_Transform );

		if ( m_bIsVisible )
			m_pppEFFECT[ nPartIDX ][ nPointIDX ]->InsertToScene ();

		if ( bLinkNODE && m_phPartVIS[ nPartIDX ] )
			m_pppEFFECT[ nPartIDX ][ nPointIDX ]->LinkNODE( m_phPartVIS[ nPartIDX ][ pCharPART->m_pDummyPoints[ nPointIDX ].m_nParent ] );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   파트 인덱스
/// @param  short nPointIDX	   파트의 포인터 인덱스
/// @param  t_HASHKEY HashEffectFILE  이펙트파일의 해쉬키
/// @brief  : 캐릭터에 붙는 효과( 모델제작상 캐릭터 툴에서 제작된)를 추가
///	@bug    : 내부에서 Add_EFFECT( CMODEL<CCharPART> *pCharPART, short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE, bool bLinkNODE ) 함수 호출
//--------------------------------------------------------------------------------

void CObjCHAR::Add_EFFECT(short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE)
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );
	this->Add_EFFECT (pCharPART, nPartIDX, nPointIDX, HashEffectFILE);
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   파트 인덱스
/// @brief  : 캐릭터에 붙는 효과( 모델제작상 캐릭터 툴에서 제작된)를 삭제
///	@bug    : /// @Bug AddEffect( .. , false ) 버그..
//--------------------------------------------------------------------------------

void CObjCHAR::Del_EFFECT( short nPartIDX )
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );
	if ( pCharPART ) {
		if ( m_pppEFFECT[ nPartIDX ] ) 
		{
			for (short nT=0; nT<pCharPART->m_nDummyPointCNT; nT++) 
			{
				///g_pEffectLIST->Del_EFFECT( m_pppEFFECT[ nPartIDX ][ nT ] );
				if( m_pppEFFECT[ nPartIDX ][ nT ] )
				{
					delete m_pppEFFECT[ nPartIDX ][ nT ];
					m_pppEFFECT[ nPartIDX ][ nT ] = NULL;
				}
				m_pppEFFECT[ nPartIDX ][ nT ] = NULL;
			}
		}
	}

	SAFE_DELETE_ARRAY( m_pppEFFECT[ nPartIDX ] );

	switch( nPartIDX ) {
		case BODY_PART_WEAPON_R :
		case BODY_PART_WEAPON_L :
			this->UnloadTRAIL( nPartIDX );
			break;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   파트 인덱스
/// @param  short nItemNo	   파트의 아이템 인덱스
/// @param  bool bLinkNODE	   링크할꺼냐?
/// @brief  : 캐릭터에 붙는 효과를 생성
///				무기의 잔상, 또한 특별한 옵션에 의해 무기또는 특정 부위에 효과를 붙여야할때..
//--------------------------------------------------------------------------------

void CObjCHAR::New_EFFECT (short nPartIdx, short nItemNo, bool bLinkNODE)
{
	// 아이템에 붙은 기본 효과 삭제.
	this->Del_EFFECT( nPartIdx );

	CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( nPartIdx, nItemNo, this->IsFemale() );	//m_pMD_CharPART[ nPartIdx ]->GetMODEL( nItemNo );
	if ( NULL == pCharPART )
		return;

	short nEffectIDX=0;
	switch( nPartIdx ) 
	{
		case BODY_PART_WEAPON_R :
			{
				nEffectIDX = WEAPON_DEFAULT_EFFECT( nItemNo );

				if ( EFFECT_TRAIL_NORMAL( nEffectIDX ) ) 
				{
					this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_R, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ) );

					/// 양손 이도류 무기일경우..
					if( ( pCharPART->m_nDummyPointCNT > 2 ) && 
						( ( WEAPON_TYPE( nItemNo ) == 251 ) || ( WEAPON_TYPE( nItemNo ) == 252 ) ) )
						this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_L, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ), 2 );
				}				
			}
			break;
		case BODY_PART_WEAPON_L :
			// nEffectIDX = SUBWPN_DEFAULT_EFFECT( nItemNo );
			if ( EFFECT_TRAIL_NORMAL( nEffectIDX ) ) {
				this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_L, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ) );
			}
			break;
		default :
			return;
	}

	if ( nEffectIDX ) 
	{
		t_HASHKEY EffectHASH;
		for (short nP=0; nP<EFFECT_POINT_CNT; nP++) 
		{
			EffectHASH = g_pEffectLIST->Get_IndexHashKEY( EFFECT_POINT( nEffectIDX, nP ) );
			if ( EffectHASH )
				this->Add_EFFECT( pCharPART, nPartIdx, nP, EffectHASH, bLinkNODE );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName 이름( 내부세어 생성 오더번호를 붙여 고유한 이름을 생성한다. )
/// @param  int iPartIDX 파트 인덱스
/// @brief  : 특정 부위 생성
//--------------------------------------------------------------------------------

void CObjCHAR::CreateSpecificPART( char *szName, int iPartIDX )
{
	m_phPartVIS[ iPartIDX ] = m_pCharMODEL->CreatePART( szName, m_hNodeMODEL, iPartIDX );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  int iPartIDX 파트 인덱스
/// @param  HNODE *pVIS  파트를 구성하는 부분들의 visiable 노드 배열
/// @brief  : 특정 부위 삭제
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteSpecificPART( short nPartIdx, HNODE *pVIS )
{
	m_pCharMODEL->DeletePART( nPartIdx, pVIS );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName 이름
/// @brief  : 캐릭터 각파트 생성, 내부에서 각파트별로 CreateSpecificPART 생성
//--------------------------------------------------------------------------------

void CObjCHAR::CreatePARTS (char *szName)
{
	m_pCharMODEL->ClearRenderUnitParts();

	// npc 무기땜에 MAX_BODY_PART까지 ...
	for (short nP=0; nP<MAX_BODY_PART; nP++) 
	{
		CreateSpecificPART( szName, nP );
		//m_phPartVIS[ nP ] = m_pCharMODEL->CreatePART( szName, m_hNodeMODEL, nP );
	}	

	// 케릭터 신장.
	m_fStature = ::getModelHeight (this->m_hNodeMODEL);
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bDelEFFECT 이펙트도 지우냐?
/// @brief  : 캐릭터 각 파트 삭제
//--------------------------------------------------------------------------------

void CObjCHAR::DeletePARTS (bool bDelEFFECT)
{
	this->RemoveFromScene (bDelEFFECT);

	if ( this->m_hNodeMODEL ) 
	{
		// addRenderUnit된것들 삭제.
		::clearRenderUnit( this->m_hNodeMODEL );
	}

	// loadVisible된것들 삭제.
	for (short nP=0; nP<MAX_BODY_PART; nP++) 
	{
		DeleteSpecificPART( nP, m_phPartVIS[ nP ] );
		//m_pCharMODEL->DeletePART( nP, m_phPartVIS[ nP ] );
		m_phPartVIS[ nP ] = NULL;
	}	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName 모델 이름
/// @brief  : 캐릭터의 엔진모델노드 생성.
//--------------------------------------------------------------------------------

bool CObjCHAR::LoadModelNODE (char *szName)
{
	HNODE hSkel  = this->Get_SKELETON ();
	if ( hSkel == NULL ) {
		LogString (LOG_DEBUG, "failed creat char.. SKEL == NULL !! \n");
		return false;
	}

	if ( !this->Set_CurMOTION( this->Get_MOTION(0) ) ) {		// default stop motion ..
		LogString (LOG_DEBUG, "failed creat char.. MOTION == NULL !! \n");
		return false;
	}

	m_hNodeMODEL = ::loadModel (szName, hSkel, this->Get_ZMOTION(), 1.0f);
	if ( m_hNodeMODEL ) 
	{
		::setCollisionLevel( m_hNodeMODEL, 4 );

		// 기본 지형에서 캐릭터 중심점 높이
		m_fHeightOfGround = ::getPositionZ (this->m_hNodeMODEL);
		m_iLastDummyIDX   = ::getNumDummies( m_hNodeMODEL ) - 1;

		::setScale   (m_hNodeMODEL,  m_fScale, m_fScale, m_fScale);
		::setPosition( m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 캐릭터의 엔진모델노드 삭제. 외부에서 등록된 모든 이펙트 들도 삭제
//--------------------------------------------------------------------------------

void CObjCHAR::UnloadModelNODE ()
{
	/// 외부에서 등록된 이펙트들도 다 삭제..	
	ClearExternalEffect();
	

	if ( m_hNodeMODEL ) 
	{	
		::unloadModel (m_hNodeMODEL);
		m_hNodeMODEL = NULL;
	}	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param char *szName			이름
/// @param CCharMODEL *pMODEL	캐릭터 모델 정보를 관리할.. 모델클래스
/// @param short nCharPartCNT	캐릭터 파트 카운트
/// @param D3DVECTOR &Position	생성할 캐릭터 위치
/// @brief  : 캐릭터 생성
///				1. 캐릭터 모델노드 생성
///				2. 캐릭터 파트들 생성
///				3. 본 이펙트( 캐릭터 툴에서 설정된 ) 생성
//--------------------------------------------------------------------------------

bool CObjCHAR::CreateCHAR (char *szName, CCharMODEL *pMODEL, short nCharPartCNT, const D3DVECTOR &Position)
{
	m_bIsVisible = false;

	m_PosBORN	 = Position;
	m_PosCUR     = Position;
	m_pCharMODEL = pMODEL;

	if ( pMODEL == NULL || nCharPartCNT <= 0 )	return false;
	
	if ( this->LoadModelNODE (szName) ) 
	{
		this->CreatePARTS( szName );

		m_ppBoneEFFECT = m_pCharMODEL->CreateBoneEFFECT( m_hNodeMODEL );
		this->InsertToScene ();

		this->SetCMD_STOP();

		DropFromSky( Position.x, Position.y );
	 	return true;
	}
	
	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 캐릭터 삭제
///				1. 본 이펙트( 캐릭터 툴에서 설정된 ) 삭제
///				2. 캐릭터 파트들 삭제
///				3. 캐릭터 모델노드 삭제
///				4. 엔진 객체 삭제( visiable )
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteCHAR (void)
{
	this->RemoveFromScene ();

	m_pCharMODEL->DeleteBoneEFFECT( m_ppBoneEFFECT );

	short nP;
	for (nP=0; nP<MAX_BODY_PART; nP++)
		this->Del_EFFECT( nP );

	this->DeletePARTS ();
	this->UnloadModelNODE ();

	for (short nI=0; nI<MAX_BODY_PART; nI++) 
	{
		SAFE_DELETE_ARRAY( m_phPartVIS[ nI ] );
	}

	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  t_HASHKEY HashMOTION 모션의 해쉬키
/// @param  float fMoveSpeed	 이동 속도
/// @param  int iRepeatCnt		 반복카운트
/// @brief  : 유져모션 세팅
//--------------------------------------------------------------------------------

void CObjCHAR::Set_UserMOITON (t_HASHKEY HashMOTION, float fMoveSpeed, int iRepeatCnt )
{
	tagMOTION *pMotion = g_MotionFILE.KEY_GetMOTION( HashMOTION );

	Set_UserMOITON( pMotion, fMoveSpeed, iRepeatCnt );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  tagMOTION* pMotion   모션.
/// @param  float fMoveSpeed	 이동 속도
/// @param  int iRepeatCnt		 반복카운트
/// @brief  : 유져모션 세팅
//--------------------------------------------------------------------------------

void CObjCHAR::Set_UserMOITON (tagMOTION *pMotion, float fMoveSpeed, int iRepeatCnt )
{	
	// 현재 진행중인 모션과 같을경우 모션이 업데이트 되지 않으므로 
	// 이동 속도 설정을 Chg_CurMOTION밖으로 꺼냄..
	this->Set_ModelSPEED( fMoveSpeed );

	if ( this->Chg_CurMOTION( pMotion ) ) 
	{

#ifndef __VIRTUAL_SERVER
		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, 1.0f  );
		::setRepeatCount( this->GetZMODEL(), iRepeatCnt );		

		// 본 애니가 없는 경우에도 메쉬 애니가 있을수 있나????
		this->m_pCharMODEL->SetMeshMOTION( m_phPartVIS, this->Get_ActionIDX() );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nActionIdx 모션의 해쉬키
/// @param  float fMoveSpeed	 이동 속도
/// @param  float fAniSpeed		 반복카운트
/// @param  float bool bAttackMotion	 반복카운트
/// @brief  : 모션 세팅
/// @bug 첨에 여기에 공격시작 사운드 넣었다가.. 프레임에 정보를 넣고 정확한 프레임에서 시작하게 옮겼다.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_MOTION (short nActionIdx, float fMoveSpeed, float fAniSpeed, bool bAttackMotion, int iRepeatCnt )
{	

	if ( !bAttackMotion && m_bAttackSTART )	{
		Attack_END ();
	}


	if ( this->Chg_CurMOTION( this->Get_MOTION (nActionIdx) ) ) 
	{

#ifndef __VIRTUAL_SERVER
//		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif
	
		this->Set_ModelSPEED( fMoveSpeed );

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, fAniSpeed  );		
		::setRepeatCount	( this->m_hNodeMODEL, iRepeatCnt  );		

		// 본 애니가 없는 경우에도 메쉬 애니가 있을수 있나????
		this->m_pCharMODEL->SetMeshMOTION( m_phPartVIS, this->Get_ActionIDX() );
	}
	
	::controlAnimatable ( this->m_hNodeMODEL, 0 );
	::controlAnimatable ( this->m_hNodeMODEL, 1 );
	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  D3DXVECTOR3& Pos 갱신할 위치
/// @brief  : 현재위치 갱신
//--------------------------------------------------------------------------------

void CObjCHAR::ResetCUR_POS( D3DXVECTOR3& Pos )
{
	CGameOBJ::Set_CurPOS( Pos );
	::setPositionVec3( m_hNodeMODEL, m_PosCUR );		
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pAtkOBJ 공격자
/// @param  WORD wDamage      데미지
/// @brief  : 데미지 적용 처리
//--------------------------------------------------------------------------------

void CObjCHAR::Apply_DAMAGE (CObjCHAR *pAtkOBJ, WORD wDamage)
{
	if ( this->Get_HP() <= DEAD_HP )
		return;

	if( IsA( OBJ_USER ) )///내가 데미지를 입었다면
		g_pAVATAR->SetBattleTime( g_GameDATA.GetGameTime() );

	if( pAtkOBJ && pAtkOBJ->IsA( OBJ_USER ) )///내가 공격했다면
		g_pAVATAR->SetBattleTime( g_GameDATA.GetGameTime() );


	uniDAMAGE Damage;
	Damage.m_wDamage = wDamage;


	if ( Damage.m_wACTION & DMG_ACT_DEAD ) 
	{
		/// 자신은 죽었다.
		this->Do_DeadAI (pAtkOBJ, Damage.m_wVALUE );

		/// 공격자와 내가 같은 놈이면.. 소환수가 줄을때처럼.. 뭐 그런.. 죽는 모션을 하면 안된다.
		/// 혹여나 유져일경우 비지빌리티가 세팅되면 죽을때 모션을 세워버리는 사태가..일단 User일경우는 패스. -04/5/25
		m_bStopDead = ( pAtkOBJ == this );
		this->Dead ();
		if( m_bStopDead && ( this->IsA( OBJ_USER ) == false ) )
		{
			m_pChangeVisibility = new CChangeVisibility( this, 5000, true );			
			m_bStopDead = false;			
		}

#ifdef _DEBUG
		if( ( pAtkOBJ == this ) && (this->IsA( OBJ_USER )) )
			assert( 0 && "얼래 공격자와 내가 같은데 웬 유져? 음 버그의 원인이군" );
#endif

		
		if ( pAtkOBJ ) 
		{
			//---------------------------------------------------------------------------------------
			/// 서버에서 요청시에만 체크하게 변경..
			//---------------------------------------------------------------------------------------
			//if( this->IsA( OBJ_MOB) ) 
			//{
			//	if ( NPC_QUEST_TYPE( this->m_nCharIdx ) == 1 ) 
			//	{
			//		/// 파티 퀘스트냐 ?
			//		if ( CParty::GetInstance().IsPartyMember( g_pObjMGR->Get_ServerObjectIndex(pAtkOBJ->Get_INDEX()) ) ||
			//			( pAtkOBJ->IsA( OBJ_USER ) && g_itMGR.IsDlgOpened( DLG_TYPE_PARTY ) ) )
			//		{
			//			/// 나의 파티 멤버인가, 죽인 케릭이 자신이고 자신이 파티중이냐 ???
			//			Do_DeadEvent(pAtkOBJ);
			//		}
			//	} else
			//	if ( pAtkOBJ->IsA( OBJ_USER ) ) {
			//		/// 개인 퀘스트면...
			//		Do_DeadEvent(pAtkOBJ);	
			//	}
			//}
			if( g_pAVATAR )
			{
				if( g_pAVATAR->Get_INDEX() == pAtkOBJ->Get_INDEX() )
				{				
					switch( this->Get_TYPE() )
					{
						case OBJ_MOB:
							sprintf( g_MsgBuf, F_STR_SUCCESS_HUNT, this->Get_NAME() );
							break;

						case OBJ_AVATAR:
							sprintf( g_MsgBuf, F_STR_WIN_PVP, this->Get_NAME() );
							break;
					}				

					g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}

			if ( pAtkOBJ->m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ) )	// 타겟 삭제.
				pAtkOBJ->m_iServerTarget = 0;

			/// 죽였을때 인공지능 수행...
			pAtkOBJ->Do_KillAI( this, Damage.m_wVALUE );

//---------------------------------------------------------------------------------------------
#if defined(_GBC)			
			//박지호::펫모드일때는 회전을 하지 않는다.
			if(GetPetMode() < 0)
				Set_ModelDIR( pAtkOBJ->Get_CurPOS() );
#else 
	Set_ModelDIR( pAtkOBJ->Get_CurPOS() );
#endif 
//---------------------------------------------------------------------------------------------
 


			//----------------------------------------------------------------------------------
			/// 패널티 경험치 보정
			//----------------------------------------------------------------------------------
			/// 내가 유져라면
			if( this->IsA( OBJ_USER ) == true )
			{
				int iPanalEXP = 0;

				/// 몬스터라면
				if( pAtkOBJ->IsA( OBJ_MOB ) ) 
				{
					/// 소한 몬스터라면 경험치 깎지 않음
					if( pAtkOBJ->m_EndurancePack.GetStateFlag() & ING_DEC_LIFE_TIME )
					{
						iPanalEXP = 0;
					}else
					{
						g_pAVATAR->Set_PenalEXP( 3 );
					}					
				}
			}			
		}

		//------------------------------------------------------------------------------------
		/// delayed exp 
		//------------------------------------------------------------------------------------
		CDelayedExp::GetSingleton().GetExp( this->Get_INDEX() );

		
		/*
		죽을때 이벤트는 CObjCHAR::Dead 로 이동..
		*/
		
		if( !m_FieldItemList.empty() )
			DropFieldItemFromList();

		this->m_lDeadTIME = 0;

	} else {
		this->Do_DamagedAI( pAtkOBJ, Damage.m_wVALUE );

		if ( this->Sub_HP( Damage.m_wVALUE ) <= 0 ) 
		{
			this->Set_HP( 1 );
		}

	}

	if ( pAtkOBJ ) 
	{
		if ( Damage.m_wACTION & DMG_ACT_CRITICAL ) 
		{
			// 크리티컬 데미지 !!!
			// AddMsgToChatWND( CStr::Printf ("Critical:: %s가 %s에게 피해 %d 입힘", pAtkOBJ->Get_NAME(), this->Get_NAME (), Damage.m_wVALUE ), g_dwRED );
		} else {
			// 일반 데미지 ...
			// AddMsgToChatWND( CStr::Printf ("%s가 %s에게 피해 %d 입힘", pAtkOBJ->Get_NAME(), this->Get_NAME (), Damage.m_wVALUE ), g_dwRED );
		}

		if ( Damage.m_wDamage & DMG_BIT_HITTED ) 
		{
			// 맞는넘 맞는 동작으로 적용 가능하면...
			/// 팻탄 상태이면 맞는 동작을 하지 않는다.( 2..4/12/10 맞는종작을 할경우 공격을 하다가 캐슬기어가 서버렸다. )
			if( this->GetPetMode() > 0 )
			{
				
			}else
			{
				if ( !(this->Get_STATE() & CS_BIT_INT2) ) 
				{
					this->Set_MOTION( this->GetANI_Hit() );
					this->Set_STATE( CS_HIT );
				}
			}
		}
	}
	
//#ifdef	_DEBUG
//	if ( this->IsA( OBJ_USER ) ) {
//		if ( pAtkOBJ ) {
//			int iDist = CD3DUtil::distance ((int)pAtkOBJ->m_PosCUR.x, (int)pAtkOBJ->m_PosCUR.y, 
//											(int)this->m_PosCUR.x, (int)this->m_PosCUR.y);
//
//			AddMsgToChatWND( CStr::Printf ("거리 %d [%.0f, %.0f] ~ %s[%d] [ %.0f, %.0f ]", 
//					iDist,
//					this->m_PosCUR.x, this->m_PosCUR.y,
//					pAtkOBJ->Get_NAME(),
//					g_pObjMGR->Get_ServerObjectIndex( pAtkOBJ->Get_INDEX() ),
//					pAtkOBJ->m_PosCUR.x, pAtkOBJ->m_PosCUR.y), g_dwGREEN );
//		} else {
//			;
//		}
//	}
//#endif
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget 타겟 오브젝트
/// @brief  : 스킬 사용 시작. 무기에 검잔상을 시작 한다던지..( 검잔상을 Attack_Start 에서만 붙였더니 .. 문제가 )
//--------------------------------------------------------------------------------

bool CObjCHAR::Skill_START (CObjCHAR *pTarget)
{
	if ( m_nActiveSkillIDX ) 
	{
		/// 근접 즉시 발동 스킬은 검잔상 발동 )
		if( SKILL_TYPE( m_nActiveSkillIDX ) == SKILL_ACTION_IMMEDIATE )
		{
			// 검잔상 시작.
			for (short nI=0; nI<2; nI++) 
			{
				if ( m_hTRAIL[ nI ] ) 
				{
					::controlTrail ( m_hTRAIL[ nI ], 0 );	// stop & clear
					::controlTrail ( m_hTRAIL[ nI ], 1 );	// start
				}
			}

		}

		return true;
	}

	return false;	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget 타겟 오브젝트
/// @brief  : 공격 시작.. 검잔상 애니매이션 시작..
//--------------------------------------------------------------------------------

bool CObjCHAR::Attack_START (CObjCHAR *pTarget)
{
	/*
	// 무기 붙이기 / 검잔상...
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iTargetObject );
	int iBulletIDX = Get_BulletNO ();
	if ( pTarget && iBulletIDX ) {
		g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX );
	}
	*/

	// 검잔상 시작.
	for (short nI=0; nI<2; nI++) 
	{
		if ( m_hTRAIL[ nI ] ) 
		{
			::controlTrail ( m_hTRAIL[ nI ], 0 );	// stop & clear
			::controlTrail ( m_hTRAIL[ nI ], 1 );	// start
		}
	}

	m_bAttackSTART  = true;
	m_iActiveObject = g_pObjMGR->Get_ClientObjectIndex( m_iServerTarget );

	_ASSERT( m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( m_iActiveObject ) );


	/// 지속 속성중 공격시작하면 풀려야 되는것들
	m_EndurancePack.ClearStateByAttack();


	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bStopTrail 검잔상 정지?
/// @brief  : 공격 끝.
//--------------------------------------------------------------------------------

void CObjCHAR::Attack_END (bool bStopTrail)
{
	// 기본 애니메이션 속도 복귀.
	::setAnimatableSpeed ( GetZMODEL(), 1.0 );

	// 검잔상 효과 모션정지..
	if ( bStopTrail ) 
	{
		for (short nI=0; nI<2; nI++) 
		{
			if ( m_hTRAIL[ nI ] ) 
			{
				::controlTrail ( m_hTRAIL[ nI ], 3 );	// no spawn
			}
		}
	}

	m_bAttackSTART = false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget 타겟 오브젝트
/// @brief  : 캐스팅 동작시작..( 스킬의 시작을 표시한다. ) 아주 중요../
///				유져일경우는 타이머를 세팅한다. 스킬의 딜레이가 있기때문에.
//--------------------------------------------------------------------------------

bool CObjCHAR::Casting_START (CObjCHAR *pTarget)
{
	SetCastingState( true );
	/// m_bProcEffectedSkill = false;

	//--------------------------------------------------------------------------------
	/// 유져일경우는 타이머를 세팅한다.
	//--------------------------------------------------------------------------------
	//if( this->IsA( OBJ_USER ) )
	//{
	//	/// 타이머 설정
	//	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	//	CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( m_nToDoSkillIDX );
	//	if( pSkill )
	//	{
	//		pSkill->SetSkillDelayTime( SKILL_RELOAD_TIME( m_nToDoSkillIDX ) * 200 );
	//	}
	//}

	/// 지속 속성중 공격시작하면 풀려야 되는것들
	if( SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_IMMEDIATE ||
		SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_FIRE_BULLET ||
		SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_SELF_DAMAGE )
	{
		m_EndurancePack.ClearStateByAttack();
	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 캐스팅 동작끝..( 스킬의 끝을 표시한다. ) 아주 중요../
//--------------------------------------------------------------------------------

void CObjCHAR::Casting_END ()
{
	/// 현재 활성화된 스킬이 있다면.. 캐스팅 상태 유지..	
	SetCastingState( false );
	m_nActiveSkillIDX = 0;
	m_nToDoSkillIDX = 0;	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 무브 시작
//--------------------------------------------------------------------------------
void CObjCHAR::MoveStart ()
{
	/// 이동과 동시에 자동으로 풀려야 하는것들..
	m_EndurancePack.ClearStateByMove();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 총알번호를 구한다.
//--------------------------------------------------------------------------------

int CObjCHAR::Get_BulletNO ()
{
	short nWeaponItem = Get_R_WEAPON();
	if( nWeaponItem )
		return WEAPON_BULLET_EFFECT( nWeaponItem );

	return 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 죽는 처리..( 강제로 캐스팅을 끝내는 루틴이 들어갔다.. 이것도 상당히 중요 )
///				m_nActiveSkillIDX 도 리셋.. 죽었을경우.. 이 플래그가 세팅되어 있으면. Casting_End 가 무효해진다.
//--------------------------------------------------------------------------------

void CObjCHAR::Dead ()
{
	if ( Get_HP() <= DEAD_HP )
	{
		SetCastingState( false );
		return;
	}


	//----------------------------------------------------------------------------------------------------
	/// 아바타가 죽을경우
	//----------------------------------------------------------------------------------------------------	
	if( this->Is_AVATAR() )
	{
		CObjAVT* pAVT = ( CObjAVT* )this;
		/// Pet 상태라면 내린다.
		if( GetPetMode() >= 0 )
		{			
			pAVT->DeleteCart( false );

			//----------------------------------------------------------------------------------------------------
			/// 죽을때 팻의 파괴 이펙트 출력..
			//----------------------------------------------------------------------------------------------------	
			if( pAVT->IsVisible() )
			{
				int iEffectNO	= PAT_DEAD_EFFECT( pAVT->m_sBodyIDX.m_nItemNo );
				int iSoundNO	= PAT_DEAD_SOUND( pAVT->m_sBodyIDX.m_nItemNo );
				
				this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );				
			}

			m_btMoveMODE = MOVE_MODE_RUN;
			pAVT->Update_SPEED();					
		}
		else
		{
			//----------------------------------------------------------------------------------------------------
			/// 캐릭터 죽을때의 이펙트 출력
			//----------------------------------------------------------------------------------------------------	
			SE_CharDie( this->Get_INDEX() );
		}

		/// 개인상점 관련 리셋..
		if( pAVT->IsPersonalStoreMode() )
		{
			pAVT->SetPersonalStoreTitle( NULL );
			g_UIMed.SubPersonalStoreIndex( this->Get_INDEX() );
		}


		/// 만약 내가 죽은 거라면..
		if( this->IsA( OBJ_USER ) )
		{
			(( CObjUSER*)pAVT)->ClearSummonedMob();
			g_itMGR.ChangeState( IT_MGR::STATE_DEAD );
		}

	}
		
	//----------------------------------------------------------------------------------------------------
	/// 몹이 죽을경우 이펙트 출력
	//----------------------------------------------------------------------------------------------------	
	if( this->IsA( OBJ_MOB ) )
	{
		///SE_MobDie( this->Get_INDEX() );
		int iMobDeadEffect = NPC_DEAD_EFFECT( this->Get_CharNO() );
		int iSoundIDX = NPC_DIE_SOUND( this->Get_CharNO() );		
		this->ShowEffectOnCharByIndex( iMobDeadEffect, iSoundIDX );		

	}

	Set_HP( DEAD_HP );
	SetCMD_DIE ();


	m_nActiveSkillIDX = 0;
	SetCastingState( false );
	
	m_EndurancePack.ClearEntityPack();
	ClearAllEntityList();

	// 루프방지.
	::setRepeatCount( m_hNodeMODEL, 1 );
	// 죽은넘은 클릭 안되게...

	if( this->Is_AVATAR() == false )
	{
		/// 2004-11-26 죽은놈도 클릭되게...
		::setCollisionLevel( m_hNodeMODEL, 0 );
	}


	::setModelBlinkCloseMode( this->GetZMODEL(), true );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iSkillIDX 스킬 인덱스
/// @param gsv_DAMAGE_OF_SKILL EffectedSkill 스킬 결과 정보
/// @param bool bDamageOfSkill	데미지 결과인가?( 범위데미지 스킬의 경우.. )
/// @brief  : 스킬결과를 액션프레임에 수행하기 위해 짱박아 둔다.
//--------------------------------------------------------------------------------

void CObjCHAR::PushEffectedSkillToList( int iSkillIDX, gsv_DAMAGE_OF_SKILL EffectedSkill, int iCasterINT, bool bDamageOfSkill )
{
	stEFFECT_OF_SKILL steffectOfSkill;
	
	steffectOfSkill.m_dwCreateTime	= g_GameDATA.GetGameTime();
	steffectOfSkill.iSkillIDX		= iSkillIDX;
	steffectOfSkill.bDamageOfSkill	= bDamageOfSkill;
	steffectOfSkill.iCasterINT		= iCasterINT;

	steffectOfSkill.EffectOfSkill = EffectedSkill;
	m_EffectedSkillList.push_back( steffectOfSkill ); 
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 타임아웃 시간이 지난 스킬결과를 처리한다.
//--------------------------------------------------------------------------------
const int SKILL_PROC_LIMIT = 1000 * 10;
void CObjCHAR::ProcTimeOutEffectedSkill()
{
	stEFFECT_OF_SKILL*	pEffectOfSkill	= NULL;	
	DWORD dwElapsedTime = 0;

	std::vector< stEFFECT_OF_SKILL >::iterator begin = m_EffectedSkillList.begin();
	for( ; begin != m_EffectedSkillList.end(); )
	{		
		pEffectOfSkill	= &( *begin );
     
		dwElapsedTime = g_GameDATA.GetGameTime() - pEffectOfSkill->m_dwCreateTime;
		if( dwElapsedTime > SKILL_PROC_LIMIT )
		{
			ProcOneEffectedSkill( pEffectOfSkill );
			begin = m_EffectedSkillList.erase( begin );
#ifdef _DEBUG
			
			sprintf( g_MsgBuf, "ProcTimeOutEffectedSkill [ 대상 : %s ] ", this->Get_NAME() );
			///assert( 0 && Buf );
			MessageBox( NULL, g_MsgBuf, "WARNING", MB_OK );
#endif //_DEBUG
		}else
			++begin;
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief /// Damage of skill 에 실려오는 스킬적용결과 반영..
//----------------------------------------------------------------------------------------------------

void CObjCHAR::ProcEffectOfSkillInDamageOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pChar, stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	int iSkillType = SKILL_TYPE( iSkillIDX );

	switch( iSkillType )
	{
		case SKILL_ACTION_SELF_AND_TARGET:
			{
				if( this->IsA( OBJ_USER ) )
				{
					for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; i++ )
					{
						if( ( 0x01 << i ) & pEffectOfSkill->EffectOfSkill.m_btSuccessBITS )
						{
							int iAbilityType = SKILL_INCREASE_ABILITY( iSkillIDX, i );
							switch( iAbilityType )
							{
								case AT_HP:
									g_pAVATAR->Add_HP( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;
								case AT_MP:
									g_pAVATAR->Add_MP( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;

								case AT_STAMINA:
									g_pAVATAR->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;

							}
						}
					}
				}
			}
			break;

		default:
			ApplyEffectOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );
			break;
	}	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 실제로 캐릭터에 스킬 적용..
//----------------------------------------------------------------------------------------------------

void CObjCHAR::ApplyEffectOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pEffectedChar, stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	if( pEffectOfSkill->EffectOfSkill.m_btSuccessBITS == 0 )/// 적용 효과후 바로 삭제..즉 스킬 적용 실패다
	{
		/// 내가 적용한 스킬일경우에만 스킬 적용 실패 메시지를 표시한다.
		int iClientObjIndex = g_pObjMGR->Get_ClientObjectIndex( pEffectOfSkill->EffectOfSkill.m_wSpellObjIDX );
		if( iClientObjIndex == g_pAVATAR->Get_INDEX() )
		{
			///AddMsgToChatWND( STR_SKILL_APPLY_FAILED, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
			;
		}		
	}else
	{
		///스킬이 지속형일 경우 최대 2개의 상태까지 바뀔수 있으므로
		for( int i = 0 ; i < 2 ; i++ ) 
		{			
			if ( ( 0x01 << i ) & pEffectOfSkill->EffectOfSkill.m_btSuccessBITS ) 
			{
				int iStateIndex = 0;

				/// 지속형이 아닌 단순 능력치 상승형..
				/*if( SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
					SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_TARGET_BOUND )*/
				{
					iStateIndex = SKILL_STATE_STB( iSkillIDX, i );

					/// 유리상태 해지, 불리상태 해지 등의 상태 해제 스킬들
					if( iStateIndex && STATE_TYPE( iStateIndex ) > ING_CHECK_END )
					{
						pEffectedChar->ProcFlushStateSkill( iStateIndex );
						continue;
					}
				}
				
				if( iStateIndex != 0 )
				/// 지속형 스킬이라면..
				/*if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )*/
				{
					/// 일단 유져일경우만 속성객체 추가..
					//if( pChar->IsA( OBJ_USER ) )
					pEffectedChar->AddEnduranceEntity( iSkillIDX,	iStateIndex, SKILL_DURATION( iSkillIDX ), ENDURANCE_TYPE_SKILL ) ;

					/// 상태 타입..
					int iStateType = STATE_TYPE( iStateIndex );
					/// 상태 번호가 1,2 번인경우에는 LIST_STATUS.STB 의 값을 참고하고
					if( iStateType == ING_INC_HP || iStateType == ING_INC_MP || iStateType == ING_POISONED )
						pEffectedChar->m_EndurancePack.SetStateValue( iStateType, STATE_APPLY_ABILITY_VALUE( iStateIndex, i ) );
					else
					{
						int iIncValue = 0;					
						/// 04/4/24
						if( pEffectedChar->IsA( OBJ_USER ) )
						{
							iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, pEffectOfSkill->iCasterINT );
						}
						else							
						{
							iIncValue = 1;

							/// 유져가 아닐경우 알수가 없다. 몬스터일경우에는 공속, 이속만 계산해본다.
							if( pEffectedChar->IsA( OBJ_MOB ) )
							{
								int iAbilityValue = 0;
								switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
								{
									case AT_SPEED:
										iAbilityValue = pEffectedChar->GetOri_RunSPEED();
										break;
									case AT_ATK_SPD:
										iAbilityValue = pEffectedChar->GetOri_ATKSPEED();
										break;
								}


								iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i ) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}	

							//--------------------------------------------------------------------------------------------
							/// 다른 아바타일경우.. MAX_HP는 고려를 한다..
							if( pEffectedChar->IsA( OBJ_AVATAR ) )
							{
								int iAbilityValue = 0;
								switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
								{
									case AT_MAX_HP:
										iAbilityValue = pEffectedChar->Get_MaxHP();
										break;								
								}

								iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i) * ( pEffectOfSkill->iCasterINT + 300 ) / 315.f );								
							}
							//--------------------------------------------------------------------------------------------

						}
						

						pEffectedChar->m_EndurancePack.SetStateValue( iStateType, iIncValue );

						/// 상태스킬이 걸릴경우 AVATAR 의 경우는 능력치 업데이트..
						if( pEffectedChar->IsA( OBJ_USER ) )
						{
							(( CObjUSER* )pEffectedChar)->UpdateAbility();
						}
					}


				}else if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND ||
							SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND )
				{
					/// hp 는 계산식 적용..
					int iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, pEffectOfSkill->iCasterINT );

					switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
					{
						case AT_HP:
							pEffectedChar->Add_HP( iIncValue );
							break;
						case AT_MP:						
							pEffectedChar->Add_MP( iIncValue );
							break;
						case AT_STAMINA:
							/// 상태스킬이 걸릴경우 AVATAR 의 경우는 능력치 업데이트..
							if( pEffectedChar->IsA( OBJ_USER ) )
							{
								(( CObjUSER* )pEffectedChar)->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}	
							break;
						default:
							g_itMGR.AppendChatMsg( "몰르는거네 추가해라..", IT_MGR::CHAT_TYPE_SYSTEM);
							break;
					}
				}			
			} 
		}
	}
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param stEFFECT_OF_SKILL*	pEffectOfSkill 스킬 결과 하나 처리..
/// @brief  : 스킬 결과 처리
//--------------------------------------------------------------------------------

void CObjCHAR::ProcOneEffectedSkill( stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	int iSkillIDX		= pEffectOfSkill->iSkillIDX;
	int iObjIDX			= pEffectOfSkill->EffectOfSkill.m_wObjectIDX;

	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );

	if( pChar == NULL )
		return;

			/*
	*	범위 마법의 경우 m_nResultVALUE 가 데미지 이다.
	*/
	if( pEffectOfSkill->bDamageOfSkill )
	{
		D3DXVECTOR3	pos = pChar->Get_CurPOS();

		pChar->Apply_DAMAGE( this, pEffectOfSkill->EffectOfSkill.m_Damage.m_wDamage );	

		/// Damage of skill 에 실려오는 스킬적용결과 반영..
		ProcEffectOfSkillInDamageOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );

		/// 스킬 타격 이펙트
		// 타격 효과.
		if ( pChar->IsVisible() ) 
		{
			g_UIMed.CreateDamageDigit( pEffectOfSkill->EffectOfSkill.m_Damage.m_wVALUE, pos.x, pos.y, pos.z + pChar->m_fStature, pChar->IsA( OBJ_USER ) );

			int iEffectIDX = SKILL_HIT_EFFECT( iSkillIDX );
			if ( iEffectIDX  ) 
			{					
				CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
				if ( pHitEFT ) 
				{
					if( SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) == INVALID_DUMMY_POINT_NUM )
					{
						pHitEFT->LinkNODE ( pChar->GetZMODEL() );
					}else
					{
						pChar->LinkDummy( pHitEFT->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) );
					}							

					pHitEFT->SetParentCHAR( pChar );
					pChar->AddExternalEffect( pHitEFT );					
	
					pHitEFT->UnlinkVisibleWorld ();
					pHitEFT->InsertToScene ();
				}					

				if ( SKILL_HIT_SOUND( iSkillIDX ) ) 
				{
					if ( pChar->IsUSER() ) 
					{ // 자기 아바타인 경우에는 안3D 모드로 출력.
						g_pSoundLIST->IDX_PlaySound( SKILL_HIT_SOUND( iSkillIDX ) );
					}
					else 
					{
						g_pSoundLIST->IDX_PlaySound3D( SKILL_HIT_SOUND( iSkillIDX ), pChar->Get_CurPOS() );
					}						
				}			
			}
		}

	}else /// 지속성이거나.. 상태를 바꾸는 스킬..
	{		
		ApplyEffectOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 보관된 모든 스킬결과들을 이벤트 프레임에 처리한다.
//--------------------------------------------------------------------------------

bool CObjCHAR::ProcEffectedSkill()
{
	bool bResult = false;

	stEFFECT_OF_SKILL*	pEffectOfSkill	= NULL;	


	for( int i = 0; i < m_EffectedSkillList.size(); i++ )
	{		
		pEffectOfSkill	= &( m_EffectedSkillList[ i ] );
        
		ProcOneEffectedSkill( pEffectOfSkill );

		bResult = true;
	}

	/// 처리후 모든 보관된 스킬결과 클리어	
    m_EffectedSkillList.clear();
	
	SetEffectedSkillFlag( false );
	Log_String( LOG_NORMAL, "ProcEffectedSkill\n" );

	return bResult;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iattacker 공격자
/// @param uniDAMAGE wDamage	적용 데미지
/// @brief  : 데미지를 리스트에 추가ㅓ..
//--------------------------------------------------------------------------------

void CObjCHAR::PushDamageToList( int iAttacker, uniDAMAGE wDamage )
{ 
	stAPPLY_DAMAGE applyDamage;
	applyDamage.m_Damage			= wDamage;
	applyDamage.m_iAttacker			= iAttacker;
	applyDamage.m_dwCreateTime		= g_GameDATA.GetGameTime();

	m_DamageList.push_back( applyDamage ); 
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iattacker 공격자
/// @brief  : 현재 적용할 데미지를 데미지 리스트에서 꺼낸다.( 공격자가 같은 데미지만 꺼냄 )
///				액션은 마지막껄 따른다...^^; 액션도 누적시키는 바람에 버그가 생겼다..
//--------------------------------------------------------------------------------

uniDAMAGE CObjCHAR::PopCurrentAttackerDamage( int iAttacker )
{
	uniDAMAGE wDamage;
	wDamage.m_wDamage = 0;	

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{	
		if( (*(begin)).m_iAttacker == iAttacker )
		{
			wDamage.m_wVALUE += (*(begin)).m_Damage.m_wVALUE;
			wDamage.m_wACTION |= (*(begin)).m_Damage.m_wACTION;

			begin = m_DamageList.erase( begin );
		}else
		{
			 ++begin;
		}
	}	

	return wDamage;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int& iMaxDamage
/// @brief  : 현재 리스트의 모든 데미지를 더해서 리턴..
//--------------------------------------------------------------------------------

uniDAMAGE CObjCHAR::PopTotalDamageFromList( int& iMaxDamage )
{
	uniDAMAGE wDamage;
	wDamage.m_wDamage = 0;
	iMaxDamage = 0;

	for( int i =0; i < m_DamageList.size() ; i++ )
	{
		/// 일단 타격치 출력은 제일 큰값..
		if( m_DamageList[ i ].m_Damage.m_wVALUE > iMaxDamage )
			iMaxDamage = m_DamageList[ i ].m_Damage.m_wVALUE;

		wDamage.m_wVALUE += m_DamageList[ i ].m_Damage.m_wVALUE;
		wDamage.m_wACTION |= m_DamageList[ i ].m_Damage.m_wACTION;
	}
	m_DamageList.clear();

	return wDamage;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : 모든 데이지 처리( 죽거나.. 뭐 그런 상황에서.. 정리 )
//--------------------------------------------------------------------------------

void CObjCHAR::ClearAllDamage()
{
	uniDAMAGE wDamage;
	
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{	
		begin = m_DamageList.erase( begin );		
	}	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param 
/// @brief  : 데미지의 시간제한 처리
//--------------------------------------------------------------------------------

void CObjCHAR::ProcDamageTimeOut()
{
	uniDAMAGE wDamage;
	
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{	
		/// 5초가 지났다면
		if( ( dwCurrentTime - (*begin).m_dwCreateTime ) > 5000 )
		{
			this->Apply_DAMAGE( g_pObjMGR->Get_CharOBJ( (*begin).m_iAttacker, true ), (*begin).m_Damage.m_wDamage );
			begin = m_DamageList.erase( begin );
		}else
		{
			++begin;		
		}
	}		
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  WORD wDamage 적용 데미지
/// @brief  : 타격치 바로 표시
//--------------------------------------------------------------------------------

void CObjCHAR::CreateImmediateDigitEffect( WORD wDamage )
{
	/// 타격수치 바로적용.. 적당한 장소가 없다 일단 여기에..
	uniDAMAGE Damage;
	Damage.m_wDamage = wDamage;
	D3DXVECTOR3	pos = this->Get_CurPOS();
	g_UIMed.CreateDamageDigit( Damage.m_wVALUE, pos.x, pos.y, pos.z + this->m_fStature, this->IsA( OBJ_USER ) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  gsv_DAMAGE_OF_SKILL stDamageOfSkill damage of kill 정보
/// @brief  : Damage_of_Skill => 일반 데미지로 전환저장..
//--------------------------------------------------------------------------------

void CObjCHAR::ConvertDamageOfSkillToDamage( gsv_DAMAGE_OF_SKILL stDamageOfSkill )
{
	uniDAMAGE		Damage;

	CObjCHAR *pAtkOBJ;	
	
	pAtkOBJ = g_pObjMGR->Get_ClientCharOBJ( stDamageOfSkill.m_wSpellObjIDX, false );
	
	if ( pAtkOBJ ) 
	{		
		/// Damage 등록..
		PushDamageToList( g_pObjMGR->Get_ClientObjectIndex( stDamageOfSkill.m_wSpellObjIDX ), 
							stDamageOfSkill.m_Damage );	
		return;
	}	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 아이템 드랍..
//--------------------------------------------------------------------------------

void CObjCHAR::DropFieldItemFromList()
{
	for( int i = 0; i < m_FieldItemList.size(); i++ )
	{
		//gsv_ADD_FIELDITEM& ItemInfo = m_FieldItemList[ i ];
		
		int iItemOBJ = g_pObjMGR->Add_GndITEM( 	m_FieldItemList[ i ].m_wServerItemIDX, 
												m_FieldItemList[ i ].m_ITEM, 
												m_FieldItemList[ i ].m_PosCUR, this->m_PosCUR.z, true );

		if ( iItemOBJ ) 
		{
			CObjITEM *pITEM = (CObjITEM *)g_pObjMGR->Get_OBJECT( iItemOBJ );
			pITEM->m_wOwnerServerObjIDX = m_FieldItemList[ i ].m_wOwnerObjIDX;
			pITEM->m_wRemainTIME		= ITEM_OBJ_LIVE_TIME;

			pITEM->InsertToScene ();

			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			tagITEM sITEM = m_FieldItemList[ i ].m_ITEM;

			/*if ( sITEM.m_cType != ITEM_TYPE_MONEY )
				AddMsgToChatWND(CStr::Printf ("아이템 %s 드롭 Type: %d, NO: %d ", 
														ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ),
														sITEM.m_cType, sITEM.m_nItemNo ), g_dwBLUE ,CChatDLG::CHAT_TYPE_SYSTEM);
			else 
				AddMsgToChatWND(CStr::Printf ("돈 드롭 %d ", sITEM.m_nItemNo), g_dwBLUE ,CChatDLG::CHAT_TYPE_SYSTEM);*/
		}
	}

	m_FieldItemList.clear();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 총알에 맞았을때 스킬처리..
//--------------------------------------------------------------------------------

bool CObjCHAR::ProcessSkillHit( CObjCHAR *pFromOBJ )
{
	if( pFromOBJ == NULL )
		return false;

	return pFromOBJ->ProcEffectedSkill();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 몹 소환.. ( 현재는 소환몹은 프레임에 않맞추고 바로 생성.. 서버와 명령 동기화 문제
///				서버에서는 생성된 몹에 공격명령을 내릴수 있는데 클라이언트에서는 아직생성 되지 않았으므로..
//--------------------------------------------------------------------------------

void CObjCHAR::SetSummonMobInfo( gsv_MOB_CHAR& MobInfo )
{
	memcpy( &m_SummonMob, &MobInfo, sizeof( gsv_MOB_CHAR ) );
	m_bHaveSummonedMob = true;

	SetEffectedSkillFlag( true );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  
/// @brief  : 몹 소환.. ( 현재는 소환몹은 프레임에 않맞추고 바로 생성.. 서버와 명령 동기화 문제
///				서버에서는 생성된 몹에 공격명령을 내릴수 있는데 클라이언트에서는 아직생성 되지 않았으므로..
//--------------------------------------------------------------------------------

void CObjCHAR::SummonMob()
{
	if( m_bHaveSummonedMob )
	{
		D3DVECTOR PosCUR;

		PosCUR.x = m_SummonMob.m_PosCUR.x;
		PosCUR.y = m_SummonMob.m_PosCUR.y;
		PosCUR.z = 0.0f;


		short nCObj;

		if ( NPC_TYPE( m_SummonMob.m_nCharIdx ) != 999 ) {
			nCObj = g_pObjMGR->Add_MobCHAR( m_SummonMob.m_wObjectIDX, 
											m_SummonMob.m_nCharIdx, 
											PosCUR, 
											m_SummonMob.m_nQuestIDX, 
											m_SummonMob.m_btMoveMODE);
			if ( g_pNet->Recv_tag_ADD_CHAR( nCObj, &m_SummonMob ) ) 
			{
				short nOffset = sizeof( gsv_MOB_CHAR );
				g_pNet->Recv_tag_ADJ_STATUS( nOffset, &m_SummonMob );
			}
		}
	}

	m_bHaveSummonedMob = false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pFromOBJ 공격자
/// @param  int iEffectIDX		총알번호
/// @param  int iSkillIDX		스킬일경우.. 스킬번호
/// @param  bool bIsSkillEffect	이것이 스킬에 의한 매직인가?
/// @param  bool bIsBulletHit   
/// @brief  : 맞았을때의 행동 처리.. 데미지를 준다던지. 이펙트를 출력한다던지.
/// @todo Reg_DAMAGE를 검색해서 타격의 종류를 판단해야...
//--------------------------------------------------------------------------------

bool CObjCHAR::Hitted (CObjCHAR *pFromOBJ, int iEffectIDX, int iSkillIDX, bool bIsSkillEffect, bool bIsBulletHit, bool bJustEffect )
{	
	if ( NULL == pFromOBJ )
		return true;

//-------------------------------------------------------------------------
///박지호::공격자가 유저인자를 판단한다.
#define HIT_AROA_EFF 1613

	BOOL IsAcceptAroa = FALSE;
   if(CCountry::GetSingleton().IsApplyNewVersion())
   {
	   if(pFromOBJ->IsUSER() || pFromOBJ->IsPET())
			IsAcceptAroa = TRUE;
   }
//-------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	/// 맞았을때 풀려야하는 효과들..
	//--------------------------------------------------------------------------------
	m_EndurancePack.ClearStateByHitted();


	D3DXVECTOR3	pos = this->Get_CurPOS();

	//--------------------------------------------------------------------------------
	/// 효과만 적용시킬때는.. ApplyDamage 따위를 하지않는다.물론 타격치도..
	//--------------------------------------------------------------------------------
	if( !bJustEffect )
	{			
		if ( this->m_lDeadTIME ) 
		{
			/// 2003. 12. 26 마지막 한방 처리 수정...
			/// pTarget은 이번에 죽어야 한다.
			/// 죽인넘이 때렸을때 쓰러질까 ? 아님 아무나 때리면 쓰러질까???
			// if ( pTarget->m_DeadDAMAGE.m_nTargetObjIDX == this->Get_INDEX() ) 
			{
				// CObjCHAR *pAttacker = g_pObjMGR->Get_CharOBJ( this->m_DeadDAMAGE.m_nTargetObjIDX, true );
				this->Apply_DAMAGE( pFromOBJ, this->m_DeadDAMAGE.m_wDamage );
				this->m_lDeadTIME = 0;

				uniDAMAGE wDamage;
				wDamage.m_wDamage = this->m_DeadDAMAGE.m_wDamage;
				g_UIMed.CreateDamageDigit( wDamage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );			

				if( wDamage.m_wACTION & DMG_ACT_CRITICAL )
				{
					if( iEffectIDX )
					{
						int iCriticalEffect = EFFECT_HITTED_CRITICAL( iEffectIDX );

						//아로아 이펙트 
						ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iCriticalEffect);

						CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iCriticalEffect, true );
						if ( pHitEFT ) 
						{
							///D3DXVECTOR3 vec( pos.x, pos.y, pos.z + m_fStature );
							///pHitEFT->SetPOSITION( vec );

							pHitEFT->SetPOSITION( this->Get_CurPOS() );

							pHitEFT->InsertToScene();
						}
					}
				}							
			}
		}else
		{
			int iTotalDmage = 0;
			uniDAMAGE stDmage;

			/// 스킬에 의한 데미지는 Damage of skill 로 날라온다.
			if( !bIsSkillEffect )
			{
				/// 서버로부터 전송받은 데미지 처리	
				

				///
				/// 결국 맞는놈이 나이거나
				/// 내가 때리는놈이 맞았거나..
				/// 나의 팻이 때리는경우( 결국 내가때리는 ) .. 의 처리..
				///

				/// 나거나, 내가때리는 놈이라면 서버에 의존한다.
				int iTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->Get_TargetIDX() );
				//--------------------------------------------------------------------------------
				/// Avatar 가 현재 팻을 타고 있으면 팻의 타겟도 검사
				int iPetTargetObject = 0;
				if( g_pAVATAR->GetPetMode() > 0 )
					 iPetTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->m_pObjCART->Get_TargetIDX() );
				//--------------------------------------------------------------------------------
				if( this->IsA( OBJ_USER ) || 
					( this->Get_INDEX() == iTargetObject ) || 
					( this->Get_INDEX() == iPetTargetObject ) )
				{	
					//stDmage = PopTotalDamageFromList( iTotalDmage );
					stDmage = PopCurrentAttackerDamage( pFromOBJ->Get_INDEX() );
					this->Apply_DAMAGE( pFromOBJ, stDmage.m_wDamage );	

					g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

					/// 타격시 흔들림..
					if( stDmage.m_wVALUE )
					{
						if( this->GetPetMode() < 0 )
							this->StartVibration();
						else
						{
							//박 지호::펫 모드에서 흔들림 설정 
							m_IsCartVA = GetPetMode() ? TRUE : FALSE;	
						}
					}
	
					if( stDmage.m_wACTION & DMG_ACT_CRITICAL )
					{
						/// 카메라 진동..
						if ( IsA( OBJ_USER ) ) 
						{
							D3DXVECTOR3 vMin(-10, -10, -10), vMax(10, 10, 50);
							::shakeCamera( g_pCamera->GetZHANDLE(), vMin, vMax, 200 ); // 카메라 흔들림.
						}

						if( iEffectIDX )
						{
							int iCriticalEffect = EFFECT_HITTED_CRITICAL( iEffectIDX );
							
							///아로아 이펙트 
							ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iCriticalEffect);
						
							CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iCriticalEffect, true );
							if ( pHitEFT ) 
							{
								///D3DXVECTOR3 vec( pos.x, pos.y, pos.z + m_fStature );
								///pHitEFT->SetPOSITION( vec );

								pHitEFT->SetPOSITION( this->Get_CurPOS() );

								pHitEFT->InsertToScene();
							}
						}
					}

				}else	/// 클라이언트에서 적절히 처리해야될 데미지..
				{
					///
					/// 뭔가.. 특수한 상황에서 나나, 내가 공격하는 놈이 아닌상황이지만 서버로부터 데미지를 받았다.
					///
					if( !m_DamageList.empty() )
					{
						stDmage = PopTotalDamageFromList( iTotalDmage );
						this->Apply_DAMAGE( pFromOBJ, stDmage.m_wDamage );								

						g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

						/// 타격시 흔들림
						if( stDmage.m_wVALUE )
						{
							if( this->GetPetMode() < 0 )
								this->StartVibration();
						}
					}else
					/// 순수하게 클라이언트에서 처리한다.
					{
						if( iSkillIDX )			
							stDmage.m_wDamage = CCal::Get_SkillDAMAGE ( pFromOBJ, this, iSkillIDX, 1 );
						else
							stDmage.m_wDamage = CCal::Get_DAMAGE ( pFromOBJ, this, 1 );					
					}

					iTotalDmage = stDmage.m_wVALUE;
				}
			}else
			{
				/// 총알이 맞았을경우.. bIsSkillEffect가 세팅되어 아래를 처리해준다.
								
				ProcessSkillHit( pFromOBJ );

				{
					//----------------------------------------------------------------------------------------------------				
					/// @brief 마나볼트같은 장거리 총알 스킬의 경우 데미지로 날라온다.
					//----------------------------------------------------------------------------------------------------
					if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_FIRE_BULLET )
					{
						/// 나거나, 내가때리는 놈이라면 서버에 의존한다.
						int iTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->Get_TargetIDX() );
						if( this->IsA( OBJ_USER ) || ( this->Get_INDEX() == iTargetObject ) )
						{	
							//stDmage = PopTotalDamageFromList( iTotalDmage );
							stDmage = PopCurrentAttackerDamage( pFromOBJ->Get_INDEX() );
							this->Apply_DAMAGE( pFromOBJ, stDmage.m_wDamage );	

							g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );
							/// 타격시 흔들림
							if( stDmage.m_wVALUE )
							{
								if( this->GetPetMode() < 0 )
									this->StartVibration();
							}
						}
					}
				}
			}
		}		
	}


	///
	///	Miss 일경우는 찍지 않는다.
	///

	//if( stDmage.m_wVALUE <= 0 )
	//	return true;
	
	// 타격 효과.
	if ( this->IsVisible() ) 
	{
		// short nEffectIDX = WEAPON_DEFAULT_EFFECT( pFromOBJ->Get_R_WEAPON() );
		if ( iEffectIDX  ) 
		{
			int iHitEffect = iEffectIDX;

			/// skill 로부터의 타격은 직접 file_effect 에서
			/// 일반공격은 무기테이블.. 즉 List_Effect 에서..
			if( !bIsSkillEffect )
				iHitEffect = EFFECT_HITTED_NORMAL( iEffectIDX ); 
			

			
			/// 총알아 맞아서 터질경우....
			if( bIsSkillEffect && bIsBulletHit )
			{
				iHitEffect = SKILL_HIT_EFFECT( iSkillIDX ); 

				if ( iHitEffect ) 
				{
					///아로아 이펙트 
					ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iHitEffect);

					CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iHitEffect, true );
					if ( pHitEFT ) 
					{	
						if( SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) == INVALID_DUMMY_POINT_NUM )
							pHitEFT->LinkNODE ( this->GetZMODEL() );
						else
							this->LinkDummy( pHitEFT->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) );						
							

						///pHitEFT->SetParentCHAR( this );
						///AddExternalEffect( pHitEFT );						

						pHitEFT->UnlinkVisibleWorld ();
						pHitEFT->InsertToScene ();
					}
				}
			}else
			// 일반 적인경우..
			{
				if ( iHitEffect ) 
				{
					///아로아 이펙트 
					ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iHitEffect);

					CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iHitEffect, true );
					if ( pHitEFT ) 
					{					
						pHitEFT->LinkNODE ( this->GetZMODEL() );

						///pHitEFT->SetParentCHAR( this );
						///AddExternalEffect( pHitEFT );						

						pHitEFT->UnlinkVisibleWorld ();
						pHitEFT->InsertToScene ();
					}
				}
			}


			//----------------------------------------------------------------------------------------------------			
			/// @brief Hit sound 출력
			//----------------------------------------------------------------------------------------------------
			int iHitSound = 0;
			if( !bIsSkillEffect )
			{
				iHitSound = EFFFCT_HIT_SND_IDX( iEffectIDX );
			}else
			{
				iHitSound = SKILL_HIT_SOUND( iSkillIDX );
			}

			if ( iHitSound ) 
			{
				g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );				
			}
			//----------------------------------------------------------------------------------------------------


		}
	}


#ifdef	__VIRTUAL_SERVER		
	g_pNet->Send_cli_DAMAGE( pFromOBJ, this, stDmage.m_wDamage );
#endif
	return true;
}


//--------------------------------------------------------------------------------
/// class : ChangeHittedEffect
/// 박지호: 아로아 및 카트 스킬 이펙트를 설정한다. 
//--------------------------------------------------------------------------------
void CObjCHAR::ChangeHittedEffect(CObjCHAR* pAttackObj,BOOL bA,int& hitIDX)
{

/*
	//카트 스킬 이펙트 
	if(pAttackObj->GetUseCartSkill())
	{
		hitIDX = SKILL_HIT_EFFECT(pAttackObj->GetCartSKIDX());
		pAttackObj->GetUseCartSkill() = FALSE;
	}
*/
	//아로아 이펙트 
	if(bA)
	{
		CObjCHAR* pTg = pAttackObj;
		
		//펫이라면...
		if(pAttackObj->IsPET())
		{	
			CObjCART* pCart = (CObjCART*)pAttackObj;
			 if(pCart)
				 pTg = pCart->GetParent();
		}

		if(pTg && bA && pTg->SetAroaState())
			hitIDX = HIT_AROA_EFF;
	}

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 공격 거리를 구한다. 스킬에 의한공격이라면 스킬테이블에서 무기에 의한 일반 공격이라면 무기테이블에서
//--------------------------------------------------------------------------------

int CObjCHAR::Get_AttackRange()
{	
	/// 스킬에 공격 거리가 입력되어 있다면 스킬거리 아니면 무기 거리..
	if ( this->m_nToDoSkillIDX > 0 ) 
	{
		if( this->m_nToDoSkillIDX < g_SkillList.Get_SkillCNT() )
		{
			if(	SKILL_DISTANCE( this->m_nToDoSkillIDX ) ) 
			{
				return SKILL_DISTANCE( this->m_nToDoSkillIDX );
			}
		}else
			assert( 0 && "Get_AttackRange Failed[ ToDoSkillIDX is invalid ]" );
	}

	if ( Get_R_WEAPON() )
	{
		return WEAPON_ATTACK_RANGE( Get_R_WEAPON() ) + ( Get_SCALE() * 120 );
	}

	// Default attack range...
	return ( Def_AttackRange () + ( Get_SCALE() * 120 ) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjCHAR::Set_EMOTION (short nEmotionIDX)
{	
	;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjCHAR::Special_ATTACK ()
{
	// CObjCHAR *pTarget = Get_TARGET ();

	;
	;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 워프.. 현재 위치를 갱신하고 워프.. 지형갱신..
//--------------------------------------------------------------------------------

void CObjCHAR::Warp_POSITION (float fX, float fY)
{
	SetCMD_STOP ();

	m_PosCUR.x = fX;
	m_PosCUR.y = fY;

	g_pTerrain->SetCenterPosition( fX, fY );

	DropFromSky( fX, fY );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 하늘과 충돌.. 그위치로..
//--------------------------------------------------------------------------------

void CObjCHAR::DropFromSky ( float fX, float fY )
{
	m_PosCUR.x = fX;
	m_PosCUR.y = fY;
	m_PosCUR.z = ::g_pTerrain->GetHeightTop(fX, fY);

	::setPosition ( m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );
	::savePrevPosition( m_hNodeMODEL );

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 모델을 특정방향으로 회전.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_ModelDIR (t_POSITION &PosToView, bool bImmediate )
{
	::setModelDirectionByPosition ( this->m_hNodeMODEL, PosToView.x, PosToView.y );	
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 모델을 특정방향으로 회전.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_ModelDIR( float fAngleDegree, bool bImmediate )
{
	::setModelDirection( this->m_hNodeMODEL, fAngleDegree, bImmediate );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 모델을 특정방향으로 회전.
//--------------------------------------------------------------------------------

void CObjCHAR::Add_ModelDIR( float fAngleDegree )
{
	float fDir = ::getModelDirection( this->m_hNodeMODEL );

	::setModelDirection( this->m_hNodeMODEL, fAngleDegree + fDir, true );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 이동 마무리..
//--------------------------------------------------------------------------------

void CObjCHAR::Move_COMPLETED ()
{
	/*
	케릭터의 이동 속도를 클라이언트가 결정하면 안됨...
	if ( !this->IsUSER() ) 
	{
		this->m_fMoveCmPerSec = this->Get_DefaultSPEED();
	}
	*/
	/// x-y 위치만 강제 이동. 안그러면 투명몹 가능성 있음.
	/// z 위치는 우선 이전위치와 동일하게 세팅.
	m_PosGOTO.z = m_PosCUR.z;
	m_PosCUR = m_PosGOTO;	
	
	::setPosition (this->m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);

	/// 2003. 12.16 :: 패킷 수신후 보정되었던 이동 속도를 현재 상태의 속도로 바꾼다.
	m_fAdjustSPEED = this->Get_DefaultSPEED();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief : 이동시 시작 위치의 높이값을 서버에서 받아온 값으로 설정하여 높이를 맞춘다.
//--------------------------------------------------------------------------------

void CObjCHAR::Reset_Position ()
{
	return;

	///m_PosCUR.z = m_PosGOTO.z; // 목표 지점의 z 값이 실은 시작 위치의 높이 값이다.
	///::setPositionVec3( this->m_hNodeMODEL, m_PosCUR );
}


//----------------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : RECOVER_STATE_CHECK_TIME 간격으로 Get_RecoverHP에서 구한 HP를 더해 준다.
///			- 서버에서의 HP를 받았을때의 클라이언트에서의 차를 일정양만큼씩 여기서 더해준다.
///			- arua 상태일경우 50% 증가 시켜준다 : 2005/07/12 - nAvy
//----------------------------------------------------------------------------------------

void CObjCHAR::RecoverHP( short nRecoverMODE )
{
	int iRecoverHP = Get_RecoverHP( nRecoverMODE );	
	int iAruaAddHP = (m_IsAroa)?iRecoverHP >> 1:0;

	Add_HP( iRecoverHP + iAruaAddHP );
	
//	_RPT2( _CRT_WARN,"RecoverHP:%d, AruaAddHP:%d\n", iRecoverHP, iAruaAddHP );

	int iReviseConstHP = iRecoverHP;//Get_MaxHP() / 30;///매틱마다 서버와의 차이를 줄이기 위한 보정값
	if( iReviseConstHP < 10 )
		iReviseConstHP = 10;


	if( m_ReviseHP > 0 )
	{
		if( m_ReviseHP > iReviseConstHP )
		{
			Add_HP( iReviseConstHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", iReviseConstHP );
			m_ReviseHP -= iReviseConstHP;
		}
		else
		{
			Add_HP( m_ReviseHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", m_ReviseHP );
			m_ReviseHP = 0;
		}
	}
	else if( m_ReviseHP < 0 )
	{
		if( abs(m_ReviseHP) > iReviseConstHP )
		{
			Add_HP( -iReviseConstHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", -iReviseConstHP );
			m_ReviseHP += iReviseConstHP;
		}
		else
		{
			Add_HP( m_ReviseHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", m_ReviseHP );
			m_ReviseHP = 0;
		}
	}

	int iMaxHP = Get_MaxHP();
	if( Get_HP() > Get_MaxHP() )
		Set_HP( Get_MaxHP() );					
}

//----------------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : RECOVER_STATE_CHECK_TIME 간격으로 Get_ReocverMP에서 구한 MP를 더해 준다.
///			- 서버에서의 MP를 받았을때의 클라이언트에서의 차를 일정양만큼씩 여기서 더해준다.
///			- arua 상태일경우 50% 증가 시켜준다 : 2005/07/12 - nAvy
//----------------------------------------------------------------------------------------

void CObjCHAR::RecoverMP( short nRecoverMODE )
{
	int iRecoverMP = Get_RecoverMP( nRecoverMODE );
	int iAruaAddMP = (m_IsAroa)?iRecoverMP >> 1:0;

	Add_MP( iRecoverMP + iAruaAddMP );

	int iReviseConstMP = iRecoverMP;//Get_MaxMP() / 30;///매틱마다 서버와의 차이를 줄이기 위한 보정값
	if( iReviseConstMP < 10 )
		iReviseConstMP = 10;

	if( m_ReviseMP > 0 )
	{
		if( m_ReviseMP > iReviseConstMP )
		{
			Add_MP( iReviseConstMP );
			m_ReviseMP -= iReviseConstMP;
		}
		else
		{
			Add_MP( m_ReviseMP );
			m_ReviseMP = 0;
		}
	}
	else if( m_ReviseMP < 0 )
	{
		if( abs(m_ReviseMP) > iReviseConstMP )
		{
			Add_MP( -iReviseConstMP );
			m_ReviseMP += iReviseConstMP;
		}
		else
		{
			Add_MP( m_ReviseMP );
			m_ReviseMP = 0;
		}
	}

	if( Get_MP() > Get_MaxMP() )
		Set_MP( Get_MaxMP() );			
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 캐릭터 높이 갱신. 각종 충돌이 여기서 처리된다.
//--------------------------------------------------------------------------------

void CObjCHAR::Adjust_HEIGHT ()
{
	m_pCollision->UpdateHeight( this );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 특정 액션 중간에 장비를 교체했을시( 특히 무기 ) 바뀐 장비에 맞는 모션으로 교체
//----------------------------------------------------------------------------------------------------

void CObjCHAR::UpdateMotionByChangingEquip()
{
	if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	{
		CObjAVT* pAvt = (CObjAVT*)this;
		/// 무기를 바꿀께 있다면..
		if( pAvt->GetUpdateMotionFlag() || pAvt->GetChangeWeaponR() || pAvt->GetChangeWeaponL() )
		{
			/// 새로운 모션 세팅을 위한 세로운 명령 세팅
			switch( pAvt->Get_COMMAND() )
			{
				case CMD_STOP:						
					{
						pAvt->SetCMD_STOP( );
					}
					break;
				case CMD_MOVE:
					{
						//pAvt->SetCMD_MOVE( pAvt->m_PosGOTO.x, pAvt->m_PosGOTO.y, pAvt->m_bRunMODE );
						pAvt->Set_MOTION( pAvt->GetANI_Move(), pAvt->m_fCurMoveSpeed, pAvt->m_fRunAniSPEED );
					}
					break;
				case CMD_ATTACK:
					{
						pAvt->SetCMD_ATTACK( pAvt->m_iServerTarget );
						//pAvt->Set_MOTION( pAvt->GetANI_Attack(), pAvt->m_fCurMoveSpeed, pAvt->m_fRunAniSPEED );
					}
					break;
				case CMD_DIE:
					break;
				case CMD_PICK_ITEM:
					break;
				case CMD_SKILL2SELF:
					break;
				case CMD_SKILL2OBJ:
					break;
				case CMD_SKILL2POS:
					break;
				case CMD_RUNAWAY:
					break;
			}

			//pAvt->Update ();		

			/// if pCHAR is my avatar, update ability
			if( pAvt->IsA( OBJ_USER ) )
			{
				((CObjUSER*)pAvt)->UpdateAbility();
			}

			pAvt->SetChangeWeaponR( 0 );
			pAvt->SetChangeWeaponL( 0 );

			pAvt->SetUpdateMotionFlag( false );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 캐릭터 처리함수
/// @todo 테스트 코드 빼라...//if (this->Get_TYPE() != OBJ_MOB ) 
/// @todo View list 고쳐라..
//--------------------------------------------------------------------------------

int CObjCHAR::Proc (void)
{
	/*
	*
	* for debugging...
	*
	*/

	//if( m_iCastingStartTime != NULL )
	//{
	//	if( ( g_GameDATA.GetGameTime() - m_iCastingStartTime ) > SKILL_PROC_LIMIT )
	//	{
	//		///Casting_END();
	//		assert( 0 && "Casting time expired" );
	//	}	
	//}

#if defined(_GBC)	
//--------------------------------------------------------------------------------
		//박지호::카트 뒷자석에 유저를 탑승 시켰다면 카트위치를 2인승 유저에게 업데이터 한다. 
		if(this->GetPetMode() > -1)
		{
			if(m_pObjCART->m_pRideUser)
			{	
				float Pos1[3] = { 0 };

				//2인승 더미 위치값을 가져온다. 
				::getDummyPosition(m_pObjCART->GetZMODEL(),10,Pos1);
				//2인승 유저 위치 업데이터 
				m_pObjCART->m_pRideUser->Set_CurPOS(D3DXVECTOR3(Pos1[0],Pos1[1],Pos1[2])); 	
			}
		}
//--------------------------------------------------------------------------------
#endif 


	//--------------------------------------------------------------------------------
	/// 오브젝트 바이브레이션 업데이트.~~ 흔들자~~
	//--------------------------------------------------------------------------------
    m_ObjVibration.Proc();



	//--------------------------------------------------------------------------------
	/// 외부 데코레이션들 업데이트	
	//--------------------------------------------------------------------------------
	m_EndurancePack.Update();

	
	//--------------------------------------------------------------------------------
	/// visibility 변화
	/// 캐릭터 사망시 아예 모션이 업데이트 안되는경우가 있다.. 이것이 아래 코드와 연관이 있는가? -04/5/25
	//--------------------------------------------------------------------------------
	if( m_pChangeVisibility != NULL )
	{
		/// if expired?
		if( m_pChangeVisibility->ProcessVisibility( g_GameDATA.GetElapsedFrameTime() ) == false )
		{
			delete m_pChangeVisibility;
			m_pChangeVisibility = NULL;

			this->m_lDeadTIME = 0;
			return 0;
		}

		assert( !(this->IsA( OBJ_USER )) && " 뭐냥 유젼데 왠 비져비리티? " );

		return 1;
	}


	//--------------------------------------------------------------------------------
	/// Update check frame
	//--------------------------------------------------------------------------------
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	m_dwFrameElapsedTime = dwCurrentTime - m_dwLastRecoveryUpdateTime;		/// 이전프레임에서 현재 프레임 사이에 흐른시간을 더해준다.
	m_dwLastRecoveryUpdateTime = dwCurrentTime;




	//--------------------------------------------------------------------------------
	/// 리커버관련 처리	
	/// 04/5/28 CObjAVT::Proc 로이동..
	//--------------------------------------------------------------------------------
	//if( this->Is_AVATAR() )
	//{
	//	switch ( Get_COMMAND() ) 
	//	{
	//		case CMD_SIT :
	//			this->RecoverHPnMP( RECOVER_STATE_SIT_ON_GROUND );
	//			break;
	//		default:
	//			/// 명령이 sit 에서 바뀌거나.. 연속적인 sit 처리가 아니라면 시간 리셋..
	//			m_dwElapsedTime = 0;
	//			break;	
	//	}

	//	//--------------------------------------------------------------------------------
	//	/// 시간에 따른 액션 모드 처리..
	//	/// if 문 줄이기 위해서 이쪽으로 이동.. 04/5/28
	//	//--------------------------------------------------------------------------------
	//	///if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	//	{
	//		m_ChangeActionMode.Proc();
	//	}		
	//}	
	
#ifdef _DEBUG
	/*
	if( ::getUserData( GetZMODEL() ) )
	{
		_ASSERT( ::getUserData( GetZMODEL() ) );

		const char* pStrName = ::getName( GetZMODEL() );
		D3DVECTOR vec;
		::getPosition( GetZMODEL(), (float*)( &vec ) );
	}
	*/
#endif

		
	//--------------------------------------------------------------------------------
	/// 일단은 모든 오브젝트 캐릭터를 다 리스트에..
	/// 미니맵 출력을 위한 리스트
	//--------------------------------------------------------------------------------
	g_pObjMGR->AddViewObject( m_nIndex );


	//--------------------------------------------------------------------------------
	/// 거리에 따른 오브젝트들의 관리( 엔진에 등록 혹은 뺀다.. )
	/// 2004/3/17 클라이언트 차원에서 거리에 따른 모델의 관리는 불필요하다. 서버에서 난라온건 다 관리해준다.
	/// 필요할경우 LOD 적용이 반영된다.
	/// 2004/7/8 InsertToScene/removeFromScene 등은 필요없다 서버에서 받는데로 추가하고 삭제하라..
	//--------------------------------------------------------------------------------
	bool bInViewfrustum = false;
	///int iDistance = CD3DUtil::distance ((int)g_GameDATA.m_PosCENTER.x, (int)g_GameDATA.m_PosCENTER.y, (int)m_PosCUR.x, (int)m_PosCUR.y);
	///if ( iDistance < 8*4*nGRID_SIZE ) 
	{
		///this->InsertToScene ();

		// char name & chatting message 
		if ( ::inViewfrustum( this->GetZMODEL() ) ) // 뷰프러스텀 안에 있으면 참, 없으면 거짓
		{ 
			bInViewfrustum = true;
			if (this->Get_TYPE() != OBJ_MOB ) 
			{
				g_pViewMSG->AddObjIDX( m_nIndex );
			}
		}		
	} /*else
	{
		this->RemoveFromScene ();
	}*/



	//--------------------------------------------------------------------------------
	/// 뷰프러스텀 안에 있는 오브젝트들만 높이값 변경(부하 많이 먹기 때문)
	//--------------------------------------------------------------------------------
	if ( bInViewfrustum ) // 뷰프러스텀 안에 있는 오브젝트들만 높이값 변경(부하 많이 먹기 때문)
	{ 
		/// 내가 누군가에에 링크되어있다면.. 충돌처리 안함..
		//조성현 캐릭터 변신할때...
		//if(m_bDisguise)
		//{
		//	::setScale(this->m_hNodeMODEL, 1.0f, 1.0f, 1.0f);
		//}
		
		


		if( this->IsChild() == false )
			Adjust_HEIGHT ();
	
	    
		//조성현 캐릭터 변신할때...
		//if(m_bDisguise)
		//{
		//	::setLightingRecursive(this->GetZMODEL(), g_GameDATA.m_hCharacterLight2);
		//	::setVisibilityRecursive(this->GetZMODEL(), 0.15f);
		//	::setScale(this->GetZMODEL(), 0.7f, 0.7f, 0.7f);
		//	::setVisibleGlowRecursive( this->GetZMODEL(), 2, 0.2f, 0.5f, 0.945f );
		//}
	
	}
	else 
	{

#if (1) /// 현재, 이동 시작시 높이 보정이 되기 때문에, 높이 보정 필요 없음.	
		::getPosition( this->m_hNodeMODEL, (float*)m_PosCUR );
		//m_PosCUR.z = g_pAVATAR->Get_CurPOS().z; // 안보이는 캐릭터의 높이는 주인공 아바타의 높이에 맞춤.
		
		/// 아바타 이거나( 카트를 탄상태가 아닌), 내가 카드일경우에만..
		///if ( ( this->Is_AVATAR() && ( this->GetPetMode() < 0 ) ) || 
		///	this->IsPET() ) 
		{
			D3DXVECTOR3 vAvatarPos	= g_pAVATAR->Get_CurPOS();
			float fDistanceSquare2D = (vAvatarPos.x - m_PosCUR.x)*(vAvatarPos.x - m_PosCUR.x) + (vAvatarPos.y - m_PosCUR.y)*(vAvatarPos.y - m_PosCUR.y);
			const float fMinDistanceSquare2D = 4000.0f*4000.0f;
			// 아주 가까운 거리에 있을 경우에만 높이 조정
			if ( fDistanceSquare2D < fMinDistanceSquare2D ) 
			{
				Adjust_HEIGHT();
			//	// 이동 중일 거라는 가정이 성립한다면, 현재 위치가 바른 위치이다.
			//	//m_PosCUR.z = g_pTerrain->GetHeightTop(m_PosCUR.x, m_PosCUR.y);
			//	::setPosition (m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);
			}
		}
#endif

	}


	//--------------------------------------------------------------------------------
	/// 모션 프레임이 끝났는가?
	//--------------------------------------------------------------------------------	
	m_bFrameING = this->ProcMotionFrame ();	

	//--------------------------------------------------------------------------------
	// 모션 루프가 끝났다.
	//--------------------------------------------------------------------------------
	if ( !m_bFrameING ) 
	{	
		//--------------------------------------------------------------------------------
		/// 항상 ActionSkillIDX 를 스킬 액션 시작시 DoingSkill 에 등록시키고 모션이 끝나면 리셋..( 모션프레임에서 스킬번호를 참조해야하기 때문에 )
		//--------------------------------------------------------------------------------
		m_nDoingSkillIDX = 0;

//박지호::카트 공격시 캐릭터를 않힌다. 
#if defined(_GBC)
		Set_SitMode();
#endif
		// { added by zho 2003-12-17
		// 검잔상 중지
		for (short nI=0; nI<2; nI++) 
		{
			if ( m_hTRAIL[ nI ] ) 
			{
				::controlTrail ( m_hTRAIL[ nI ], 3 );	// 점차 사라짐.
			}
		}				

		//--------------------------------------------------------------------------------
		/// 장비교체에 따른 적당한 모션 교체..
		//--------------------------------------------------------------------------------
		UpdateMotionByChangingEquip();

	} else if ( Get_STATE() & CS_BIT_INT )	// 이동작중에는 암거두 할수 없다.
	{
		return 1;
	}

	//--------------------------------------------------------------------------------
	/// 큐에 쌓인 명령 소진...
	/// CanApplyCommand 가 false 일때는 적용이 불가능하다.
	/// 현재는 m_bCastingSTART = true 일때만이다.
	//--------------------------------------------------------------------------------
	ProcQueuedCommand();


	//--------------------------------------------------------------------------------
	/// 캐스팅을 위한 틀별한 처리
	/// Skill_START 에서 m_bCastingSTART = true 로 만든다.
	/// 그러나 마지막 스킬액션이 끝난 시점을 알수 없으므로.. 일단
	/// 이것에 의존한다.
	/// 04/5/28 - Skill 시전( DO_SKILL 함수 ) 를 리팩토링 하면서 없어짐.( 필요가 없게 되었다. )
	//--------------------------------------------------------------------------------
	///Casting_END ();




	//--------------------------------------------------------------------------------
	/// 데미지 타임아웃 체크
	//--------------------------------------------------------------------------------
	this->ProcDamageTimeOut();

	//--------------------------------------------------------------------------------
	/// Effect_Of_Skill 타임아웃 체크
	//--------------------------------------------------------------------------------
	ProcTimeOutEffectedSkill();

	//--------------------------------------------------------------------------------
	/// 서버로 부터 죽음을 통보 받았다...
	//--------------------------------------------------------------------------------
	if ( this->m_lDeadTIME ) 
	{
		CObjCHAR *pAttacker = g_pObjMGR->Get_CharOBJ( this->m_DeadDAMAGE.m_nTargetObjIDX, true );

		/// 2003. 12. 26 마지막 한방 처리 수정...
		/// 난 죽은지 10초가 지났는데, 아무도 날 때리지 않는 경우...
		if ( g_GameDATA.GetGameTime() - this->m_lDeadTIME >= 1000 * 5 
				|| pAttacker == NULL  
				|| pAttacker->m_iServerTarget != g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ) ) 
		{
			this->Apply_DAMAGE( pAttacker, this->m_DeadDAMAGE.m_wDamage );

			/// 자살일경우.. 데미지 표시하지 않음..
			if( pAttacker != this )
			{
				uniDAMAGE wDamage;
				wDamage.m_wDamage = this->m_DeadDAMAGE.m_wDamage;
				
				D3DXVECTOR3	pos = this->Get_CurPOS();
				g_UIMed.CreateDamageDigit( wDamage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );				

				this->m_lDeadTIME = 0;				
			}else
			{
				this->m_lDeadTIME = 100;				
			}			
			return 1;
		}
	}




	
	/// if current hp <= 0 do CMD die
	if( Get_HP() == 0 )
	{
		_ASSERT( 0 && "/// if current hp <= 0 do CMD die" );
		this->SetCMD_DIE();
	}

	

	switch( Get_COMMAND() ) 
	{
		case CMD_DIE	: 
			return this->IsUSER ();

		case CMD_SIT	:
			return ProcCMD_SIT ();

		case CMD_STOP   :
			return ProcCMD_STOP ();

		case CMD_MOVE	   :
			return ProcCMD_MOVE ();

		case CMD_PICK_ITEM :
			return ProcCMD_PICK_ITEM ();

		case CMD_ATTACK :
			return	ProcCMD_ATTACK ();			
				
		case CMD_SKILL2SELF :
			return ProcCMD_Skill2SELF ();

		case CMD_SKILL2POS :
			return ProcCMD_Skill2POSITION ();

		case CMD_SKILL2OBJ :
			return ProcCMD_Skill2OBJECT ();
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: Set_SitMode() 
///       : 카트가 공격할때 아바타를 앉친다		
///				
//--------------------------------------------------------------------------------
void CObjCHAR::Set_SitMode(void)
{
	
	if(this->GetPetMode() > 0)
	{
//		if(this->m_pObjCART->Get_COMMAND() == CMD_ATTACK)
			this->Set_MOTION(this->m_pObjCART->GetRideAniPos());
	}

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: ProcCMD_Skill2OBJECT_PET() 
///       : 펫 상태의 타켓스킬의 설정한다. 				
///				
//--------------------------------------------------------------------------------
int CObjCHAR::ProcCMD_Skill2OBJECT_PET()
{
	
	CObjCHAR *pTarget = CSkillManager::GetSkillTarget( m_iServerTarget, (this->m_nToDoSkillIDX)? this->m_nToDoSkillIDX : this->m_nActiveSkillIDX);
		
	if ( pTarget ) 
	{
		//-----------------------------------------------------------------------------------------
		/// 아직 스킬 캐스팅을 시작하지 않았다면..
		//-----------------------------------------------------------------------------------------	
		if ( !m_bCastingSTART )
		{
			int iAttackRange = this->Get_AttackRange();
			
			//타켓 정보를 설정한다. 
			m_pObjCART->m_PosGOTO       = pTarget->Get_CurPOS();
			m_pObjCART->Set_TargetIDX(m_iServerTarget);

			if( m_pObjCART->Goto_TARGET( pTarget, iAttackRange ) == false )
			{	m_pObjCART->SetCMD_MOVE(pTarget->Get_CurPOS(),TRUE);	return 1;	}
			else
			{
				m_pObjCART->Set_STATE(CS_STOP);
				m_pObjCART->m_fCurMoveSpeed = 0;
				
				Set_STATE(CS_STOP);
				m_fCurMoveSpeed = 0; 
			}

			if( !bCanStartSkill() ) 
			{
				//Pet
				m_pObjCART->Set_STATE(CS_STOP);
				m_pObjCART->m_fCurMoveSpeed = 0;

				//아바타 
				Set_STATE(CS_STOP);
				m_fCurMoveSpeed = 0; 
				///this->Set_MOTION( this->GetANI_Casting() );
				return 1;
			}			

			m_pObjCART->Set_ModelDIR( pTarget->m_PosCUR );
			
		}	

		m_pObjCART->Set_ModelDIR( pTarget->m_PosCUR );
	
		//-----------------------------------------------------------------------------------------
		/// 타겟이 죽어버렸다...
		//-----------------------------------------------------------------------------------------			
		if( pTarget->m_bDead )
		{
			SetEffectedSkillFlag( true );
		}

		//스킬 타입을 가져와서 카트가 스킬을 발동할지  체크한다. 
		int sType = SKILL_TYPE(this->m_nToDoSkillIDX);

		///카트::캐스팅 / 실제동작 적용...
		if(sType == -1)
		{
			m_pObjCART->m_nToDoSkillIDX	= this->m_nToDoSkillIDX;
			
			if(m_pObjCART->m_SkillActionState == 0)
				m_pObjCART->m_SkillActionState = 1; 

			m_pObjCART->Do_SKILL(this->Get_TargetIDX(),pTarget);
		}
		///아바타::캐스팅 / 실제동작 적용...
		else
		{ 	this->Do_SKILL(this->Get_TargetIDX(),pTarget);		}	

	} 

	else
	{
		SetEffectedSkillFlag( true );
		m_nActiveSkillIDX = 0;
		Casting_END();		
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: SetNewCommandAfterSkill_PET() 
///       : 펫 상태의 타켓스킬의 설정한다. 				
///				
//--------------------------------------------------------------------------------
void CObjCHAR::SetNewCommandAfterSkill_PET(int iSkillNO)
{
	
	switch( SKILL_ACTION_MODE( iSkillNO ) )
	{
		case SA_STOP:
			{
				m_pObjCART->Set_COMMAND(CMD_STOP);
				this->Set_COMMAND(CMD_STOP);
				
				break;
			}
			
		case SA_ATTACK:
			{
				CObjCHAR* pTarget = (CObjCHAR*)(this->Get_TargetOBJ());
				//g_pObjMGR->Get_ClientCharOBJ(this->m_iServerTarget,false);
				
				if(pTarget)
				{
					/// 나일경우 PVP존이 아닌존에서 유져공격명령은 취소한다.
					if(this->IsA( OBJ_USER ) && pTarget->IsUSER() )
					{
						if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
						{ m_pObjCART->Set_COMMAND(CMD_STOP); this->Set_COMMAND(CMD_STOP); }
					}
					else
					{
						// 죽을때 까지 공격 !!!
						m_pObjCART->Start_ATTACK(pTarget);
						m_pObjCART->Set_COMMAND(CMD_ATTACK);
						this->Set_COMMAND(CMD_ATTACK);
					}
				}
				else
				{
					m_pObjCART->Set_COMMAND(CMD_STOP);
					this->Set_COMMAND(CMD_STOP);
				}
				
				break;
			}
		
		case SA_RESTORE:
			{
				//이전 공격으로 설정
				this->Set_COMMAND(this->Get_BECOMMAND());
				this->Set_BECOMMAND(CMD_STOP);
				
				if(m_pObjCART->Get_COMMAND() == CMD_ATTACK)
				{
					CObjCHAR* pTarget = (CObjCHAR*)(this->Get_TargetOBJ());
					//g_pObjMGR->Get_ClientCharOBJ(this->m_iServerTarget,false);

					if(pTarget)
					{
						/// 나일경우 PVP존이 아닌존에서 유져공격명령은 취소한다.
						if(this->IsA( OBJ_USER ) && pTarget->IsUSER() )
						{
							if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
							{ m_pObjCART->Set_COMMAND(CMD_STOP); this->Set_COMMAND(CMD_STOP); }
						}
						// 아니면 공격을 시도한다. 
						else
						{
							m_pObjCART->Start_ATTACK(pTarget);
							m_pObjCART->Set_COMMAND(CMD_ATTACK);
							this->Set_COMMAND(CMD_ATTACK);
						}
					}
				}
			}

	}

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: SetRideUser() 
///       : 2인승을 한다. 			
///				
//--------------------------------------------------------------------------------
BOOL CObjCHAR::SetRideUser(WORD irideIDX)
{
	//펫모드...
	if(this->GetPetMode() < 0)
		return FALSE;

	//카트생성...
	if(this->m_pObjCART == NULL)
		return FALSE;

	m_iRideIDX = g_pObjMGR->Get_ClientObjectIndex(irideIDX);

	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(m_iRideIDX,false);
		if(pTarget == NULL)
			return FALSE;

	//펫 모드 설정 
	pTarget->SetPetType(this->GetPetMode());
	pTarget->m_pObjCART = this->m_pObjCART;
	pTarget->m_IsRideUser = TRUE;

	//카트에 탑승을 한다. 
	this->m_pObjCART->Create(pTarget);

	pTarget->Set_COMMAND(CMD_STOP);
	pTarget->Set_STATE(CS_STOP);


	//아바타 무기 및 날개는 안보이도록 설정 
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART) 
		{
			short nI;			

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] ) 
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 0.0f );
				}
			}
		}
	}

	return TRUE;

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: ReleaseRideUser() 
///       : 2인승을 해제한다.  			
///				
//--------------------------------------------------------------------------------
void CObjCHAR::ReleaseRideUser(void)
{

	if(m_iRideIDX == 0)
		return;

	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(m_iRideIDX,false);
	if(pTarget == NULL)
		return;

	m_iRideIDX = 0;

	//펫 모드 설정 
	pTarget->SetPetType(-1);
	pTarget->m_pObjCART = NULL;
	pTarget->m_IsRideUser = FALSE;
	
	pTarget->Set_STATE(CS_STOP);
	pTarget->SetCMD_STOP();


	//아바타 무기 및 날개는 보이도록 설정 
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART) 
		{
			short nI;			

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] ) 
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
				}
			}
		}
	}	

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: ReleaseRideUser() 
///       : 카트 보조에 탑승 했다면 자신은 뛰어 내린다. 			
///				
//--------------------------------------------------------------------------------
void CObjCHAR::ReleaseRideUser(WORD irideIDX)
{
	
	//m_iRideIDX = g_pObjMGR->Get_ClientObjectIndex(irideIDX);
	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(irideIDX,false);
		if(pTarget == NULL)
			return;

	pTarget->m_pObjCART->UnLinkChild(1);

	//부모의 연결해제 
	pTarget->m_pObjCART->GetParent()->m_iRideIDX	= 0;
	pTarget->m_pObjCART->GetParent()->m_pRideUser	= NULL;
	//펫 모드 설정 
	pTarget->SetPetType(-1);
	pTarget->m_pObjCART = NULL; 
	pTarget->m_IsRideUser = FALSE;
	
	pTarget->Set_STATE(CS_STOP);
	//pTarget->SetCMD_STOP();

	//뛰어 내리는 모션을 설정한다. 
	pTarget->Set_MOTION(SKILL_ANI_ACTION_TYPE(27),0,1,false,1);
	
	
	//아바타 무기 및 날개는 보이도록 설정 
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART) 
		{
			short nI;			

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] ) 
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
				}
			}
		}
	}	

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: Ride_Cansel_Motion() 
///       : 탑승 거부 모션을 설정한다. 
///				
//--------------------------------------------------------------------------------
void CObjCHAR::Ride_Cansel_Motion(void)
{
	
	this->Set_COMMAND(CMD_STOP);
	this->SetCMD_STOP();
	
	this->Set_MOTION(453,0,1,false,1);

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// class : Stop_Cart
/// 박지호: 카트를 정지 시킨다.  			
///				
//--------------------------------------------------------------------------------
void CObjCHAR::Stop_Cart(void)
{

	if(!m_pObjCART)
		return;
	
	m_pObjCART->Set_COMMAND(CMD_STOP);
	m_pObjCART->SetCMD_STOP();

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : queuing the command 
///				현재 명령을 적용할수 있는가?
///				현재의 명령을 세팅할수 없는 상태를 파악하라..
//--------------------------------------------------------------------------------

bool CObjCHAR::CanApplyCommand()
{
	/// 캐스팅이 시작되었다면 적용할수 없음..
	/// 또 result_of_skill 을 받았다면.. 결국 현재 시전할 스킬이 끝나기 전에는 모든 명령을 큐에넣는다.
	/// 이미 서버에선 결과가 적용된거기때문에 클라이언트도 무조건 스킬을 시전해야된다.
	if( this->m_bCastingSTART && bCanActionActiveSkill() )
	{
		return false;
	}

	/// 현재 수행되어야할 명령큐가 비어있지 않다면 먼저 큐의 명령을 수행해야하므로..
	if( this->m_CommandQueue.IsEmpty() == false )
	{
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 쌓여진 명령들을 수행한다.
///			  매프레임 수행해야하는가?
//--------------------------------------------------------------------------------

void CObjCHAR::ProcQueuedCommand()
{
	if( m_CommandQueue.IsEmpty() )
		return;

	/// 현재 스킬관련 명령이 수행중이거나 수행해야될 스킬이 세팅되어 있다면 기다려라..
    if( this->m_bCastingSTART && bCanActionActiveSkill() )
	{
		return;
	}

	/// 일단 제일 마지막껏만 수행
	bool bSkillCommand = false;
	CObjCommand* pCommand = m_CommandQueue.PopLastCommand( bSkillCommand );
		
	if( pCommand )
	{
		pCommand->Execute( this );
		Log_String( LOG_NORMAL, "!!큐의정보로 부터 실행!!\n" ); 

		/// Skill 명령이고 이미 Result 를 받은 명령이라면..
		if( bSkillCommand )
		{
			if( pCommand->bGetResultOfSkil() )
			{
				SetEffectedSkillFlag( true );
				Log_String( LOG_NORMAL, "!!큐의정보로 부터 effectedskillflag 세팅!!\n" ); 
			}
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSit()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_SIT );

	if( pCommand )
	{
		((CObjSitCommand*)pCommand)->SetCMD_SIT( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandStand()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_STAND );

	if( pCommand )
	{
		((CObjStandCommand*)pCommand)->SetCMD_STAND( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandStop()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_STOP );

	if( pCommand )
	{
		((CObjStopCommand*)pCommand)->SetCMD_STOP( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandMove( const D3DVECTOR& PosTO, BYTE btRunMODE )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_MOVE );

	if( pCommand )
	{
		((CObjMoveCommand*)pCommand)->SetCMD_MOVE( PosTO, btRunMODE );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandMove( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_MOVE );

	if( pCommand )
	{
		((CObjMoveCommand*)pCommand)->SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandAttack( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_ATTACK );

	if( pCommand )
	{
		((CObjAttackCommand*)pCommand)->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosTO );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandDie()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_DIE );

	if( pCommand )
	{
		((CObjDieCommand*)pCommand)->SetCMD_DIE( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandToggle( BYTE btTYPE )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_TOGGLE );

	if( pCommand )
	{
		((CObjToggleCommand*)pCommand)->SetCMD_TOGGLE( btTYPE );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Self( short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2SELF );

	if( pCommand )
	{
		((CObjSkill2SelfCommand*)pCommand)->SetCMD_Skill2SELF( nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Obj( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2OBJ );

	if( pCommand )
	{
		((CObjSkill2ObjCommand*)pCommand)->SetCMD_Skill2OBJ( wSrvDIST, PosTO, iServerTarget, nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Pos( const D3DVECTOR& PosGOTO, short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2POS );

	if( pCommand )
	{
		((CObjSkill2PosCommand*)pCommand)->SetCMD_Skill2POS( PosGOTO, nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Start vibration
//----------------------------------------------------------------------------------------------------

void CObjCHAR::StartVibration()
{
	m_ObjVibration.StartVibration();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : 
//--------------------------------------------------------------------------------

void	CObjCHAR::Set_HP (int iHP)
{	
	if( iHP == DEAD_HP )
		m_iHP = iHP;
	else
	{
		if( iHP <= 0 )
			m_iHP = 1;
		else
			m_iHP = iHP;			
	}
}
void	CObjCHAR::Set_MP (int iMP)
{	
	m_iMP = iMP;			
}

int	CObjCHAR::Add_HP (int iAdd)
{	
	m_iHP += iAdd;

	if( m_iHP <= 0 )
	{		
		LogString( LOG_DEBUG,"Caution : HP < 0 @CObjCHAR::Add_HP" );
		m_iHP = 1;
	}

	if( m_iHP > Get_MaxHP() )
		m_iHP = Get_MaxHP();
	return m_iHP;	
}

int	CObjCHAR::Sub_HP (int iSub)
{	
	m_iHP -= iSub;
	if( m_iHP <= 0 )
		m_iHP = 1;

	return m_iHP;	
}


//----------------------------------------------------------------------------------------------------	
/// @brief 캐릭터 위에 이펙트를 붙인다.
//----------------------------------------------------------------------------------------------------
void CObjCHAR::ShowEffectOnCharByIndex( int iEffectIDX, int iSountIDX, bool bWeatherEffect )
{
	if( iEffectIDX )
	{
		if ( this->IsVisible() ) 
		{
			CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
			if( pEffect )
			{
				pEffect->LinkNODE ( this->GetZMODEL() );
				
				pEffect->SetParentCHAR( this );
				

				if( !bWeatherEffect )
					this->AddExternalEffect( pEffect );		
				else
					this->AddWeatherEffect( pEffect );

				
				pEffect->InsertToScene ();
			}
		}
	}

	if ( iSountIDX )
	{	
		g_pSoundLIST->IDX_PlaySound3D( iSountIDX, Get_CurPOS() );		
	}
}

void CObjCHAR::ShowEffectOnCharByHash( int iEffectHash, int iSoundIDX, bool bWeatherEffect )
{		
	if( iEffectHash )
	{
		if ( this->IsVisible() ) 
		{
			CEffect *pEffect = g_pEffectLIST->Add_EFFECT( (t_HASHKEY)iEffectHash, true );
			pEffect->LinkNODE ( this->GetZMODEL() );
			
			pEffect->SetParentCHAR( this );

			if( !bWeatherEffect )
				this->AddExternalEffect( pEffect );		
			else
				this->AddWeatherEffect( pEffect );
			
			pEffect->InsertToScene ();
		}
	}

	if ( iSoundIDX )
	{	
		g_pSoundLIST->IDX_PlaySound3D( iSoundIDX, Get_CurPOS() );		
	}
}

void CObjCHAR::AddWeatherEffect( CEffect* pEffect )
{
	m_WeatherEffectLIST.AllocNAppend( pEffect );
}

void CObjCHAR::DeleteWeatherEffect()
{
	classDLLNODE< CEffect* > *pNode;
	pNode = m_WeatherEffectLIST.GetHeadNode ();
	while( pNode ) 
	{		
		/// 이펙트만 지우고 이펙트의 부모처리는 안한다. 왜냐? 내가 부모니까..
		g_pEffectLIST->Del_EFFECT( pNode->DATA, false );

		m_WeatherEffectLIST.DeleteNFree( pNode );
		pNode = m_WeatherEffectLIST.GetHeadNode( );
	}

	m_WeatherEffectLIST.ClearList();
}


void CObjCHAR::SetClanMark( WORD wMarkBack, WORD wMarkCenter )
{
	m_wClanMarkBack		= wMarkBack;
	m_wClanMarkCenter   = wMarkCenter;
	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}

void CObjCHAR::SetClan( DWORD dwClanID, WORD wMarkBack, WORD wMarkCenter, const char* pszName , int iLevel, BYTE btClanPos )
{
	//assert( pszName && dwClanID );
	if( pszName && dwClanID)
	{

		m_dwClanID			= dwClanID;
		m_wClanMarkBack		= wMarkBack;
		m_wClanMarkCenter   = wMarkCenter;
		m_iClanLevel		= iLevel;
		m_strClanName		= pszName;
		m_strClanName.erase( remove( m_strClanName.begin(), m_strClanName.end(),' ' ) , m_strClanName.end() );
		m_btClanPos			= btClanPos;
	}
}

BYTE  CObjCHAR::GetClanPos()
{
	return m_btClanPos;
}

void CObjCHAR::SetClanPos( BYTE btPos )
{
	m_btClanPos = btPos;
}

DWORD CObjCHAR::GetClanID()
{
	return m_dwClanID;
}

WORD CObjCHAR::GetClanMarkBack()
{
	return m_wClanMarkBack;
}

WORD CObjCHAR::GetClanMarkCenter()
{
	return m_wClanMarkCenter;
}

const char* CObjCHAR::GetClanName()
{
	return m_strClanName.c_str();
}

void CObjCHAR::ResetClan()
{
	m_dwClanID					= 0;
	m_btClanPos					= 0;
	m_wClanMarkBack				= 0;
	m_wClanMarkCenter			= 0;

	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}
void CObjCHAR::SetClanLevel( int iLevel )
{
	m_iClanLevel = iLevel;
}
int CObjCHAR::GetClanLevel()
{
	return m_iClanLevel;
}

void CObjCHAR::SetUserDefinedClanMark( CClanMarkUserDefined* pUserDefinedClanMark )
{ 
	assert( pUserDefinedClanMark );
	m_ClanMarkUserDefined = pUserDefinedClanMark; 
	pUserDefinedClanMark->AddRef();
}

///현재 서버와 클라이언트와 HP양이 틀린경우 그 값을 저장한다.
void CObjCHAR::SetReviseHP( int hp )
{
	m_ReviseHP = hp;
}

///현재 서버와 클라이언트와 MP양이 틀린경우 그 값을 저장한다.
void CObjCHAR::SetReviseMP( int mp )
{
	m_ReviseMP = mp;
}
//-----------------------------------------------------------------------------
/// @brief 아루아 상태일경우 추가 능력치 계산관련: 2005/7/13 - nAvy
//-----------------------------------------------------------------------------
void CObjCHAR::Calc_AruaAddAbility()
{
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		if( m_IsAroa )
			m_AruaAddMoveSpeed   = GetOri_RunSPEED() * 0.2;
		else
			m_AruaAddMoveSpeed   = 0;
	}
}
//-----------------------------------------------------------------------------
/// @brief 공격속도
//-----------------------------------------------------------------------------
short CObjCHAR::Get_nAttackSPEED ()
{	
	int iR = GetOri_ATKSPEED() + m_EndurancePack.GetStateValue( ING_INC_ATK_SPD )
								- m_EndurancePack.GetStateValue( ING_DEC_ATK_SPD );

	return ( iR > 30 ) ? ( iR ) : 30;
}
//-----------------------------------------------------------------------------
/// @brief 엔진에 공격속도변경시 이용되는 메쏘드			: 2005/7/13 - nAvy
//-----------------------------------------------------------------------------
float CObjCHAR::Get_fAttackSPEED ()
{	
	int iR = GetOri_ATKSPEED() + m_EndurancePack.GetStateValue( ING_INC_ATK_SPD )
								- m_EndurancePack.GetStateValue( ING_DEC_ATK_SPD );

	return ( iR > 30 ) ? ( iR/100.f ) : 0.3f;
}
//---------------------------------------------------------------------------------------------------------------
/// @brief 엔진에 공격속도변경시 이용되는 메쏘드 : 아루아 상태일경우 추가 속도 증가하도록 수정 : 2005/7/13 - nAvy
//---------------------------------------------------------------------------------------------------------------
float CObjCHAR::Get_MoveSPEED ()			
{	
	return m_fAdjustSPEED;		
}	

//-----------------------------------------------------------------------------
/// @brief 아루아 상태일경우 추가 이동속도 증가하도록 수정 : 2005/7/13 - nAvy
//-----------------------------------------------------------------------------
float CObjCHAR::Get_DefaultSPEED ()		
{	
	if ( !m_bRunMODE && ( m_btMoveMODE <= MOVE_MODE_RUN ) ) 
		return GetOri_WalkSPEED();
	
	short nR = ( GetOri_RunSPEED() + m_EndurancePack.GetStateValue( ING_INC_MOV_SPD ) 
									- m_EndurancePack.GetStateValue( ING_DEC_MOV_SPD ) );

	nR += m_AruaAddMoveSpeed;

	return ( nR > 200.0f ) ? nR : 200.0f;
}
//---------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : 몹 생성 순서를 .. static
//--------------------------------------------------------------------------------

DWORD CObjMOB::m_dwCreateOrder=0;

//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : constructor
//--------------------------------------------------------------------------------

CObjMOB::CObjMOB()
{
	m_nQuestIDX = 0;
	

	m_iMobAniSkill = MOB_ANI_CASTION01;	
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : destructor
//--------------------------------------------------------------------------------

CObjMOB::~CObjMOB ()
{
	/*
	if ( m_nQuestIDX ) {
		g_pEventLIST->Del_EVENT (xxx);
	}
	*/
//	LogString (LOG_DEBUG, "   CObjMOB::~CObjMOB ( charNo:%d, %s ), Obj:%d  \n", m_nCharIdx, Get_NAME(), m_nIndex );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param CGameOBJ *pSourOBJ 
/// @param short nEventIDX
/// @brief  : 몹에 설정된 이벤트 처리..( NPC의 대화 이벤트 등 )
//--------------------------------------------------------------------------------

bool CObjMOB::Check_EVENT (CGameOBJ *pSourOBJ, short nEventIDX)
{
	CGameOBJ* pGameObj = pSourOBJ;
	/// Pet 라면 부모를 체크
	if( pSourOBJ->IsPET() )
	{
		pGameObj = (CGameOBJ*)((CObjCART*)pSourOBJ)->GetParent();
	}


	if ( pGameObj->IsA( OBJ_USER ) && 
		 m_nQuestIDX ) 
	{
		return g_pEventLIST->Run_EVENT( this->Get_INDEX(), m_nQuestIDX, nEventIDX );
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : CObjCHAR *pAtk
/// @brief  : 사망시 발생 이벤트..
//--------------------------------------------------------------------------------

void CObjMOB::Do_DeadEvent	(CObjCHAR *pAtk)
{
	QF_doQuestTrigger( NPC_DESC( m_nCharIdx ) );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 몹 생성
//--------------------------------------------------------------------------------

bool CObjMOB::Create (short nCharIdx, const D3DVECTOR& Position, short nQuestIDX, bool bRunMODE)
{
	char *szName  = CStr::Printf (NAME_MOB_MODEL, NPC_NAME( nCharIdx ), this->m_dwCreateOrder++);

	CCharMODEL *pMODEL = g_MOBandNPC.GetMODEL( nCharIdx );

	
	m_nCharIdx = nCharIdx;
	m_fScale = NPC_SCALE( nCharIdx ) / 100.f;
	if ( CObjCHAR::CreateCHAR (szName, pMODEL, pMODEL->GetPartCNT(), Position) ) 
	{
		this->m_iHP = NPC_HP( m_nCharIdx );
		this->m_iMaxHP = NPC_HP( m_nCharIdx ) * NPC_LEVEL( m_nCharIdx );

		if ( this->m_iHP < 0 )
			g_pCApp->ErrorBOX ("ERROR:: HP <= 0 !!!", (char*)NPC_NAME( nCharIdx ) );

		this->m_bRunMODE      = bRunMODE;
		this->m_fRunAniSPEED  = 1.0f;
//		this->m_fAtkAniSPEED  = (NPC_ATK_SPEED( m_nCharIdx ) / 100.f);

		//this->Adjust_HEIGHT ();

		// EVENT ...
		m_nQuestIDX = nQuestIDX;

		this->New_EFFECT( BODY_PART_WEAPON_R, NPC_R_WEAPON( m_nCharIdx ) );
		this->New_EFFECT( BODY_PART_WEAPON_L, NPC_L_WEAPON( m_nCharIdx ) );


		//----------------------------------------------------------------------------------------------------
		/// Glow effect
		//----------------------------------------------------------------------------------------------------
		unsigned int iColor = NPC_GLOW_COLOR( nCharIdx );
		if( iColor )
		{
			D3DXCOLOR color = CGameUtil::GetRGBFromString( iColor );
			::setVisibleGlowRecursive( this->GetZMODEL(), 2, color.r, color.g, color.b );
		}

		return true;
	}

	LogString (LOG_DEBUG, "MOB Char create failed .. %d: %s \n", nCharIdx, NPC_NAME( nCharIdx ) );

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 도망..
//--------------------------------------------------------------------------------

void CObjMOB::Run_AWAY (int iDistance)
{
	D3DVECTOR pos;

	pos.x = RANDOM( iDistance*2 ) - iDistance;
	pos.y = RANDOM( iDistance*2 ) - iDistance;
	pos.x += m_PosBORN.x;
	pos.y += m_PosBORN.y;

	this->SetCMD_MOVE (pos, true);
	this->Set_COMMAND( CMD_RUNAWAY );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 캐릭터 변경..( 그래서 모델노드는 안지우나? )
//--------------------------------------------------------------------------------

bool CObjMOB::Change_CHAR (int nCharIDX)
{
	this->DeleteCHAR ();

	D3DVECTOR PosBORN = Get_BornPOSITION ();

	if ( !this->Create (nCharIDX, m_PosCUR, m_nQuestIDX, m_bRunMODE) ) {
		return false;
	}

	m_PosBORN = PosBORN;
	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 구현사항 없음
//--------------------------------------------------------------------------------

bool CObjMOB::Create_PET (int nCharIDX)
{
#ifdef	__VIRTUAL_SERVER
	return ( g_pObjMGR->Add_MobCHAR( 0, nCharIDX, m_PosCUR, 0, true ) ? true : false );
#else
	return false;
#endif
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 인공지능 메세지..
//--------------------------------------------------------------------------------

void CObjMOB::Say_MESSAGE (char *szMessage)
{
	//AddMsgToChatWND (CStr::Printf("%s> %s", this->Get_NAME (), szMessage ), g_dwBLACK );
	g_UIMed.AddChatMsg( g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ), szMessage, g_dwRED );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 지속형의 변경수치 적용을 위해서 현재 적용되어있는 능력수치( 패시브 스킬 포함 )
//--------------------------------------------------------------------------------

int	CObjMOB::Get_DefaultAbilityValue( int iType )
{
	switch( iType )
	{
		case AT_SPEED:		
		case AT_ATK_SPD:
			return 1;			

		case AT_MAX_HP:
			{
				return this->m_iMaxHP;
			}
			break;

		case AT_MAX_MP:
			{
				return this->m_iMaxMP;
			}
			break;

		default:
			return 1;
	}

	return 1;
}

/// NPC의 경우 STB에 강제로 높이가 들어가 있으면 그 높이를 사용한다.
void CObjMOB::GetScreenPOS ( D3DVECTOR &PosSCR)
{
	float fStature = NPC_HEIGHT( this->m_nCharIdx );

	if( fStature != 0 )
	{
		::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
		return;
	}

	// 모델의 좌표에 키를 더한 위치를 이름출력 위치로 설정
	::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + m_fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
}

//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : 
/// @brief  : 공격 거리.. 스킬사용중이면 스킬테이블에서 아니면 NPC 테이블이겠지..
//--------------------------------------------------------------------------------

int CObjMOB::Get_AttackRange()
{
	/// 스킬에 공격 거리가 입력되어 있다면 스킬거리 아니면 무기 거리..
	if ( this->m_nToDoSkillIDX > 0 ) 
	{
		if( this->m_nToDoSkillIDX < g_SkillList.Get_SkillCNT() )
		{
			if(	SKILL_DISTANCE( this->m_nToDoSkillIDX ) ) 
			{
				return SKILL_DISTANCE( this->m_nToDoSkillIDX );
			}
		}else
			assert( 0 && "Get_AttackRange Failed[ ToDoSkillIDX is invalid ]" );
	}

	/// 스킬에 공격 거리가 입력되어 있다면 스킬거리 아니면 무기 거리..
	//if ( this->m_nToDoSkillIDX && SKILL_DISTANCE( this->m_nToDoSkillIDX ) ) 
	//{
	//	return SKILL_DISTANCE( this->m_nToDoSkillIDX );
	//}
	
	// Default attack range...
	return ( Def_AttackRange () + ( Get_SCALE() * 120 ) );
}

//------------------------------------------------------------------------------------------------
/// 몬스터 정지시 사운드 출력..
//------------------------------------------------------------------------------------------------
void CObjMOB::PlayStopSound()
{
	int iIndex = NPC_NORMAL_EFFECT_SOUND( this->Get_CharNO() );
	g_pSoundLIST->IDX_PlaySound3D( iIndex, this->Get_CurPOS() );				
}

/*override*/ int CObjMOB::Proc()
{
	int iResult = CObjCHAR::Proc();

	//--------------------------------------------------------------------------------
	// 모션 루프가 끝났다.
	//--------------------------------------------------------------------------------
	if( this->Get_COMMAND() == CMD_STOP )	
	{			
		if ( !m_bFrameING ) 
		{
			if( RANDOM( 100 ) < 20 )
				PlayStopSound();
		}
	}

	return iResult;
}



//--------------------------------------------------------------------------------
// 05.05.19 icarus:: WOW방식의 퀘스트 노출 시스템 적용을 위해 추가..
CObjNPC::CObjNPC()
{
	m_nQuestSignal = -1;
}

void CObjNPC::SetEventValue( int iEventValue )
{ 
	m_iEventSTATUS = iEventValue; 
}

int CObjNPC::GetEventValue()
{ 
	return m_iEventSTATUS; 
}


int CObjNPC::Proc ()
{
	if( CCountry::GetSingleton().IsApplyNewVersion() && ( this->m_nQuestSignal < 0 || g_pAVATAR->m_bQuestUpdate ) )
	{
		// 내 아바타의 퀘스트 상태가 갱신되었거나 이전에 퀘스트 상태를 판단한적이 없다면...
		this->m_nQuestSignal = SC_QF_GetNpcQuestStatus( this->m_nCharIdx );
		_RPT1( _CRT_WARN,"SC_QF_GetNpcQuestStatus :%d \n", m_nQuestSignal );
	}

	return CObjMOB::Proc ();
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : constructor
//--------------------------------------------------------------------------------

CObjAVT::CObjAVT()
{
//	m_fScale  = 0.5;
	m_dwSitTIME			= 0;
	m_bIsFemale			= false;
	m_bRunMODE			= true;	// 아바타 기본은 달리기..
	m_nRunSPEED			= WALK_CmPerSec;

	m_pCharMODEL		= &m_CharMODEL;	// Init ..

	m_iExpression		= 0;

	m_iDoChangeWeaponR	= 0;
	m_iDoChangeWeaponL	= 0;
	m_iCon				= 1;
	m_iRecoverHP		= 1;
	m_iRecoverMP		= 1;


	m_iLevel = 1;

	::ZeroMemory( m_sPartItemIDX, sizeof(tagPartITEM)*MAX_BODY_PART );

	///
	/// Personal store
	///
	m_bPersonalStoreMode	= false;
	m_pObjPersonalStore		= NULL;
    
	///
	/// pet
	///
	::ZeroMemory( m_sPetPartItemIDX, sizeof(tagPartITEM)*MAX_RIDING_PART );
	//m_iPetType			= 0;
	//m_pObjCART			= NULL;
	m_btWeightRate		= 0;

	m_pWeaponJemEffect	= 0;
	m_nStamina			= 0;///최소값으로 셋팅
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : destructor
//--------------------------------------------------------------------------------

CObjAVT::~CObjAVT ()
{	
	DeleteCart();

	//--------------------------------------------------------------------------------
	/// 재밍,재련관련 효과 지우기.
	//--------------------------------------------------------------------------------
	DeleteGemmingEffect();
	DeleteGreadEffect();

	if( m_pObjPersonalStore )
	{
		delete m_pObjPersonalStore;
		m_pObjPersonalStore = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 파트 조각 세팅( 세팅만 )
//--------------------------------------------------------------------------------

void CObjAVT::SetPartITEM (short nPartIdx, short nItemNo)
{
	this->New_EFFECT( nPartIdx, nItemNo, false );

	m_sPartItemIDX[ nPartIdx ].m_nItemNo = nItemNo;

	/// 아이템이 비워지는거라면 모든 데이터 클리어.
	if( nItemNo == 0 )	
	{
		m_sPartItemIDX[ nPartIdx ].m_bHasSocket = false;
		m_sPartItemIDX[ nPartIdx ].m_cGrade = 0;
		m_sPartItemIDX[ nPartIdx ].m_nGEM_OP = 0;
	}

	//g_pSoundLIST->IDX_PlaySound( SOUND_EQUIP_ITEM  );
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : ** 엔진 좌표가 넘어와야 한다.
///				아바자 생성
//--------------------------------------------------------------------------------

bool CObjAVT::Create (const D3DVECTOR &Position, BYTE btCharRACE)
{
	m_btRace = btCharRACE;
	m_bIsFemale = btCharRACE & 0x01;
	m_CharMODEL.Set_SEX( m_bIsFemale );

	for (short nItemIDX, nI=0; nI<MAX_BODY_PART; nI++) 
	{
		nItemIDX = m_sPartItemIDX[ nI ].m_nItemNo;

		/// 머리카락이라면 모자와 어울리는 머리카락으로 교체.
		if ( nI == BODY_PART_HAIR ) 
		{
			nItemIDX += HELMET_HAIR_TYPE( m_sPartItemIDX[ BODY_PART_HELMET ].m_nItemNo );
		}

		/// 얼굴이라면 현재의 표정 얼굴로 교체..
		if( nI == BODY_PART_FACE )
		{
			nItemIDX += GetCharExpression();
		}

		this->m_CharMODEL.SetPartMODEL( nI, nItemIDX );
		//this->New_EFFECT( nI, nItemIDX );
	}

	D3DVECTOR charPos = Position;
	charPos.z = g_pTerrain->GetHeightTop(Position.x, Position.y);

	if ( CObjCHAR::CreateCHAR ( (char*)m_Name.c_str(), &m_CharMODEL, MAX_BODY_PART, charPos) ) {
		m_nAtkAniSPEED = Cal_AtkAniSPEED( this->GetPartITEM(BODY_PART_WEAPON_R) );

		m_iHP = 100;

		// SetPartITEM에서 생성된 효과 link
		this->Link_EFFECT ();

		// 케릭터 신장.
		m_fStature = ::getModelHeight (this->m_hNodeMODEL);

		this->New_EFFECT( BODY_PART_WEAPON_R, this->GetPartITEM(BODY_PART_WEAPON_R));
		this->New_EFFECT( BODY_PART_WEAPON_L, this->GetPartITEM(BODY_PART_WEAPON_L));		


		CreateGemmingEffect();
		CreateGradeEffect();

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 클래스 내부에 설정된 아바타 정보를 이용해서, 업데이트 한다.
//--------------------------------------------------------------------------------

void CObjAVT::Update (bool bUpdateBONE )
{
	// 0. 효과 unlink  :: DeletePARTS에서 삭제되면서 엔진에서 자동으로 unlink ??
	this->Unlink_EFFECT ();

	// 1. 지우기
	// addRenderUnit 된것들은 clearRenderUnit ( HNODE hVisible ); 로 삭제
	// loadVisible 된것등은 unloadVisible로 삭제...
	this->DeletePARTS (false);	

	//--------------------------------------------------------------------------------
	/// 재밍,재련관련 효과 지우기.
	//--------------------------------------------------------------------------------
	DeleteGemmingEffect();
	DeleteGreadEffect();


	if ( bUpdateBONE ) {
		m_pCharMODEL->UnlinkBoneEFFECT( m_ppBoneEFFECT );

		this->UnloadModelNODE ();
		this->LoadModelNODE ( (char*)m_Name.c_str() );
		if ( m_bIsVisible ) {
			::insertToScene( m_hNodeMODEL );
			::setPosition (this->m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);			
		}

		m_pCharMODEL->LinkBoneEFFECT( m_hNodeMODEL, m_ppBoneEFFECT );		
	}

	// 2. 데이타 설정.
	for (short nItemIDX, nI=0; nI<MAX_BODY_PART; nI++) 
	{
		nItemIDX = m_sPartItemIDX[ nI ].m_nItemNo;

		if ( nI == BODY_PART_HAIR ) 
		{
			nItemIDX += HELMET_HAIR_TYPE( m_sPartItemIDX[ BODY_PART_HELMET ].m_nItemNo );
		}

		
		/// 얼굴이라면 현재의 표정 얼굴로 교체..
		if( nI == BODY_PART_FACE )
		{
			nItemIDX += GetCharExpression();
		}
		

		this->m_CharMODEL.SetPartMODEL( nI, nItemIDX );		
	}


	// 4. 모델 설정.
	this->CreatePARTS ( (char*)m_Name.c_str() );

	this->InsertToScene ();

	// 5. 효과 relink
	this->Link_EFFECT ();


	//----------------------------------------------------------------------------------------------------		
	/// @brief 재밍, 재련관련 이펙트 생성
	//----------------------------------------------------------------------------------------------------
	
	//2005. 06. 15 박 지호
	Set_RareITEM_Glow();

	CreateGemmingEffect();
	CreateGradeEffect();


	// 6. 무기가 바뀌어 현재 진행중 모션이 틀려 질경우...
	#pragma message ("TODO:: change motion ..." __FILE__)



	// 케릭터 신장. // 여긴 이상함 검잔상이나 기타 오브젝트의 높이도 구함.. InsertToScene 안으로 옮기자..
	m_fStature = ::getModelHeight (this->m_hNodeMODEL);


	/// 카트를 타고 있다면..
	if( GetPetMode() >= 0 )
	{
		this->UpdatePet();
	}

	/// CheckVisibiliey
	m_EndurancePack.UpdateVisibility();
}


//----------------------------------------------------------------------------------------------------		
/// @brief 재밍, 재련관련 이펙트..
//----------------------------------------------------------------------------------------------------
const int iWeaponAttachedEffectDummyNO = 2;
void CObjAVT::CreateGemmingEffect()
{
	//----------------------------------------------------------------------------------------------------		
	/// @brief 재밍이나 재련관련.. 붙었다면 효과를 붙여라..
	//----------------------------------------------------------------------------------------------------
	
	for( int i = BODY_PART_WEAPON_R; i <= BODY_PART_WEAPON_L ; i++ )
	{
		int iPartIdx = i;
		int iItemIDX = m_sPartItemIDX[ iPartIdx ].m_nItemNo;
		if( iItemIDX == 0 )
			continue;

		if( m_sPartItemIDX[ iPartIdx ].m_bHasSocket && m_sPartItemIDX[ iPartIdx ].m_nGEM_OP > 300 )
		{			
			int iEffectIDX = 0;
			
			iEffectIDX = GEMITEM_ATTACK_EFFECT( m_sPartItemIDX[ iPartIdx ].m_nGEM_OP );			
			if( iEffectIDX )
			{
				/// List_Effect 에는 4개까지 효과를 박을수 있지만.. 하나만..
				int iStartPointNO = 0;
				switch( iPartIdx )
				{
					case BODY_PART_WEAPON_R:
						iStartPointNO = WEAPON_GEMMING_POSITION( iItemIDX );
						break;

					case BODY_PART_WEAPON_L:
						iStartPointNO = SUBWPN_GEMMING_POSITION( iItemIDX );
						break;
				}

				for( int i = 0; i < 1 ; i++ )
				{
					int iEffect = EFFECT_POINT( iEffectIDX, i );
					
					m_pWeaponJemEffect = g_pEffectLIST->Add_EffectWithIDX( iEffect );
					if ( m_pWeaponJemEffect ) 
					{
						CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( iPartIdx, iItemIDX, this->IsFemale() );

						if( pCharPART && 
							m_phPartVIS[ iPartIdx ] &&
							iStartPointNO < pCharPART->m_nDummyPointCNT &&
							m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_nParent ] )
						{								
							m_pWeaponJemEffect->Rotation( pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_Rotate );
							m_pWeaponJemEffect->Transform( pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_Transform );						
							m_pWeaponJemEffect->LinkNODE( m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_nParent ] );
						}else
						{
							g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
						}
					}
				}

				/*m_pWeaponJemEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX );
				if ( m_pWeaponJemEffect ) 
				{
					CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( iPartIdx, iItemIDX, this->IsFemale() );

					if( iWeaponAttachedEffectDummyNO < pCharPART->m_nDummyPointCNT )
					{				
						m_pWeaponJemEffect->Transform( pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_Transform );
						m_pWeaponJemEffect->Rotation( pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_Rotate );
						m_pWeaponJemEffect->LinkNODE( m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_nParent ] );
					}else
					{
						g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
					}
				}*/
			}
		}	
	}
}

//----------------------------------------------------------------------------------------------------		
/// @brief 재밍, 재련관련 이펙트..
//----------------------------------------------------------------------------------------------------
void CObjAVT::DeleteGemmingEffect()
{
	if( m_pWeaponJemEffect )
	{
		m_pWeaponJemEffect->UnlinkNODE();

		g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
		m_pWeaponJemEffect = NULL;
	}
}


//2005.06. 15 박지호 
//----------------------------------------------------------------------------------------------------		
/// 대만일 경우 레어 아이템일때 glow 효과를 3단계 설정한다. 
//----------------------------------------------------------------------------------------------------
void CObjAVT::Set_RareITEM_Glow(void)
{
	
	//대만이 아니면 리턴한다.
//	if(!CCountry::GetSingleton().IsTAIWAN()) 
//	  return;

	//대만과 필리핀에 적용됩니다. 수정 2005.6.20 nAvy
	if( !CCountry::GetSingleton().IsApplyNewVersion() )
		return;

	CInventory	m_Inventory;				
	short		m_iType = 0;
	

	for( int i = 1; i < MAX_BODY_PART ; i++ )
	{
		int iItemIDX = m_sPartItemIDX[ i ].m_nItemNo;
		
		if( iItemIDX == 0 )
			continue;
		
		//레어 아이템인지 체크한다. 
		m_iType = m_Inventory.GetBodyPartToItemType(i);	
			if(m_iType == MAX_BODY_PART)
				continue;
		

		if(ITEM_RARE_TYPE(m_iType,m_sPartItemIDX[i].m_nItemNo) != 3)
			continue;

		//3단계 Glow 효과 설정 
		m_sPartItemIDX[i].m_cGrade = 3;
	}

}


//----------------------------------------------------------------------------------------------------		
/// @brief 재밍, 재련관련 이펙트..
//----------------------------------------------------------------------------------------------------
void CObjAVT::CreateGradeEffect()
{
	CMODEL<CCharPART> *pCharPART = NULL;

	for( int i = 0; i < MAX_BODY_PART ; i++ )
	{
		int iItemIDX = m_sPartItemIDX[ i ].m_nItemNo;
		if( iItemIDX == 0 )
			continue;

		if( m_sPartItemIDX[ i ].m_cGrade != 0 )
		{
			//----------------------------------------------------------------------------------------------------
			/// Glow effect
			//----------------------------------------------------------------------------------------------------
			unsigned int iColor = ITEMGRADE_GLOW_COLOR( m_sPartItemIDX[ i ].m_cGrade );
			if( iColor )
			{
				D3DXCOLOR color = CGameUtil::GetRGBFromString( iColor );
				
				/// Skinning 되는 오브젝트가 아니라면..
				if( m_pCharMODEL )
				{
					if( m_pCharMODEL->m_RenderUnitPart[ i ].empty() )
					{
						pCharPART = g_DATA.Get_CharPartMODEL( i, iItemIDX, this->IsFemale() );
						if( pCharPART )
						{
							for( int j = 0; j < pCharPART->m_nPartCNT; j++ )
							{
								if( m_phPartVIS[ i ] && m_phPartVIS[ i ][ j ] )
									::setVisibleGlow( m_phPartVIS[ i ][ j ], ZZ_GLOW_TEXTURE, color.r, color.g, color.b );
							}
						}
					}else
					{
						/// 스키닝 되는 오브젝트일 경우..
						std::list< int >::iterator	begin = m_pCharMODEL->m_RenderUnitPart[ i ].begin();
						for( ; begin != m_pCharMODEL->m_RenderUnitPart[ i ].end() ; ++begin )
						{
							int iRenderUnitIndex = *begin;
							::setVisibleRenderUnitGlow( this->GetZMODEL(), iRenderUnitIndex, ZZ_GLOW_TEXTURE, color.r, color.g, color.b );
							//::setVisibleGlow( this->GetZMODEL(), ZZ_GLOW_SIMPLE, 1.0f, 1.0f, 1.0f );
						}
					}
				}
			}	
		}
	}
}

//----------------------------------------------------------------------------------------------------		
/// @brief 재밍, 재련관련 이펙트..
//----------------------------------------------------------------------------------------------------
void CObjAVT::DeleteGreadEffect()
{
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 총알 번호를 구한다. 장거리 무기의 경우.. 현재 세팅된 총알에 따라서 판단
//--------------------------------------------------------------------------------

/*override*/int	CObjAVT::Get_BulletNO ()
{	
	tagITEM sItem;
	sItem.m_cType   = ITEM_TYPE_WEAPON;
	sItem.m_nItemNo = Get_R_WEAPON();

	int iBulletNo = 0;
	int iShotType = sItem.GetShotTYPE();

	/// 총알소모 안하는 마법 무기
	if( iShotType >= MAX_SHOT_TYPE )
	{
		switch( WEAPON_TYPE( sItem.m_nItemNo ) ) 
		{
			case WEAPON_ITEM_NOT_USE_BULLET:
				{
					iBulletNo = WEAPON_BULLET_EFFECT( sItem.m_nItemNo );
					return iBulletNo;
				}
				break;		
		}		
	}

	if( iShotType < MAX_SHOT_TYPE && m_ShotData[iShotType].m_nItemNo )
		iBulletNo = NATURAL_BULLET_NO( m_ShotData[iShotType].m_nItemNo );

	return iBulletNo;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 현재 액션과 무기에 맞는 모션을 구한다.
//--------------------------------------------------------------------------------

tagMOTION *CObjAVT::Get_MOTION (short nActionIdx)
{
	int iActionIdx = m_ChangeActionMode.GetAdjustedActionIndex( nActionIdx );

	// 오른손 무기 종류에따라...
	short nWeaponTYPE = WEAPON_MOTION_TYPE( this->m_sRWeaponIDX.m_nItemNo );

	short nFileIDX = FILE_MOTION( nWeaponTYPE, iActionIdx );

	if ( 0 == nFileIDX ) {
		// 모션이 없으면 맨손 모션으로 대체..
		nFileIDX = FILE_MOTION( 0, nActionIdx );
	}

	tagMOTION *pMOTION = g_MotionFILE.IDX_GetMOTION( nFileIDX, m_bIsFemale );
	if ( pMOTION ) {
		pMOTION->m_nActionIdx = nActionIdx;
	}
	return pMOTION;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 앉기 서기보드 토글
//--------------------------------------------------------------------------------

bool CObjAVT::ToggleSitMODE ()
{
	if ( this->Get_COMMAND() == CMD_SIT ) {
		this->SetCMD_STAND ();
	} else {
		// 클라이언트는 무조건 앉힌다.
		m_dwSitTIME = 0;
		this->SetCMD_SIT ();
	}
	::setRepeatCount( m_hNodeMODEL, 1 );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : float fAdjRate : 현재 보정된 이동속도의 디폴트 속도에 대한 비율..
/// @brief  : 뛰기 걷기 모드 토글
///				m_bRunMODE 변수의 상태에 따라 속도 계산시 default speed가 틀려진다.
///             이걸 이전에는 함수 내부에서 구현했으나 이제는 밖에서..
//--------------------------------------------------------------------------------

bool CObjAVT::ToggleRunMODE ( float fAdjRate )
{	
	///float fAdjRate = this->Get_MoveSPEED() / this->Get_DefaultSPEED ();
	//m_bRunMODE = !m_bRunMODE;
	if ( Get_STATE() == CS_MOVE ) 
	{
		m_fAdjustSPEED = this->Get_DefaultSPEED () * fAdjRate;

		this->Set_CurMOTION( this->Get_MOTION( this->GetANI_Move() ) );
#ifdef	_DEBUG
		float fMoveSpeed = this->Get_MoveSPEED();
		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif

		// 이동 속도 모션 스피드 설정...
		this->Set_ModelSPEED( this->Get_MoveSPEED() );

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, ( m_bRunMODE )?this->m_fRunAniSPEED:1.0f );

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 현재 펫모드
//--------------------------------------------------------------------------------

int	CObjAVT::GetPetMode()			
{
	if( m_pObjCART )
		return m_pObjCART->GetCartType();
	return -1; 
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 현재 펫모드
//--------------------------------------------------------------------------------

bool CObjAVT::CanAttackPetMode()
{
	int iPetMode = this->GetPetMode();
	if( iPetMode > 0 )
	{
		/// Pet mode 일 경우에는..
		/*switch( iPetMode )
		{
			case PET_TYPE_CART01:
				return false;
			case PET_TYPE_CASTLE_GEAR01:
				return true;
		}*/
		//----------------------------------------------------------------------------------------------------
		/// @brief Pet 의 타입이 아니라 공격거리가 있냐 없냐로 공격가능여부 판정..
		//----------------------------------------------------------------------------------------------------

//박지호::
#if defined(_GBC)
		if( PAT_ITEM_ATK_RANGE( this->m_sWeaponIDX.m_nItemNo ) <= 0 )
#else
		if( PAT_ITEM_ATK_RANGE( this->m_sAbilIDX.m_nItemNo ) <= 0 )
#endif 		
			return false;	

	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE )
{ 
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_MOVE( PosTO, btRunMODE ); 
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget)
{ 
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget ); 
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_STOP( void )
{ 
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_STOP(); 
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_ATTACK( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO )
{ 
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosTO ); 		
	}
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 
//--------------------------------------------------------------------------------

void CObjAVT::RideCartToggle( bool bRide )
{
	if( bRide == false )
	{
		if( GetPetMode() >= 0 )
		{
			/// 내리기
			DeleteCart( true );
		}else
		{
			/// 타기
			CreateCartFromMyData( true );
		}

	}else
	{
		/// 현재 카트를 탄상태가 아닐경우에만 타기
		if( GetPetMode() < 0 )			
			CreateCartFromMyData( true );
	}

	//버프 지움.
	this->m_EndurancePack.ClearStateByDriveCart();

}


//-------------------------------------------------------------------------------------------------
#define	CHECK_TIME	1500

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : HP와MP리젠이 CObjAI::RecoverHPnMP() 에서 처리되므로 이전 코드인 이곳은 지운다.	- 2004/02/11 : nAvy 
///           Aroa상태일경우 50% 추가로 회복속도증가											- 2005/07/12 : nAvy
//--------------------------------------------------------------------------------

int CObjAVT::Proc ()
{
	m_dwElapsedTime += m_dwFrameElapsedTime;

	//--------------------------------------------------------------------------------
	/// 리커버관련 처리	
	//--------------------------------------------------------------------------------
	///if( GetStamina() > 3000 )
	{
		int	iRecoverStateCheckTime = RECOVER_STATE_CHECK_TIME_OLD;
		if( CCountry::GetSingleton().IsApplyNewVersion() )
			iRecoverStateCheckTime = RECOVER_STATE_CHECK_TIME_NEW;

		int iRecoverStateSitOnGround = RECOVER_STATE_SIT_ON_GROUND_OLD;
		if( CCountry::GetSingleton().IsApplyNewVersion() )
			iRecoverStateSitOnGround = RECOVER_STATE_SIT_ON_GROUND_NEW;


		int iRecoverStateStopWalk    = RECOVER_STATE_STOP_OR_WALK_OLD;
		if( CCountry::GetSingleton().IsApplyNewVersion() )
			iRecoverStateStopWalk = RECOVER_STATE_STOP_OR_WALK_NEW;



		if ( m_dwElapsedTime > iRecoverStateCheckTime ) 
		{
			//_RPT3( _CRT_WARN,"Update Recover HP/MP ElapsedTime:%d, CheckTime:%d, CurrTime:%d \n", m_dwElapsedTime, iRecoverStateCheckTime, timeGetTime() );
			switch ( Get_COMMAND() ) 
			{
			case CMD_SIT :
				this->RecoverHP( iRecoverStateSitOnGround );
				this->RecoverMP( iRecoverStateSitOnGround );							
				break;
			case CMD_DIE:
				break;
			default:
				/// 캐슬기어 탑승중일때는 회복 안함
				if( this->GetPetMode() < 0 )
				{
					/// 앉기가 아닌동작에서는 HP만 회복
					this->RecoverHP( iRecoverStateStopWalk );			
					this->RecoverMP( iRecoverStateStopWalk );
				}
				break;	
			}	
			m_dwElapsedTime -= iRecoverStateCheckTime;
		}
	}/*else
	{
		m_dwElapsedTime = 0;
	}*/

	//--------------------------------------------------------------------------------
	// 박지호: 펫 바이브레이션 
	// 최종진: 조건문에 m_pObjCART추가(맞을때 카트에서 내릴때 오류 발생 ) 2005/7/31
	//--------------------------------------------------------------------------------
	if(GetPetMode() && m_pObjCART && SetCartVA())
	{	
		m_pObjCART->m_ObjVibration.StartVibration();
		SetCartVA() = FALSE;
	}

	if(GetPetMode() && m_pObjCART)
		m_pObjCART->m_ObjVibration.Proc();

	//--------------------------------------------------------------------------------
	/// 시간에 따른 액션 모드 처리..
	/// if 문 줄이기 위해서 이쪽으로 이동.. 04/5/28
	//--------------------------------------------------------------------------------
	///if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	{
		m_ChangeActionMode.Proc();
	}		
	
	return CObjCHAR::Proc ();
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 총알 데이터 설정
//--------------------------------------------------------------------------------

void CObjAVT::SetShotData( int i, int iItemNo )
{ 
	if( i < 0 || i >= MAX_SHOT_TYPE )
		return;

	m_ShotData[i].m_nItemNo = iItemNo; 
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 지속형의 변경수치 적용을 위해서 현재 적용되어있는 능력수치( 패시브 스킬 포함 )
//--------------------------------------------------------------------------------

int	CObjAVT::Get_DefaultAbilityValue( int iType )
{
	switch( iType )
	{
		case AT_SPEED:
			{
				return this->GetOri_WalkSPEED();
			}
			break;

		case AT_ATK_SPD:
			{
				return this->GetOri_ATKSPEED();
			}
			break;

		case AT_MAX_HP:
			{
				return this->m_iMaxHP;
			}
			break;

		case AT_MAX_MP:
			{
				return this->m_iMaxMP;
			}
			break;

		default:
			return 1;
	}

	return 1;
}




/*override*/int CObjAVT::GetANI_Stop()			
	{
		if( m_pObjCART )
			return GetANI_Ride();
		return  AVT_ANI_STOP1;				
	}
/*override*/int	CObjAVT::GetANI_Move()		{	return  m_bRunMODE ? AVT_ANI_RUN : AVT_ANI_WALK;	}
/*override*/int	CObjAVT::GetANI_Attack()	{	return( AVT_ANI_ATTACK+RANDOM(3) );	}
/*override*/int	CObjAVT::GetANI_Die ()			
{	
	if( m_bStopDead )
		return AVT_ANI_STOP1;
	return  AVT_ANI_DIE;				
}
/*override*/int	CObjAVT::GetANI_Hit()		{	return  AVT_ANI_HIT;				}
/*override*/int	CObjAVT::GetANI_Casting ()	{	return	SKILL_ANI_CASTING(m_nActiveSkillIDX);	}
/*override*/int	CObjAVT::GetANI_CastingRepeat()	{   return SKILL_ANI_CASTING_REPEAT(m_nActiveSkillIDX);	}		/// 루프동작은 캐스팅동작으로 사용..
/*override*/int	CObjAVT::GetANI_Skill ()	{	return	SKILL_ANI_ACTION_TYPE(m_nActiveSkillIDX);	}
/*override*/int	CObjAVT::GetANI_Sitting()	{	return	AVT_ANI_SITTING;			}
/*override*/int	CObjAVT::GetANI_Standing()	{	return	AVT_ANI_STANDUP;			}
/*override*/int	CObjAVT::GetANI_Sit()		{	return	AVT_ANI_SIT;				}
/*override*/int	CObjAVT::GetANI_Ride()		{   return  ( m_pObjCART )? m_pObjCART->GetRideAniPos():0; }
/*override*/int	CObjAVT::GetANI_PickITEM()	{	return  AVT_ANI_PICKITEM; }




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
WORD CObjAVT::GetPetState()
{
	if( m_pObjCART )
		return m_pObjCART->Get_STATE();

	return 0;
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 현재 내부에 설정된 데이터로 카트 생성
//--------------------------------------------------------------------------------

bool CObjAVT::CreateCartFromMyData( bool bShowEffect )
{	

	//----------------------------------------------------------------------------------------------------
	/// Pet type 결정
	/// 아이템 교체시에는 타입이 결정되지만 서버로부터 받은상태에선 알수없다.
	//----------------------------------------------------------------------------------------------------
	/*CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	CItem* pBodyItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 );*/

	if( m_sBodyIDX.m_nItemNo )
	{
		this->SetPetType( PAT_ITEM_PART_TYPE( m_sBodyIDX.m_nItemNo ) );
	}else
		return false; /// 바디부폼이 없으면 못탐


#ifdef _GBC
	bool bResult = CreateCart( m_iPetType, m_sEngineIDX.m_nItemNo, m_sBodyIDX.m_nItemNo,
		m_sLegIDX.m_nItemNo, m_sAbilIDX.m_nItemNo, m_sWeaponIDX.m_nItemNo );
#else
	bool bResult = CreateCart( m_iPetType, m_sEngineIDX.m_nItemNo, m_sBodyIDX.m_nItemNo,
		m_sLegIDX.m_nItemNo, m_sAbilIDX.m_nItemNo );
#endif
	
	
	/// 모든 유리상태 해지하지 말것 2005/7/30 - nAvy
	/// 
	///this->m_EndurancePack.ClearStateByDriveCart();

	//----------------------------------------------------------------------------------------------------
	/// 카트를 탈때는 무기, 날개는 안보이게
	//----------------------------------------------------------------------------------------------------		
	/*if( m_phPartVIS[ BODY_PART_KNAPSACK ] )
		::setVisibilityRecursive( *m_phPartVIS[ BODY_PART_KNAPSACK ], 0.0f );
	if( m_phPartVIS[ BODY_PART_WEAPON_R ] )
		::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_R ], 0.0f );
	if( m_phPartVIS[ BODY_PART_WEAPON_L ] )
		::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_L ], 0.0f );*/

	int iVisibilityPart[3] = { BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L };
	for( int i = 0; i < 3; i++ )
	{
		CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( iVisibilityPart[ i ] );
		if ( pCharPART  ) 
		{
			short nI;			

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
			{
				if ( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ] ) 
				{
					::setVisibilityRecursive( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 0.0f );
				}
			}
		}
	}
	

	//----------------------------------------------------------------------------------------------------
	/// 탈때의 효과 출력
	//----------------------------------------------------------------------------------------------------		
	if( bShowEffect )
	{
		int iEffectNO	= PAT_RIDE_EFFECT( m_sBodyIDX.m_nItemNo );
		int iSoundNO	= PAT_RIDE_SOUND( m_sBodyIDX.m_nItemNo );
	
		this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );				
	}
	
	return bResult;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 카드 생성. 인자로 각 파트 정보를 받는다.
//--------------------------------------------------------------------------------
bool CObjAVT::CreateCart( unsigned int iPetType, int iEnginePart, int iBodyPart, int iLegPart, int iAbilIPart,int iWeaponPart )
{
	if( iBodyPart == 0 || iEnginePart == 0 || iLegPart == 0 )
	{
		return false;
	}

	m_ObjVibration.EndVibration();

	m_iPetType = iPetType;

	if( m_pObjCART != NULL )
	{
		delete m_pObjCART;
		m_pObjCART = NULL;
	}

	m_pObjCART = g_pObjMGR->Add_CartCHAR( iPetType, this, 0 );
	if( m_pObjCART == NULL )
	{
		assert( 0 && "Create cart failed" );
		///g_itMGR.OpenMsgBox( "Failed to create cart" );
		return false;
	}

	SetPetParts( RIDE_PART_BODY,	iEnginePart, false );
	SetPetParts( RIDE_PART_ENGINE,	iBodyPart, false );
	SetPetParts( RIDE_PART_LEG,		iLegPart, false );

	///<- 2005/7/25 카트 시스템 파츠 추가로 수정 : nAvy
	//SetPetParts( RIDE_PART_ARMS,	iArmsIDX, false );
#ifdef _GBC	
	SetPetParts( RIDE_PART_ABIL,	iAbilIPart, false );
	SetPetParts( RIDE_PART_ARMS,	iWeaponPart, false );
#else
	SetPetParts( RIDE_PART_ARMS,	iAbilIPart, false );
#endif
	
	///->

	if( m_pObjCART->Create( this, m_iPetType, this->Get_CurPOS() ) == false )
	{
		g_pObjMGR->Del_Object( m_pObjCART );
		return false;
	}

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ResetCUR_POS( pos );
	::savePrevPosition( m_hNodeMODEL );

	this->Set_ModelDIR( 0.0f );

	SetCMD_STOP();

	this->m_btMoveMODE = MOVE_MODE_DRIVE;

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 카트 제거
//--------------------------------------------------------------------------------

void CObjAVT::DeleteCart( bool bShowEffect )
{
	if( m_pObjCART )
	{
		float fDir = ::getModelDirection( m_pObjCART->GetZMODEL() );
		this->Set_ModelDIR( fDir );

//박지호
#if defined(_GBC)
		//운전자 (2인승 중이라면)
		if(GetRideUserIndex())
		{	
			//20050901 홍근 2인승 카트 보조석 탑승자가 내렸을 경우 State를 Normal로 바꿔준다.
			CObjAVT *oSrc = g_pObjMGR->Get_ClientCharAVT( g_pObjMGR->Get_ServerObjectIndex(GetRideUserIndex()), true ); 
			if(oSrc)
			{				
				if( !strcmp( oSrc->Get_NAME() , g_pAVATAR->Get_NAME()) )
				{
					g_pAVATAR->Set_Block_CartRide( false );
				}							
			}		

			m_pObjCART->UnLinkChild();		
			
			//실제 오브젝트 매니져 리스트에서 카트 객체를 삭제함.
			if(m_pObjCART)
			{	g_pObjMGR->Del_Object( m_pObjCART );	m_pObjCART = NULL;	}
			
			SetCMD_STOP();			
			
			goto CHAR_VISIBLE; 
		}
       //2인승 탑승자 
		else if(IsRideUser())
		{
			if(m_pObjCART)
			{	
				ReleaseRideUser(m_pObjCART->GetParent()->GetRideUserIndex());	

				//20050901 홍근 2인승 카트 보조석 탑승자가 내렸을 경우 State를 Normal로 바꿔준다.
				if( !strcmp( this->Get_NAME(), g_pAVATAR->Get_NAME()) )
				{
					g_pAVATAR->Set_Block_CartRide( false );
				}				

				return;
			}
		}
		//혼자 타고 있다면...
		else
		{	m_pObjCART->UnLinkChild();		}
#else	
		m_pObjCART->UnLinkChild();	
#endif 

		if(m_pObjCART)
		{
			g_pObjMGR->Del_Object( m_pObjCART );
			m_pObjCART = NULL;				
		}

		SetCMD_STOP();

		this->m_btMoveMODE = this->m_bRunMODE;


		//----------------------------------------------------------------------------------------------------
		/// 카트를 탈때는 무기, 날개는 안보이게
		//----------------------------------------------------------------------------------------------------		
		/*if( m_phPartVIS[ BODY_PART_KNAPSACK ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_KNAPSACK ], 1.0f );
		if( m_phPartVIS[ BODY_PART_WEAPON_R ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_R ], 1.0f );
		if( m_phPartVIS[ BODY_PART_WEAPON_L ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_L ], 1.0f );*/
CHAR_VISIBLE:
		int iVisibilityPart[3] = { BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L };
		for( int i = 0; i < 3; i++ )
		{
			CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( iVisibilityPart[ i ] );
			if ( pCharPART  ) 
			{
				short nI;			

				for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
				{
					if ( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ] ) 
					{
						::setVisibilityRecursive( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
					}
				}
			}
		}

		//----------------------------------------------------------------------------------------------------
		/// 내릴때의 효과출력..
		//----------------------------------------------------------------------------------------------------	
		if( bShowEffect )
		{
			int iEffectNO	= PAT_GETOFF_EFFECT( m_sBodyIDX.m_nItemNo );
			int iSoundNO	= PAT_GETOFF_SOUND( m_sBodyIDX.m_nItemNo );
		
			this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );				
		}
	}	
}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// 박지호: Process_JOIN_RIDEUSER() 
///       : 운전자가 존 워프시 2인승 탑승자를 해제한다.  
///				
//--------------------------------------------------------------------------------
void CObjAVT::Process_JOIN_RIDEUSER(void)
{

	if(this->GetPetMode() < 0)
		return;
	
	DeleteCart(true);

}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 카트 파트정보세팅
//--------------------------------------------------------------------------------

void CObjAVT::SetPetParts( unsigned int iPetPartIDX, unsigned int iItemIDX, bool bJustInfo )
{
	if( iPetPartIDX >= MAX_RIDING_PART )
		return;

	m_sPetPartItemIDX[ iPetPartIDX ].m_nItemNo = iItemIDX;

	if( bJustInfo == false )
	{
		if( m_pObjCART == NULL )
			return;

		m_pObjCART->SetPetParts( iPetPartIDX, iItemIDX );
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 
/// @brief  : 카트 파트 변경등 발생시 카트 업데이트
//--------------------------------------------------------------------------------

void CObjAVT::UpdatePet()
{
	if( m_pObjCART )
	{
		DeleteCart();
		CreateCartFromMyData();	
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : 새로운 소지 아이템 무게비율
//--------------------------------------------------------------------------------

void CObjAVT::SetWeightRate( BYTE btWeightRate )
{
	m_btWeightRate = btWeightRate;
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @return : 현재 소지 아이템 비율 
//--------------------------------------------------------------------------------

BYTE CObjAVT::GetWeightRate()
{
	return m_btWeightRate;
}

//-------------------------------------------------------------------------------------------
/// @brief 모든 개인상점을 열고 닫는 시작점..
/// 개인상점 전용 모델을 보여주기위해 일련의 작업들을 한다.
//-------------------------------------------------------------------------------------------
void CObjAVT::SetPersonalStoreTitle( char* strTitle, int iPersonalStoreType )
{ 
	if( strTitle == NULL )
	{
		if( m_phPartVIS[ 0 ] == NULL )
			m_pCharMODEL->ClearRenderUnitParts();

		// loadVisible된것들 삭제.
		for (short nP=0; nP<MAX_BODY_PART; nP++) 
		{
			if( m_phPartVIS[ nP ] == NULL )
				CreateSpecificPART( (char*)m_Name.c_str(), nP );			
		}

		this->CreateGemmingEffect();
		this->CreateGradeEffect();

		m_bPersonalStoreMode = false;

		if( m_pObjPersonalStore )
		{			
			m_pObjPersonalStore->UnlinkVisibleWorld();
			///m_pObjPersonalStore->RemoveFromScene();
			delete m_pObjPersonalStore;
			m_pObjPersonalStore = NULL;			
		}
		return;
	}

	m_pObjPersonalStore = new CObjTREE();
		
	///D3DXVECTOR3 vPos = this->Get_CurPOS();	
	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
	const int iPersonalStoreNO = 260;
	if( m_pObjPersonalStore->Create( g_DATA.m_ModelFieldITEM.GetMODEL( iPersonalStoreNO + iPersonalStoreType ), vPos ) )
	{
		//-------------------------------------------------------------------------------------------
		/// 모든 캐릭터 파트들 삭제
		//-------------------------------------------------------------------------------------------
		::clearRenderUnit( this->m_hNodeMODEL );
		for (short nP=0; nP<MAX_BODY_PART; nP++) 
		{
			DeleteSpecificPART( nP, m_phPartVIS[ nP ] );
			//m_pCharMODEL->DeletePART( nP, m_phPartVIS[ nP ] );
			m_phPartVIS[ nP ] = NULL;
		}	

		m_pObjPersonalStore->LinkToModel( this->GetZMODEL() );		
	}else
	{
		delete m_pObjPersonalStore;
		m_pObjPersonalStore = NULL;
	}
	
	m_bPersonalStoreMode = true;
	m_strPersonalStoreTitle = std::string( strTitle ); 
}

const char*	CObjAVT::GetPersonalStoreTitle()
{ 
	return m_strPersonalStoreTitle.c_str(); 
}

bool CObjAVT::IsPersonalStoreMode()
{ 
	return m_bPersonalStoreMode; 
}

/// 장비 교체시 장비에 걸려있던 속성 해제..
void CObjAVT::ClearRWeaponSkillEffect()
{
	this->m_EndurancePack.ClearRWeaponSkillEffect();
}

void CObjAVT::ClearLWeaponSkillEffect()
{
	this->m_EndurancePack.ClearLWeaponSkillEffect();
}

//----------------------------------------------------------------------------------------------------	
/// @brief 팻모드 상태일때는 팻의 좌표를 리턴하고, 일반적일때는 내 좌표를 리턴함
//----------------------------------------------------------------------------------------------------
const D3DVECTOR& CObjAVT::GetWorldPos()
{
	if( GetPetMode() > 0 )
	{
		if( m_pObjCART )
		{
			return m_pObjCART->Get_CurPOS();
		}
	}

	return Get_CurPOS();
}


//----------------------------------------------------------------------------------------------------	
/// @brief 모델에 등록된 renderUnit외의것들에 대한 충돌판정.. (아바타일경우만)
//----------------------------------------------------------------------------------------------------
bool CObjAVT::IsIntersectAccessory( float &fCurDistance )
{
	/// 개인상점 대표모델
	if( this->m_pObjPersonalStore )
	{
		return m_pObjPersonalStore->IsIntersectForCamera( fCurDistance );
	}
	return false;
}
//----------------------------------------------------------------------------------------------------	
/// @brief 파티멤버의 자동 HP회복을 위한 Stamina 관련 Method
///		   CObjUSER의 경우 CUserDATA::GetCur_STAMINA를 호출한다. 주의할것
//----------------------------------------------------------------------------------------------------
short CObjAVT::GetStamina()
{
	return m_nStamina;
}
void CObjAVT::SetStamina( short nStamina )
{
	m_nStamina = nStamina;
}

//----------------------------------------------------------------------------------------------------
/// @brief m_dwSubFLAG 플래그에 따란 특수한 상태로의 전환
//----------------------------------------------------------------------------------------------------

void CObjAVT::ChangeSpecialState( DWORD dwSubFLAG )
{
	m_dwSubFLAG = dwSubFLAG;

	if( m_dwSubFLAG & FLAG_SUB_HIDE )
	{
		::setVisibilityRecursive( this->GetZMODEL(), 0.0f ); 
	}
	//곽홍근::투명
	else if( !(m_dwSubFLAG & FLAG_SUB_HIDE) )
	{
		::setVisibilityRecursive( this->GetZMODEL(), 1.0f ); 
	}
}

//----------------------------------------------------------------------------------------------------
/// @brief 클릭할수 있는 객체인가?
//----------------------------------------------------------------------------------------------------
bool CObjAVT::CanClickable()
{
	if( m_dwSubFLAG & FLAG_SUB_HIDE )
		return false;

	if( ::getVisibility( this->GetZMODEL() ) < 0.1 )
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @brief virtual From CObjAI   : 추가  2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
int CObjAVT::GetOri_MaxHP()
{
	return m_iMaxHP;
}


///////////////////////////////////////////////////////////////////////////////////////////	
// 2005. 6. 20	박 지호 
//
// 여신소환 연출 클래스 
///////////////////////////////////////////////////////////////////////////////////////////	


CGoddessMgr goddessMgr;
//생성자 
CGoddessMgr::CGoddessMgr()
{
	
	Init();

}


//소멸자
CGoddessMgr::~CGoddessMgr()
{

//	Release();
	
}


//초기화 
BOOL CGoddessMgr::Init(void)
{

	m_IsUse		= FALSE;
	m_IsAvata	= FALSE;
	
	m_dwAvata	= 0;
	m_Count		= 0;


	return TRUE;

}


//메모리 해제 
void CGoddessMgr::Release(void)
{

//리스트 해제 
//-----------------------------------------------------------------------------------
	GODDESSSTR* pGds = NULL;
	for(GODLIST itr = m_list.begin(); itr != m_list.end();itr++)
	{
		pGds = &(itr->second);
		 if(pGds == NULL)
			continue;

		 Release_List(pGds);
	}

	m_list.clear();	
	m_Count = 0;
//------------------------------------------------------------------------------------
 	
	Init();
}


//하나의 리스트만 삭제한다. 
void CGoddessMgr::Release_List(GODDESSSTR* pGDS)
{

	delete pGDS->pGODModel;
	delete pGDS->pSprModel;
	
	for(register int i=0;i<2;i++)
	{
		delete  pGDS->pEffect[i];
		pGDS->pEffect[i]	= NULL;
		pGDS->hParticle[i]  = NULL;
	}

	pGDS->pGODModel		= NULL;
	pGDS->pSprModel		= NULL;
	
	pGDS->hGoddess		= NULL;
	pGDS->hSprite		= NULL;

}


//허용국가 체크 
BOOL CGoddessMgr::Permission_Country(void)
{

	//국가  체크 후 결과를 세팅한다. 
	if( CCountry::GetSingleton().IsApplyNewVersion())
		return TRUE;


	return FALSE;

}


//여신,요정 객체를 로드한다. 
BOOL CGoddessMgr::Register_God_And_Sprite(void)
{
	
	if(m_IsUse)
		return TRUE;

	return (m_IsUse = TRUE);

}


//랜더링 오브젝트 객체를 등록한다. 
BOOL CGoddessMgr::Register_RenderObj(GODDESSSTR& gds)
{

	//아바타 객체를 가져온다. 
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT(gds.idxMaster,TRUE);
		if(pCHAR == NULL)
			return FALSE;

	//아바타 상태 설정 
	pCHAR->SetAroaState() = gds.bEnable;

	//
	pCHAR->Calc_AruaAddAbility();

	//자신이라면 아바타 정보를 저장한다. 
	m_IsAvata = (lstrcmp(pCHAR->Get_NAME(),g_pAVATAR->Get_NAME()) ? FALSE : TRUE);
		
		
	char* ParticlName[] = 
	{ "3Ddata\\Effect\\_arua_ghost01.eft", "3Ddata\\Effect\\end_effect_02.eft" };

	for(register int i=0;i<2;i++)
	{
		if(gds.pEffect[i]!= NULL)
		{ delete gds.pEffect[i]; gds.pEffect[i] = NULL; }

		DWORD hKey = CStr::GetHASH(ParticlName[i]);
		
		gds.pEffect[i] = g_pEffectLIST->Add_EFFECT(hKey);
		if(	gds.pEffect[i] == NULL)
		{
			_ASSERT(gds.pEffect[i]);
			return FALSE;
		}
		
		gds.pEffect[i]->LinkNODE(pCHAR->GetZMODEL());
		gds.hParticle[i] = gds.pEffect[i]->GetZNODE();
	}

	
	//여신 로드 
	if(gds.pGODModel == NULL)
	{
		gds.pGODModel = new CObjMOB;
		if (gds.pGODModel == NULL)
		{
			_ASSERT(gds.pGODModel);
			return FALSE;
		}

		D3DXVECTOR3 cpos = pCHAR->Get_CurPOS();
		if (!gds.pGODModel->Create(939,cpos,0,TRUE)) 
		{
			LogString (LOG_DEBUG, "Create Goddess Model Data\n");
			delete gds.pGODModel; gds.pGODModel = NULL;
		}
		
		gds.hGoddess = gds.pGODModel->GetZMODEL();
		::setRepeatCount(gds.hGoddess,1);
		::setAnimatableFrame(gds.hGoddess,0);
		::controlAnimatable(gds.hGoddess,0);
	}


	//요정 로드 
	if(gds.pSprModel == NULL)
	{
		gds.pSprModel = new CObjMOB;
		if (gds.pSprModel == NULL)
		{
			_ASSERT(gds.pSprModel);
			return FALSE;
		}

		D3DXVECTOR3 cpos = pCHAR->Get_CurPOS();
	
		if (!gds.pSprModel->Create(940,cpos,0,FALSE)) 
		{
			LogString (LOG_DEBUG, "Create Sprite Model Data\n");
			delete gds.pSprModel; gds.pSprModel = NULL;
		}
	
		gds.hSprite = gds.pSprModel->GetZMODEL();
	}
   
	::controlAnimatable(gds.hGoddess,0);
	::setVisibilityRecursive(gds.hGoddess,0.0f);
	::setVisibilityRecursive(gds.hSprite,0.0f);

	return TRUE;

}


void CGoddessMgr::Set_GDSData(GODDESSSTR& gds,BOOL bonOff,BOOL bPrograss)
{

	if(bPrograss)
	{
		if(bonOff)
			gds.god_State = GOD_SPRITE_EFFECT;	
		else
			gds.god_State = GOD_NONE;
	}
	else
	{
		if(bonOff)
			gds.god_State = GOD_APPEAR_PARTCLE;	
		else
			gds.god_State = GOD_END_EFFECT;
	}

}


//여신 소환 on/off 를 처리한다. 
BOOL CGoddessMgr::SetProcess(DWORD bonOff,WORD idx,BOOL bPrograss)
{
	
  //국가 코드  체크 
	if(!Permission_Country())
		return TRUE;


	GODDESSSTR Gds,*pGds = NULL;

	BOOL bOnOff = bonOff ? 1 : 0;
	

//Old list
//-------------------------------------------------------------------
	GODLIST itr = m_list.find(idx);
	
	if(itr != m_list.end())
	{
		itr->second.bEnable		= bOnOff;
		itr->second.sTick		= GetTickCount();
				
		Set_GDSData(itr->second,bOnOff,bPrograss);
		
		if(!Register_RenderObj(itr->second))
			return FALSE;

		pGds = &(itr->second);

		goto SET_EFF;
	}  
//--------------------------------------------------------------------
	

//New list
//--------------------------------------------------------------------
	//처음에 OFF 라면 리스트를 생성할 필요가 없음.
	if(bOnOff == FALSE)
		return TRUE;
		

	Gds.Init();

	Gds.bEnable		= bOnOff;
	Gds.sTick		= GetTickCount();
	Gds.idxMaster	= idx;
		
	Set_GDSData(Gds,bOnOff,bPrograss);

	if(!Register_RenderObj(Gds))
		return FALSE;
	
	m_list[idx] = Gds;
	m_Count = m_list.size();

	pGds = &Gds;
//--------------------------------------------------------------------

//이펙트 랜더링 세팅 
SET_EFF:

	if(m_IsAvata)
		m_dwAvata = bonOff;


	//요정 모드라면 보이게 만든다. 
	if(pGds->god_State == GOD_SPRITE_EFFECT)
	{
		pGds->fviewSpr = 1.0f;
		::setVisibilityRecursive(pGds->hSprite,1.0f);
	}


	//파티클 애니메이션만 설정 설정 
	if(pGds->god_State == GOD_APPEAR_PARTCLE)
	{
		pGds->pEffect[0]->StartEffect();	
		pGds->fviewGODD = 0.0f;
	}
	else
		pGds->pEffect[0]->StopEffect();	
	
	if(pGds->god_State == GOD_END_EFFECT)
	{	
		pGds->pEffect[1]->StartEffect();	
		pGds->fviewSpr = 1.0f;
	}
	else
		pGds->pEffect[1]->StopEffect();	


	return TRUE;

}


void CGoddessMgr::Update(void)
{
	
	//국가 코드  체크 
	if(!Permission_Country())
		return;


#define TIME_GOD_ACCEPT				9100
#define TIME_APPEAR_GODDESS			3000


	if(m_Count == 0)
		return;


	CObjAVT *pCHAR = NULL;
	DWORD curTick=0,tempTick=0; 
	D3DXVECTOR3 tPos = D3DXVECTOR3(0,0,0);
	
	BOOL IsBeginList = FALSE;
	GODLIST preNode; 
	
	float fRot[4] = { 0 };
	int t1=0,t2=0;



	for(GODLIST itr=m_list.begin();itr!=m_list.end();++itr)
	{

		GODDESSSTR* gds = &(itr->second);
			if(gds == NULL)
				continue;
		

	//	if(gds->god_State == GOD_NONE) 
	//		continue;


		//아바타 객체를 가져온다. 
		pCHAR = g_pObjMGR->Get_ClientCharAVT(gds->idxMaster,TRUE);
			if((pCHAR == NULL) || (gds->god_State == GOD_NONE))
			{
				Release_List(&(itr->second));
				if(itr != m_list.begin())
				{
					preNode = --itr;	
					++itr;
					IsBeginList = FALSE;
				}
				else
					IsBeginList = TRUE;


				m_list.erase(itr);

				if(!(m_Count = m_list.size()))
					break;
				
				//리스트의 구간이 아니라면 Begin 리스트 이다
				if(IsBeginList)
					itr = m_list.begin();
				else
					itr = preNode;
	
				continue;
			}


		//아바타의 좌표
		D3DXVECTOR3 cPos = tPos = pCHAR->Get_CurPOS();
			
		//아바타 회전
		::getRotationQuad(pCHAR->GetZMODEL(),fRot);
	

		//step1 :  파티클 이펙트 출력 
		if(gds->god_State == GOD_APPEAR_PARTCLE)
		{
			curTick  = GetTickCount();
			tempTick = (curTick - gds->sTick);

			//여신 캐릭터 출력 유무 
			if(tempTick >= TIME_APPEAR_GODDESS)
			{
				if(gds->fviewGODD == 0)
				{
					::setRepeatCount(gds->hGoddess,1);
					::setAnimatableFrame(gds->hGoddess,0);
					::controlAnimatable(gds->hGoddess,1);
				}

				//여신 캐릭터를 나타나게 한다. 
				if(ProcessVisible(gds->fviewGODD,0.0008f) == 1)
					gds->god_State = GOD_APPEAR_GODDESS;
				
				::setRotationQuat(gds->hGoddess,fRot);
				::setPosition(gds->hGoddess,tPos.x,tPos.y,tPos.z);	
				::setVisibilityRecursive(gds->hGoddess,gds->fviewGODD);
			}
		}

	
		//step2 : 파티클 & 여신 이펙트 출력 
		if(gds->god_State == GOD_APPEAR_GODDESS)
		{
			curTick  = GetTickCount();
			tempTick = (curTick - gds->sTick);
		

			//일정한 시전이펙트가 끝나면 요정 트래킹모드로 전환한다. 
			if(tempTick >= TIME_GOD_ACCEPT)
			{	
				//여신 캐릭터를 나타나게 한다. 
				t1 = ProcessVisible(gds->fviewGODD,-0.0009f);
				t2 = ProcessVisible(gds->fviewSpr,0.001f);
			
				if((t1 == 0) && (t2 ==    1))
				{	
					gds->god_State = GOD_SPRITE_EFFECT;		
					::controlAnimatable(gds->hGoddess,0);
				}
				
				//요정 
				::setRotationQuat(gds->hSprite,fRot);
				::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);	
				::setVisibilityRecursive(gds->hSprite,gds->fviewSpr);
			}
			            			
			//여신 
			::setRotationQuat(gds->hGoddess,fRot);
			::setPosition(gds->hGoddess,tPos.x,tPos.y,tPos.z);	
			::setVisibilityRecursive(gds->hGoddess,gds->fviewGODD);
		}

		//step3 : 요정 트래킹 모드 
		if(gds->god_State == GOD_SPRITE_EFFECT)
		{
			::setRotationQuat(gds->hSprite,fRot);
			::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);	
					
		}

		//step4 ; 요정을 사라지게 한다. 
		if(gds->god_State == GOD_END_EFFECT)
		{
	
			if(ProcessVisible(gds->fviewSpr,-0.001f) == 0)
				gds->god_State = GOD_NONE;	

			::setRotationQuat(gds->hSprite,fRot);
			::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);	
			::setVisibilityRecursive(gds->hSprite,gds->fviewSpr);
		}
		
	}

}


//Visible 처리를 한다. 
int CGoddessMgr::ProcessVisible(float& fv,float fseq)
{
	
	float fdelta =  g_GameDATA.GetElapsedFrameTime();
	
	fv+=(fseq * fdelta);

	if(fv > 1.0f)
	{	fv = 1.0f; return 1; }

	if(fv < 0.0f)
	{	fv = 0.0f; return 0; }


	return -1;

}
