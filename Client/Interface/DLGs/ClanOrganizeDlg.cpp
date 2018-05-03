#include "stdafx.h"
#include ".\clanorganizedlg.h"
#include "tgamectrl/jtable.h"
#include "subclass/ClanMark.h"
#include "../IO_ImageRes.h"
#include "../CTDrawImpl.h"
#include "../IT_MGR.h"
#include "../../Network/CNetwork.h"
#include "../../GameCommon/CFilterWord.h"

#include "tgamectrl/teditbox.h"
CClanOrganizeDlg::CClanOrganizeDlg(void)
{
	m_iSelectedClanBack		= 0;
	m_iSelectedClanCenter	= 0;
}

CClanOrganizeDlg::~CClanOrganizeDlg(void)
{

}

unsigned CClanOrganizeDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam,lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				switch( uiProcID )
				{
				case IID_BTN_CONFIRM:
					OrganizeClan();
					break;
				case IID_BTN_CLOSE:
				case IID_BTN_CANCEL:
					Hide();
					break;
				default:
					break;
				}
			}
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CClanOrganizeDlg::Create( const char* IDD )
{
	bool bRet = false;
	if( bRet = CTDialog::Create( IDD ) )
	{
		CWinCtrl* pCtrl = Find( IID_TABLE_CLANCENTER );
		assert( pCtrl ); if( pCtrl == NULL ) return true;


		CJTable* pTable = (CJTable*)pCtrl;

		
		CClanMark* pMark = NULL;
		CImageRes* pRes = CImageResManager::GetSingleton().GetImageRes( IMAGE_RES_CLANCENTER );
		if( pRes )
		{
			for( int i = 1; i < pRes->GetSpriteCount(); ++i )
			{
				pMark = new CClanMark;
				pMark->SetModuleID( IMAGE_RES_CLANCENTER );
				pMark->SetGraphicID( i );
				pMark->SetControlID( i );
				pTable->Add( pMark );
			}

			if( pRes->GetSpriteCount() > 0 )
				m_iSelectedClanCenter	= rand() % (pRes->GetSpriteCount() - 1) + 1;
		}

		pCtrl = Find( IID_TABLE_CLANBACK );
		assert( pCtrl ); if( pCtrl == NULL ) return true;


		pTable = (CJTable*)pCtrl;

		pMark = NULL;
		pRes = CImageResManager::GetSingleton().GetImageRes( IMAGE_RES_CLANBACK );
		if( pRes )
		{
			for( int i = 1; i < pRes->GetSpriteCount(); ++i )
			{
				pMark = new CClanMark;
				pMark->SetModuleID( IMAGE_RES_CLANBACK );
				pMark->SetControlID( i );
				pMark->SetGraphicID( i );
				pTable->Add( pMark );
			}

			if( pRes->GetSpriteCount() > 0 )
				m_iSelectedClanBack	= rand() % ( pRes->GetSpriteCount() - 1 ) + 1;
		}


	}
	return bRet;
}

void CClanOrganizeDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	g_DrawImpl.Draw( (int)m_sPosition.x + 186, (int)m_sPosition.y + 162, IMAGE_RES_CLANBACK,   m_iSelectedClanBack );
	g_DrawImpl.Draw( (int)m_sPosition.x + 186, (int)m_sPosition.y + 162, IMAGE_RES_CLANCENTER, m_iSelectedClanCenter );
}

void CClanOrganizeDlg::OnLButtonDown( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case IID_TABLE_CLANCENTER:
		{
			CWinCtrl* pCtrl = Find( IID_TABLE_CLANCENTER );
			assert( pCtrl ); if( pCtrl == NULL ) return;

			CJTable* pTable = (CJTable*)pCtrl;
			m_iSelectedClanCenter = pTable->GetSelectedItemID();
			break;
		}
	case IID_TABLE_CLANBACK:
		{
			CWinCtrl* pCtrl = Find( IID_TABLE_CLANBACK );
			assert( pCtrl ); if( pCtrl == NULL ) return;

			CJTable* pTable = (CJTable*)pCtrl;
			m_iSelectedClanBack = pTable->GetSelectedItemID();
		}
		break;
	default:
		break;
	}
}

void CClanOrganizeDlg::OrganizeClan()
{
	//조건 체크
	//1. 내가 다른 클랜에 가입하지 않았다.
	//2. 돈이나..레벨등의 조건에 만족한다.
	
	if( g_pAVATAR->Get_LEVEL() >= 30 && g_pAVATAR->Get_MONEY() >= 1000000 )
	{
		CWinCtrl* pCtrl = Find( IID_EDIT_TITLE );
		assert( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX );
		if( pCtrl == NULL ) return;
		if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;

		CTEditBox* pEditBox = (CTEditBox*)pCtrl;
		char* pszTitle = pEditBox->get_text();
		if( pszTitle == NULL ) return;
		if( !pszTitle[0] ) return;

		if( !CFilterWord::GetInstance().IsValidName( pszTitle ) )
		{
			g_itMGR.OpenMsgBox(STR_INVALID_CHARACTER_NAME );
			pEditBox->clear_text();
			return;
		}



		pCtrl = Find( IID_EDIT_SLOGAN );
		if( pCtrl == NULL ) return;
		if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;

		pEditBox = (CTEditBox*)pCtrl;
		char* pszSlogan = pEditBox->get_text();

		if( pszSlogan == NULL )
		{
			g_itMGR.OpenMsgBox( STR_CLAN_INPUT_SLOGAN );
			return;
		}

		if( !pszSlogan[0] )
		{
			g_itMGR.OpenMsgBox( STR_CLAN_INPUT_SLOGAN );
			return;
		}

		g_pNet->Send_cli_CLAN_CREATE( (WORD)m_iSelectedClanBack,(WORD)m_iSelectedClanCenter, pszTitle, pszSlogan );

	}
	else
	{
		std::string strMsg = STR_CLAN_RESULT_CLAN_CREATE_NO_CONDITION;
		strMsg.append(", ");
		strMsg.append( STR_CLAN_CREATE_CONDITION );
		g_itMGR.OpenMsgBox( strMsg.c_str() );
	}
}