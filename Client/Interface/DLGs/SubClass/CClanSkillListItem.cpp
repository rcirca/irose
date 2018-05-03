#include "stdafx.h"
#include ".\cclanskilllistitem.h"
#include "../../interfaceType.h"
#include "../../IO_ImageRes.h"
#include "../../interfaceType.h"
#include "../../CToolTipMgr.h"

#include "tgamectrl/tdialog.h"
#include "../../../Object.h"
#include "../../CDragItem.h"
#include "../../command/dragcommand.h"
#include "../../../Network/CNetwork.h"
#include "../../it_mgr.h"
#include "../../icon/CIconSkillClan.h"
#include "../CSkillDlg.h"
#include "../../../GameCommon/Skill.h"
#include "../../../GameData/CClan.h"

CClanSkillListItem::CClanSkillListItem(void)
{
	m_Slot.SetOffset( 1, 10 );
	m_Slot.SetParent( DLG_TYPE_CLAN );


	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdDragClanSkill2QuickBar;
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );

	m_Slot.SetDragAvailable();
	m_Slot.SetDragItem( m_pDragItem );

	SetHeight( 44 );

}

CClanSkillListItem::~CClanSkillListItem(void)
{
	SAFE_DELETE( m_pDragItem );
	m_Slot.DetachIcon();
}

void CClanSkillListItem::Draw()
{
	if( !IsVision() ) return;

	/// 게이지 진행상태를 보여주기 위해서..
	::endSprite();
	::beginSprite( D3DXSPRITE_ALPHABLEND ); 

	m_Slot.Draw();	
	
	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
	{
		CIconSkillClan* pSkillIcon = (CIconSkillClan*)pIcon;
		CSkill* pSkill = pSkillIcon->GetSkill();
		
		assert( pSkill );
		
		if( pSkill )
		{
			int iSkillNo = pSkill->GetSkillIndex();
			int iLevel = pSkill->GetSkillLevel();

			if( pIcon->GetName() )
			{
				if( iLevel <= 0 )
					drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 50,3, g_dwBLACK, pIcon->GetName() );
				else
					drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 50,3, g_dwBLACK, CStr::Printf("%s(Lv:%d)", pIcon->GetName(), iLevel ) );
			}

			//char*	pszBuf		= NULL;
			//int		iUseValue	= 0; 
			//POINT	ptDraw = { 50 , 25 };
			//for( int i = 0; i < SKILL_USE_PROPERTY_CNT; ++i )
			//{
			//	if( SKILL_USE_PROPERTY(iSkillNo, i) )
			//	{
			//		iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, i );
			//		pszBuf = CStr::Printf( "%s %d", CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY(iSkillNo,i) ), iUseValue );

			//		if( g_pAVATAR->GetCur_AbilityValue(SKILL_USE_PROPERTY( iSkillNo, i)) >= iUseValue )
			//			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, ptDraw.x, ptDraw.y, g_dwGREEN ,pszBuf ); 
			//		else
			//			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, ptDraw.x, ptDraw.y, g_dwRED ,pszBuf ); 

			//		ptDraw.x += 50;
			//	}
			//}
		}
	}
}

void CClanSkillListItem::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );
}

void CClanSkillListItem::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	m_Slot.Update( ptMouse );
}

unsigned CClanSkillListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( m_Slot.Process( uiMsg, wParam, lParam ) )
		return GetControlID();

	return 0;
}

void CClanSkillListItem::Show()
{
	CWinCtrl::Show();
	m_Slot.Show();
}

void CClanSkillListItem::Hide()
{
	CWinCtrl::Hide();
	m_Slot.Hide();
}

void CClanSkillListItem::SetIcon( CIconSkillClan* pIcon )
{
	m_Slot.DetachIcon();
	m_Slot.AttachIcon( pIcon );
	assert( pIcon );
	if( pIcon )
	{
		//assert( g_pAVATAR );
		CClanSkillSlot* pSlots = CClan::GetInstance().GetClanSkillSlot();
		assert( pSlots );
		int iSkillIndex = pSlots->GetSkillIndex( pIcon->GetIndex() );
		if( SKILL_TYPE( iSkillIndex ) == SKILL_TYPE_PASSIVE )
			m_Slot.SetDragDisable();
		else 
			m_Slot.SetDragAvailable();
	}
}

CIconSkillClan*	CClanSkillListItem::GetIcon()
{
	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
		return (CIconSkillClan*)pIcon;

	return NULL;
}
