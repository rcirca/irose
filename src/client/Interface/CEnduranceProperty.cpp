#include "stdafx.h"	
#include "CEnduranceProperty.h"	
#include <algorithm>
#include <functional>
#include "../CObjCHAR.h"	
#include "it_mgr.h"	
#include "CToolTipMgr.h"
#include "IO_ImageRes.h"
#include "TypeResource.h"
#include "OBJECT.h"
#include "tgamectrl/resourcemgr.h"
#include "../GameCommon/Item.h"
#include "../Country.h"

#include "Dlgs/ChattingDlg.h"
#include "../System/CGame.h"
#include "CTDrawImpl.h"

int CEndurancePack::m_iDrawIconCnt = 0;
std::map< int , int > CEndurancePack::m_StateFlagTable;
std::map< int , int > CEndurancePack::m_StateSTBIDXTable;


const int ENDU_ICON_X		= 250;
const int ENDU_ICON_Y		= 40;

const int ENDU_ICON_WIDTH	= 20;
const int ENDU_ICON_HEIGHT	= 20;





void DeleteEntity( CEnduranceProperty* pEnduranceProperty )
{
	if( pEnduranceProperty )
	{
		pEnduranceProperty->DeleteEffect();
		delete pEnduranceProperty;
		pEnduranceProperty = NULL;
	}
}

//void ClearEffectResource( CEnduranceProperty* pEnduranceProperty )
//{
//	if( pEnduranceProperty )
//	{
//		pEnduranceProperty->ClearEffectResource();
//		delete pEnduranceProperty;
//		pEnduranceProperty = NULL;
//	}
//}

///
/// Endurance entity pack.
///

CEndurancePack::CEndurancePack()
{
	m_pObjCHAR = NULL;
	m_dwStateFlag = 0;

	MakeStateFlagTable();
	MakeStateSTBIDXTable();

	memset( m_EntityList, 0, sizeof( m_EntityList ) );
}

CEndurancePack::CEndurancePack( CObjCHAR* pObjCHAR ) : m_pObjCHAR( pObjCHAR )
{
	m_dwStateFlag = 0;	

	//m_CurrentStateValue.reserve( ING_MAX_STATUS );
	//for( int i = 0; i < ING_MAX_STATUS ;i++ )
	//{
	//	m_CurrentStateValue.push_back( 0 );
	//}

	memset( m_CurrentStateValue, 0, sizeof( m_CurrentStateValue ) );

	MakeStateFlagTable();	
	MakeStateSTBIDXTable();

	memset( m_EntityList, 0, sizeof( m_EntityList ) );

	m_dwUpdateCheckTime = g_GameDATA.GetGameTime();
	m_dwElapsedUpdateTime = 0;
}

CEndurancePack::~CEndurancePack()
{
	ClearEntityPack();
}

void CEndurancePack::ResetTimer()
{
	m_dwUpdateCheckTime = g_GameDATA.GetGameTime();
	m_dwElapsedUpdateTime = 0;
}

void CEndurancePack::MakeStateFlagTable()
{
	if( !CEndurancePack::m_StateFlagTable.empty() )
		return;

	m_StateFlagTable[ FLAG_ING_HP ]				= ING_INC_HP;
	m_StateFlagTable[ FLAG_ING_MP ]				= ING_INC_MP;
	m_StateFlagTable[ FLAG_ING_POISONED ]		= ING_POISONED;

	m_StateFlagTable[ FLAG_ING_MAX_HP ]			= ING_INC_MAX_HP;
	m_StateFlagTable[ FLAG_ING_MAX_MP ]			= ING_INC_MAX_MP;
	m_StateFlagTable[ FLAG_ING_INC_MOV_SPEED ]	= ING_INC_MOV_SPD;
	m_StateFlagTable[ FLAG_ING_DEC_MOV_SPEED ]	= ING_DEC_MOV_SPD;
	m_StateFlagTable[ FLAG_ING_INC_ATK_SPEED ]	= ING_INC_ATK_SPD;
	m_StateFlagTable[ FLAG_ING_DEC_ATK_SPEED ]	= ING_DEC_ATK_SPD;
	m_StateFlagTable[ FLAG_ING_INC_APOWER ]		= ING_INC_APOWER;
	m_StateFlagTable[ FLAG_ING_DEC_APOWER ]		= ING_DEC_APOWER;
	m_StateFlagTable[ FLAG_ING_INC_DPOWER ]		= ING_INC_DPOWER;
	m_StateFlagTable[ FLAG_ING_DEC_DPOWER ]		= ING_DEC_DPOWER;

	m_StateFlagTable[ FLAG_ING_INC_RES ]		= ING_INC_RES;
	m_StateFlagTable[ FLAG_ING_DEC_RES ]		= ING_DEC_RES;
	m_StateFlagTable[ FLAG_ING_INC_HIT ]		= ING_INC_HIT;
	m_StateFlagTable[ FLAG_ING_DEC_HIT ]		= ING_DEC_HIT;
	m_StateFlagTable[ FLAG_ING_INC_CRITICAL ]	= ING_INC_CRITICAL;
	m_StateFlagTable[ FLAG_ING_DEC_CRITICAL ]	= ING_DEC_CRITICAL;
	m_StateFlagTable[ FLAG_ING_INC_AVOID ]		= ING_INC_AVOID;
	m_StateFlagTable[ FLAG_ING_DEC_AVOID ]		= ING_DEC_AVOID;

	m_StateFlagTable[ FLAG_ING_DUMB ]			= ING_DUMB;
	m_StateFlagTable[ FLAG_ING_SLEEP ]			= ING_SLEEP;
	m_StateFlagTable[ FLAG_ING_FAINTING ]		= ING_FAINTING;
	m_StateFlagTable[ FLAG_ING_DISGUISE ]		= ING_DISGUISE;
	m_StateFlagTable[ FLAG_ING_TRANSPARENT ]	= ING_TRANSPARENT;
	m_StateFlagTable[ FLAG_ING_SHIELD_DAMAGE ]	= ING_SHIELD_DAMAGE;
	m_StateFlagTable[ FLAG_ING_DUMMY_DAMAGE ]	= ING_DUMMY_DAMAGE;	

	m_StateFlagTable[ FLAG_ING_DEC_LIFE_TIME ]	= ING_DEC_LIFE_TIME;		
}

