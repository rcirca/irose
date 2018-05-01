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
#include "GameCommon/Skill.h"
#include "Event/Quest_FUNC.h"
#include "Common/IO_PAT.h"
#include "Country.h"



void CObjCHAR::FireBulletWithPetWeapon( CObjCHAR* pTarget )
{
	if( this->IsPET() )
	{

		int iBulletNO = PAT_BULLET( ((CObjCART*)this)->GetPetParts( RIDE_PART_ARMS ) );


		if ( pTarget && iBulletNO ) 
		{
			//박지호::발사 포인터를 가져온다. 
			///<- 2005/7/25 카트 시스템 파츠 추가로 수정 : nAvy
			/// int	btPoint = PAT_BULLET_FIRE_POINT(((CObjCART*)this)->GetPetParts(RIDE_PART_ARMS),64);
			int btPoint = 0;

			btPoint = PAT_BULLET_FIRE_POINT(((CObjCART*)this)->GetPetParts(RIDE_PART_ARMS),64);

#ifdef _GBC
			g_pBltMGR->Add_BULLET( this, pTarget, iBulletNO, true, btPoint );
#else
			g_pBltMGR->Add_BULLET( this, pTarget, iBulletNO, true, 8 );
#endif			
		}
	}
}



//-------------------------------------------------------------------------------------------------
/// 타격효과를 붙이고, 
/// @todo 타격치는 서버로부터의 전송에 의존한다.. 현재는
void CObjCHAR::ActionEVENT (int iActionIDX)
{
	// 모션중 액션 프레임에 데이타가 없는 경우 
	// 강제로 넣어 죽을때도 액션이 발생한다. 
	if ( this->Get_HP() <= 0 )
		return;

	/// 현재 프레임의 이벤트 진행..
	switch( iActionIDX ) 
	{
		/// 특수 프레임...
	case 1:
	case 2:
	case 3:

	case 91:
		{
			ActionSpecialFrame( iActionIDX );
		}
		break;

		/// 근접		
	case 21:
	case 31:
	case 41:
		{
			// 근접 !
			ActionInFighting( iActionIDX );
		}
		break;

		/// 활		
	case 22:
	case 32:
	case 42:
		{
			// 활공격 !
			ActionBow( iActionIDX );
		}
		break;

		/// 총		
	case 23:
	case 33:
	case 43:
		{
			// 총공격 !
			ActionGun( iActionIDX );
		}
		break;

		/// 스킬		
	case 24:
	case 34:	
		{
			// 스킬공격 !
			ActionSkill( iActionIDX );
		}
		break;

		/// 스킬 근접형 타격 스킬	
	case 25:
	case 35:	
	case 10:
	case 20:
	case 56:
	case 66:

		/// 왼손 무기 총알변경
	case 26:	
		{
			// 스킬공격 !
			ActionImmediateSkill( iActionIDX );
		}
		break;

	case 44:
	case 64:
	case 74:	
	case 84:
		{
			ShowCastingEffect( iActionIDX );
		}

	default :
		//LogString (LOG_DEBUG, "Invalid Action event[ %d ] in %s \n", iActionIDX, Get_NAME() );
		break;
	}

	/*
	/// 한번 때리고 중지
	if ( m_bAttackSTART )
	Attack_END( false );
	*/	
}

