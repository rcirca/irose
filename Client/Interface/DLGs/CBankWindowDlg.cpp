#include "stdafx.h"
#include ".\cbankwindowdlg.h"
#include "../typeresource.h"
#include "../../Game.h"
#include "../../Object.h"
#include "../../Network/CNetwork.h"
#include "../../GameData/CBank.h"
#include "../IT_MGR.h"
#include "../CToolTipMgr.h"


#include "TEditBox.h"

CBankWindowDlg::CBankWindowDlg(void)
{

}

CBankWindowDlg::~CBankWindowDlg(void)
{

}

void CBankWindowDlg::PostMsg( unsigned msg, unsigned param1, unsigned param2 )
{
	switch( msg )
	{
	case MSG_SETYPE:
		{
			m_Type = param1;
			RECT rc = { 8, 8, 168, 22 };
			m_Title.set_color( g_dwORANGE );
			m_Title.set_format( DT_CENTER | DT_VCENTER );
			//m_Title.set
			switch( m_Type )
			{
			case TYPE_SAVE:
				m_Title.set_string( STR_QUERY_BANK_SAVE, rc, 0 );
				break;
			case TYPE_WITHDRAW:
				m_Title.set_string( STR_QUERY_BANK_WITHDRAW, rc, 0 );
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void CBankWindowDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	m_Title.draw();
	//switch( m_Type )
	//{
	//case TYPE_SAVE:
	//	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 5, 10, g_dwORANGE, STR_QUERY_BANK_SAVE );
	//	//::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 5, 10, g_dwORANGE, "111111111111111111111111111111111111111111111111111111111111111" );
	//	break;
	//case TYPE_WITHDRAW:
	//	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 5, 10, g_dwORANGE, STR_QUERY_BANK_WITHDRAW );
	//	break;
	//default:
	//	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 5, 10, g_dwORANGE, "Invalid Type" );
	//	break;
	//}
}

unsigned CBankWindowDlg::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) 
		return 0;
	
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )
			OnLButtonUp( uiProcID );

		return uiMsg;
	}
	return 0;
}

void CBankWindowDlg::OnLButtonUp( unsigned proc_id )
{
	switch( proc_id )
	{
	case IID_BTN_OK:
		{
			CWinCtrl* pCtrl = Find( IID_EDITBOX );
			if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
			{
				CTEditBox* pEditBox = (CTEditBox*) pCtrl;
				if( char* text = pEditBox->get_text() )
				{
					__int64 zuly = _atoi64( text );
					if( zuly > 0 )
					{
						switch( m_Type )	
						{
						case TYPE_SAVE:
							if( zuly <= g_pAVATAR->Get_MONEY() )
								g_pNet->Send_cli_MOVE_ZULY_INV2BANK( zuly );
							else
								g_itMGR.OpenMsgBox( STR_BANK_SAVE_FAIL );
							break;
						case TYPE_WITHDRAW:
							if( zuly <= CBank::GetInstance().GetMoney() )
								g_pNet->Send_cli_MOVE_ZULY_BANK2INV( zuly );
							else
								g_itMGR.OpenMsgBox( STR_BANK_WITHDRAW_FAIL );
							break;
						default:
							break;
						}
					}
				}
			}
			Hide();
		}
		break;
	case IID_BTN_CANCEL:
		Hide();
		break;
	default:
		break;
	}
}



void CBankWindowDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) 
		return;

	CTDialog::Update( ptMouse );

	///이하 툴팁 표시
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
			return ;

	if( m_Title.is_ellipsis() )
	{
		POINT ptPoint = { ptMouse.x - m_sPosition.x , ptMouse.y - m_sPosition.y };
		const RECT& rect_zonename = m_Title.get_rect();
		if( PtInRect( &rect_zonename, ptPoint ) )
		{
			CToolTipMgr::GetInstance().RegToolTip( m_sPosition.x - 20, m_sPosition.y - 20, m_Title.get_string() );
			return;
		}
	}
}