#include "stdafx.h"
#include ".\cmsgbox.h"
#include "tgamectrl/tcommand.h"
#include "tgamectrl/tmsgbox.h"
#include "tgamectrl/tlistbox.h"

#include "../IO_ImageRes.h"

#include "../it_mgr.h"
#include "../ExternalUI/ExternalUILobby.h"
#include "../../System/CGame.h"
#include "../Command/CTCmdOpenDlg.h"
#include "../icon/cicon.h"
CMsgBox::CMsgBox(void)
{
	m_pCmdOk = NULL;
	m_pCmdCancel = NULL;
	
	CSlot Slot;
	for( int i = 0 ; i < 4; ++i )
	{
		Slot.SetParent( DLG_TYPE_MSGBOX );
		Slot.SetOffset( 2, 0 );
		m_Slots.push_back( Slot );
	}
}

CMsgBox::~CMsgBox(void)
{
	std::vector<CSlot>::iterator iter;
	for( iter = m_Slots.begin(); iter != m_Slots.end();  ++iter)
		iter->DetachIcon();


	ClearCommand();
}

void CMsgBox::Hide()
{
	CTMsgBox::Hide();
	ClearCommand();
	if( CGame::GetInstance().GetCurrStateID() == CGame::GS_MAIN )
	{
		CTCommand* pCmd = new CTCmdDeleteMsgBox(this);
		g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
	}
}
void CMsgBox::ClearCommand()
{
	if( m_pCmdOk )
	{
		delete m_pCmdOk;
		m_pCmdOk = NULL;
	}
	if( m_pCmdCancel )
	{
		delete m_pCmdCancel;
		m_pCmdCancel = NULL;
	}

}

unsigned int CMsgBox::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;
	
	unsigned iProcID = 0;
	if( iProcID = CTMsgBox::Process(uiMsg,wParam,lParam)) 
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch( iProcID )
			{
			case IID_BTN_OK:
				SendCommand( m_iInvokerDlgID, m_pCmdOk );
				m_pCmdOk = NULL;
				ClearCommand();
				Hide();
				break;
			case IID_BTN_CANCEL:
				SendCommand( m_iInvokerDlgID, m_pCmdCancel );
				m_pCmdCancel = NULL;
				ClearCommand();
				Hide();
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}

void CMsgBox::SetCommand( CTCommand* pCmdOk, CTCommand* pCmdCancel )
{
	ClearCommand();

    m_pCmdOk = pCmdOk;
	m_pCmdCancel = pCmdCancel;
}

void CMsgBox::SetInvokerDlgID( unsigned int iDlgID )
{
	m_iInvokerDlgID = iDlgID;
}

void CMsgBox::SendCommand( unsigned int iDlgID, CTCommand* pCmd )
{
	if( pCmd )
	{
		if( CGame::GetInstance().GetCurrStateID() == CGame::GS_MAIN )
			g_itMGR.AddTCommand( iDlgID, pCmd );
		else
			g_EUILobby.AddTCommand( iDlgID, pCmd );
	}
}

void CMsgBox::SetIcons( std::list<CIcon*>& Icons )
{
	//assert( m_Slots.size() == 0 );
	std::list<CIcon*>::iterator iter;

	int iCount = 0;
	for( iter = Icons.begin(); iter != Icons.end(); ++iter, ++iCount )
	{
		assert( iCount < (int)m_Slots.size() );
		if( iCount < (int)m_Slots.size() )
			m_Slots[iCount].AttachIcon( *iter);
	}
	
	if( !Icons.empty() )
	{
		iter = Icons.begin();
		AddExtraHeight( (*iter)->GetHeight() + 20 );
	}
}
void CMsgBox::Draw()
{
	if( !IsVision() ) return;
	CTMsgBox::Draw();

	std::vector<CSlot>::iterator iter;
	for( iter =	m_Slots.begin(); iter != m_Slots.end(); ++iter )
		iter->Draw();
}

void CMsgBox::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTMsgBox::Update( ptMouse );
	///Update Icons;
	std::vector<CSlot>::iterator iter;
	for( iter =	m_Slots.begin(); iter != m_Slots.end(); ++iter )
		iter->Update( ptMouse );
}
void CMsgBox::MoveWindow( POINT pt )
{
	CTMsgBox::MoveWindow( pt );
	std::vector<CSlot>::iterator iter;
	for( iter =	m_Slots.begin(); iter != m_Slots.end(); ++iter )
		iter->MoveWindow( m_sPosition );
}

void CMsgBox::SetString( const char* szString )
{
	CTMsgBox::SetString( szString );
	std::vector<CSlot>::iterator iter;
	int iCount = 0;
	POINT pt;
	for( iter =	m_Slots.begin(); iter != m_Slots.end(); ++iter , ++iCount)
	{
		pt.y = GetExtraHeightStartPoint() + 20;
		pt.x = 10 + (iter->GetWidth() + 5) * iCount;
		iter->SetOffset( pt );
	}
}
