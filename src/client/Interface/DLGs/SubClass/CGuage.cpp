#include "stdafx.h"
#include ".\cguage.h"
#include "../../CTDrawImpl.h"
#include "../../../Game.h"
CGuage::CGuage( int iGuageImageID, int iBGImageID ,int iModuleID , int iWidth )
{
	m_iModuleID				 = iModuleID;
	m_iBGImageID			 = iBGImageID;
	m_iGuageImageID			 = iGuageImageID;
	m_bAutoIncrement		 = false;
	m_iAutoIncrementValue	 = 0;///초당 증가할 % ( 1 ~ 100 )
	m_iAutoIncrementMaxValue = 0;
	m_iValue				 = 0;
	m_iWidth				 = iWidth;
	m_iHeight				 = 0;

	ZeroMemory(&m_ptPosition, sizeof(m_ptPosition));
	ZeroMemory(&m_ptOffset, sizeof( m_ptOffset ));
	
}

CGuage::~CGuage(void)
{

}

void	CGuage::Draw()
{
	if( m_iBGImageID )
		g_DrawImpl.Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, m_iModuleID , m_iBGImageID);

	int iDrawGuageWidth = m_iWidth * m_iValue / 100;

	if( iDrawGuageWidth > m_iWidth )
		iDrawGuageWidth = m_iWidth;

	g_DrawImpl.Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, iDrawGuageWidth, m_iModuleID, m_iGuageImageID );
	if( !m_strText.empty())
	{
		RECT rcDraw = { m_ptPosition.x, m_ptPosition.y , m_ptPosition.x + m_iWidth, m_ptPosition.y + m_iHeight };	
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ],true, &rcDraw,g_dwWHITE, DT_CENTER, m_strText.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// RETURN VALUE
///  0: NORMAL 
/// -1: DELETE
///  1: UPDATE에서 할일을 끝냈다. 다음처리해라.
int		CGuage::Update( POINT ptMouse, DWORD dwPrevTime, DWORD dwCurrTime )
{
	if( m_bAutoIncrement )
	{
		if( m_iValue >= m_iAutoIncrementMaxValue )
		{
			LogString (LOG_NORMAL, "UPDATE_END:Guage Value:%d %d %d\n", m_iValue, dwCurrTime, dwPrevTime );
			return UPDATE_END;
		}
		DWORD dwUpdateSpace = dwCurrTime - dwPrevTime;
		m_iValue += m_iAutoIncrementValue * dwUpdateSpace / 1000;
		if( m_iValue > m_iAutoIncrementMaxValue )
			m_iValue = m_iAutoIncrementMaxValue;

		LogString (LOG_NORMAL, "UPDATE_NORMAL:Guage Value:%d %d %d\n", m_iValue, dwCurrTime, dwPrevTime );
	}
	return UPDATE_NORMAL;
}

void	CGuage::SetAutoIncrement(int iAutoIncrementValue, int iAutoIncrementMaxValue )
{
	m_bAutoIncrement		 = true;
	m_iAutoIncrementValue	 = iAutoIncrementValue;
	m_iAutoIncrementMaxValue = iAutoIncrementMaxValue;
}

void	CGuage::SetPosition( POINT ptPos )
{
	m_ptPosition = ptPos;
}
void	CGuage::SetOffset( POINT ptOffset )
{
	m_ptOffset = ptOffset;
}

void	CGuage::MoveWindow( POINT pt )
{
	POINT ptNew = { pt.x + m_ptOffset.x, pt.y + m_ptOffset.y };
	SetPosition( ptNew );
}

void	CGuage::SetText( const char* szTxt)
{
	if( szTxt == NULL )
		m_strText.erase( m_strText.begin(), m_strText.end() );

	m_strText = szTxt;
}
const   char* CGuage::GetText()
{
	return m_strText.c_str();
}
void	CGuage::SetValue( int iValue )
{
	m_iValue = iValue;
}
int		CGuage::GetValue()
{
	return m_iValue;
}

void	CGuage::SetWidth( int iWidth )
{
	m_iWidth = iWidth;
}
void	CGuage::SetHeight( int iHeight )
{
	m_iHeight = iHeight;
}
void	CGuage::SetGuageImageID( int iImageID )
{
	m_iGuageImageID = iImageID;
}
void    CGuage::SetAutoIncrementMaxValue( int iAutoIncrementMaxValue )
{
	m_iAutoIncrementMaxValue = iAutoIncrementMaxValue;
}