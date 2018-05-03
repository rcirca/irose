#include "StdAfx.h"
#include ".\tdialog.h"
#include "TCaption.h"
#include "TStatusBar.h"
#include "TControlMgr.h"
#include "TCommand.h"
#include "ITSound.h"
#include "TImage.h"
#include "TEditBox.h"
#include <algorithm>
#include <zmouse.h>

//-----------------------------------------------------------------------------------------------------------
///	2003/12/22 최종진
/// 1. CommandQ추가 
///		1) Hide()와 ~CTDailog()에 모든 TCommand삭제
///		2) Update시( IsVision())에 Q에서 TCommand 를 가져와서 처리
///			(1) 1 Frame당 하나의 TCommand만 처리된다.
///		3) !IsVision()에서는 TCommand를 저장만 한다.( ???? ) => Show후 Update에서 처리 ( 
///			(1) 문제 발생시 TCommand처리 루틴을 Update()안에서 맨위로 올려라.
///
///
/// 2003/11/24 최종진
/// 1. 추가 개발 사항및 고려사항
///		1) Dialog In Dialog
///		2) Process Return Value
//-----------------------------------------------------------------------------------------------------------

CTDialog* CTDialog::m_pProcessMouseOverDialog = NULL;

CTDialog::CTDialog(void)
{
	ZeroMemory( &m_ptCaptionClicked, sizeof( POINT ));
	m_pCaption				= NULL;
	m_pStatusBar			= NULL;
	m_pImage				= NULL;

	m_dwDialogType			= 0;
	m_iShowSoundID			= 0;
	m_iHideSoundID			= 0;		

	m_iXPos					= TDXP_LEFT;
	m_iYPos					= TDYP_TOP;
	m_iAdjustXPos			= 0;
	m_iAdjustYPos			= 0; 

	m_bDefaultVisible		= false;
	m_bModal				= false;
	m_iExtent				= 1;
}

CTDialog::~CTDialog(void)
{
	m_listChild.clear();
	CWinCtrl* pCtrl = NULL;
	WINCTRL_LIST_ITOR iter;

	m_OwnerDrawChildren.clear();

	for( iter = m_listChild.begin(); iter != m_listChild.end(); )
	{
		pCtrl = *iter;
		iter = m_listChild.erase( iter );
		delete pCtrl;
	}
	ClearCommandQ();

	if( m_pCaption )
	{
		delete m_pCaption;
		m_pCaption = NULL;
	}

	if( m_pStatusBar )
	{
		delete m_pStatusBar;
		m_pStatusBar = NULL;
	}
	if( m_pImage )
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

bool CTDialog::Create( int iScrX, int iScrY, int iWidth, int iHeight)
{
	Init( CTRL_DIALOG, iScrX, iScrY, iWidth, iHeight );

	return true;
}

bool CTDialog::Create( const char* IDD )
{
	return CTControlMgr::GetInstance()->MakeDialogByXML( IDD, this );
}

void CTDialog::ChangeCaptionRect( RECT rc )
{
	if( m_pCaption )
	{
		POINT ptOffset = { rc.left, rc.top };
		m_pCaption->SetOffset( ptOffset );
		m_pCaption->SetWidth( rc.right - rc.left );
		m_pCaption->SetHeight( rc.bottom - rc.top );
		MoveWindow( GetPosition() );
	}
}

unsigned int CTDialog::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() ) return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	if( m_pStatusBar && m_pStatusBar->Process( uiMsg, wParam, lParam ) )
		return m_pStatusBar->GetControlID();

	WINCTRL_LIST_RITOR riter;
	CWinCtrl* pCtrl = NULL;
	unsigned iID;
	for( riter = m_listChild.rbegin(); riter != m_listChild.rend(); ++riter )
	{
		pCtrl = *riter;
		iID = pCtrl->Process( uiMsg, wParam, lParam );
		if( iID )
		{
			/* WM_LBUTTONDOWN을 처리한 버튼은 항상 리스트의 맨뒤로 보내서 
			   그리기가 제일 뒤에서 하게끔한다. 
			   예) ComboBox의 경우 Ctrl들끼리 Draw의 Layer가 엉망될수 있다.
			*/
			if( uiMsg == WM_LBUTTONDOWN )
			{
				if( pCtrl->GetControlType() != CTRL_TABBEDPANE && pCtrl->GetControlType() != CTRL_PANE )///컨테이너는 기본적으로 위로 올라오면 안되요..
					MoveCtrl2ListEnd( pCtrl->GetControlID() );
			}

			return iID;
		}
	}

	

	if( m_pCaption )
	{
		unsigned iID = m_pCaption->Process( uiMsg, wParam, lParam );
		switch( iID )
		{
		case CTCaption::IID_BTN_ICON:
			///if( uiMsg == WM_LBUTTONUP )
			///아이콘화	
			return m_pCaption->GetControlID();
		case CTCaption::IID_BTN_CLOSE:
			if( uiMsg == WM_LBUTTONUP )
				Hide();
			return m_pCaption->GetControlID();
		default:
			if( m_pCaption->IsClicked() && uiMsg == WM_LBUTTONDOWN )
			{
				m_ptCaptionClicked.x = ptMouse.x - m_sPosition.x;
				m_ptCaptionClicked.y = ptMouse.y - m_sPosition.y;
			}
			break;
		}
	}
	


	if( IsInside( ptMouse.x, ptMouse.y ) && uiMsg != WM_KEYDOWN && uiMsg != WM_CHAR && uiMsg != WM_SYSKEYDOWN )
	{
		if( uiMsg == WM_LBUTTONDOWN )
			ProcessLButtonDown();

		return m_iControlID;
	}


	




	return 0;
}