//////////////////////////////////////////////////////////////////////////
/// 특수 액션 프레임
//////////////////////////////////////////////////////////////////////////
const int CHAR_STEP_SOLID = 653;
void CObjCHAR::ActionSpecialFrame( int iActionIDX )
{
	switch( iActionIDX )
	{
		/// 발자국 소리
		case 1:
			{				
				if( this->IsA( OBJ_USER) )
				{
					D3DVECTOR vPos = g_pAVATAR->Get_CurPOS();

					CMAP_PATCH* pPatch = g_pTerrain->GetPatchFromWorld( vPos.x, vPos.y );
					if( pPatch )
					{
						int iTileNO = pPatch->GetUpTileNO();

						int iZoneNO = g_pTerrain->GetZoneNO();
						int iZoneType = ZONE_TYPE( iZoneNO );

						int iWalkSoundIDX = CHAR_STEP_SOLID;
						if( this->GetCollisionGroundNode() == NULL )
							iWalkSoundIDX = STEPSOUND( iZoneType, iTileNO );

						g_pSoundLIST->IDX_PlaySound3D( iWalkSoundIDX, this->Get_CurPOS() );				
					}

#ifdef _DEBUG
					if( g_GameDATA.m_bShowCurPos )
						g_pNet->Send_cli_DROP_ITEM( 0, (int)1 );
#endif
				}				
			}
			break;

		//----------------------------------------------------------------------------------------------------
		/// Castle Gear 의 발자국.. 그리고.. 발자국 사운드
		//----------------------------------------------------------------------------------------------------
		case 2:
		case 3:
			{
				if( this->IsPET() )
				{
					int iEffectIDX = PAT_MOVE_EFFECT( ((CObjCART*)this)->GetPetParts( RIDE_PART_LEG ) );
					int iDummyIDX = 0;
					switch( iActionIDX )
					{
						/// 1번 더미에 부착
						case 2:
							{
								iDummyIDX = 1;
							}
							break;
						/// 2번 더미에 부착
						case 3:
							{
								iDummyIDX = 2;
							}
							break;
					}
					CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
					if ( pEffect ) 
					{
						this->LinkDummy( pEffect->GetZNODE(), iDummyIDX );
						this->AddExternalEffect( pEffect );
						pEffect->SetParentCHAR( this );

						pEffect->InsertToScene();
					}
					

					int iSoundIDX  = PAT_MOVE_SOUND( ((CObjCART*)this)->GetPetParts( RIDE_PART_LEG ) );
					if ( iSoundIDX )
					{		
						/*CObjCHAR* pParent = NULL;
						if( pParent )*/
						{
							D3DXVECTOR3 vCur = Get_CurPOS();
							g_pSoundLIST->IDX_PlaySound3D( iSoundIDX, vCur );
							Log_String( LOG_NORMAL, "Play sound[ %d ][ %d ][ %.2f, %.2f, %.2f ]\n", iDummyIDX, iSoundIDX, vCur.x, vCur.y, vCur.z );  
						}/*else
						{
							assert( 0 && "The Parent of cart is invalid" );
						}*/
					}
				}
			}
			break;

		/// resurrection 스킬 사용
		case 91:
			{
				CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iActiveObject, false );

				if( pTarget )
				{
					::setModelBlinkCloseMode( pTarget->GetZMODEL(), false );

					///파티원일경우 서버에서 새로운 HP가 온다. 이경우 다시 리셋시키지 않기 위한 조건
					///2005/3/22-nAvy
					if( pTarget->Get_HP() <= 0 )
						pTarget->Set_HP( 1 );

					pTarget->SetCMD_STOP();

					int iBulletIDX = SKILL_BULLET_NO( m_nDoingSkillIDX );
					if ( iBulletIDX ) 
					{
						short nEffectFileIDX = EFFECT_BULLET_NORMAL( iBulletIDX );
						/// 지역공격마법은 총알을 쏘는게 아니라 타겟에 이펙트를 붙인다.
						CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( nEffectFileIDX, true );
						if ( pHitEFT ) 
						{								
							pHitEFT->LinkNODE ( pTarget->GetZMODEL() );
							pHitEFT->UnlinkVisibleWorld ();
														
							pHitEFT->InsertToScene ();
						}
					}

					if( pTarget->IsA( OBJ_USER ) )
					{
						g_itMGR.CloseDialog( DLG_TYPE_RESTART );
						g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );

						pTarget->Set_HP( ( g_pAVATAR->Get_MaxHP() * 3 ) / 10 );

						/// 경험치 복구
						int iSkillRecoverEXPConst = SKILL_POWER( m_nDoingSkillIDX );						
						g_pAVATAR->Cancel_PenalEXP( iSkillRecoverEXPConst );

						
					}
				}
			}
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
/// 근접 공격 액션
//////////////////////////////////////////////////////////////////////////
void CObjCHAR::ActionInFighting( int iActionIDX )
{
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iActiveObject, true );

	if ( pTarget ) 
	{	
		switch( iActionIDX )
		{
			/// 타격치
		case 21:
			{
				int iEffectIDX = 0;

				//----------------------------------------------------------------------------------------------------				
				/// @brief 팻모드에 따라서 이펙트 선택( 팻은 Arms 의 정보로 부터 효과를 가져온다. )
				//----------------------------------------------------------------------------------------------------
				if( this->IsPET() == false  )
				{
					/// 무기가 있을경우..					
					iEffectIDX = WEAPON_DEFAULT_EFFECT( this->Get_R_WEAPON() );

					/// 무기가 없을경우..
					if( !this->Get_R_WEAPON() )
					{	
						if( this->IsA( OBJ_MOB ) )
							iEffectIDX = NPC_HAND_HIT_EFFECT( this->Get_CharNO() );
					}
				

					pTarget->Hitted( this, iEffectIDX, this->m_nActiveSkillIDX );					

					/// 사운드
					if( pTarget->IsUSER() == false )
					{
						/// 타격사운드 출력
						short nCharIdx = pTarget->Get_CharNO();

						short nHitSound = WEAPON_ATK_HIT_SOUND( this->Get_R_WEAPON() );			
						short nHittedSound = HIT_SOUND( nHitSound, NPC_HIT_MATERIAL_TYPE( nCharIdx ) );
						if ( nHittedSound )
						{
							g_pSoundLIST->IDX_PlaySound3D( nHittedSound, pTarget->Get_CurPOS() );
						}
					}else // 자기 아바타가 타겟인 경우
					{
						/// 타격사운드 출력
						short nHitSound = WEAPON_ATK_HIT_SOUND( this->Get_R_WEAPON() );			
						short nHittedSound = HIT_SOUND( nHitSound, 1 );
						if ( nHittedSound )
						{
							//g_pSoundLIST->IDX_PlaySound( nHittedSound ); // 안3D 로 출력							
							g_pSoundLIST->IDX_PlaySound3D( nHittedSound, Get_CurPOS() );
						}
					}
				}
				else
				{
			 		///<- 2005/7/25 카트 시스템 파츠 추가로 수정 : nAvy
					///전처리기 정의로 구조가 바뀜에 따라 다시 복구 2005/8/31
					iEffectIDX = PAT_HIT_EFFECT( ((CObjCART*)this)->GetPetParts( RIDE_PART_ARMS ) );					

					pTarget->Hitted( this, iEffectIDX, 0 );

					int iSoundIDX = 0;
			 		///<- 2005/7/25 카트 시스템 파츠 추가로 수정 : nAvy
					///전처리기 정의로 구조가 바뀜에 따라 다시 복구 2005/8/31 navye
					iSoundIDX  = PAT_HIT_SOUND( ((CObjCART*)this)->GetPetParts( RIDE_PART_ARMS ) );
					//if( CCountry::GetSingleton().IsApplyBattleCart() )
					//	iSoundIDX  = PAT_HIT_SOUND( ((CObjCART*)this)->GetPetParts( RIDE_PART_ARMS ) );
					//else
					//	iSoundIDX  = PAT_HIT_SOUND( ((CObjCART*)this)->GetPetParts( RIDE_PART_ABIL ) );
					///->

					if ( iSoundIDX )
						g_pSoundLIST->IDX_PlaySound3D( iSoundIDX, Get_CurPOS() );

				}
			}
			break;

			/// 공격 사운드
		case 31:
			{
				/// 공격 사운드 설정			
				short nHitStartSound = WEAPON_ATK_START_SOUND( this->Get_R_WEAPON() );	

				if( !( this->Get_R_WEAPON() ) && this->IsA( OBJ_MOB ) )
				{
					nHitStartSound = NPC_ATTACK_SOUND( this->m_nCharIdx );
				}

				if ( nHitStartSound )
				{
					if( this->IsUSER() ) // 자기 아바타가 공격하는 경우에는 안3D 로 출력
						//g_pSoundLIST->IDX_PlaySound( nHitStartSound );
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
					else 
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
				}	
			}
			break;			
		}
	}			
}

