#include "stdafx.h"
#include ".\partyoptiondlg.h"
#include "../../GameData/CParty.h"
#include "../../GameData/Event/CTEventParty.h"
#include "../interface/it_mgr.h"
#include "../interface/TypeResource.h"
#include "../../GameCommon/StringManager.h"
#include "../../CClientStorage.h"



#include "tgamectrl/tbutton.h"
#include "tgamectrl/tradiobox.h"
#include "tgamectrl/tcheckbox.h"
CPartyOptionDlg::CPartyOptionDlg(void)
{
	m_party_rule = 0;
}

CPartyOptionDlg::~CPartyOptionDlg(void)
{
}

unsigned CPartyOptionDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CPartyOptionDlg::OnLButtonUp( unsigned uiProcID )
{
	switch( uiProcID )
	{
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_CONFIRM:
		{
			CWinCtrl* pCtrl = Find( IID_CHECKBOX_SHOW_PARTYMEMBER_HPGUAGE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
			{
				CTCheckBox* p =(CTCheckBox*)pCtrl;
				if( p->IsCheck() )
					g_ClientStorage.SetShowPartyMemberHpGuage( true );
				else
					g_ClientStorage.SetShowPartyMemberHpGuage( false );
			}

			if( !CParty::GetInstance().IsPartyLeader() )///파티장이 아닐경우에는 다른 옵션 변경은 무시( 정상적으로는 변하지도 않는다. );
			{
				Hide();			
				break;
			}

			BYTE btPartyRule = 0;
			pCtrl = Find( IID_RADIOBOX_ITEM );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			UINT uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBUTTON_ITEM_PICK:
				break;
			case IID_RADIOBUTTON_ITEM_SEQUENCE:
				btPartyRule |= BIT_PARTY_RULE_ITEM_TO_ORDER;
				break;
			default:
				assert( 0 && "Invalid Selected RadioButton" );
				break;
			}

			///유저가 입력한 값을 이전 값으로 돌리고 갱신이 될경우에는 서버에서 받아서 바꾸도록한다.
			if( m_party_rule & BIT_PARTY_RULE_ITEM_TO_ORDER )
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_SEQUENCE );
			else
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_PICK );


			
			pCtrl = Find( IID_RADIOBOX_EXP );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			pRadioBox = (CTRadioBox*)pCtrl;
			uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBOX_EXP_EQUALITY:
				break;
			case IID_RADIOBOX_EXP_RATIO_LEVEL:
				btPartyRule |= BIT_PARTY_RULE_EXP_PER_PLAYER;
				break;
			default:
				assert( 0 && "Invalid Selected RadioButton" );
				break;
			}

			///유저가 입력한 값을 이전 값으로 돌리고 갱신이 될경우에는 서버에서 받아서 바꾸도록한다.
			if( m_party_rule & BIT_PARTY_RULE_EXP_PER_PLAYER )
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_RATIO_LEVEL );
			else
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_EQUALITY );


			CParty::GetInstance().SendChangePartyRule( btPartyRule );
			Hide();
		}
		break;
	default:
		break;
	}
}
void CPartyOptionDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL )
		return;

	if( strcmp( pObj->toString(), "Party" ) )
	{
		assert( 0 && "Event Type is Invalid @PartyDlg" );	
		return;
	}

	CTEventParty* pEvent = (CTEventParty*)pObj;
	CParty& Party = CParty::GetInstance();
	int iObjSvrIdx = pEvent->GetObjSvrIdx();
	switch( pEvent->GetID() )
	{
	case CTEventParty::EID_CHANGE_RULE:
		{
			BYTE btRule		= pEvent->GetRule();
			m_party_rule	= btRule;
			g_itMGR.AppendChatMsg( STR_CHANGED_PARTY_CONFIG, IT_MGR::CHAT_TYPE_SYSTEM );

			CWinCtrl* pCtrl = Find( IID_RADIOBOX_ITEM );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			
			if( btRule & BIT_PARTY_RULE_ITEM_TO_ORDER )
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", STR_ITEM_PICKUP_RIGHT, STR_PARTY_ITEM_PICKUP_SEQENCE) , IT_MGR::CHAT_TYPE_SYSTEM  );
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_SEQUENCE );
			}
			else
			{
				g_itMGR.AppendChatMsg(CStr::Printf( "%s: %s",STR_ITEM_PICKUP_RIGHT, STR_PARTY_ITEM_PICKUP_RIGHT_ALL), IT_MGR::CHAT_TYPE_SYSTEM );
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_PICK );
			}

			pCtrl = Find( IID_RADIOBOX_EXP );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;
			pRadioBox = (CTRadioBox*)pCtrl;

			if( btRule & BIT_PARTY_RULE_EXP_PER_PLAYER )
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", CStringManager::GetSingleton().GetAbility( AT_EXP ), STR_PARTY_EXP_LEVELRATE), IT_MGR::CHAT_TYPE_SYSTEM );
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_RATIO_LEVEL );
			}
			else
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", 
						CStringManager::GetSingleton().GetAbility( AT_EXP ),
						STR_PARTY_EXP_EQUALITY), 
						IT_MGR::CHAT_TYPE_SYSTEM 
				);
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_EQUALITY );
			}
		}
		break;
	case CTEventParty::EID_CHANGE_LEADER:
		{
			if( Party.IsPartyLeader() )
			{
				SetEnableChild( IID_RADIOBUTTON_ITEM_PICK, true );
				SetEnableChild( IID_RADIOBUTTON_ITEM_SEQUENCE, true );
				SetEnableChild( IID_RADIOBOX_EXP_EQUALITY, true );
				SetEnableChild( IID_RADIOBOX_EXP_RATIO_LEVEL, true );
			}
			else
			{
				SetEnableChild( IID_RADIOBUTTON_ITEM_PICK, false );
				SetEnableChild( IID_RADIOBUTTON_ITEM_SEQUENCE, false );
				SetEnableChild( IID_RADIOBOX_EXP_EQUALITY, false );
				SetEnableChild( IID_RADIOBOX_EXP_RATIO_LEVEL, false );
			}
		}
		break;
	default:
		break;
	}

}

void CPartyOptionDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 20,  38, g_dwBLACK, STR_PARTYEXP_DISTRIBUTE_FORM );

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 38,  55, g_dwBLACK, STR_PARTY_EXP_EQUALITY);

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 38,  77, g_dwBLACK, STR_PARTY_EXP_LEVELRATE);

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 20, 104, g_dwBLACK, STR_ITEM_PICKUP_RIGHT);

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 38, 122, g_dwBLACK, STR_PARTY_ITEM_PICKUP_RIGHT_ALL);

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 38, 144, g_dwBLACK, STR_PARTY_ITEM_PICKUP_SEQENCE);

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 38, 182, g_dwBLACK, STR_SHOW_PARTYMEMBER_HPGUAGE);
}

void CPartyOptionDlg::Show()
{
	CTDialog::Show();
	if( CWinCtrl* pCtrl = Find( IID_CHECKBOX_SHOW_PARTYMEMBER_HPGUAGE ) )
	{
		if( pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* p =(CTCheckBox*)pCtrl;
			if( g_ClientStorage.IsShowPartyMemberHpGuage() )
				p->SetCheck();
			else
				p->SetUncheck();
		}
	}
}
	
bool CPartyOptionDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		CWinCtrl* pCtrl = Find( IID_BTN_CONFIRM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
		{
			CTButton* p = (CTButton*)pCtrl;
			p->SetText( STR_OK );
		}
		return true;
	}
	return false;
}