void CEndurancePack::MakeStateSTBIDXTable()
{
	if( !CEndurancePack::m_StateSTBIDXTable.empty() )
		return;

	///
	/// STATE_PRIFITS_LOSSES_BY_STATE 원래 여기게 넣기로 했었는데...
	///
	
	m_StateSTBIDXTable[ ING_INC_HP ]				= 1;
	m_StateSTBIDXTable[ ING_INC_MP ]				= 4;
	m_StateSTBIDXTable[ ING_POISONED ]				= 7;

	m_StateSTBIDXTable[ ING_INC_MAX_HP ]			= 12;
	m_StateSTBIDXTable[ ING_INC_MAX_MP ]			= 13;	

	m_StateSTBIDXTable[ ING_INC_MOV_SPD ]			= 14;
	m_StateSTBIDXTable[ ING_DEC_MOV_SPD ]			= 15;
	m_StateSTBIDXTable[ ING_INC_ATK_SPD ]			= 16;
	m_StateSTBIDXTable[ ING_DEC_ATK_SPD ]			= 17;
	m_StateSTBIDXTable[ ING_INC_APOWER ]			= 18;	// 유지 : 공격력
	m_StateSTBIDXTable[ ING_DEC_APOWER ]			= 19;
	m_StateSTBIDXTable[ ING_INC_DPOWER ]			= 20;	// 유지 : 방어력
	m_StateSTBIDXTable[ ING_DEC_DPOWER ]			= 21;
	m_StateSTBIDXTable[ ING_INC_RES ]				= 22;	// 유지 : 항마력
	m_StateSTBIDXTable[ ING_DEC_RES ]				= 23;
	m_StateSTBIDXTable[ ING_INC_HIT ]				= 24;	// 유지 : 명중
	m_StateSTBIDXTable[ ING_DEC_HIT ]				= 25;
	m_StateSTBIDXTable[ ING_INC_CRITICAL ]			= 26;	// 유지 : 크리티컬
	m_StateSTBIDXTable[ ING_DEC_CRITICAL ]			= 27;
	m_StateSTBIDXTable[ ING_INC_AVOID ]				= 28;	// 유지 : 회피력 
	m_StateSTBIDXTable[ ING_DEC_AVOID ]				= 29;
	m_StateSTBIDXTable[ ING_DUMB ]					= 30;	// 유지 : 벙어리 : skill stb 5번 컬럼이 7~14인 스킬 사용 못함.
	m_StateSTBIDXTable[ ING_SLEEP ]					= 31;	// 유지 : 수면 : 명령 불능 상태로.. 공격 받으면 풀림
	m_StateSTBIDXTable[ ING_FAINTING ]				= 32;	// 유지 : 기절 : 수면과 같으나 공격 받아도 안풀림
	m_StateSTBIDXTable[ ING_DISGUISE ]				= 33;	// 유지 : 위장 : 
	m_StateSTBIDXTable[ ING_TRANSPARENT ]			= 34;	// 유지 : 투명 :
	m_StateSTBIDXTable[ ING_SHIELD_DAMAGE ]			= 35;	// 유지 : 방패 데미지
	m_StateSTBIDXTable[ ING_DUMMY_DAMAGE ]			= 36;	// 유지 : 추가 데미지
		
	m_StateSTBIDXTable[ ING_DEC_LIFE_TIME ]			= 43;	// 처리 : 소환된 케릭의 생명 시간...	추가: 2004. 4. 6 	
}	
	

CEnduranceProperty* CEndurancePack::GetEntity( int Index )
{
	if( Index >= ING_MAX_STATUS || m_EntityList == NULL )
		return NULL;

	return m_EntityList[ Index ];
}

CEnduranceProperty*	CEndurancePack::GetEntityByStateType( int iStateType )
{
	return GetEntity( iStateType );
}