/// 활 공격 액션
void CObjCHAR::ActionBow( int iActionIDX )
{
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iActiveObject, true );

	if ( pTarget ) 
	{	
		switch( iActionIDX )
		{			
			/// 발사, 사운드
		case 22:
			{

				//----------------------------------------------------------------------------------------------------
				/// Pet 이 쏘는거라면 팻의 정보로 총알을 쏜다.
				//----------------------------------------------------------------------------------------------------
				if( this->IsPET() )
				{
					FireBulletWithPetWeapon( pTarget );
					return;
				}

				// 발사
				// 공격 동작시 장착된 무기 발사...				
				int iBulletIDX = Get_BulletNO ();
				if ( pTarget && iBulletIDX ) 
				{
					g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX );
					if( IsA( OBJ_USER ))
						g_pAVATAR->SubActiveBulletQuantity();


				}

				//if( pTarget->IsUSER() == false )
				{
					/// 타격사운드 출력
					short nFireSound = WEAPON_ATK_FIRE_SOUND( this->Get_R_WEAPON() );								
					if ( nFireSound )
					{
						if ( IsUSER() ) {
							//g_pSoundLIST->IDX_PlaySound( nFireSound );
							g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
						}
						else {
							g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
						}
					}
				}
			}
			break;		

			/// 공격 사운드
		case 32:
			{
				/// 공격 사운드 설정			
				short nHitStartSound = WEAPON_ATK_START_SOUND( this->Get_R_WEAPON() );					
				if ( nHitStartSound )
				{
					if ( IsUSER() ) 
						//g_pSoundLIST->IDX_PlaySound( nHitStartSound );
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
					else
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
				}	
			}
			break;			
		}
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 총 공격 액션
//----------------------------------------------------------------------------------------------------

