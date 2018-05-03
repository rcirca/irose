#include "stdafx.h"
#include ".\cpartydlg.h"
#include "CPartyMember.h"
#include "ExchangeDlg.h"
#include "../IT_MGR.h"
#include "../CToolTipMgr.h"
#include "../IO_ImageRes.h"
#include "../CTDrawImpl.h"
#include "../Command/CTCmdHotExec.h"

#include "../../Object.h"
#include "../../Game.h"
#include "../../Object.h"
#include "../../Network/CNetwork.h"
#include "../../GameData/Event/CTEventParty.h"
#include "../../GameData/CParty.h"

#include "tgamectrl/resourcemgr.h"
#include "tgamectrl/tlistbox.h"
#include "tgamectrl/teditbox.h"
#include "tgamectrl/tabbedpane.h"
#include "tgamectrl/jcontainer.h"
#include "tgamectrl/tscrollbar.h"
#include "tgamectrl/tradiobox.h"
#include "tgamectrl/tradiobutton.h"

#include <algorithm>

const int   g_iAutoExpDecreaseTimeSpace = 10000;///클라이언트에서 파티 경험치를 얼마의 시간마다 파티레벨만큼 빼주나
///TDialog의 Left-Top에서부터의 거리

const int	g_iPartyExpGuageWidth	= 111;
const int	g_iPartyExpGuageHeight	= 9;


CPartyDlg::CPartyDlg(void)
{
	m_iGuagePartyExpIdx		= 0;
	m_iGuagePartyExpBGIdx	= 0;
}

CPartyDlg::~CPartyDlg(void)
{
	ClearMemberList();
}


/// 방장은 맨위로 ,, 나머지는 그대로
void CPartyDlg::ResetMemberPositions()
{
	POINT pt = { m_sPosition.x + 5 , m_sPosition.y + m_iHeight + 5 };
	POINT ptMember = pt;
	LIST_PARTY_MEMBER_ITER iter;

	int iCount = 0;
	
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter, ++iCount )
	{
		ptMember.y  = pt.y + iCount * (*iter)->GetHeight();
		(*iter)->SetPosition( ptMember );
	}
}

unsigned int CPartyDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( !IsVision() )
		return 0;

	if( uiMsg == WM_LBUTTONDOWN )
	{
		CPartyMember* pMember = NULL;
		LIST_PARTY_MEMBER_ITER iter,iter2;
		for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
		{
			if( (*iter)->Process( uiMsg, wParam, lParam ))
			{
				pMember = *iter;
				for( iter2 = m_listPartyMember.begin(); iter2 != m_listPartyMember.end(); ++iter2 )
				{
					if( pMember == *iter2 )
						(*iter2)->SetSelected( true );
					else
						(*iter2)->SetSelected( false );
				}
				return uiMsg;
			}
		}
	}

	unsigned iProcID =  0;
	if( iProcID = CTDialog::Process( uiMsg,wParam, lParam ) )
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	else
	{
	}
	return 0;
}

void	CPartyDlg::OnLButtonUp( unsigned iProcID )
{

	switch( iProcID )
	{
	case IID_BTN_OPTION:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PARTYOPTION ) )
		{
			POINT pt = { m_sPosition.x - pDlg->GetWidth(),m_sPosition.y + 80};
			if( pt.x < 0 )
				pt.x = m_sPosition.x + m_iWidth;
			pDlg->MoveWindow( pt );
		}
		g_itMGR.OpenDialog( DLG_TYPE_PARTYOPTION );
		break;
	case IID_BTN_ENTRUST:
		{
			if( CPartyMember* pMember = GetSelectedMember())
			{
				//자기자신은 안되지
				if( pMember->GetObjectTag() == g_pAVATAR->GetUniqueTag() )
					break;
				if( pMember->GetObjSvrIdx() <= 0 )
					break;
				CTCmdSendPacketPartyReq* pCmd = new CTCmdSendPacketPartyReq( PARTY_REQ_CHANGE_OWNER, pMember->GetObjSvrIdx() );
				sprintf(g_MsgBuf,FORMAT_STR_PARTY_QUERY_ENTRUST,pMember->GetName());
				g_itMGR.OpenMsgBox(g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0 ,pCmd, NULL );

			}
			break;
		}
	case IID_BTN_CLOSE:
	case IID_BTN_LEAVE:
		{
			CTMacroCommand* pMacroCmd = new CTMacroCommand;

			CTCmdSendPacketPartyReq*	pSubCmd1 = new CTCmdSendPacketPartyReq(PARTY_REQ_LEFT, g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX() ) );
			CTCmdLeaveParty*			pSubCmd2 = new CTCmdLeaveParty;

			pMacroCmd->AddSubCommand( pSubCmd1 );
			pMacroCmd->AddSubCommand( pSubCmd2 );

			g_itMGR.OpenMsgBox( STR_PARTY_QUERY_LEAVE,CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0 ,pMacroCmd, NULL );
		}
		break;
	case IID_BTN_BAN:
		{
			if( CPartyMember* pMember = GetSelectedMember())
			{
				//자기자신은 안되지
				if( pMember->GetObjectTag()  == g_pAVATAR->GetUniqueTag() )
					break;

				CTCmdSendPacketPartyReq* pCmd = new CTCmdSendPacketPartyReq( PARTY_REQ_BAN, pMember->GetObjectTag() );
				sprintf(g_MsgBuf,FORMAT_STR_PARTY_QUERY_BAN,pMember->GetName());
				g_itMGR.OpenMsgBox(g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0 ,pCmd, NULL );
			}
			break;
		}
	default:
		break;
	}
}

