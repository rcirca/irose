#include "stdafx.h"
#include "system/CGAME.h"
#include ".\ctcmdhotexec.h"
#include "../../CApplication.h"
#include "../../Network/CNetwork.h"
#include "../it_mgr.h"
#include "../Dlgs/ExchangeDlg.h"
#include "../Dlgs/CStoreDlg.h"
#include "../Dlgs/MakeDlg.h"
#include "../Dlgs/CPartyDlg.h"
#include "../Dlgs/ChattingDlg.h"
#include "../../GameData/CParty.h"
CTCmdExit::CTCmdExit(void)
{
}

CTCmdExit::~CTCmdExit(void)
{
}

bool CTCmdExit::Exec( CTObject* pObj )
{

	g_pCApp->SetExitGame();

	return true;
}




CTCmdReLogin::CTCmdReLogin(void)
{
}

CTCmdReLogin::~CTCmdReLogin(void)
{
}

bool CTCmdReLogin::Exec( CTObject* pObj )
{
	g_pCApp->ResetExitGame();
	CGame::GetInstance().ChangeState( CGame::GS_RELOGIN );

	return true;
}


/*--------------------------------------------------------------------------------------------------------------------------------------*/
/// 1:1 거래 관련
bool CTCmdAcceptTradeReq::Exec( CTObject* pObj )
{
	CExchangeDLG* pDlg = g_itMGR.GetExchangeDLG();

	if( pDlg == NULL )
		return true;


	if( pDlg )
	{
		CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ(m_wReqServerObjectIdx, false);
		if ( pObjChar == NULL ) 
			return true;
		CExchange::GetInstance().StartExchange( m_wReqServerObjectIdx ,pObjChar->Get_NAME() );
	}

	g_pNet->Send_cli_TRADE_P2P(  m_wReqServerObjectIdx  , RESULT_TRADE_ACCEPT  );


	int iPosX = g_pCApp->GetWIDTH() / 2 - pDlg->GetWidth();
	int iPosY = g_pCApp->GetHEIGHT() / 2 - pDlg->GetHeight() / 2;
	g_itMGR.OpenDialog( DLG_TYPE_EXCHANGE , true, iPosX, iPosY );	


	iPosX = g_pCApp->GetWIDTH() / 2;
	g_itMGR.OpenDialog( DLG_TYPE_ITEM , false, iPosX, iPosY );

	return true;
}

bool CTCmdRejectTradeReq::Exec( CTObject* pObj )
{
	CExchangeDLG* pDlg = g_itMGR.GetExchangeDLG();
	if( pDlg == NULL )
		return true;


	g_pNet->Send_cli_TRADE_P2P(  m_wReqServerObjectIdx  , RESULT_TRADE_REJECT  );
	return true;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
bool CTCmdHideDialog::Exec( CTObject* pObj )
{
	CTDialog* pDlg = g_itMGR.FindDlg( m_iDialogID );
	if( pDlg && pDlg->IsVision() )
		pDlg->Hide();

	return true;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
bool CTCmdCloseStore::Exec( CTObject* pObj )
{
	// 상점 닫기 
	CStoreDLG* pStore = (CStoreDLG*)g_itMGR.FindDlg( DLG_TYPE_STORE );
	if( pStore )
	{
		pStore->SetTab(0);
		pStore->Hide();
	}
	
	// 거래창 닫기 
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DEAL );
	
	if( pDlg && pDlg->IsVision() )
		pDlg->Hide();

	///숫자입력기가 떠 있다면 닫는다.
	pDlg = g_itMGR.FindDlg( DLG_TYPE_N_INPUT );
	if( pDlg && pDlg->IsVision() )
		pDlg->Hide();
	
	return true;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
///현재 CMakeDLG에만 적용 ChangeState 란 virtual Func를 TDialog에 추가하자
bool CTCmdChangeState::Exec( CTObject* pObj )
{
	if( pObj == NULL ) return true;

	CTDialog* pDlg = (CTDialog*) pObj ;
	CMakeDLG* pMakeDlg = ( CMakeDLG* )pDlg;
	pMakeDlg->ChangeState( m_iState );
	return true;
}

/*------------------------------------------------------------------------------------------*/
/// 파티 관련
bool CTCmdSendPacketPartyReq::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_PARTY_REQ( m_btRequest, m_dwObjectIDXorTAG );
	return true;
}


bool CTCmdSendPacketPartyReply::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_PARTY_REPLY( m_btReply, m_wObjSvrIdx );
	return true;
}


bool CTCmdLeaveParty::Exec( CTObject* pObj )
{
	CParty::GetInstance().Leave();
	return true;
}

bool CTCmdAcceptPartyJoin::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_PARTY_REPLY(PARTY_REPLY_ACCEPT_MAKE, m_wDestObjSvrIdx );

	g_itMGR.AppendChatMsg( STR_PARTY_MADE, IT_MGR::CHAT_TYPE_SYSTEM);

	return true;
}

bool CTCmdRejectPartyJoin::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_REJECT_JOIN, m_wDestObjSvrIdx );
	return true;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
// 2인승 카트
bool CTCmdAcceptCartRide::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_CART_RIDE( CART_RIDE_ACCEPT, m_wOwnerIDX, m_wGuestIDX );
	return true;
}
// 2인승 카트
bool CTCmdRejectCartRide::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_CART_RIDE( CART_RIDE_REFUSE, m_wOwnerIDX, m_wGuestIDX );
	return true;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/