void CObjCHAR::ActionGun( int iActionIDX )
{
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iActiveObject, true );

	if ( pTarget ) 
	{	
		switch( iActionIDX )
		{

			/// 발사, 사운드
		case 23:
			{
				//----------------------------------------------------------------------------------------------------
				/// Pet 이 쏘는거라면 팻의 정보로 총알을 쏜다.
				//----------------------------------------------------------------------------------------------------
				if( this->IsPET() )
				{
					FireBulletWithPetWeapon( pTarget );
					return;
				}



				// 발사
				// 공격 동작시 장착된 무기 발사...				
				int iBulletIDX = Get_BulletNO ();
				if ( pTarget && iBulletIDX ) 
				{
					g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX );

					if( IsA( OBJ_USER ))
						g_pAVATAR->SubActiveBulletQuantity();

				}


				//if( pTarget->IsUSER() == false )
				{
					/// 발사사운드 출력
					short nFireSound = WEAPON_ATK_FIRE_SOUND( this->Get_R_WEAPON() );								
					if ( nFireSound )
					{
						if ( IsUSER() ) 
							//g_pSoundLIST->IDX_PlaySound( nFireSound );
							g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
						else
							g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
					}
				}
			}
			break;		

			/// 공격 사운드
		case 33:
			{
				/// 공격 사운드 설정			
				short nHitStartSound = WEAPON_ATK_START_SOUND( this->Get_R_WEAPON() );					
				if ( nHitStartSound )
				{
					D3DXVECTOR3 Velocity = pTarget->Get_CurPOS() - Get_CurPOS(); // 1초 동안에 이동하는 벡터

					if ( IsUSER() ) 
						//g_pSoundLIST->IDX_PlaySound( nHitStartSound );
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS(), &Velocity );
					else
						g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS(), &Velocity );
				}	
			}
			break;			
		}
	}
}