bool CEndurancePack::AddEntity( int iEntityIDX, int iStatusSTBNO, int iEnduranceTime, int iEntityType )
{	
	if( m_pObjCHAR == NULL || m_pObjCHAR->Get_HP() <= 0 )
	{
		assert( 0 && "AddEntity failed[ HP <= 0 ]" );
		return false;
	}

	if( iEnduranceTime <= 0 )
		return false;

	CEnduranceProperty* pEnduranceProperty = NULL;

	switch( iEntityType )
	{
		case ENDURANCE_TYPE_SKILL:
			pEnduranceProperty = new CEnduranceSkill();
			if( pEnduranceProperty == NULL )
				return false;

			if( pEnduranceProperty->CreateEnduranceEntity(  m_pObjCHAR, iEntityIDX, iStatusSTBNO, iEnduranceTime ) == false )
			{				
				sprintf( g_MsgBuf, "Create EnduranceSkill failed[ %d ]", iEntityIDX );
				g_itMGR.OpenMsgBox( g_MsgBuf );

				delete pEnduranceProperty;
				return false;
			}

			break;

		case ENDURANCE_TYPE_USEITEM:
			pEnduranceProperty = new CEnduranceItem( );
			if( pEnduranceProperty == NULL )
				return false;

			if( pEnduranceProperty->CreateEnduranceEntity(  m_pObjCHAR, iEntityIDX, iStatusSTBNO, iEnduranceTime ) == false )
			{				
				sprintf( g_MsgBuf, "Create EnduranceItem failed[ %d ]", iEntityIDX );
				g_itMGR.OpenMsgBox( g_MsgBuf );

				delete pEnduranceProperty;
				return false;
			}

			break;
	}

	if( iStatusSTBNO != 0 )
	{
		if( ING_MAX_STATUS > STATE_TYPE( iStatusSTBNO ) )
		{		
			/// for vector container
			/// 중복 적용된 상태가 있다면 제거한다.
			if( m_EntityList[ STATE_TYPE( iStatusSTBNO ) ] )
			{
				delete m_EntityList[ STATE_TYPE( iStatusSTBNO ) ];
				m_EntityList[ STATE_TYPE( iStatusSTBNO ) ] = NULL;
			}else
			{
				if( m_pObjCHAR->IsA( OBJ_USER ) )
				{
					/// 쳇박스에 정보 출력
					g_itMGR.AppendChatMsg( STATE_SETTING_STRING( iStatusSTBNO ), IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}

			m_EntityList[ STATE_TYPE( iStatusSTBNO ) ] = pEnduranceProperty;		

			AddStateFlag( c_dwIngFLAG[ STATE_TYPE( iStatusSTBNO ) ] );
		}
	}	

	return true;
}

void CEndurancePack::ClearEntityPack()
{
	std::for_each( m_EntityList, m_EntityList + ING_MAX_STATUS, DeleteEntity );	
	memset( m_EntityList, 0, sizeof( m_EntityList ) );

	memset( m_CurrentStateValue, 0, sizeof( m_CurrentStateValue ) );
	m_dwStateFlag = 0;	
}

/// Update all entities.
/// 지속적인 상태 변경은 직접 여기서 처리해준다.
/// 기타 효과 표현이나 화면출력등은 각 엔티티에 미룬다.
void CEndurancePack::Update()
{
	if( m_pObjCHAR == NULL )
		assert( 0 && "CEndurancePack::Update() has null char object.");

	/*if( m_CurrentStateValue.empty() )
		return;*/

	//////////////////////////////////////////////////////////////////////////	
	/// 지속적인 적용능력 적용..	
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	m_dwElapsedUpdateTime += dwCurrentTime - m_dwUpdateCheckTime;		/// 이전프레임에서 현재 프레임 사이에 흐른시간을 더해준다.
	m_dwUpdateCheckTime = dwCurrentTime;
	
	/// 속성 수정은 초당 적용..
	if( m_dwElapsedUpdateTime > 1000 )
	{
		/// hp
		if( m_CurrentStateValue[ ING_INC_HP ] )
		{
			m_pObjCHAR->Add_HP( m_CurrentStateValue[ ING_INC_HP ] );
			if( m_pObjCHAR->Get_HP() >= m_pObjCHAR->Get_MaxHP() )
			{
				m_pObjCHAR->Set_HP( m_pObjCHAR->Get_MaxHP() );					
			}
		}

		/// mp
		if( m_CurrentStateValue[ ING_INC_MP ] )
		{
			m_pObjCHAR->Add_MP( m_CurrentStateValue[ ING_INC_MP ] );
			if( m_pObjCHAR->Get_MP() >= m_pObjCHAR->Get_MaxMP() )
			{
				m_pObjCHAR->Set_MP( m_pObjCHAR->Get_MaxMP() );					
			}
		}		
		
		/// 중독		
		if( m_CurrentStateValue[ ING_POISONED ] )
		{
			if( ( m_pObjCHAR->Get_HP() - m_CurrentStateValue[ ING_POISONED ] ) > 0 )
			{				
				m_pObjCHAR->Sub_HP( m_CurrentStateValue[ ING_POISONED ] );
			}
		}

		/// 소환수
		if( m_EntityList[ ING_DEC_LIFE_TIME ] )
		{
			if( ( m_pObjCHAR->Get_HP() - STATE_APPLY_ABILITY_VALUE( 43, 0 ) ) > 0 )
			{
				if( m_pObjCHAR->Sub_HP( STATE_APPLY_ABILITY_VALUE( 43, 0 ) ) <= 0 )
					m_pObjCHAR->Set_HP( 1 );
			}
		}

		m_dwElapsedUpdateTime -= 1000;		
	}

	CEnduranceProperty* pEntity = NULL;
	for( int i = 0; i < ING_MAX_STATUS; i++ )
	{
		pEntity = m_EntityList[ i ];
		if( pEntity )
		{
			if( i != STATE_TYPE( pEntity->GetStateSTBNO() ) )
				assert( 0 && "STATE_TYPE mismatch!!");
			/// if expired...
			if( Proc( pEntity ) == false )		
			{
				/// 해당 상태에 대한 엔티티를 제거한다.
				DeleteEntityByStateType( i );
				m_EntityList[ i ] = NULL;
			}		
		}
	}	
}

void CEndurancePack::SetStateValue( int iType, int iValue )
{ 
	switch( iType )
	{
		case ING_INC_MOV_SPD:
		case ING_DEC_MOV_SPD:
			{
				/// 이동 속도에 관한것이라면 한번 갱신해준다.
				float fAdjRate = m_pObjCHAR->Get_MoveSPEED() / m_pObjCHAR->Get_DefaultSPEED ();

				m_CurrentStateValue[ iType ] = iValue; 

				m_pObjCHAR->m_fAdjustSPEED = m_pObjCHAR->Get_DefaultSPEED () * fAdjRate;
				

				// 이동 속도 모션 스피드 설정...
				//if( m_pObjCHAR->Get_STATE() | CS_MOVE )
				//	m_pObjCHAR->Set_ModelSPEED( m_pObjCHAR->Get_MoveSPEED() );
			}
			break;

		/*case ING_INC_MAX_HP:
			{
				m_CurrentStateValue[ iType ] = iValue; 	

				g_pAVATAR->Add_HP( iValue );
			}
			break;
		case ING_INC_MAX_MP:
			{
				m_CurrentStateValue[ iType ] = iValue; 	

				g_pAVATAR->Add_MP( iValue );
			}
			break;*/

		default:
			m_CurrentStateValue[ iType ] = iValue; 	
			break;
	}
}

bool CEndurancePack::Proc( CEnduranceProperty* pEntity )
{
	/// if expired, return.
	if( !(pEntity->Proc( m_pObjCHAR )) )
		return false;
	
	return true;
}

void CEndurancePack::Draw()
{
	///reset displayed icon count
	CEndurancePack::m_iDrawIconCnt = 0;
	RECT	rcIcon;
	CInfo	Tooltip;
	POINT	ptMouse;
	POINT	ptDrawTooltip;

	CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
	CEnduranceProperty* pEntity = NULL;
	for( int i = 0; i < ING_MAX_STATUS; i++ )
	{
		pEntity = m_EntityList[ i ];
		if( pEntity )
		{			/// If pObjChar is my avatar, draw icon
			if( m_pObjCHAR->IsA( OBJ_USER ) )
			{
				int iIconNO = pEntity->GetIconNO();
				rcIcon.left = ENDU_ICON_X + ( CEndurancePack::m_iDrawIconCnt * ( ENDU_ICON_WIDTH + 2 ) );
				rcIcon.top  = ENDU_ICON_Y;
				rcIcon.right  = rcIcon.left + ENDU_ICON_WIDTH;
				rcIcon.bottom = rcIcon.top  + ENDU_ICON_HEIGHT;

				///------------------------------------------------------------------------------
				/// flash..
				int iElapsedTime = g_GameDATA.GetGameTime() - pEntity->GetStartTime();
				int iEnduranceTime = pEntity->GetEnduranceTime() * 1000;
				int iRemainedTime = abs( iEnduranceTime - iElapsedTime );
				int iVisibility = 255;
				if( iRemainedTime < 10000 || ( iElapsedTime > ( iEnduranceTime ) ) )
					iVisibility = ( ( iRemainedTime / 200 ) & 1 )? 255 : 0;
				///------------------------------------------------------------------------------

				pDrawObj->Draw( rcIcon.left, rcIcon.top, IMAGE_RES_STATE_ICON, iIconNO, D3DCOLOR_RGBA( 255,255,255,iVisibility ) );

				CGame::GetInstance().Get_MousePos( ptMouse );
				if( PtInRect(&rcIcon, ptMouse ))
				{
					int iSeconds = (iRemainedTime / 1000) % 60;
					int iMinutes = (iRemainedTime / 1000) / 60;
					char* cTimeRemaining = "";
					switch ( iMinutes )
					{
					case 0:
						break;
					case 1:
						cTimeRemaining = CStr::Printf("%i minute and ", iMinutes);
						break;
					default:
						cTimeRemaining = CStr::Printf("%i minutes and ", iMinutes);
						break;
					}

					switch ( iSeconds )
					{
					case 1:
						cTimeRemaining = CStr::Printf("%s%i second remaining.", cTimeRemaining, iSeconds);
						break;
					default:
						cTimeRemaining = CStr::Printf("%s%i seconds remaining.", cTimeRemaining, iSeconds);
						break;
					}

					Tooltip.Clear();
					Tooltip.AddString( STATE_NAME(pEntity->GetStateSTBNO()) );
					Tooltip.AddString( cTimeRemaining );
					ptDrawTooltip.x = rcIcon.left;
					ptDrawTooltip.y = rcIcon.top - Tooltip.GetHeight();
					Tooltip.SetPosition( ptDrawTooltip );
					CToolTipMgr::GetInstance().RegistInfo( Tooltip );
				}
				CEndurancePack::m_iDrawIconCnt++;
			}
		}
	}	

	if( m_pObjCHAR->IsA(OBJ_USER) )
	{
		int iMax = g_pAVATAR->GetCur_SummonMaxCapacity();
		int iValue = g_pAVATAR->GetCur_SummonUsedCapacity();

		int iWidth = 0;

		POINT ptDraw = { ENDU_ICON_X, ENDU_ICON_Y + 65 };
		RECT rcHittest = { ENDU_ICON_X , ENDU_ICON_Y + 65, ENDU_ICON_X + 100, ENDU_ICON_Y + 65 + 20 };		
		
		if( iMax > 0 && iValue > 0 )///Summon Guage
		{
			iWidth = 100 * iValue / iMax;

			int iBackImageNo   =  CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI00_GUAGE_BACKGROUND" );
			int iVioletImageNo =  CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI00_GUAGE_VIOLET" );

			pDrawObj->Draw( ptDraw.x, ptDraw.y, IMAGE_RES_UI, iBackImageNo );
			pDrawObj->Draw( ptDraw.x, ptDraw.y, iWidth, IMAGE_RES_UI, iVioletImageNo );
			RECT rcDraw = { 0, 0, 100, 20 };
			
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwBLACK,DT_CENTER, "%d / %d", iValue, iMax );
			SetRect( &rcDraw, rcDraw.left-1, rcDraw.top-1, rcDraw.right-1, rcDraw.bottom-1 );
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE,DT_CENTER, "%d / %d", iValue, iMax );

			POINT ptMouse;
			CGame::GetInstance().Get_MousePos( ptMouse );
			if( PtInRect( &rcHittest, ptMouse ) && CTDialog::GetProcessMouseOverDialog() == NULL )
				CToolTipMgr::GetInstance().RegToolTip( rcHittest.left, rcHittest.top - 20 , STR_SUMMON_CAPACITY );

			ptDraw.x += 110;
			rcHittest.left  += 110;
			rcHittest.right += 110;
		}
		
		
		if( g_pAVATAR && g_pAVATAR->GetPetMode() >= 0 && !g_pAVATAR->IsRideUser() )///Fuel Guage & Pat Guage & 보조석에 타고 있지 않을때.
		{
			int iBackImageNo  = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI00_GUAGE_BACKGROUND" );
			int iGreenImageNo = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI00_GUAGE_GREEN" );
			int iRedImageNo   = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI00_GUAGE_RED" );

			CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
			CItem* pEngineItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE);///

			assert( pEngineItem );

			if( pEngineItem )
			{
				iValue = pEngineItem->GetItem().GetLife() / 10;
				if( pEngineItem->GetItem().GetLife() % 10 )
					iValue += 1;

				iWidth = 100 * iValue / 100;


				pDrawObj->Draw( ptDraw.x, ptDraw.y, IMAGE_RES_UI, iBackImageNo );
				pDrawObj->Draw( ptDraw.x, ptDraw.y, iWidth, IMAGE_RES_UI, iGreenImageNo );
				RECT rcDraw = { 0, 0, 100, 20 };

				drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwBLACK,DT_CENTER, "%d%%", iValue );
				SetRect( &rcDraw, rcDraw.left-1, rcDraw.top-1, rcDraw.right-1, rcDraw.bottom-1 );
				drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE,DT_CENTER, "%d%%", iValue );

				POINT ptMouse;
				CGame::GetInstance().Get_MousePos( ptMouse );
				if( PtInRect( &rcHittest, ptMouse ) && CTDialog::GetProcessMouseOverDialog() == NULL )
					CToolTipMgr::GetInstance().RegToolTip( rcHittest.left, rcHittest.top - 20 , STR_FUEL );
			}

