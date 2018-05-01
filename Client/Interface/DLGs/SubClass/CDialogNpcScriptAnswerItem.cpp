#include "stdafx.h"
#include ".\cdialognpcscriptansweritem.h"
#include "../../../game.h"
#include "../../CTDrawImpl.h"
#include "../../IO_ImageRes.h"
#include "../../../Util/Localizing.h"


//const int c_iBorderLeft			= 25;
//const int c_iBodyImageHeight	= 25;
CDialogNpcScriptAnswerItem::CDialogNpcScriptAnswerItem(int iIndex,char* pszScript, int iEventID , void (*fpEventHandle)(int iEventID), int iImageID, int iLineHeight, int iWidth )
{
	m_highlight		= false;
	SetWidth( iWidth );

	m_iModuleID		= IMAGE_RES_UI;
	m_iEventID		= iEventID;
	m_fpEventHandle = fpEventHandle;
	m_iIndex		= iIndex;
	m_iLineHeight	= iLineHeight;
	m_iImageID		= iImageID;
	m_iBorderWidth  = 15;
	switch( iIndex )
	{
	case 0:
		SetHeight( iLineHeight );
		break;
	case -1:
		SetHeight( iLineHeight );
		break;
	default:
		{
		/*	assert( pszScript );
			if( pszScript )
			{
				m_iIndent = 15;
				m_SplitScript.Split( FONT_NORMAL, pszScript , m_iWidth - m_iIndent - c_iBorderLeft * 2,CLocalizing::GetSingleton().GetCurrentCodePageNO());
				SetHeight( m_iLineHeight * m_SplitScript.GetLineCount() );
			}
		*/
			assert( pszScript );
			if( pszScript )
			{
				m_Script.SetSplitType( CJStringParser::SPLIT_WORD );
				m_Script.SetDefaultColor( g_dwBLACK );
				m_Script.SetString( CStr::Printf( "%d. %s",iIndex, pszScript), m_iWidth - m_iBorderWidth * 2 - 12);
				SetHeight( iLineHeight * m_Script.GetStringCount() );
			}

			break;
		}
	}
}

CDialogNpcScriptAnswerItem::~CDialogNpcScriptAnswerItem(void)
{

}

void CDialogNpcScriptAnswerItem::Draw()
{
	if( int iLineCount = m_Script.GetStringCount() )
	{
		POINT ptDraw = m_sPosition;
		CJString* pJString;
		DWORD force_text_color = 0;
		if( m_highlight )
			force_text_color = g_dwBLUE;

		for( int i = 0; i < iLineCount; ++i )
		{
			g_DrawImpl.Draw( ptDraw.x, ptDraw.y, m_iModuleID, m_iImageID );
			if( pJString = m_Script.GetString( i ) )
				pJString->Draw( m_iBorderWidth ,8,  false , force_text_color );

			ptDraw.y += m_iLineHeight;
		}
	}
	else
	{
		g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iImageID );
	}
}

unsigned CDialogNpcScriptAnswerItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	POINT ptMouse = { LOWORD( lParam ),HIWORD(lParam ) };
	if( !IsInside( ptMouse.x, ptMouse.y )) return 0;
	if( uiMsg == WM_LBUTTONUP )
	{
		if(m_fpEventHandle) 
			m_fpEventHandle( m_iEventID );
	}
	return 1;
}

void CDialogNpcScriptAnswerItem::Update( POINT ptMouse )
{
	if( m_Script.GetStringCount() )
	{
		if( !IsInside( ptMouse.x, ptMouse.y )) 
		{
			m_highlight   = false;
		}
		else
		{
			m_highlight   = true;
		}
	}
}