/// 스킬 공격 액션
void CObjCHAR::ActionSkill( int iActionIDX )
{
	CObjCHAR *pTarget = 0;

	switch( iActionIDX )
	{		
		/// 발사, 사운드
	case 24:
		{
			int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
			if( iSkillIdx == 0 )
				break;

			switch( SKILL_TYPE( iSkillIdx ) )
			{
				///// self bound
			case SKILL_ACTION_SELF_BOUND_DURATION:
			case SKILL_ACTION_SELF_BOUND:
			case SKILL_ACTION_SELF_STATE_DURATION:
			case SKILL_ACTION_SELF_DAMAGE:
				{
					ProcEffectedSkill();
					CSkillManager::ActionSkill( iSkillIdx, this, this );		
				}						
				break;

				///// Target bound
			case SKILL_ACTION_TARGET_BOUND_DURATION:
			case SKILL_ACTION_TARGET_BOUND:
			case SKILL_ACTION_TARGET_STATE_DURATION:
				{
					CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );

					/// 타겟이 먼저 죽었어도.. 소진해야된다..
					ProcEffectedSkill();
					if( pTarget )
					{							
						CSkillManager::ActionSkill( iSkillIdx, this, pTarget );		
					}
				}						
				break;



				/// 타겟대상 마법
			case SKILL_ACTION_FIRE_BULLET:
				{
					CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );
					if ( pTarget ) 
					{
						_ASSERT( this->m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( pTarget->Get_INDEX() ) );
					}

					ProcEffectedSkill();
					CSkillManager::ActionSkill( iSkillIdx, this, pTarget );		
				}
				break;
				/// 지역대상 마법
			case SKILL_ACTION_AREA_TARGET:
				{
					CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );
					CSkillManager::ActionSkill( iSkillIdx, this, pTarget, this->m_PosGOTO, true );		
					ProcEffectedSkill();
				}
				break;

			case SKILL_ACTION_SUMMON_PET:
				{
					SummonMob();
				}
				break;

			default:
				break;

			}							



			/// 발사 사운드 설정			
			short nFireSound = SKILL_BULLET_FIRE_SOUND( iSkillIdx );					
			if ( nFireSound )
			{
				if ( IsUSER() ) 
					//g_pSoundLIST->IDX_PlaySound( nFireSound );
					g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
				else
					g_pSoundLIST->IDX_PlaySound3D( nFireSound, Get_CurPOS() );
			}

		}
		break;		


		/// 타격( 발사 ) 사운드
	case 34:
		{
			int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
			if( iSkillIdx == 0 )
				break;

			/// 공격 사운드 설정			
			short nHitStartSound = SKILL_BULLET_FIRE_SOUND( this->m_nActiveSkillIDX );					
			if ( nHitStartSound )
			{
				if ( IsUSER() ) 
					//g_pSoundLIST->IDX_PlaySound( nHitStartSound );
					g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
				else
					g_pSoundLIST->IDX_PlaySound3D( nHitStartSound, Get_CurPOS() );
			}	
			//CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );

			///// 타겟이 있고 셀프가 아닐경우...
			//if( pTarget && 
			//	( SKILL_TYPE( this->m_nActiveSkillIDX ) != SKILL_ACTION_SELF_BOUND_DURATION &&
			//	SKILL_TYPE( this->m_nActiveSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
			//	SKILL_TYPE( this->m_nActiveSkillIDX ) != SKILL_ACTION_SELF_STATE_DURATION ) )
			//{
			//	if( this->m_nActiveSkillIDX == 0 )
			//		break;

			//	int iEffectIDX = SKILL_HIT_EFFECT( this->m_nActiveSkillIDX );
			//	if ( iEffectIDX ) 
			//	{
			//		CEffect *pCastingEFT = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
			//		if ( pCastingEFT ) 
			//		{
			//			if( SKILL_HIT_EFFECT_LINKED_POINT( this->m_nActiveSkillIDX ) == INVALID_DUMMY_POINT_NUM )
			//			{
			//				pCastingEFT->LinkNODE ( pTarget->GetZMODEL() );
			//			}else
			//			{
			//				pTarget->LinkDummy( pCastingEFT->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( this->m_nActiveSkillIDX ) );
			//			}						

			//			pCastingEFT->SetParentCHAR( pTarget );
			//			pTarget->AddExternalEffect( pCastingEFT );

			//			//pCastingEFT->UnlinkVisibleWorld ();
			//			pCastingEFT->InsertToScene ();
			//		}
			//	}
			//}
		}
		break;
	}	
}