WORD	CPartyDlg::GetSelectedMemberSvrIdx()
{
	LIST_PARTY_MEMBER_ITER iter;
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end();  ++iter)
	{
		if( (*iter)->IsSelected() )
			return (*iter)->GetObjSvrIdx();
	}
	return 0;
}
DWORD  CPartyDlg::GetSelectedMemberUserTag()
{
	LIST_PARTY_MEMBER_ITER iter;
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end();  ++iter)
	{
		if( (*iter)->IsSelected() )
			return (*iter)->GetObjectTag();
	}
	return 0;
}

CPartyMember* CPartyDlg::GetSelectedMember()
{
	LIST_PARTY_MEMBER_ITER iter;
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end();  ++iter)
	{
		if( (*iter)->IsSelected() )
			return (*iter);
	}
	return NULL;
}
CPartyMember* CPartyDlg::FindMember( WORD wObjSvrIdx )
{
	LIST_PARTY_MEMBER_ITER iter;
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end();  ++iter)
	{
		if( (*iter)->GetObjSvrIdx() == wObjSvrIdx )
			return *iter;
	}
	return NULL;
}

void	CPartyDlg::ClearMemberList()
{
	LIST_PARTY_MEMBER_ITER iter;
	for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end();  )
	{
		delete *iter;
		iter = m_listPartyMember.erase( iter );
	}
}



void CPartyDlg::Show()
{
	
	m_iGuagePartyExpIdx		= CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI18_GUAGE_PARTYLEVEL");
	m_iGuagePartyExpBGIdx	= CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI18_GUAGE_PARTYLEVEL_BASE");

	CTDialog::Show();
}

void CPartyDlg::Hide()
{
	CTDialog::Hide();
}

void CPartyDlg::Draw()
{
	if( !IsVision() )
		return;

	CTDialog::Draw();

	CParty& Party = CParty::GetInstance();
	for_each( m_listPartyMember.begin(), m_listPartyMember.end(), mem_fun( &CPartyMember::Draw) );

	///파티레벨, 파티 경험치
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y, 0.0f);
	::setTransformSprite( mat );

	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 35,  7, g_dwBLACK, STR_PARTY );
	//drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 17, 34, g_dwWHITE, CStr::Printf("%s %d", STR_PARTYLEVEL, Party.GetLevel() ));
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 11, 32, g_dwWHITE, CStr::Printf("%s %d", STR_PARTYLEVEL, Party.GetLevel() ));

	///경험치
	short nWidth = 0;
	
	int iPartyExpMax = Party.GetMaxExp();
	int iPartyExp	 = Party.GetCurrExp();

	if( iPartyExpMax <= 0 )///Devide By Zero
		iPartyExpMax = 1;

	if ( iPartyExp < 0 ) 
		iPartyExp = 0;

	nWidth = ( g_iPartyExpGuageWidth * iPartyExp)/ iPartyExpMax;

	g_DrawImpl.Draw( m_sPosition.x + 96, m_sPosition.y + 34, IMAGE_RES_UI, m_iGuagePartyExpBGIdx );
	g_DrawImpl.Draw( m_sPosition.x + 96, m_sPosition.y + 34, nWidth, IMAGE_RES_UI,  m_iGuagePartyExpIdx );
	
	///
	float fExp = iPartyExp * 100 / (float)iPartyExpMax;

	RECT rcDraw = { 0,0,g_iPartyExpGuageWidth,g_iPartyExpGuageHeight + 5};

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER,
		CStr::Printf( "%2.1f%%", fExp  ) );		
}


