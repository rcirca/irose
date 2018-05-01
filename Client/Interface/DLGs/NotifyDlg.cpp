#include "stdafx.h"
#include ".\notifydlg.h"

#include "TButton.h"

CNotifyDlg::CNotifyDlg(void)
{
	m_iExtentLine = 4;
}

CNotifyDlg::~CNotifyDlg(void)
{
	m_Script.Clear();
}

bool CNotifyDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		Clear();
		return false;
	}
	return false;
}

void CNotifyDlg::SetScript( const char* pszScript )
{
	assert( pszScript );
	if( pszScript == NULL ) return;

	m_iDrawLine = 0;
	SetEnableChild( IID_BTN_PREV, false );
	SetEnableChild( IID_BTN_NEXT, false );

	m_Script.SetDefaultFont( 0 );
	m_Script.SetDefaultColor( D3DCOLOR_ARGB( 255, 0, 0, 0) );
	m_Script.SetString( pszScript, m_iWidth - 50 );
	if( m_Script.GetStringCount() > m_iExtentLine )
		SetEnableChild( IID_BTN_NEXT, true );

	
}

void CNotifyDlg::Clear()
{
	m_Script.Clear();
	m_iDrawLine = 0;
	SetEnableChild( IID_BTN_PREV, false );
	SetEnableChild( IID_BTN_NEXT, false );
}

void CNotifyDlg::Hide()
{
	CTDialog::Hide();
	Clear();
}

void CNotifyDlg::SetAdjPos()
{
	//20050728 홍근 창 모드 해상도에 따른 버그 : TCtrl을 고쳐야 하는데 일이 커져서 여기만 임시로 고침.
	if( getScreenWidth() < m_sPosition.x + GetWidth())
	{				
		m_sPosition.x = (getScreenWidth() - GetWidth());		
		MoveWindow(m_sPosition );		
	}	
}

void CNotifyDlg::Draw()
{
	if( !IsVision() ) return;

	SetAdjPos();

	CTDialog::Draw( );	

	if( int iStringCount = m_Script.GetStringCount() )
	{

		D3DXMATRIX mat;			
				
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		
		::setTransformSprite( mat );

		int y = 10;
		int iEndLine = ( iStringCount > m_iDrawLine + m_iExtentLine )? m_iDrawLine + m_iExtentLine : iStringCount;
		CJString* pJString = NULL;

		for( int i = m_iDrawLine; i < iEndLine ; ++i )
		{
			if( pJString = m_Script.GetString( i ) )
				pJString->Draw( 10, y, false );

			y += 20;
		}
	}
}

unsigned CNotifyDlg::Process( unsigned uiMsg, WPARAM wParam ,LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch( uiProcID )
			{
			case IID_BTN_CLOSE:
				Hide();
				break;
			case IID_BTN_PREV:
				SetEnableChild( IID_BTN_NEXT, true );
				m_iDrawLine -= m_iExtentLine;
				if( m_iDrawLine <= 0 )
					SetEnableChild( IID_BTN_PREV, false );
				break;
			case IID_BTN_NEXT:
				SetEnableChild( IID_BTN_PREV, true );
				m_iDrawLine += m_iExtentLine;
				if( m_iDrawLine + m_iExtentLine >= m_Script.GetStringCount() )
					SetEnableChild( IID_BTN_NEXT, false );
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}
void CNotifyDlg::Show()
{
	if( CWinCtrl* pCtrl = Find( IID_BTN_NEXT ) )
	{
		if(pCtrl->IsEnable() )
		{
			CTButton* pBtn = (CTButton*)pCtrl;
			pBtn->SetBlink( 1, 500 );
		}
	}
	CTDialog::Show();
}