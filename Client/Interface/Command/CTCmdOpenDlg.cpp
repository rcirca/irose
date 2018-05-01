#include "stdafx.h"
#include "ctcmdopendlg.h"
#include "../it_mgr.h"
#include "CTCmdNumberInput.h"
#include <algorithm>
#include "DLGs/CMsgBoxCartRide.h"

CTCmdOpenDlg::CTCmdOpenDlg(void)
{
}

CTCmdOpenDlg::~CTCmdOpenDlg(void)
{

}
/*-------------------------------------------------------------------------------------------------------------------------------*/
CTCmdCreateMsgBox::CTCmdCreateMsgBox( CreateMsgBoxData& Data )
{
	m_Data = Data;
}

CTCmdCreateMsgBox::~CTCmdCreateMsgBox(void)
{
	if( m_Data.pCmdOk )
	{
		delete m_Data.pCmdOk;
		m_Data.pCmdOk = NULL;
	}
	if( m_Data.pCmdCancel )
	{
		delete m_Data.pCmdCancel;
		m_Data.pCmdCancel = NULL;
	}
}

/**
* 메세지 박스의 경우 여러개가 띄어질수 있다
*	- 타입별로 구분하며 각 타입별로 인덱스(현재 개수+1)가 부여된다.
*
* @warning		메세지 박스가 닫힐경우 같은 타입의 모든 메세지 박스를 삭제한다.( 그렇지 않을경우 인덱싱에 문제 발생할수 있다.)
* @Author		최종진
* @Date			2005/9/5
*/
bool CTCmdCreateMsgBox::Exec( CTObject* pObj )
{
	list_dlgs_itor iter;
	CMsgBox*	pMsgBox = NULL;
	CTDialog*	pDlg	= NULL;

	int iMsgBoxCount		= 0;
	int iMsgBoxTypeCount	= 0;

	for( iter = g_itMGR.m_Dlgs.begin(); iter != g_itMGR.m_Dlgs.end(); ++iter )
	{
		if( (*iter)->GetDialogType() == DLG_TYPE_MSGBOX )
		{
			iMsgBoxCount++;			
			pMsgBox = (CMsgBox*)*iter;
			if( pMsgBox->GetMsgType() == m_Data.iMsgType )
			{
				iMsgBoxTypeCount++;
			}
		}
	}

	switch( m_Data.iMsgType )
	{
	case CMsgBox::MSGTYPE_RECV_CART_RIDE_REQ:
		pMsgBox = new CMsgBox_CartRide;
		((CMsgBox_CartRide*)pMsgBox)->m_wOwner = m_Data.parm1;
		((CMsgBox_CartRide*)pMsgBox)->m_wGuest = m_Data.parm2;
		break;
	default:
		pMsgBox = new CMsgBox;
		break;
	}
	
	pMsgBox->Create("MsgBox" );

	POINT pt = { 
		g_pCApp->GetWIDTH() / 2 - pMsgBox->GetWidth() / 2,
			g_pCApp->GetHEIGHT() / 2 - pMsgBox->GetWidth() / 2
	};

	if( !m_Data.m_Icons.empty() )
	{
		pMsgBox->SetIcons( m_Data.m_Icons );
		m_Data.m_Icons.clear();
	}
	
	pMsgBox->SetInvokerDlgID( m_Data.iInvokerDlgID );
	pMsgBox->SetButtonType( m_Data.iButtonType );
	if( m_Data.bModal )
		pMsgBox->SetModal();
	else
		pMsgBox->SetModeless();

	pMsgBox->SetMsgType( m_Data.iMsgType );
	pMsgBox->SetString( m_Data.strMsg.c_str() );
	pMsgBox->Show();
	pMsgBox->SetCommand( m_Data.pCmdOk, m_Data.pCmdCancel );

	pt.x += iMsgBoxCount * 10;
	pt.y += iMsgBoxCount * 10;
	pMsgBox->MoveWindow(pt);
	g_itMGR.AppendDlg( DLG_TYPE_MSGBOX, pMsgBox, iMsgBoxTypeCount + 1);

	m_Data.pCmdOk     = NULL;
	m_Data.pCmdCancel = NULL;

	return true;
}

CTCmdDeleteMsgBox::CTCmdDeleteMsgBox( CTDialog* pDlg )
{
	m_pDialog = pDlg;
}

