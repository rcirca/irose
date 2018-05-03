#include "StdAfx.h"
#include ".\tcandidatewindowlist.h"
#include "TControlMgr.h"
#include "ResourceMgr.h"
#include "ITFont.h"

CTCandidateWindowList::CTCandidateWindowList(void)
{
	m_Candidats.Create( 0, 0, 100, 100, 9 ,10, 14 );
	m_Candidats.SetOffset( 0 , 0 );
	m_Candidats.SetHeight( 150 );
	m_Candidats.SetWidth( 100 );
	SetOffset( 0,0 );
	SetHeight( 150 );
	SetWidth( 100 );
	m_iMaxWidth = 0;
}

CTCandidateWindowList::~CTCandidateWindowList(void)
{

}

void CTCandidateWindowList::Draw()
{
	if( IsVision() )
	{
		int iGraphicID = CResourceMgr::GetInstance()->GetImageNID( 0 , "ID_BLACK_PANEL");
		ITDraw* pDraw = CTControlMgr::GetInstance()->GetDrawMgr();
		int iCount = m_Candidats.GetMaximum();

		int iX = m_sPosition.x - 5;
		int iY = m_sPosition.y - 5;
		for( int i = 0 ; i < iCount; ++i )
		{
			pDraw->Draw( iX, iY, m_iMaxWidth + 20, 0, iGraphicID ,D3DCOLOR_ARGB( 128,255,255,255) );
			iY += 17;
		}

		m_Candidats.Draw();
	}
}

unsigned CTCandidateWindowList::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void CTCandidateWindowList::AddCandidate( const char* pszCandidate )
{
	_ASSERT( pszCandidate );
	if( pszCandidate == NULL )
		return;

	SIZE size = CTControlMgr::GetInstance()->GetFontMgr()->GetFontTextExtent( 0, pszCandidate );

	if( size.cx > m_iMaxWidth )
		m_iMaxWidth = size.cx;

	m_Candidats.SetWidth( m_iMaxWidth );
	m_Candidats.AppendText( pszCandidate, D3DCOLOR_ARGB( 255,255,255,255) , false );
	m_Candidats.SetExtent( m_Candidats.GetMaximum() );

	SetWidth( m_iMaxWidth );
	m_Candidats.SetWidth( m_iMaxWidth );

	SetHeight( GetHeight() + 17 );
	m_Candidats.SetHeight( GetHeight() );
}

void CTCandidateWindowList::Clear()
{
	SetHeight( 0 );
	m_Candidats.SetHeight( 0 );
	m_iMaxWidth = 0;
	m_Candidats.ClearText();	
}

void CTCandidateWindowList::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	m_Candidats.MoveWindow( pt );
}

void CTCandidateWindowList::Show()
{
	CWinCtrl::Show();
	m_Candidats.Show();
}

void CTCandidateWindowList::Hide()
{
	CWinCtrl::Hide();
	m_Candidats.Hide();
}

void CTCandidateWindowList::SetSelection( short n )
{
	m_Candidats.SetSelectedItem( n );
}