void CObjCHAR::ShowCastingEffect( int iActionIDX )
{
	switch( iActionIDX )
	{
		/// 스킬 이펙트 시연.
		/// 캐스팅시 이벤트는 m_nActiveSkillIDX 의 인덱스 이용..
		case 44:				
			{
				ActionSkillCastingFrame( 0 );
			}
			break;		

			/// 캐스팅시 이벤트는 m_nActiveSkillIDX 의 인덱스 이용..
		case 64:		
			{
				ActionSkillCastingFrame( 1 );
			}	
			break;

			/// 캐스팅시 이벤트는 m_nActiveSkillIDX 의 인덱스 이용..
		case 74:
			{
				ActionSkillCastingFrame( 2 );
			}
			break;
		case 84:
			{
				ActionSkillCastingFrame( 3 );
			}
			break;
	}
}

/// 44,64,75, 45,65,75 처리..
void CObjCHAR::ActionSkillCastingFrame( int iCastingOrder )
{
	int iSkillIDX = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
	if( iSkillIDX == 0 )
		return;

	int iEffectIDX = SKILL_CASTING_EFFECT( iSkillIDX, iCastingOrder );
	if ( iEffectIDX ) 
	{
		CEffect *pCastingEFT = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
		if ( pCastingEFT ) 
		{			
			
			if( SKILL_CASTING_EFFECT_POINT( iSkillIDX, iCastingOrder ) == INVALID_DUMMY_POINT_NUM )
			{
				pCastingEFT->LinkNODE ( this->GetZMODEL() );
			}else
			{
				LinkDummy( pCastingEFT->GetZNODE(), SKILL_CASTING_EFFECT_POINT( iSkillIDX, iCastingOrder ) );
			}					


			/// 내 주위 범위 데미지 타입이거나 지역 스킬 이라면..
			if(	SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_DAMAGE ||
				SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_AREA_TARGET )
			{
				/// 스킬 실제 액션 동작중의 캐스팅 이펙트일 경우만..
				if( iCastingOrder == 2 || iCastingOrder == 3 )
				{
					/// 링크 하지 않는다.
					pCastingEFT->UnlinkVisibleWorld ();
				}
			}else
			{
				pCastingEFT->SetParentCHAR( this );
				AddExternalEffect( pCastingEFT );
			}

			//pCastingEFT->UnlinkVisibleWorld ();
			pCastingEFT->InsertToScene ();
		}
	}

	/// 타격 사운드 설정			
	int iHitSound = SKILL_HIT_DUMMY_SOUND( iSkillIDX, iCastingOrder );					
	if ( iHitSound )
	{
		if ( IsUSER() ) 
			//g_pSoundLIST->IDX_PlaySound( nFireSound );
			g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );
		else
			g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );
	}		
}

/// 연타공격 효과( 더미.. )
void CObjCHAR::RepeatedlyHitEffect( int iHitOrder )
{
	int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
	if( iSkillIdx == 0 )
		return;

	CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );
	if ( pTarget ) 
	{
		_ASSERT( this->m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( pTarget->Get_INDEX() ) );

		int iEffectIDX = SKILL_HIT_DUMMY_EFFECT( iSkillIdx, iHitOrder );
		if ( iEffectIDX ) 
		{
			CEffect *pDummyEFT = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
			if ( pDummyEFT ) 
			{
				pDummyEFT->LinkNODE ( pTarget->GetZMODEL() );

				pDummyEFT->SetParentCHAR( pTarget );
				pTarget->AddExternalEffect( pDummyEFT );

				pDummyEFT->InsertToScene ();
			}
		}	
	}		

	/// 타격 사운드 설정			
	int iHitSound = SKILL_HIT_DUMMY_SOUND( iSkillIdx, iHitOrder );					
	if ( iHitSound )
	{
		if ( IsUSER() ) 
			//g_pSoundLIST->IDX_PlaySound( nFireSound );
			g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );
		else
			g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );
	}
}