bool CTCmdDeleteMsgBox::Exec( CTObject* pObj )
{
	list_dlgs_itor iter;
	CMsgBox* pMsgBox = (CMsgBox*)m_pDialog;
	int	iMsgType = pMsgBox->GetMsgType();
	CTDialog*	pDlg = NULL;

	iter = find( g_itMGR.m_Dlgs.begin(), g_itMGR.m_Dlgs.end() , m_pDialog );
	if( iter != g_itMGR.m_Dlgs.end() )
	{
		CTDialog* pDlg = *iter;
		g_itMGR.m_Dlgs.erase( iter );
		delete pDlg;
	}


	if( iMsgType )
	{
		CMsgBox* pCompareMsgBox = NULL;
		for( iter = g_itMGR.m_Dlgs.begin(); iter != g_itMGR.m_Dlgs.end();++iter)
		{
			if( (*iter)->GetDialogType() == DLG_TYPE_MSGBOX )
			{
				pCompareMsgBox = ( CMsgBox* ) *iter;
				if( pCompareMsgBox->GetMsgType() == iMsgType )
					(*iter)->Hide();
			}
		}
	}
	return true;
}
/*-------------------------------------------------------------------------------------------------------------------------------*/
CTCmdOpenPrivateChatDlg::CTCmdOpenPrivateChatDlg( DWORD dwUserTag , BYTE btStatus, const char* pszName )
{
	m_dwUserTag = dwUserTag;
	m_btStatus	= btStatus;
	assert( pszName );
	if( pszName )
		m_strName	= pszName;
}

#include "../Dlgs/CPrivateChatDlg.h"
bool CTCmdOpenPrivateChatDlg::Exec( CTObject* pObj )
{
	if( g_itMGR.GetPrivateChatDlg( m_dwUserTag ) )///기존에 해당 유저와의 대화창이 열려있다면 열지 않는다.
		return true;

	int iCount  = 0;
	int iNextID = 0;
	list_dlgs_ritor riter;
	CTDialog* pDlg = NULL;
	for( riter = g_itMGR.m_Dlgs.rbegin() ; riter != g_itMGR.m_Dlgs.rend() ; ++riter )
	{
		if( (*riter)->GetDialogType() == DLG_TYPE_PRIVATECHAT )
		{
			if( iCount == 0 )
			{
				/// 현재 맨 앞에 있는 채팅창
				/// 1. 포커스는 이놈한테 간다.
				/// 2. 새로 만든 창은 이놈 밑으로 간다.
				pDlg = *riter;
				iNextID = pDlg->GetControlID();
			}
			++iCount;
		}
	}

	CPrivateChatDlg* pPrivateChatDlg = new CPrivateChatDlg( DLG_TYPE_PRIVATECHAT );
	pPrivateChatDlg->Create( "DlgPrivateChat" );
	pPrivateChatDlg->SetOther( m_dwUserTag, m_btStatus, m_strName.c_str() );
	if( pDlg )
		g_itMGR.InsertDlg( DLG_TYPE_PRIVATECHAT, pPrivateChatDlg, iNextID, m_dwUserTag );
	else
		g_itMGR.AppendDlg( DLG_TYPE_PRIVATECHAT, pPrivateChatDlg, m_dwUserTag );///처음 요청된 1:1대화창이라면 맨뒤로 가서 제일 먼저 프로세스 되도록 한다.
	
	
	POINT pt = { 0 + iCount * 30 , 120 + iCount * 30};
	
	pPrivateChatDlg->MoveWindow( pt );
	pPrivateChatDlg->Show();

	if( pDlg )
		pDlg->Show();

	return true;
}

CTCmdClosePrivateChatDlg::CTCmdClosePrivateChatDlg( DWORD dwUserTag )
{
	m_dwUserTag = dwUserTag;
}

bool CTCmdClosePrivateChatDlg::Exec( CTObject* pObj )
{
	list_dlgs_itor iter;
	CTDialog* pDlg;
	for( iter = g_itMGR.m_Dlgs.begin() ; iter != g_itMGR.m_Dlgs.end() ; ++iter )
	{
		pDlg = *iter;
		if( pDlg->GetControlID() == m_dwUserTag )
		{
			g_itMGR.m_Dlgs.erase( iter );
			delete pDlg;
			return true;
		}
	}
	return true;
}
/*-------------------------------------------------------------------------------------------------------------------------------*/