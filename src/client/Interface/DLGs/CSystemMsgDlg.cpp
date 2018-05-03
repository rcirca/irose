#include "stdafx.h"
#include "csystemmsgdlg.h"
#include "../../Game.h"
#include "../CHelpMgr.h"
#include "../it_MGR.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../../util/Localizing.h"
#include "tgamectrl/resourcemgr.h"
#include "zz_interface.h"
CSystemMsgDlg::CSystemMsgDlg(void)
{
	m_hFont = g_GameDATA.m_hFONT[ FONT_NORMAL ];
	m_dwShowTime = 0;
}

CSystemMsgDlg::~CSystemMsgDlg(void)
{

}

void CSystemMsgDlg::Draw()
{
	if( !IsVision() ) return;
	if( m_stTitle.empty() || m_stMsg.empty() )
		return;

	int iLineCount = m_Notice.GetLineCount();
	int iPosY = m_sPosition.y + 3;
	for( int i = 0; i < iLineCount; ++i)
	{
		g_DrawImpl.Draw( m_sPosition.x, iPosY, m_iWidth, IMAGE_RES_UI, m_iImageIndex ,D3DCOLOR_ARGB(128,255,255,255) );
		::drawFontf( m_hFont, true, 2, 2, m_Color, "%s", m_Notice.GetString( i ) );
		iPosY += 17;
	}
}

void CSystemMsgDlg::Update(POINT ptMouse)
{
	CTDialog::Update( ptMouse );


	DWORD dwNowTime = g_GameDATA.GetGameTime();

	if( dwNowTime - m_dwMsgSetTime >= m_dwShowTime )
	{
		m_Notice.Clear();
		m_stTitle.clear();
		m_stMsg.clear();
		Hide();
	}
}


unsigned int CSystemMsgDlg::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	return 0;
}

bool CSystemMsgDlg::SetMessage( const char* szTitle, const char* szMsg, short iType, D3DCOLOR color, DWORD dwShowTime, HNODE hFont)
{
	if( szMsg == NULL || szTitle == NULL )
		return false;


	/// 기존 팁이나 헬프일경우 공지사항을 우선순위로 표시한다.
	/// 공지사항일 경우에도 새로운 공지사항이 우선한다.
	if( m_Notice.GetLineCount() && m_iMsgType == MSG_TYPE_NOTICE )
		if( iType != MSG_TYPE_NOTICE )
			return false;


	if( hFont == NULL )
		m_hFont = g_GameDATA.m_hFONT[ FONT_NORMAL ];
	else
		m_hFont = hFont;

	if( color == NULL )
		m_Color = D3DCOLOR_XRGB( 255, 255, 255);
	else
		m_Color = color;

	m_stTitle = szTitle;
	m_stMsg   = szMsg;

	m_dwShowTime = dwShowTime;
	m_iMsgType = iType;

	m_dwMsgSetTime = g_GameDATA.GetGameTime();

	int iBaseInfoDlgWidth   = 252;
	int iMapDlgWidth		= 192;
	int iImageMaxWidth		= 512;
	int iPanelWidth			= 0;

	CTDialog* pDlg  = g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
	if( pDlg )
		iMapDlgWidth = pDlg->GetWidth();

	m_iWidth = g_pCApp->GetWIDTH() - iMapDlgWidth - iBaseInfoDlgWidth;

	if( m_iWidth > iImageMaxWidth )
		m_iWidth = iImageMaxWidth;

	
	m_sPosition.x = iBaseInfoDlgWidth;

#ifdef _NEWUI
	m_sPosition.y = 30;
#else
	m_sPosition.y = 2;
#endif
	
	m_iImageIndex =  CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"ID_BLACK_PANEL");
	if( m_iImageIndex < 0 )
		return false;

	m_Notice.Clear();

	std::string TempString;
	TempString = m_stTitle;
	TempString.append(" ");
	TempString.append( m_stMsg );

	m_Notice.Split( FONT_NORMAL, (char*)TempString.c_str() , m_iWidth ,CLocalizing::GetSingleton().GetCurrentCodePageNO());

	

	Show();
	return true;
}
