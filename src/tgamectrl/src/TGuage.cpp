#include "StdAfx.h"
#include ".\tguage.h"
#include "TControlMgr.h"

CTGuage::~CTGuage(void)
{
	m_iModuleID				 = 0;
	m_iBGImageID			 = 0;
	m_iGuageImageID			 = 0;
	m_bSizeFit				= false;
	m_pFontImpl	= NULL;
	m_pDrawImpl = NULL;
}

CTGuage::CTGuage()
{
	m_iModuleID				 = 0;
	m_iBGImageID			 = 0;
	m_iGuageImageID			 = 0;
	m_bSizeFit				= false;
	m_pFontImpl	= NULL;
	m_pDrawImpl = NULL;
}

bool CTGuage::Create( int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID,int iBGraphicID ,int iModuleID )
{
	Init( CTRL_GUAGE, iScrX, iScrY, iWidth, iHeight );
	m_pFontImpl	= CTControlMgr::GetInstance()->GetFontMgr();
	m_pDrawImpl = CTControlMgr::GetInstance()->GetDrawMgr();
	m_iValue				 = 0;
	m_iWidth				 = iWidth;
	
	m_iHeight				 = 15;	

//	m_bAutoIncrement		 = false;
//	m_iAutoIncrementValue	 = 0;///초당 증가할 % ( 1 ~ 100 )
//	m_iAutoIncrementMaxValue = 0;
	m_iModuleID				 = iModuleID;
	m_iBGImageID			 = iBGraphicID;
	m_iGuageImageID			 = iGraphicID;
	return true;
}

void	CTGuage::Draw()
{
	if( !IsVision() ) return;

	if( m_iBGImageID )
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID , m_iBGImageID);

	int iDrawGuageWidth = m_iWidth * m_iValue / 1000;

	if( iDrawGuageWidth > m_iWidth )
		iDrawGuageWidth = m_iWidth;


	if( m_bSizeFit )
	{
		m_pDrawImpl->DrawFitW(	(int)m_sPosition.x, (int)m_sPosition.y,
								m_iModuleID, m_iGuageImageID,
								iDrawGuageWidth,
								D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleWidth > 0 && m_fScaleHeight > 0)
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, m_fScaleWidth, m_fScaleHeight, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleWidth > 0 )
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, m_fScaleWidth, 1, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleHeight > 0)
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, 1, m_fScaleHeight, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, iDrawGuageWidth, m_iModuleID, m_iGuageImageID );	
	}
	
	if( !m_strText.empty())
	{
		RECT rcDraw = { 1, 1, m_iWidth+1 , m_iHeight+1};	
	
		m_pFontImpl->Draw( 0, true, &rcDraw, D3DCOLOR_ARGB( 255, 0, 0, 0), DT_CENTER | DT_VCENTER, m_strText.c_str());
		SetRect( &rcDraw, 0, 0, m_iWidth, m_iHeight);
		m_pFontImpl->Draw( 0, true, &rcDraw, D3DCOLOR_ARGB(255,255,255,255), DT_CENTER | DT_VCENTER, m_strText.c_str());
	}
	
}

void	CTGuage::SetText( const char* szTxt)
{
	if( szTxt == NULL )
		m_strText.erase( m_strText.begin(), m_strText.end() );

	m_strText = szTxt;
}
const   char* CTGuage::GetText()
{
	return m_strText.c_str();
}

void	CTGuage::SetValue( int iValue )
{
	m_iValue = iValue;
}

int		CTGuage::GetValue()
{
	return m_iValue;
}

void	CTGuage::SetGuageImageID( int iImageID )
{
	m_iGuageImageID = iImageID;
}
/*
void    CGuage::SetAutoIncrementMaxValue( int iAutoIncrementMaxValue )
{
	m_iAutoIncrementMaxValue = iAutoIncrementMaxValue;
}
*/
void	CTGuage::SetSizeFit( bool bFit )
{
	m_bSizeFit = bFit;
}
bool	CTGuage::GetSizeFit()
{
	return m_bSizeFit;
}