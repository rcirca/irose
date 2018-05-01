#include "stdafx.h"
#include "ctargetmenu.h"
#include "CPartyDlg.h"
#include "ExchangeDlg.h"
#include "ChattingDlg.h"
#include "CAvatarStoreDlg.h"

#include "../../JCommandState.h"
#include "../../Object.h"
#include "../it_mgr.h"

#include "../../Network/CNetwork.h"
#include "../../GameData/CExchange.h"
#include "../../GameData/CParty.h"

CTargetMenu::CTargetMenu(void)
{
	m_iTargetAvatarID = 0;
}

CTargetMenu::~CTargetMenu(void)
{
}

unsigned int CTargetMenu::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;
	
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			{
				if( OnLButtonUp( iProcID ) )
					return uiMsg;
				break;
			}
		case WM_RBUTTONDOWN:
			Hide();
			break;
		default:
			break;
		}
		return uiMsg;
	}
	else
	{
		if( uiMsg == WM_RBUTTONDOWN )
		{
			Hide();
			return uiMsg;
		}
	}
	return 0;
}

bool	CTargetMenu::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_FRIEND:
		{
			CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( m_iTargetAvatarID , true );
			if( pObjAVT && pObjAVT->IsPersonalStoreMode() )
			{
				g_pNet->Send_cli_P_STORE_LIST_REQ( g_pObjMGR->Get_ServerObjectIndex(m_iTargetAvatarID) );			
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					pStoreDlg->SetMasterSvrObjIdx( g_pObjMGR->Get_ServerObjectIndex(m_iTargetAvatarID) );
				}
			}

			Hide();
			return true;
		}
	case IID_BTN_EXCHANGE:
		{
			CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( m_iTargetAvatarID , true );
			if( pObjAVT )
			{
				if( CExchange::GetInstance().SendCliTradeReq( g_pObjMGR->Get_ServerObjectIndex(pObjAVT->Get_INDEX()) ) )
				{
					string Temp = pObjAVT->Get_NAME();
					Temp.append(STR_REQ_TRADE);
					g_itMGR.AppendChatMsg( Temp.c_str(), IT_MGR::CHAT_TYPE_SYSTEM);
				}
			}
			Hide();
		}
		return true;
	case IID_BTN_PARTY:
		{
	/*		g_UserInputSystem.ClearMouseState();
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PARTY );
			if( pDlg )
			{
				CPartyDlg* pPartyDlg = ( CPartyDlg* ) pDlg;
				CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( m_iTargetAvatarID, true );

				if( pObjChar == NULL )
					break;*/

				///REMARK REMOVE TEST CODE
				///레벨차가 너무 클경우
/*				if( abs(pObjChar->Get_LEVEL() - g_pAVATAR->Get_LEVEL()) > 5 )
				{
					char szBuf[256];
					sprintf( szBuf,"%s님은 레벨이 맞지 않아 파티에 들어올 수 없습니다", pObjChar->Get_NAME());
					g_itMGR.AppendChatMsg( szBuf, g_dwWHITE );
				}
			//	else /// 레벨차가 적당할 경우 */
			//	{
					if( CParty::GetInstance().HasParty() )
					{
						///파티 초대
						if( CParty::GetInstance().IsPartyLeader() )
						{
							///파티 풀
							if( CParty::GetInstance().IsPartyFull() )
								g_itMGR.AppendChatMsg(STR_PARTY_IS_FULL, IT_MGR::CHAT_TYPE_SYSTEM);
							else
								g_pNet->Send_cli_PARTY_REQ(PARTY_REQ_JOIN,  g_pObjMGR->Get_ServerObjectIndex(m_iTargetAvatarID));
						}
						else///Error
						{
							g_itMGR.AppendChatMsg(STR_CANT_INVITE_TO_PARTY, IT_MGR::CHAT_TYPE_SYSTEM);
						}
					}
					else
					{
						///파티 결성
						g_pNet->Send_cli_PARTY_REQ(PARTY_REQ_MAKE,  g_pObjMGR->Get_ServerObjectIndex(m_iTargetAvatarID));
					}
			//	}
			//}
			Hide();
			return true;
		}
	case IID_BTN_INFORMATION:
		Hide();
		return true;
	default:
		break;
	}
	return false;
}

void CTargetMenu::Show()
{
	CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( m_iTargetAvatarID, true );

	if( pObj == NULL )
		return;
	
	///Target이 아바타일경우에만 현재 스크린 좌표에 따라서 윈도우의 위치 조정
	if( pObj->Get_TYPE() == OBJ_AVATAR ) 
	{
		D3DVECTOR   PosSCR;
		POINT		ptNew;
		pObj->GetScreenPOS (PosSCR);	
		ptNew.x = (int)PosSCR.x - GetWidth() / 2;
		ptNew.y = (int)PosSCR.y - ( GetHeight() * 2 / 3 );
		MoveWindow( ptNew );
		CTDialog::Show();
	}
	SetEnableChild( IID_BTN_FRIEND, false );
	SetEnableChild(	IID_BTN_INFORMATION,false );

}

void CTargetMenu::Update( POINT ptMouse )
{
	if( !IsVision()) return;

	CTDialog::Update( ptMouse );
	if( m_iTargetAvatarID )
	{
		CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( m_iTargetAvatarID, true );
		/// 유효하지 않은 타겟이다.. 마우스 컴맨드 초기화
		if( pObj == NULL )
		{
			Hide();
		}
		else
		{
			if( g_pAVATAR->Get_DISTANCE( pObj ) >= 1200 )
				Hide();
			else
			{
				D3DVECTOR   PosSCR;
				POINT		ptNew;
				pObj->GetScreenPOS (PosSCR);	
				ptNew.x = (int)PosSCR.x - GetWidth() / 2;
				ptNew.y = (int)PosSCR.y - ( GetHeight() * 2 / 3 );
				MoveWindow( ptNew );

			}
		}
	}
	

	WINCTRL_LIST_ITOR iter;
	CWinCtrl*	pCtrl;
	
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter)
	{
		pCtrl = *iter;
		if( pCtrl->GetControlType() == CTRL_IMAGE )
			continue;

		if( pCtrl->IsInside(ptMouse.x, ptMouse.y ) )
		{
//			g_itMGR.DrawToolTip( (short)ptMouse.x, (short)ptMouse.y, GetDialogType(), pCtrl->GetControlID());
			break;
		}
	}


/*
	int iTargetID = g_pAVATAR->m_iServerTarget;
	if( iTargetID )
	{
		CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( g_CommandState.GetCurrentTarget(), true );
		/// 유효하지 않은 타겟이다.. 마우스 컴맨드 초기화
		if( pObj == NULL )
		{
			g_CommandState.ClearMouseState();
			Hide();
			return;
		}
		
		///Target이 아바타일경우에만 현재 스크린 좌표에 따라서 윈도우의 위치 조정
		if( pObj->Get_TYPE() == OBJ_AVATAR ) 
		{
			D3DVECTOR   PosSCR;
			POINT		ptNew;
			pObj->GetScreenPOS (PosSCR);	
			ptNew.x = (int)PosSCR.x - GetWidth() / 2;
			ptNew.y = (int)PosSCR.y - ( GetHeight() * 2 / 3 );
			MoveWindow( ptNew );
			m_iTargetAvatarID = iTargetID;
//			Show();
		}
		else
		{
			Hide();
		}
	}
	else
		Hide();
*/
}

void CTargetMenu::Hide()
{
	m_iTargetAvatarID = 0;
	CTDialog::Hide();
}