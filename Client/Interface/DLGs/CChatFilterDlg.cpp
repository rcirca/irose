#include "stdafx.h"
#include ".\cchatfilterdlg.h"
#include "../it_mgr.h"

#include "tgamectrl/tcheckbox.h"
CChatFilterDlg::CChatFilterDlg(void)
{

}

CChatFilterDlg::~CChatFilterDlg(void)
{

}

void CChatFilterDlg::Show()
{
	CTDialog::Show();
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
	{
		CChatDLG* pChatDlg = (CChatDLG*)pDlg;
		m_filter = pChatDlg->GetCurrentTabFilter();

		CWinCtrl* pCtrl = NULL;

		if( pCtrl = Find( CHECKBOX_ALL ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetUncheck();
		}


		if( pCtrl = Find( CHECKBOX_SYSTEM ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetEnable( true );
			switch( m_filter.Filters[1])
			{
			case 0:
				pCheckBox->SetUncheck();
				break;
			case 1:
				pCheckBox->SetCheck();
				break;
			case 2:
				pCheckBox->SetCheck();
				pCheckBox->SetEnable( false );
				break;
			default:
				break;
			}
		}



		if( pCtrl = Find( CHECKBOX_TRADE ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetEnable( true );
			switch( m_filter.Filters[2])
			{
			case 0:
				pCheckBox->SetUncheck();
				break;
			case 1:
				pCheckBox->SetCheck();
				break;
			case 2:
				pCheckBox->SetCheck();
				pCheckBox->SetEnable( false );
				break;
			default:
				break;
			}
		}


		if( pCtrl = Find( CHECKBOX_PARTY ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetEnable( true );
			switch( m_filter.Filters[3])
			{
			case 0:
				pCheckBox->SetUncheck();
				break;
			case 1:
				pCheckBox->SetCheck();
				break;
			case 2:
				pCheckBox->SetCheck();
				pCheckBox->SetEnable( false );
				break;
			default:
				break;
			}
		}

		if( pCtrl = Find( CHECKBOX_CLAN ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetEnable( true );
			switch( m_filter.Filters[4])
			{
			case 0:
				pCheckBox->SetUncheck();
				break;
			case 1:
				pCheckBox->SetCheck();
				break;
			case 2:
				pCheckBox->SetCheck();
				pCheckBox->SetEnable( false );
				break;
			default:
				break;
			}
		}

		if( pCtrl = Find( CHECKBOX_ALLIED ) )
		{
			CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
			pCheckBox->SetEnable( true );
			switch( m_filter.Filters[5])
			{
			case 0:
				pCheckBox->SetUncheck();
				break;
			case 1:
				pCheckBox->SetCheck();
				break;
			case 2:
				pCheckBox->SetCheck();
				pCheckBox->SetEnable( false );
				break;
			default:
				break;
			}
		}


	}
	///현재 채팅창에서 선택된 탭의 필터링 옵션을 가져와서 체크박스를 초기화 한다.
}

unsigned CChatFilterDlg::Process( unsigned uiMsg, WPARAM wParam , LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONDOWN )
		{
			switch( uiProcID )
			{
			case CHECKBOX_ALL:
				{
					if( m_filter.Filters[1] != 2 )
					{
						if( CWinCtrl* pCtrl = Find( CHECKBOX_SYSTEM ) )
						{
							CTCheckBox* pBox = (CTCheckBox*)pCtrl;
							pBox->SetCheck();
						}
						m_filter.Filters[1] = 1;
					}


					if( m_filter.Filters[2] != 2 )
					{
						if( CWinCtrl* pCtrl = Find( CHECKBOX_TRADE ) )
						{
							CTCheckBox* pBox = (CTCheckBox*)pCtrl;
							pBox->SetCheck();
						}
						m_filter.Filters[2] = 1;
					}

					if( m_filter.Filters[3] != 2 )
					{
						if( CWinCtrl* pCtrl = Find( CHECKBOX_PARTY ) )
						{
							CTCheckBox* pBox = (CTCheckBox*)pCtrl;
							pBox->SetCheck();
						}
						m_filter.Filters[3] = 1;
					}

					if( m_filter.Filters[4] != 2 )
					{
						if( CWinCtrl* pCtrl = Find( CHECKBOX_CLAN ) )
						{
							CTCheckBox* pBox = (CTCheckBox*)pCtrl;
							pBox->SetCheck();
						}
						m_filter.Filters[4] = 1;
					}

					if( m_filter.Filters[5] != 2 )
					{
						if( CWinCtrl* pCtrl = Find( CHECKBOX_ALLIED ) )
						{
							CTCheckBox* pBox = (CTCheckBox*)pCtrl;
							pBox->SetCheck();
						}
						m_filter.Filters[5] = 1;
					}


					if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
					{
						CChatDLG* pChatDlg = (CChatDLG*)pDlg;
						pChatDlg->SetCurrentTabFilter( m_filter );
					}

				}
				break;
			case CHECKBOX_SYSTEM:
				{
					if( CWinCtrl* pCtrl = Find( CHECKBOX_SYSTEM ) )
					{
						CTCheckBox* pBox = (CTCheckBox*)pCtrl;
						if( pBox->IsCheck() )
							m_filter.Filters[1] = 1;
						else
							m_filter.Filters[1] = 0;

						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SetCurrentTabFilter( m_filter );
						}
					}
				}
				break;
			case CHECKBOX_TRADE:
				{
					if( CWinCtrl* pCtrl = Find( CHECKBOX_TRADE ) )
					{
						CTCheckBox* pBox = (CTCheckBox*)pCtrl;
						if( pBox->IsCheck() )
							m_filter.Filters[2] = 1;
						else
							m_filter.Filters[2] = 0;

						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SetCurrentTabFilter( m_filter );
						}
					}
				}
				break;
			case CHECKBOX_PARTY:
				{
					if( CWinCtrl* pCtrl = Find( CHECKBOX_PARTY ) )
					{
						CTCheckBox* pBox = (CTCheckBox*)pCtrl;
						if( pBox->IsCheck() )
							m_filter.Filters[3] = 1;
						else
							m_filter.Filters[3] = 0;

						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SetCurrentTabFilter( m_filter );
						}
					}
				}
				break;
			case CHECKBOX_CLAN:
				{
					if( CWinCtrl* pCtrl = Find( CHECKBOX_CLAN ) )
					{
						CTCheckBox* pBox = (CTCheckBox*)pCtrl;
						if( pBox->IsCheck() )
							m_filter.Filters[4] = 1;
						else
							m_filter.Filters[4] = 0;

						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SetCurrentTabFilter( m_filter );
						}
					}
				}
			case CHECKBOX_ALLIED:
				{
					if( CWinCtrl* pCtrl = Find( CHECKBOX_ALLIED ) )
					{
						CTCheckBox* pBox = (CTCheckBox*)pCtrl;
						if( pBox->IsCheck() )
							m_filter.Filters[5] = 1;
						else
							m_filter.Filters[5] = 0;

						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SetCurrentTabFilter( m_filter );
						}
					}
				}
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}