#include "stdafx.h"
#include ".\cskilllistitem.h"
#include "tgamectrl/tbutton.h"
#include "tgamectrl/resourcemgr.h"
#include "../../Icon/CIconSkill.h"
#include "../../IO_ImageRes.h"
#include "../../interfaceType.h"
#include "../../CToolTipMgr.h"

#include "tgamectrl/tdialog.h"
#include "../../../Object.h"
#include "../../CDragItem.h"
#include "../../command/dragcommand.h"
#include "../../../Network/CNetwork.h"
#include "../../it_mgr.h"
#include "../CSkillDlg.h"
#include "../../../GameCommon/Skill.h"
#include "../../../Country.h"
CSkillListItem::CSkillListItem(void)
{
	CResourceMgr* pResMgr = CResourceMgr::GetInstance();

	m_pButton		= new CTButton;
	m_pButton->Create( 0,0, 17, 17, 
		pResMgr->GetImageNID( IMAGE_RES_UI,"UI09_BTN_PLUS_NORMAL" ),
		pResMgr->GetImageNID( IMAGE_RES_UI,"UI09_BTN_PLUS_OVER" ),
		pResMgr->GetImageNID( IMAGE_RES_UI,"UI09_BTN_PLUS_DOWN" ),
		IMAGE_RES_UI 
	);

	m_pButton->SetDisableGrahicID( pResMgr->GetImageNID( IMAGE_RES_UI,"UI09_BTN_PLUS_DISABLE" ));
	m_pButton->SetOffset( 171, 24 );
	m_pButton->SetControlID( 10 );///임의ID Process에서 리턴을 받기 위해서

	m_Slot.SetOffset( 17, 4 );
	m_Slot.SetParent( DLG_TYPE_SKILL );


	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdDragSkill2QuickBar;
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );

#ifdef _NEW_UI 2nd SkillBar
	pCmd = new CTCmdDragSkill2QuickBar( DLG_TYPE_QUICKBAR_EXT );
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR_EXT, pCmd );
#endif

	m_Slot.SetDragAvailable();
	m_Slot.SetDragItem( m_pDragItem );

	SetHeight( 44 );
}

CSkillListItem::~CSkillListItem(void)
{
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pButton );
	m_Slot.DetachIcon();
}

CIconSkill*	CSkillListItem::GetIcon()
{
	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
		return (CIconSkill*)pIcon;

	return NULL;
}

void CSkillListItem::SetIcon( CIconSkill* pIcon )
{
	m_Slot.DetachIcon();
	m_Slot.AttachIcon( pIcon );
	assert( pIcon );
	if( pIcon )
	{
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
		int iSkillSlotIndex = pSkillIcon->GetSkillSlotFromIcon();
		if( iSkillSlotIndex / MAX_LEARNED_SKILL_PER_PAGE == CSkillDLG::TAB_PASSIVE )
			m_Slot.SetDragDisable();
		else 
			m_Slot.SetDragAvailable();

		Update();
	}
}

//--------------------------------------------------------------------------------
/// @brief 스킬에 변경되었을때 호출되어 지는 method( 최초 등록시& 레벨업 )
//--------------------------------------------------------------------------------
void CSkillListItem::Update()
{
	CIcon* pIcon = m_Slot.GetIcon();
	if( NULL != pIcon )
	{	
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;

		RECT rtDraw = { 50 , 25 , 50 + 100, 25 + 18 };

		CSkill* pSkill = pSkillIcon->GetSkill();
		
		int iSkillNo = pSkill->GetSkillIndex();

		int iUseValue = 0;
	
		std::string strUseAbility;
					
		for( int i = 0; i < SKILL_USE_PROPERTY_CNT; ++i )
		{
			if( SKILL_USE_PROPERTY(iSkillNo, i) )
			{
				iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, i );
				strUseAbility.append( CStr::Printf( "%s:%d", CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY(iSkillNo,i) ), iUseValue ) );
				strUseAbility.append("   ");
			}
		}
		
		if( !strUseAbility.empty() )
			m_strUseAbility.set_string( strUseAbility.c_str(), rtDraw, FONT_NORMAL );
	}
}