void CTDialog::Draw()
{
	if( !IsVision() ) 
		return;

	if( m_pCaption )
		m_pCaption->Draw();

	//assert( m_pImage );
	if( m_pImage )
	{
		POINT ptDraw = { m_sPosition.x, m_sPosition.y };
		
		if( m_pCaption )
			ptDraw.y += m_pCaption->GetHeight();

		for( int i = 0; i < m_iExtent; ++i )
		{
			m_pImage->Draw( ptDraw );
			ptDraw.y += m_pImage->GetHeight();
		}
	}

	if( m_pStatusBar )
		m_pStatusBar->Draw();

	WINCTRL_LIST_ITOR iter;
#ifdef _DEBUG
	int iIndex = 0;
#endif
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
#ifdef _DEBUG
		iIndex++;
#endif	
		(*iter)->Draw();
	}

	for( iter = m_OwnerDrawChildren.begin(); iter != m_OwnerDrawChildren.end(); ++iter )
		(*iter)->OwnerDraw();

}

void CTDialog::Update(POINT ptMouse )
{

	if( !IsVision() ) return;

	if( m_pCaption )
	{
		if(m_pCaption->IsClicked())
		{
			POINT ptNew = {0,0 };
			ptNew.x = ptMouse.x - m_ptCaptionClicked.x;
			ptNew.y = ptMouse.y - m_ptCaptionClicked.y;
			MoveWindow( ptNew );
		}

		m_pCaption->Update( ptMouse );

	}

	if( m_pStatusBar )
		m_pStatusBar->Update( ptMouse );

	WINCTRL_LIST_ITOR iter;

	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter)
		(*iter)->Update( ptMouse );


	if( IsInside( ptMouse.x, ptMouse.y ) )
	{
		if( GetProcessMouseOverDialog() == NULL )
			SetProcessMouseOverDialog( this );	

	}

	CTCommand* pCmd = GetTCommand();
	if( pCmd )
	{
		if( pCmd->Exec( this ) )
			delete pCmd;
		else
		{
			if( pCmd->IsWaitExecSucceed() )
				Push_Front_Cmd( pCmd );
			else
				Push_Back_Cmd( pCmd );
		}
	}
}

void CTDialog::SetStatusBar( CTStatusBar* pStatusBar )
{
	m_pStatusBar = pStatusBar;

}

void CTDialog::SetCaption( CTCaption* pCaption )
{
	pCaption->SetParent( this );
	pCaption->SetOffset( pCaption->GetPosition() );
	pCaption->MoveWindow( GetPosition() );

	m_pCaption = pCaption;
}

void CTDialog::Add( CWinCtrl* pChild )
{

	pChild->SetParent( this );
	pChild->SetOffset( pChild->GetPosition() );
	///등록된 Child Control의 좌표를 상대좌표로 계산하여 현재 Dialog의 좌표에 더해준다.
	pChild->MoveWindow( GetPosition() );

	m_listChild.push_back( pChild );
}

bool CTDialog::Remove( int iID )
{
	WINCTRL_LIST_ITOR iter;
	POINT ptOffset  = { 0 , 0 };
	CWinCtrl* pCtrl = NULL;

	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( (int)pCtrl->GetControlID() == iID )
		{
			pCtrl->SetParent( 0 );
			pCtrl->SetOffset( ptOffset );
			m_listChild.erase( iter );
			return true;
		}
	}
	return false;
}