#if defined(_GBC)		
			int iMax = g_pAVATAR->GetPatGuageMax();
			if( iMax > 0 )
			{
				iValue = g_pAVATAR->GetCur_PatHP();

				iWidth = 100 * iValue / iMax;
				int iPercent = 100 * iValue / iMax;

				int iReviseY = 20;

				ptDraw.x += 110;
				rcHittest.left  += 110;
				rcHittest.right += 110;

				pDrawObj->Draw( ptDraw.x, ptDraw.y, IMAGE_RES_UI, iBackImageNo );
				pDrawObj->Draw( ptDraw.x, ptDraw.y, iWidth, IMAGE_RES_UI, iRedImageNo );
				RECT rcDraw = { 0, 0, 100, 20 };

				drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwBLACK,DT_CENTER, "%d%%", iPercent );
				SetRect( &rcDraw, rcDraw.left-1, rcDraw.top-1, rcDraw.right-1, rcDraw.bottom-1 );
				drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE,DT_CENTER, "%d%%", iPercent );

				POINT ptMouse;
				CGame::GetInstance().Get_MousePos( ptMouse );
				if( PtInRect( &rcHittest, ptMouse ) && CTDialog::GetProcessMouseOverDialog() == NULL )
					CToolTipMgr::GetInstance().RegToolTip( rcHittest.left, rcHittest.top - 20 , CStringManager::GetSingleton().GetAbility( AT_PATHP ) );
			}
