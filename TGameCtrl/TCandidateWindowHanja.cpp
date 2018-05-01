#include "StdAfx.h"
#include ".\tcandidatewindowhanja.h"
#include "TControlMgr.h"
#include "ITFont.h"
CTCandidateWindowHanja::CTCandidateWindowHanja(void)
{

}

CTCandidateWindowHanja::~CTCandidateWindowHanja(void)
{

}

void CTCandidateWindowHanja::Draw()
{
	if( !IsVision() ) return;
	ITFont* pFontMgr = CTControlMgr::GetInstance()->GetFontMgr();
	pFontMgr->SetTransformSprite( (float)m_sPosition.x, (float)m_sPosition.y, 0 );

	//int iGraphicID = CResourceMgr::GetInstance()->GetImageNID( 0 , "ID_BLACK_PANEL");
	//ITDraw* pDraw = CTControlMgr::GetInstance()->GetDrawMgr();
	//pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iWidth, 0, iGraphicID ,D3DCOLOR_ARGB( 128,255,255,255) );

	int iDrawX = 0;

	std::vector< std::string >::iterator iter;
	for( iter = m_Candidates.begin(); iter != m_Candidates.end(); ++iter )
	{
		pFontMgr->Draw( 0, true, iDrawX, 0, D3DCOLOR_ARGB( 255,255,255,255 ), iter->c_str() );
		iDrawX += 30;
	}
}

void CTCandidateWindowHanja::AddCandidate( const char* pszCandidate )
{
	_ASSERT( pszCandidate );
	if( pszCandidate == NULL )
		return;

	m_Candidates.push_back( pszCandidate );
}

void CTCandidateWindowHanja::Clear()
{
	m_Candidates.clear();
}