void CSkillListItem::Draw()
{
	if( !IsVision() ) return;

	/// 게이지 진행상태를 보여주기 위해서..
	::endSprite();
	::beginSprite( D3DXSPRITE_ALPHABLEND ); 

	m_Slot.Draw();	
	
	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
	{
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
		CSkill* pSkill = pSkillIcon->GetSkill();
		
		assert( pSkill );
		
		if( pSkill )
		{
			int iSkillNo = pSkill->GetSkillIndex();
			int iLevel = pSkill->GetSkillLevel();

			CSinglelineString sStrBuf;
			RECT	rtDraw = { 50, 5, 50+110, 5+18 };

			if( pIcon->GetName() )
			{
				if( iLevel <= 0 )
				{
					sStrBuf.set_string( pIcon->GetName(), rtDraw, FONT_NORMAL );
					sStrBuf.draw();					
				}
				else
				{					
					sStrBuf.set_string( CStr::Printf("%s(Lv:%d)", pIcon->GetName(), iLevel ), rtDraw, FONT_NORMAL );
					sStrBuf.draw();					
				}
			}

			m_strUseAbility.draw();
			//m_strUseAbility.draw();
			//char*	pszBuf		= NULL;
			//int		iUseValue	= 0; 

			//SetRect( &rtDraw, 50 , 25 , 50 + 100, 25 + 18 );
			//std::string strUseAbility;
			//			
			//for( int i = 0; i < SKILL_USE_PROPERTY_CNT; ++i )
			//{
			//	if( SKILL_USE_PROPERTY(iSkillNo, i) )
			//	{
			//		iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, i );
			//		strUseAbility.append( CStr::Printf( "%s:%d", CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY(iSkillNo,i) ), iUseValue ) );
			//		strUseAbility.append("   ");
			//		//if( g_pAVATAR->GetCur_AbilityValue(SKILL_USE_PROPERTY( iSkillNo, i)) >= iUseValue )
			//		//{
			//		//	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, ptDraw.x, ptDraw.y, g_dwGREEN ,pszBuf ); 
			//		//	sStrBuf.set_string( pszBuf, rtDraw, FONT_NORMAL );
			//		//}
			//		//else
			//		//{
			//		//	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, ptDraw.x, ptDraw.y, g_dwRED ,pszBuf ); 						
			//		//}
			//		//ptDraw.x += 50;					
			//	}
			//}
			//
			//if( !strUseAbility.empty() )
			//{
			//	sStrBuf.set_string( strUseAbility.c_str(), rtDraw, FONT_NORMAL );
			//	sStrBuf.draw();
			//}
		}
	}
	

	if( m_pButton )
		m_pButton->Draw();
}

void CSkillListItem::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );

	if( m_pButton )
		m_pButton->MoveWindow( m_sPosition );
}

void CSkillListItem::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	m_Slot.Update( ptMouse );

	if( m_pButton )
	{
		m_pButton->Update( ptMouse );
		CIcon* pIcon = m_Slot.GetIcon();
		if( pIcon )
		{
			CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
			if( pDlg && pDlg->GetDialogType() != DLG_TYPE_SKILL)
				return;

		
			if( m_strUseAbility.is_ellipsis() )
			{

				POINT ptPoint = { ptMouse.x - m_sPosition.x , ptMouse.y - m_sPosition.y };

				const RECT& rect_useability = m_strUseAbility.get_rect();

				if( PtInRect( &rect_useability, ptPoint ) )
				{
					CToolTipMgr::GetInstance().RegToolTip( m_sPosition.x , m_sPosition.y + 20, m_strUseAbility.get_string() );
					return;
				}
			}
			
			
			CWinCtrl* pCtrl = CWinCtrl::GetProcessMouseOverCtrl();
			if( pCtrl && pCtrl == m_pButton )
			{
				CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
				
				if( IsHasNextLevel( pSkillIcon->GetSkillSlotFromIcon() ) )
				{
					int iNeedSkillPoint = g_pAVATAR->Get_NeedPoint2SkillUP( pSkillIcon->GetSkillSlotFromIcon() );
					int iRequireJuly    = SKILL_LEVELUP_NEED_ZULY( pSkillIcon->GetSkillIndex() + 1) * 100;
					int iRequireLevel   = SKILL_NEED_ABILITY_VALUE( pSkillIcon->GetSkillIndex() + 1, 0 );

					CInfo Info;
					Info.Clear();
					
					DWORD text_color = g_dwWHITE;

					///필요 SP
					if( iNeedSkillPoint > g_pAVATAR->GetCur_SkillPOINT() )
						text_color = g_dwRED;
					else
						text_color = g_dwWHITE;
					Info.AddString(CStr::Printf( "%s: %d", STR_REQUIRE_SP, iNeedSkillPoint) , text_color);

					if( CCountry::GetSingleton().IsApplyNewVersion() )
					{
						//필요 줄리
						if( iRequireJuly > g_pAVATAR->Get_MONEY() )
							text_color = g_dwRED;
						else
							text_color = g_dwWHITE;

						Info.AddString(CStr::Printf( "%s: %d", STR_REQUIRE_JULY, iRequireJuly ), text_color );
						
						//필요 레벨
						if( iRequireLevel > g_pAVATAR->Get_LEVEL() )
							text_color = g_dwRED;
						else
							text_color = g_dwWHITE;

						Info.AddString(CStr::Printf( "%s: %d", STR_REQUIRE_LEVEL, iRequireLevel ), text_color );
					}

					POINT ptDraw = ptMouse;
					ptDraw.y -= Info.GetHeight();
					Info.SetPosition( ptDraw );
					CToolTipMgr::GetInstance().RegistInfo( Info );

				}
			}
		}
	}
}