#endif
		}


		///Draw EquipItem Status(수명이 5%이하인 아이템이 있을경우 그려준다. );
		ptDraw.x = ENDU_ICON_X;
		ptDraw.y = ENDU_ICON_Y + 25;
		CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
		assert( pItemSlot );
		CItem* pItem = NULL;
		
		for( int i = 1; i < MAX_EQUIP_IDX; ++i )
		{
			pItem = pItemSlot->GetItem( i );
			if( pItem  && pItem->GetItem().GetLife() < 50 )
			{
				///scale * 0.5, color = 붉은색
				pDrawObj->Draw( ptDraw.x, ptDraw.y, IMAGE_RES_ITEM, ITEM_ICON_NO( pItem->GetType(), pItem->GetItemNo() ), 0.5f, 0.5f, D3DCOLOR_ARGB( 255, 255, 180, 180 ));
				ptDraw.x += 21;
			}
		}
		/// 카트 아이템 출력 추가 
		for( int i = INVENTORY_RIDE_ITEM0 ; i < INVENTORY_RIDE_ITEM0 + MAX_RIDING_PART; ++i )
		{
			pItem = pItemSlot->GetItem( i );
			if( pItem  && pItem->GetItem().GetLife() < 50 )
			{
				///scale * 0.5, color = 붉은색
				pDrawObj->Draw( ptDraw.x, ptDraw.y, IMAGE_RES_ITEM, ITEM_ICON_NO( pItem->GetType(), pItem->GetItemNo() ), 0.5f, 0.5f, D3DCOLOR_ARGB( 255, 255, 180, 180 ));
				ptDraw.x += 21;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 시간이 다되거나, 파괴되어야할 상태객체를 파괴..
//----------------------------------------------------------------------------------------------------

void CEndurancePack::DeleteExpiredEntity()
{
	CEnduranceProperty* pEntity = NULL;
	for( int i = 0; i < ING_MAX_STATUS; i++ )
	{
		pEntity = m_EntityList[ i ];
		if( pEntity )
		{
			if( i != STATE_TYPE( pEntity->GetStateSTBNO() ) )
				assert( 0 && "STATE_TYPE mismatch!!");

			if( pEntity->IsExpired() )
			{
				/// 해당 상태에 대한 엔티티를 제거한다.
				DeleteEntityByStateType( i );
				m_EntityList[ i ] = NULL;				
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------
/// @param int iStateType 상태타입( STB 번호가 아니다. )
/// @brief 해당 상태에 대한 엔티티를 제거한다.
//----------------------------------------------------------------------------------------------------

void CEndurancePack::DeleteEntityByStateType( int iStateType )
{
	if( m_EntityList[ iStateType ] )
	{		
		CEnduranceProperty* pEntity = m_EntityList[ iStateType ];				

		if( pEntity == NULL )
			return;

		m_EntityList[ iStateType ] = NULL;

		if( m_pObjCHAR->IsA( OBJ_USER ) )
		{
			/// 쳇박스에 정보 출력
			g_itMGR.AppendChatMsg( STATE_DELETE_STRING( pEntity->GetStateSTBNO() ), IT_MGR::CHAT_TYPE_SYSTEM );
		}

		DeleteEntity( pEntity );
		
		m_dwStateFlag &= ~c_dwIngFLAG[ iStateType ];		/// 상태 플래그 리셋..

		/// 유효한 속성이라면
		//if( std::find( m_CurrentStateValue.begin(), m_CurrentStateValue.end(), iStateType ) == m_CurrentStateValue.end() )
		{	
			switch( iStateType )
			{
				case ING_INC_MOV_SPD:
				case ING_DEC_MOV_SPD:
					{
						/// 이동 속도에 관한것이라면 한번 갱신해준다.
						float fAdjRate = m_pObjCHAR->Get_MoveSPEED() / m_pObjCHAR->Get_DefaultSPEED ();
						m_CurrentStateValue[ iStateType ] = 0;
						m_pObjCHAR->m_fAdjustSPEED = m_pObjCHAR->Get_DefaultSPEED () * fAdjRate;

						// 이동 속도 모션 스피드 설정...
						//if( m_pObjCHAR->Get_STATE() | CS_MOVE )
						//	m_pObjCHAR->Set_ModelSPEED( m_pObjCHAR->Get_MoveSPEED() );
					}
					break;	
				case ING_INC_MAX_HP:
					{
						m_CurrentStateValue[ iStateType ] = 0; 	

						if( m_pObjCHAR->Get_HP() > m_pObjCHAR->Get_MaxHP() )
							m_pObjCHAR->Set_HP( m_pObjCHAR->Get_MaxHP() );
						
					}
					break;
				case ING_INC_MAX_MP:
					{
						m_CurrentStateValue[ iStateType ] = 0; 	

						if( m_pObjCHAR->Get_MP() > m_pObjCHAR->Get_MaxMP() )
							m_pObjCHAR->Set_MP( m_pObjCHAR->Get_MaxMP() );
					}
					break;
				default:
					m_CurrentStateValue[ iStateType ] = 0;
					break;
			}
		}		
	}
}


/// 상태 해제
void CEndurancePack::DeleteEnduranceState( DWORD dwNewState )
{
	DWORD dwResult = m_dwStateFlag & (~dwNewState);
	DWORD dwTemp = 0;

	int iStateType = 0;

	if( dwResult == 0 )
		return;

	/// 모든 상태 비트들 체크.
	for( int i = 0; i < sizeof( DWORD )*8; i++ )
	{
		dwTemp = 0x00000001 & ( dwResult >> i );

		if( dwTemp == 0 )
			continue;

		if( CEndurancePack::m_StateFlagTable.find( ( dwTemp << i ) ) != CEndurancePack::m_StateFlagTable.end() )
		{
			iStateType = CEndurancePack::m_StateFlagTable[ ( dwTemp << i ) ];
			DeleteEntityByStateType( iStateType );
		}/*else
		{
			AddMsgToChatWND( STR_UNKNOWN_STATE_FLAG, g_dwRED ,                                                                                                                                                                                                                                                           CChatDLG::CHAT_TYPE_SYSTEM);
		}*/
	}

	m_dwStateFlag = dwNewState;
}

/// 유리상태 해지, 불리상태 해지 등..
int CEndurancePack::ProcFlushStateSkill( int iStateTableIndex )
{
	if( iStateTableIndex && STATE_TYPE( iStateTableIndex ) <= ING_CHECK_END )
		return 0;

	ProcFlushStateSkillByIngState( STATE_TYPE( iStateTableIndex ) );
	
	return 1;
}

void CEndurancePack::ProcFlushStateSkillByIngState( int iStateType )
{
	int iSTBIDX = 0;
	switch( iStateType )
	{
		case ING_CLEAR_GOOD:
			{
				for( int i = 0; i < ING_MAX_STATUS ; i++ )
				{
					CEnduranceProperty* pEntity = m_EntityList[ i ];
					if( pEntity )
					{
						iSTBIDX = pEntity->GetStateSTBNO();
						/// 유리한 상태 모두 해지..
						if( STATE_PRIFITS_LOSSES( iSTBIDX ) == 0 )
							DeleteEntityByStateType( i );
					}
				}
			}
			break;
		case ING_CLEAR_BAD:
			{
				for( int i = 0; i < ING_MAX_STATUS ; i++ )
				{
					CEnduranceProperty* pEntity = m_EntityList[ i ];
					if( pEntity )
					{
						iSTBIDX = pEntity->GetStateSTBNO();
						/// 불리한 상태 모두 해지..
						if( STATE_PRIFITS_LOSSES( iSTBIDX ) == 1 )
							DeleteEntityByStateType( i );
					}
				}
			}
			break;
		case ING_CLEAR_ALL:
			{
				for( int i = 0; i < ING_MAX_STATUS ; i++ )
				{
					CEnduranceProperty* pEntity = m_EntityList[ i ];
					if( pEntity )
					{
						iSTBIDX = pEntity->GetStateSTBNO();
						/// 모두 해지..
						if( STATE_PRIFITS_LOSSES( iSTBIDX ) != 2 )
							DeleteEntityByStateType( i );
					}
				}
			}
			break;
	}

}

//----------------------------------------------------------------------------------------------------
/// 카트탐승시 풀려야 될 스킬
/// @brief 모든 유리스킬 + 2 번 타입
//----------------------------------------------------------------------------------------------------
void CEndurancePack::ClearStateByDriveCart()
{
	ProcFlushStateSkillByIngState( ING_CLEAR_GOOD );	

	for( int i = 0; i < ING_MAX_STATUS ; i++ )
	{		
		CEnduranceProperty* pEntity = m_EntityList[ i ];
		if( pEntity )
		{
			int iSTBIDX = pEntity->GetStateSTBNO();
			/// 유리, 불리 정보가 2번인 상태 모두 해지..
			if( STATE_PRIFITS_LOSSES( iSTBIDX ) == 2 )
			{
				if( STATE_TYPE( iSTBIDX ) != ING_INC_HP &&
					STATE_TYPE( iSTBIDX ) != ING_INC_MP )
					DeleteEntityByStateType( i );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 오른손 장비교체시 해당 장비에 걸려있던 상태 제거..
///			ING_INC_ATK_SPD		공격속도
///			ING_INC_APOWER		공격력
///			ING_INC_HIT			명중률
///			ING_INC_CRITICAL	크리티컬
//----------------------------------------------------------------------------------------------------
void CEndurancePack::ClearRWeaponSkillEffect()
{
	DeleteEntityByStateType( ING_INC_ATK_SPD );
	DeleteEntityByStateType( ING_INC_APOWER );
	DeleteEntityByStateType( ING_INC_HIT );
	DeleteEntityByStateType( ING_INC_CRITICAL );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 왼손 장비교체시 해당 장비에 걸려있던 상태 제거..
///			ING_INC_DPOWER		방어력
///			ING_INC_RES			항마력
///			ING_SHIELD_DAMAGE	방패데미지
//----------------------------------------------------------------------------------------------------
void CEndurancePack::ClearLWeaponSkillEffect()
{
	DeleteEntityByStateType( ING_INC_DPOWER );
	DeleteEntityByStateType( ING_INC_RES );
	DeleteEntityByStateType( ING_SHIELD_DAMAGE );	
}

//------------------------------------------------------------------------------------------
/// 맞았을때 자동으로 풀려야하는 것들
//------------------------------------------------------------------------------------------
void CEndurancePack::ClearStateByHitted()
{
	///DeleteEntityByStateType( ING_DISGUISE );
	///DeleteEntityByStateType( ING_TRANSPARENT );
	DeleteEntityByStateType( ING_SLEEP );	
}

//------------------------------------------------------------------------------------------
/// 공격시 자동으로 풀려야하는 스킬들
//------------------------------------------------------------------------------------------
void CEndurancePack::ClearStateByAttack()
{
	DeleteEntityByStateType( ING_DISGUISE );
	///DeleteEntityByStateType( ING_TRANSPARENT );		
}

//------------------------------------------------------------------------------------------
/// 이동시 자동으로 풀려야하는 스킬들
//------------------------------------------------------------------------------------------
void CEndurancePack::ClearStateByMove()
{
	//DeleteEntityByStateType( ING_DISGUISE );
}

/// visibility 관련 스킬이 걸렸을경우.. 업데이트 해야될시기에는( 무기를 교체한다던지 ) 비지빌리티 업데이트
void CEndurancePack::UpdateVisibility()
{
	if( m_EntityList[ ING_DISGUISE ] ||
		m_EntityList[ ING_TRANSPARENT ] )
	{
		if( g_pAVATAR->Is_ALLIED( m_pObjCHAR ) || m_pObjCHAR->IsA( OBJ_USER ) )
			::setVisibilityRecursive( m_pObjCHAR->GetZMODEL(), 0.3f ); 
		else
			::setVisibilityRecursive( m_pObjCHAR->GetZMODEL(), 0.0f ); 			
	}
}


///
/// Base class of endurance property entity
///

CEnduranceProperty::CEnduranceProperty()
{
	m_pObjCHAR				= NULL;
	m_pEffect				= NULL;

	m_iEndityIdx			= 0;
	m_iEnduranceTime		= 0;
	m_iEnduranceEndityType	= 0;
	m_bExpired				= false;
}

CEnduranceProperty::~CEnduranceProperty()
{
}

/// pure virtual function body.
bool CEnduranceProperty::CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iEntityIdx, int iStatusSTBNO, int iEnduranceTime )
{
	*(static_cast< int* >(0)) = 10; 

	return false;
}





///
/// Endurance skill class
///

CEnduranceSkill::CEnduranceSkill()
{
	m_iEnduranceEndityType = ENDURANCE_TYPE_SKILL;
	m_pEffect = NULL;
}

CEnduranceSkill::~CEnduranceSkill()
{
	DeleteEffect();
}


bool CEnduranceSkill::CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iSkillIdx, int iStatusSTBNO, int iEnduranceTime )
{
	m_pObjCHAR = pObjCHAR;

	m_iEnduranceEndityType	= ENDURANCE_TYPE_SKILL;
	m_iEndityIdx			= iSkillIdx;
	m_iEnduranceTime		= iEnduranceTime;
	m_bExpired				= false;

	m_iStartTime = g_GameDATA.GetGameTime();

	m_iStatusSTBNO = iStatusSTBNO;

	//int iEnduranceEffect = SKILL_DURATION_EFFECT( iSkillIdx );
	/// 상태당 지속효과
	int iEnduranceEffect = STATE_STEP_EFFECT( iStatusSTBNO );

	if( iEnduranceEffect )
	{
		if( CreateEffect( pObjCHAR, iEnduranceEffect ) == false )
			return false;
	}	

	///
	/// 투명등의 특수 상태 적용	
	///
	switch( STATE_TYPE( iStatusSTBNO ) )
	{
		/// 투명
		case ING_DISGUISE:
		case ING_TRANSPARENT:
			{
				if( g_pAVATAR->Is_ALLIED( m_pObjCHAR ) || m_pObjCHAR->IsA( OBJ_USER ) )
					::setVisibilityRecursive( pObjCHAR->GetZMODEL(), 0.3f ); 
				else
					::setVisibilityRecursive( pObjCHAR->GetZMODEL(), 0.0f ); 
			}
			break;

		/// 수면
		case ING_SLEEP:
			{
				/// 이미 캐스팅이 시작되었고, 결과를 받지 못ㅎㅆ다면.. 캐스팅 취소....
				if( m_pObjCHAR->m_bCastingSTART && m_pObjCHAR->bCanActionActiveSkill() )
				{
					m_pObjCHAR->PushCommandStop();					
				}
				else
				{
					if( m_pObjCHAR->m_bCastingSTART )
						m_pObjCHAR->Casting_END();
					m_pObjCHAR->SetCMD_STOP();
				}
			}
			break;

		/// 기절
		case ING_FAINTING:
			{
				/// 이미 캐스팅이 시작되었고, 결과를 받지 못ㅎㅆ다면.. 캐스팅 취소....
				if( m_pObjCHAR->m_bCastingSTART && m_pObjCHAR->bCanActionActiveSkill() )
				{
					m_pObjCHAR->PushCommandStop();					
				}
				else
				{
					if( m_pObjCHAR->m_bCastingSTART )
						m_pObjCHAR->Casting_END();
					m_pObjCHAR->SetCMD_STOP();
				}
			}
			break;
	}


	return true;
}

bool CEnduranceSkill::CreateEffect( CObjCHAR* pObjCHAR, int iEffectIDX )
{
	m_pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, false );	

	if( m_pEffect )
	{
		m_pEffect->LinkNODE ( pObjCHAR->GetZMODEL() );

		///m_pEffect->SetParentCHAR( pObjCHAR );
		///pObjCHAR->AddExternalEffect( m_pEffect );

		//m_pEffect->UnlinkVisibleWorld ();
		m_pEffect->InsertToScene ();

		return true;
	}

	return false;
}

void CEnduranceSkill::DeleteEffect()
{
	if( m_pEffect )
	{
		///g_pEffectLIST->Del_EFFECT( m_pEffect, true );
		delete m_pEffect;
		m_pEffect = NULL;
	}

	
	switch( STATE_TYPE( m_iStatusSTBNO ) )
	{

		///
		/// 투명등의 특수 상태 적용
		///

		/// 투명
		case ING_DISGUISE:
		case ING_TRANSPARENT:
			{
				::setVisibilityRecursive( m_pObjCHAR->GetZMODEL(), 1.0f ); 
			}
			break;

		/// Map Hp
		case ING_INC_MAX_HP:
			{
				if( m_pObjCHAR->Get_HP() > m_pObjCHAR->Get_MaxHP() )
					m_pObjCHAR->Set_HP( m_pObjCHAR->Get_MaxHP() );
			}
			break;

		/// Map Mp
		case ING_INC_MAX_MP:
			{
				if( m_pObjCHAR->Get_MP() > m_pObjCHAR->Get_MaxMP() )
					m_pObjCHAR->Set_MP( m_pObjCHAR->Get_MaxMP() );
			}
			break;

		///
		/// 공속, 이속등의 변경시 속도 리셋
		///
		/// 공속
		case ING_INC_ATK_SPD:
		case ING_DEC_ATK_SPD:
			/// 잠시보류..
			break;
	}


}
//
//void CEnduranceSkill::ClearEffectResource()
//{
//	if( m_pEffect )
//	{
//		///g_pEffectLIST->Del_EFFECT( m_pEffect, true );
//		delete m_pEffect;
//		m_pEffect = NULL;
//	}
//}

int CEnduranceSkill::GetIconNO()
{
	return STATE_SYMBOL( m_iStatusSTBNO );
}

/// if return value is false, this endurance entity is expired.
/// 스킬의 종료 여부는 서버로부터 통보 받음..
/// 2/20 클라이언트 자체 판단 종료조건도 서버에서 통보받음( HP, MP 등의 증가,감소 지속 )
bool CEnduranceSkill::Proc( CObjCHAR* pObjCHAR )
{
	/// 일경우 종료조건을 자체 판단한다.
	//if( iStatusSTBNO == ING_INC_HP || iStatusSTBNO == ING_INC_MP )
	//{
	//	INT64 iCurrentTime	= g_GameDATA.GetGameTime();
	//	int iElaspedTime	= ( iCurrentTime - m_iStartTime );	

	//	/// expire
	//	if( ( iCurrentTime - m_iStartTime ) >= m_iEnduranceTime )
	//	{
	//		SetExpire();
	//		return false;
	//	}

	//	/// Expire
	//	if( g_pAVATAR->Get_HP() >= g_pAVATAR->Get_MaxHP() )
	//	{
	//		m_bExpired = true;
	//		return false;
	//	}
	//	/// Expire
	//	if( g_pAVATAR->Get_MP() >= g_pAVATAR->Get_MaxMP() )
	//	{
	//		m_bExpired = true;
	//		return false;
	//	}
	//}


	/// 위장술/투명의 경우 자체판단으로 해제해야할 경우가있다.
	/// 이후에 투명의 경우 안풀리제.. 구체적인건 위키 참조..
	//switch( STATE_TYPE( iStatusSTBNO ) )
	//{
	//	/// 위장술의 경우 이동이나 공격을 하면 풀린다.
	//	case ING_DISGUISE:
	//		{
	//			if( pObjCHAR->Get_COMMAND() != CMD_STOP )
 //                   SetExpire();					
	//		}
	//		break;
	//	/// 투명의 경우 공격을 하면 풀린다.
	//	case ING_TRANSPARENT:
	//		{
	//			if( pObjCHAR->Get_COMMAND() == CMD_ATTACK )
	//				SetExpire();					
	//		}
	//		break;
	//}

	/// expire
	/*if(m_bExpired == true )
	{
		DeleteEffect();
		return false;
	}	*/

	return true;
}




///
/// Endurance item class
///

CEnduranceItem::CEnduranceItem()
{
	m_iEnduranceEndityType = ENDURANCE_TYPE_USEITEM;
}

CEnduranceItem::~CEnduranceItem()
{
}

bool CEnduranceItem::CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iUseItemIdx, int iStateNO, int iEnduranceTime )
{
	m_pObjCHAR = pObjCHAR;

	m_iEnduranceEndityType	= ENDURANCE_TYPE_USEITEM;
	m_iEndityIdx			= iUseItemIdx;
	m_iEnduranceTime		= iEnduranceTime;
	m_bExpired				= false;

	m_iStartTime = g_GameDATA.GetGameTime() + 500;
	
	memset( m_iTotalAppliedProperty, 0, sizeof( m_iTotalAppliedProperty ) );
	
	m_iStatusSTBNO = iStateNO;

	int iEnduranceEffect = 0;
	if( iEnduranceEffect )
	{
		if( CreateEffect( pObjCHAR, iEnduranceEffect ) == false )
			return false;
	}

	return true;
}

bool CEnduranceItem::CreateEffect( CObjCHAR* pObjCHAR, int iEffectIDX )
{
	m_pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, false );

	if( m_pEffect )
	{
		m_pEffect->LinkNODE ( pObjCHAR->GetZMODEL() );

		///m_pEffect->SetParentCHAR( pObjCHAR );
		///pObjCHAR->AddExternalEffect( m_pEffect );

		//m_pEffect->UnlinkVisibleWorld ();
		m_pEffect->InsertToScene ();

		return true;
	}

	return false;
}

void CEnduranceItem::DeleteEffect()
{
	if( m_pEffect )
	{
		///g_pEffectLIST->Del_EFFECT( m_pEffect, true );
		delete m_pEffect;
		m_pEffect = NULL;
	}
}

int CEnduranceItem::GetIconNO()
{
	return STATE_SYMBOL( m_iStatusSTBNO );
}

/// if return value is false, this endurance entity is expired.
bool CEnduranceItem::Proc( CObjCHAR* pObjCHAR )
{
	int	 iSTBLineNO		= 0;
	int iApplyProperty	= 0;

	
	DWORD iCurrentTime	= g_GameDATA.GetGameTime();
	int iElaspedTime	= ( iCurrentTime - m_iStartTime );

	if( iElaspedTime < 0 )
		return true;
	
	/// expire
	if( ( iCurrentTime - m_iStartTime ) >= m_iEnduranceTime )
	{
		for( int i = 0; i < STATE_APPLY_ABILITY_CNT ; i++ )
		{
			/// 서버로 부터 종료결과를 받지못했지만 클라이언트는 다채웠다. 그렇다면 그냥 진행..
			if( m_iTotalAppliedProperty[ i ] >= USEITEM_ADD_DATA_VALUE( m_iEndityIdx ) )
				continue;

			iSTBLineNO = STATE_APPLY_ING_STB( USEITME_STATUS_STB( m_iEndityIdx ), i );
			iApplyProperty = USEITEM_ADD_DATA_VALUE( m_iEndityIdx ) - m_iTotalAppliedProperty[ i ];
			
			/// 일단 상태 적용되는 아이템은 세개다..
			switch( STATE_TYPE( iSTBLineNO ) )
			{
				/// hp
			case ING_INC_HP:
				pObjCHAR->Add_HP( iApplyProperty );
				if( pObjCHAR->Get_HP() >= pObjCHAR->Get_MaxHP() )
				{
					pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );				
				}
				break;

				/// mp
			case ING_INC_MP:
				pObjCHAR->Add_MP( iApplyProperty );
				if( pObjCHAR->Get_MP() >= pObjCHAR->Get_MaxMP() )
				{
					pObjCHAR->Set_MP( pObjCHAR->Get_MaxMP() );
					//m_bExpired = true;
					//return false;
				}
				break;

				/// poison
			case ING_POISONED:
				pObjCHAR->Add_HP( iApplyProperty );
				break;
			}

			m_iTotalAppliedProperty[ i ] += iApplyProperty;
		}

		/// 클라이언트 에서 자체적으로 파괴하면 안된다.
		///m_bExpired = true;
		///return false;

		//------------------------------------------------------------------------------------
		/// 2004/10/4
		/// 클라이언트 자체 판단이 아니라.. 서버로부터 종료패킷이 왔을경우 파괴시킨다.
		/// 이부분은 서로 동기화를 맞추기위함이라.. 상당히 중요한 부분이다.
		//------------------------------------------------------------------------------------
		return true;
	}	

	for( int i = 0; i < STATE_APPLY_ABILITY_CNT ; i++ )
	{	
		iSTBLineNO = STATE_APPLY_ING_STB( USEITME_STATUS_STB( m_iEndityIdx ), i );
		int iAddValue = USEITEM_ADD_DATA_VALUE( m_iEndityIdx );
		iApplyProperty = ( ( iAddValue ) * iElaspedTime ) / ( m_iEnduranceTime );

		/// 일단 상태 적용되는 아이템은 세개다..
		switch( STATE_TYPE( iSTBLineNO ) )
		{
			/// hp
			case ING_INC_HP:
				pObjCHAR->Add_HP( iApplyProperty - m_iTotalAppliedProperty[ i ] );
				if( pObjCHAR->Get_HP() >= pObjCHAR->Get_MaxHP() )
				{
					pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );
					//m_bExpired = true;
					//return false;
				}
				break;

			/// mp
			case ING_INC_MP:
				pObjCHAR->Add_MP( iApplyProperty - m_iTotalAppliedProperty[ i ] );
				if( pObjCHAR->Get_MP() >= pObjCHAR->Get_MaxMP() )
				{
					pObjCHAR->Set_MP( pObjCHAR->Get_MaxMP() );
					//m_bExpired = true;
					//return false;
				}
				break;

			/// poison
			case ING_POISONED:
				pObjCHAR->Add_HP( iApplyProperty - m_iTotalAppliedProperty[ i ] );
				if( pObjCHAR->Get_HP() <= 0 )
				{
					pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );
					//m_bExpired = true;
					//return false;
				}
				break;
		}

		m_iTotalAppliedProperty[ i ] = iApplyProperty;
	}

	return true;
}