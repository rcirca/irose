#include "stdafx.h"
#include "CSkillDLG.h"
#include "../it_Mgr.h"
#include "../CToolTipMgr.h"
#include "../Object.h"
#include "../IO_ImageRes.h"
#include "../icon/ciconskill.h"
#include "../../Network/CNetwork.h"
#include "../../GameCommon/Skill.h"
#include "../../gamecommon/item.h"
#include "subclass/CSkillListItem.h"


#include "JContainer.h"
#include "TabbedPane.h"
#include "TContainer.h"
#include "JListBox.h"
#include "TButton.h"

#define SKILL_SLOT_SX		24
#define SKILL_SLOT_SY		91

#define SKILL_SLOT_WIDTH	33
#define SKILL_SLOT_HEIGHT	33

#define SKILL_NEED_POINT_SX 85
#define SKILL_NEED_POINT_SY 251

#define SKILL_POINT_SX		151
#define SKILL_POINT_SY		301

#define SKILL_POINTTEXT_WIDTH	54
#define SKILL_POINTTEXT_HEIGHT	19

#include "ZListBox.h"
CSkillDLG::CSkillDLG( int iType )
{
	m_iCurrentSkillTab	= TAB_BASIC;
}

CSkillDLG::~CSkillDLG()
{
	
}

 
unsigned int CSkillDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}


void CSkillDLG::Draw()
{
	if( !IsVision() ) return ;
	CTDialog::Draw();

	DrawTexts();
}

bool CSkillDLG::On_LButtonUP( unsigned iProcID , WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_OPEN_SKILLTREE:
		g_itMGR.OpenDialog( DLG_TYPE_SKILLTREE );
		break;
	default:
		break;
	}
	
	return true;
}


void CSkillDLG::DrawTexts()
{
	D3DXMATRIX mat;	
	
	D3DXMatrixTranslation( &mat, (float)( m_sPosition.x + 41 ), (float)(m_sPosition.y + m_iHeight - 22 ),0.0f);
	::setTransformSprite( mat );

	

	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 0,0, g_dwWHITE, "%d" , g_pAVATAR->GetCur_SkillPOINT());		

}

bool CSkillDLG::IsValidLevelUp( int iSkillSlotIdx )
{
	if( iSkillSlotIdx  < 0 ) 
		return false;

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///스킬레벨이 10이상이거나 필요 포인트보다 현재 포인트가 적다면 false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		 SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) {
		return false;
	}

	if( g_pAVATAR->Get_NeedPoint2SkillUP( (short)iSkillSlotIdx ) <= 0 )
		return false;

	if( g_pAVATAR->Get_NeedPoint2SkillUP( (short)iSkillSlotIdx ) > g_pAVATAR->GetCur_SkillPOINT() )
		return false;


	int iNeedAbilityType = 0;
	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i)
	{
		iNeedAbilityType  = SKILL_NEED_ABILITY_TYPE( nSkillIDX,i );
		if( iNeedAbilityType )
		{
			if( g_pAVATAR->Get_AbilityValue( iNeedAbilityType ) < SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ))
				return false;
		}
	}

	return true;
}

bool CSkillDLG::IsVision()
{
	if( g_pAVATAR == NULL )
		return CTDialog::IsVision();
	else
		return( CTDialog::IsVision() && g_pAVATAR->Get_HP() > 0 );

	return false;
}
//-----------------------------------------------------------------------------------------------------
/// @brief 스킬에 대한 소모 능력치및 기타 변경사항에 대한 인터페이스 적용을 위한 Method
//-----------------------------------------------------------------------------------------------------
void CSkillDLG::UpdateSkillLists()
{
	CZListBox*		pListBox	= NULL;
	CJContainer *pContainer		= NULL;
	CTabbedPane* pTabbedPane	= NULL;
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	assert( pCtrl );
	if( pCtrl == NULL )
		return;

	pTabbedPane = (CTabbedPane*)pCtrl;


	pContainer = pTabbedPane->GetTabContainer( IID_TAB_BASIC );
	pCtrl = pContainer->Find( IID_ZLISTBOX_BASIC );
	assert( pCtrl );

	if( pCtrl )
	{
		pListBox =(CZListBox*)pCtrl;
		UpdateSkillListBySkillLevelUp( pListBox );
	}

	pContainer = pTabbedPane->GetTabContainer( IID_TAB_ACTIVE );
	pCtrl = pContainer->Find( IID_ZLISTBOX_ACTIVE );
	assert( pCtrl );
	if( pCtrl )
	{
		pListBox =(CZListBox*)pCtrl;
		UpdateSkillListBySkillLevelUp( pListBox );
	}


	pContainer = pTabbedPane->GetTabContainer( IID_TAB_PASSIVE );
	pCtrl = pContainer->Find( IID_ZLISTBOX_PASSIVE );
	assert( pCtrl );
	if( pCtrl )
	{
		pListBox =(CZListBox*)pCtrl;
		UpdateSkillListBySkillLevelUp( pListBox );
	}

}
void CSkillDLG::UpdateSkillListBySkillLevelUp( CZListBox* pListBox )
{

	assert( pListBox );
	if( NULL == pListBox )
		return;

	CSkillListItem* pSkillListItem = NULL;
	int max_size = pListBox->GetSize();

	for( int i = 0 ; i < max_size; ++i )
	{
		pSkillListItem = (CSkillListItem*)pListBox->GetItem( i );
		pSkillListItem->Update();
	}
}

void CSkillDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObj == NULL || strcmp( pObj->toString(), "CTEventSkill" ) )
	{
		assert( 0 && "Invalid Event Type or Null" );
		return;
	}

	CTEventSkill* pEvent = (CTEventSkill*)pObj;

	int iSkillSlotIndex = pEvent->GetIndex();
	int iPage = iSkillSlotIndex / MAX_LEARNED_SKILL_PER_PAGE;


	switch( pEvent->GetID() )
	{
	case CTEventSkill::EID_LEVELUP :
		{
			UpdateSkillLists();
		}
		break;
	case CTEventSkill::EID_ADD_SKILL :
		{
			CZListBox*		pListBox	= NULL;
			CJContainer *pContainer		= NULL;
			CTabbedPane* pTabbedPane	= NULL;
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl );
			if( pCtrl == NULL )
				break;

			pTabbedPane = (CTabbedPane*)pCtrl;

			switch( iPage )
			{
			case TAB_BASIC:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_BASIC );
				pCtrl = pContainer->Find( IID_ZLISTBOX_BASIC );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;
				break;
			case TAB_ACTIVE:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_ACTIVE );
				pCtrl = pContainer->Find( IID_ZLISTBOX_ACTIVE );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;
				break;
			case TAB_PASSIVE:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_PASSIVE );
				pCtrl = pContainer->Find( IID_ZLISTBOX_PASSIVE );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;
				break;
			default:
				break;
			}

			if( pListBox )
			{
				CIconSkill* pIcon		= new CIconSkill( iSkillSlotIndex );
				CSkillListItem* pItem	= new CSkillListItem;
				pItem->Show();
				pItem->SetIcon(pIcon );
				pListBox->Add( pItem );
			}
			break;
		}
	case CTEventSkill::EID_DEL_SKILL :
		{
			CJContainer*	pContainer	= NULL;
			CTabbedPane*	pTabbedPane	= NULL;
			CZListBox*		pListBox	= NULL;
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl );
			if( pCtrl == NULL )
				break;

			pTabbedPane = (CTabbedPane*)pCtrl;

			switch( iPage )
			{
			case TAB_BASIC:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_BASIC );
				pCtrl = pContainer->Find( IID_ZLISTBOX_BASIC );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;
				break;
			case TAB_ACTIVE:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_ACTIVE );
				pCtrl = pContainer->Find( IID_ZLISTBOX_ACTIVE );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;
				break;
			case TAB_PASSIVE:
				pContainer = pTabbedPane->GetTabContainer( IID_TAB_PASSIVE );
				pCtrl = pContainer->Find( IID_ZLISTBOX_PASSIVE );
				assert( pCtrl );
				if( pCtrl )
					pListBox =(CZListBox*)pCtrl;

				break;
			default:
				break;
			}

			if( pListBox )
				RemoveSkillListItem( pListBox, iSkillSlotIndex );

			break;
		}
	default:
//		assert( 0 && "Invalid Event Skill Type");
		break;
	}
}

void CSkillDLG::RemoveSkillListItem( CZListBox* pListBox, int iSkillSlotIndex )
{
	CSkillListItem* pItem;
	CWinCtrl* pCtrl = NULL;
	CIconSkill* pSkillIcon = NULL;
	for( int i = 0 ; i < pListBox->GetSize(); ++i )
	{
		pCtrl = pListBox->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CSkillListItem*)pCtrl;
			pSkillIcon = pItem->GetIcon();
			assert( pSkillIcon );
			if( pSkillIcon )
			{
				if( pSkillIcon->GetSkillSlotFromIcon() == iSkillSlotIndex )
				{
					pListBox->DelItem( i );
					break;
				}
			}
		}
	}
}

bool CSkillDLG::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		if( CWinCtrl* p = Find( IID_BTN_OPEN_SKILLTREE ) )
		{
			if( p->GetControlType() == CTRL_BUTTON )
			{
				CTButton* pBtn = (CTButton*)p;
				pBtn->SetText( STR_SKILLTREE );
			}
		}
		return true;
	}
	return false;
}

void CSkillDLG::Show( )
{
	CTDialog::Show();
	if( g_pAVATAR )
	{
		if( g_pAVATAR->Get_JOB() )
			ShowChild( IID_BTN_OPEN_SKILLTREE );
		else
			HideChild( IID_BTN_OPEN_SKILLTREE );
	}

	UpdateSkillLists();

}