void CPartyDlg::Update( POINT ptMouse )
{
	if( !IsVision() )
		return ;

	static DWORD s_dwPrevTime = g_GameDATA.GetGameTime();
	//DWORD	dwCurrTime        = g_GameDATA.GetGameTime();


	CTDialog::Update( ptMouse );

	for_each( m_listPartyMember.begin(), m_listPartyMember.end(), bind2nd( mem_fun( &CPartyMember::Update), ptMouse ));

	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
		return ;


	CWinCtrl* pCtrl;
	pCtrl = Find( IID_BTN_LEAVE );
	if( pCtrl && pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
	{
		CToolTipMgr::GetInstance().RegToolTip( pCtrl->GetPosition().x - 20, pCtrl->GetPosition().y - 20 , TT_PARTY_LEAVE );
		return;
	}


	pCtrl = Find( IID_BTN_OPTION );
	if( pCtrl && pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
	{
		CToolTipMgr::GetInstance().RegToolTip( pCtrl->GetPosition().x - 20, pCtrl->GetPosition().y - 20, STR_OPTION );
		return;
	}


	pCtrl = Find( IID_BTN_ENTRUST );
	if( pCtrl && pCtrl->IsVision() && pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
	{
		CToolTipMgr::GetInstance().RegToolTip( pCtrl->GetPosition().x - 20, pCtrl->GetPosition().y - 20, TT_PARTY_ENTRUST );
		return;
	}

	pCtrl = Find( IID_BTN_BAN );
	if( pCtrl && pCtrl->IsVision() && pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
	{
		CToolTipMgr::GetInstance().RegToolTip( pCtrl->GetPosition().x - 20, pCtrl->GetPosition().y - 20, TT_PARTY_BAN );
		return;
	}

}



void CPartyDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	ResetMemberPositions();
}


void CPartyDlg::Update( CObservable* pObservable, CTObject* pObj )
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
	case CTEventParty::EID_MEMBER_DISCONNECTED:
		{
			LIST_PARTY_MEMBER_ITER iter;
			//CPartyMember* pMember = NULL;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
			{
				if( (*iter)->GetObjectTag() == pEvent->GetObjectTag() )
				{
					(*iter)->SetObjSvrIdx( iObjSvrIdx );
					break;
				}
			}
			break;
		}
	case CTEventParty::EID_JOIN_MEMBER:
		{
			CPartyMember* pMember = new CPartyMember( pEvent->GetObjectTag(), pEvent->GetObjSvrIdx(), pEvent->GetName() );
			m_listPartyMember.push_back( pMember );
			ResetMemberPositions();

			g_itMGR.AppendChatMsg( CStr::Printf(FORMAT_STR_PARTY_JOIN, pEvent->GetName() ), IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		}
	case CTEventParty::EID_LEAVE_MEMBER:
		{
			LIST_PARTY_MEMBER_ITER iter;
			CPartyMember* pMember = NULL;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
			{
				if( (*iter)->GetObjSvrIdx() == iObjSvrIdx )
				{
					pMember = *iter;
					m_listPartyMember.erase( iter );
					
					g_itMGR.AppendChatMsg( 
						CStr::Printf( FORMAT_STR_PARTY_LEAVE, pMember->GetName() ), 
						IT_MGR::CHAT_TYPE_SYSTEM);

					delete pMember;
					ResetMemberPositions();

					break;
				}
			}
			break;
		}
	case CTEventParty::EID_CHANGE_LEADER:
		{
			CWinCtrl* pCtrl = NULL;

			LIST_PARTY_MEMBER_ITER iter;
			
			CPartyMember* pMember = NULL;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
			{
				pMember = *iter;
				if( pMember->GetObjSvrIdx() == iObjSvrIdx )
				{
					m_listPartyMember.erase( iter );
					m_listPartyMember.push_front( pMember );

					if( Party.IsPartyLeader() )
					{
						ShowChild(IID_BTN_ENTRUST);
						ShowChild(IID_BTN_BAN);
						g_itMGR.AppendChatMsg( STR_PARTY_ENTRUSTED_LEADER, IT_MGR::CHAT_TYPE_SYSTEM );
					}
					else
					{
						HideChild(IID_BTN_ENTRUST);
						HideChild(IID_BTN_BAN);
						g_itMGR.AppendChatMsg( CStr::Printf(FORMAT_STR_PARTY_CHANGE_LEADER,pMember->GetName()), IT_MGR::CHAT_TYPE_SYSTEM );
					}
					break;
				}
			}
			
			ResetMemberPositions();
			
			break;
		}
	case  CTEventParty::EID_BAN_MEMBER:
		{
			LIST_PARTY_MEMBER_ITER iter;
			CPartyMember* pMember = NULL;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
			{
				if( (*iter)->GetObjectTag() == pEvent->GetObjectTag() )
				{
					pMember = *iter;
					m_listPartyMember.erase( iter );
					
					g_itMGR.AppendChatMsg( CStr::Printf(FORMAT_STR_PARTY_BANNED,pMember->GetName()), IT_MGR::CHAT_TYPE_SYSTEM );

					delete pMember;
					ResetMemberPositions();
					break;
				}
			}
			break;
		}
	case CTEventParty::EID_CLEAR:
		{
			LIST_PARTY_MEMBER_ITER iter;
			CPartyMember* pMember = NULL;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); )
			{
				pMember = *iter;
				iter = m_listPartyMember.erase( iter );
				delete pMember;
			}

			Hide();
			ClearMemberList();
			g_itMGR.DelDialogIcon( GetDialogType() );

			break;
		}
	case CTEventParty::EID_CHANGE_OBJECT_SERVERINDEX:
		{
			LIST_PARTY_MEMBER_ITER iter;
			for( iter = m_listPartyMember.begin(); iter != m_listPartyMember.end(); ++iter )
			{
				if( (*iter)->GetObjectTag() == pEvent->GetObjectTag() )
				{
					(*iter)->SetObjSvrIdx( pEvent->GetObjSvrIdx() );
					break;
				}
			}
			break;
		}
	case CTEventParty::EID_INIT:
		ClearMemberList();
		Show();
		break;
	default:
		break;
	}
}