void CObjCHAR::FireEffectBullet( CObjCHAR *pTarget, int iBulletIDX, bool bDummy, int iSkillIDX )
{	
	if ( pTarget && iBulletIDX ) 
	{
		if( !bDummy )
			g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX, ( iSkillIDX == 0 ), 0, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), false, bDummy, iSkillIDX );		
		else
		{
			g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX, true, 0, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), false, bDummy, iSkillIDX );			
		}
	}
}

/// 근접 타격 스킬 공격 액션
void CObjCHAR::ActionImmediateSkill( int iActionIDX )
{
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iActiveObject, true );

	if( pTarget )
	{
		switch( iActionIDX )
		{

			/// 왼손 무기 총알변경
			/// 56, 66 은 총알에 데미지가 없는 더미 총알이라는걸 넣는다.
		case 26:
			{
				int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
				if( iSkillIdx == 0 )
					return;

				if( IsA( OBJ_USER ))
					g_pAVATAR->SubActiveBulletQuantity();

				FireEffectBullet( pTarget, SKILL_BULLET_NO( iSkillIdx ), false, iSkillIdx );
			}
			break;
		case 56:
		case 66:
			{
				int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
				if( iSkillIdx == 0 )
					return;

				FireEffectBullet( pTarget, SKILL_BULLET_NO( iSkillIdx ), true, iSkillIdx );
			}
			break;
		

			/// 연타 더미 이펙트
		case 10:
			{
				RepeatedlyHitEffect( 0 );
			}
			break;
		case 20:
			{
				RepeatedlyHitEffect( 1 );
			}
			break;

			///타격
		case 25:
			{
				int iSkillIdx = ( this->m_nActiveSkillIDX )? this->m_nActiveSkillIDX : this->m_nDoingSkillIDX;
				if( iSkillIdx == 0 )
					return;

				CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( this->m_iServerTarget, true );
				if ( pTarget ) {
					_ASSERT( this->m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( pTarget->Get_INDEX() ) );
				}

				ProcEffectedSkill();
				CSkillManager::ActionSkill( iSkillIdx, this, pTarget );	


				///
				/// 타격
				///

				int iEffectIDX;

				if( pTarget )
				{
					iEffectIDX = SKILL_HIT_EFFECT( iSkillIdx );
					if( iEffectIDX == 0 )
					{
						iEffectIDX = WEAPON_DEFAULT_EFFECT( this->Get_R_WEAPON() );

						/// 무기가 없을경우..
						if( !this->Get_R_WEAPON() )
						{	
							if( this->IsA( OBJ_MOB ) )
								iEffectIDX = NPC_HAND_HIT_EFFECT( this->Get_CharNO() );
						}

						pTarget->Hitted( this, iEffectIDX, 0, false, false, true );
					}else
					{
						pTarget->Hitted( this, iEffectIDX, this->m_nActiveSkillIDX, true, false, true );
					}
				}

				/// 발사 사운드 설정			
				short nHittedSound = SKILL_HIT_SOUND( iSkillIdx );					
				if ( nHittedSound )
				{
					if ( IsUSER() ) 
						//g_pSoundLIST->IDX_PlaySound( nHittedSound );
						g_pSoundLIST->IDX_PlaySound3D( nHittedSound, Get_CurPOS() );
					else
						g_pSoundLIST->IDX_PlaySound3D( nHittedSound, Get_CurPOS() );
				}

			}
			break;

			/// 공격 사운드 설정
		case 35:
			{
			}
			break;
		}
	}
}