CWinCtrl* CTDialog::Find( int iID )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( (int)pCtrl->GetControlID() == iID )
			return pCtrl;
	}
	return NULL;
}

CWinCtrl*	CTDialog::Find( const char * strName )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( strcmp( pCtrl->GetControlName(), strName) == 0 )
			return pCtrl;

		pCtrl = pCtrl->Find( strName );
		if( pCtrl )
		{
			return pCtrl;
		}
	}
	return NULL;
}


bool	CTDialog::ShowChild( unsigned int iID )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( pCtrl->GetControlID() == iID )
		{
			pCtrl->Show();
			return true;
		}
	}
	return false;
}

bool	CTDialog::ShowChild( const char * strName )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( strcmp( pCtrl->GetControlName(), strName) == 0 )
		{
			pCtrl->Show();
			return true;
		}
	}
	return false;
}

bool	CTDialog::HideChild( unsigned int iID )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( pCtrl->GetControlID() == iID )
		{
			pCtrl->Hide();
			return true;
		}
			
	}
	return false;
}

bool	CTDialog::HideChild( const char * strName )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl* pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		pCtrl = *iter;
		if( strcmp( pCtrl->GetControlName(), strName) == 0 )
		{
			pCtrl->Hide();
			return true;
		}

	}
	return false;
}

bool	CTDialog::SetEnableChild( unsigned int iID, bool bEnable )
{
	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl )
	{
		pCtrl->SetEnable( bEnable );
		return true;
	}
	return false;
}

void CTDialog::Show()
{
	if( m_iShowSoundID > 0 )
		CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iShowSoundID );

	CWinCtrl::Show();
	
	if( m_pCaption )
		m_pCaption->Show();

	///Body를 그리고

	///풋터를 그리고

	WINCTRL_LIST_ITOR iter;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
		(*iter)->Show();

}

void CTDialog::Hide()
{
	if( m_iHideSoundID > 0 )
		CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iHideSoundID );

	CWinCtrl::Hide();
	
	if( m_pCaption )
		m_pCaption->Hide();

	WINCTRL_LIST_ITOR iter;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
		(*iter)->Hide();


	ClearCommandQ();
}

void CTDialog::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );

	if( m_pCaption )
		m_pCaption->MoveWindow( pt );


	WINCTRL_LIST_ITOR iter;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
		(*iter)->MoveWindow( pt );
}

void CTDialog::MoveWindow( int x, int y )
{
	POINT pt = {x, y};
	MoveWindow( pt );
}

long CTDialog::GetChildCount()
{ 
	return (long)m_listChild.size(); 
}


void CTDialog::Push_Back_Cmd( CTCommand* pCmd )
{
	m_CommandQ.Push_Back( pCmd );
}

void CTDialog::Push_Front_Cmd( CTCommand* pCmd )
{
	m_CommandQ.Push_Front( pCmd );
}

void CTDialog::ClearCommandQ()
{
	m_CommandQ.Clear();
}
CTCommand* CTDialog::GetTCommand()
{
	return m_CommandQ.GetCommand();
}


void CTDialog::MoveCtrl2ListEnd( int iID )
{
	WINCTRL_LIST_ITOR iter;
	CWinCtrl*		pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		///REMARK : For Debuging
		pCtrl = *iter;
		if( (int)pCtrl->GetControlID() == iID )
		{
			m_listChild.erase( iter );
			m_listChild.push_back( pCtrl );
			return;
		}
	}
}

void CTDialog::SetModal()
{
	m_bModal = true;
}

void CTDialog::SetModeless()
{
	m_bModal = false;
}

bool CTDialog::IsModal()
{ 
	return m_bModal; 
}


void CTDialog::SetExtent( int i )
{
	m_iExtent = i;
}
void CTDialog::SetImage( CTImage* pImage )
{
	m_pImage = pImage;
}
///Children 중에 첫번째 EditBox에 Focus를 준다.
void CTDialog::ProcessLButtonDown()
{
	if( CTEditBox::s_pFocusEdit && CTEditBox::s_pFocusEdit->GetParent() == (CWinCtrl*)this )
		return;

	WINCTRL_LIST_ITOR iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter )
	{
		///REMARK : For Debuging
		pCtrl = *iter;
		if( pCtrl->GetControlType() == CTRL_EDITBOX )
		{
			pCtrl->SetFocus( true );
			return;
		}
	}

}
void CTDialog::RefreshDlg()
{
}

void CTDialog::SetInterfacePos_After()
{
}