void CSkillListItem::Show()
{
	CWinCtrl::Show();
	m_Slot.Show();
	if( m_pButton )
		m_pButton->Show();
}

void CSkillListItem::Hide()
{
	CWinCtrl::Hide();
	m_Slot.Hide();
	if( m_pButton )
		m_pButton->Hide();
}

unsigned CSkillListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( m_Slot.Process( uiMsg, wParam, lParam ) )
		return GetControlID();

	if( m_pButton && m_pButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )///서버에 Level Up을 요청한다.그전에 레벨업이 가능한지 한번더 체크할것
		{
			CIcon* pIcon = m_Slot.GetIcon();
			if( pIcon )
			{
				CIconSkill* pSkillIcon = (CIconSkill*)pIcon;			
				if( IsValidLevelUp( pSkillIcon->GetSkillSlotFromIcon() )  )
					g_pNet->Send_cli_SKILL_LEVELUP_REQ( (BYTE)pSkillIcon->GetSkillSlotFromIcon() );
				else
					g_itMGR.AppendChatMsg( m_error_msg.c_str(), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		return GetControlID();
	}

	return 0;
}

bool CSkillListItem::IsHasNextLevel( int iSkillSlotIdx )
{
	if( iSkillSlotIdx  < 0 ) 
		return false;

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///스킬레벨이 10이상이거나 필요 포인트보다 현재 포인트가 적다면 false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		 SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) 
	{
		//g_itMGR.AppendChatMsg( STR_CANT_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM  );		
		m_error_msg = STR_CANT_LEVELUP;
		return false;
	}
	return true;
}

bool CSkillListItem::IsValidLevelUp( int iSkillSlotIdx )
{
	if( iSkillSlotIdx  < 0 ) 
		return false;

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///스킬레벨이 10이상이거나 필요 포인트보다 현재 포인트가 적다면 false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		 SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) 
	{
		//g_itMGR.AppendChatMsg( STR_CANT_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM  );		
		m_error_msg = STR_CANT_LEVELUP;
		return false;
	}

/*	if( g_pAVATAR->Get_NeedPoint2SkillUP( iSkillSlotIdx ) <= 0 )
	{
		g_itMGR.AppendChatMsg( STR_CANT_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM );		
		return false;
	}
*/
	if( g_pAVATAR->Get_NeedPoint2SkillUP( iSkillSlotIdx ) > g_pAVATAR->GetCur_SkillPOINT() )
	{
		//g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT,IT_MGR::CHAT_TYPE_SYSTEM );	
		m_error_msg = STR_NOT_ENOUGH_SKILLPOINT;
		return false;
	}


	int iNeedAbilityType = 0;
	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i)
	{
		iNeedAbilityType  = SKILL_NEED_ABILITY_TYPE( nSkillIDX,i );
		if( iNeedAbilityType )
		{
			if( g_pAVATAR->Get_AbilityValue( iNeedAbilityType ) < SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ))
			{
				//g_itMGR.AppendChatMsg( CStr::Printf(STR_NEED_ABILITY, 
				//	CStringManager::GetSingleton().GetAbility( iNeedAbilityType ),
				//	SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ) ),
				//	IT_MGR::CHAT_TYPE_SYSTEM
				//);		
				m_error_msg = CStr::Printf(STR_NEED_ABILITY, 
					CStringManager::GetSingleton().GetAbility( iNeedAbilityType ),
					SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ) );
				return false;
			}
		}
	}
	return true;
}

