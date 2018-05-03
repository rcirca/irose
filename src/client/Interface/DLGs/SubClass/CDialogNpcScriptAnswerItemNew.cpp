#include "stdafx.h"
#include ".\cdialognpcscriptansweritemnew.h"
#include "../../../Game.h"

CDialogNpcScriptAnswerItemNew::CDialogNpcScriptAnswerItemNew(int iIndex,char* pszScript, int iEventID , void (*fpEventHandle)(int iEventID), int iWidth )
{
	m_highlight		= false;
	SetWidth( iWidth );

	m_iEventID		= iEventID;
	m_fpEventHandle = fpEventHandle;
	m_iIndex		= iIndex;

	assert( pszScript );
	if( pszScript )
	{
		m_Script.SetSplitType( CJStringParser::SPLIT_WORD );
		m_Script.SetDefaultColor( D3DCOLOR_ARGB( 255, 205, 205, 0) );
		m_Script.SetDefaultFont( FONT_OUTLINE_11_BOLD );
		
		int iOnelineWidth = m_Script.SetString( CStr::Printf( "%d. %s",iIndex, pszScript), m_iWidth - 60 );
//		SetWidth( iOnelineWidth );

		
		if( m_Script.GetStringCount() )
			SetHeight( m_Script.GetString(0)->GetStringHeight() * m_Script.GetStringCount() + 15 );
	}
}

CDialogNpcScriptAnswerItemNew::~CDialogNpcScriptAnswerItemNew(void)
{
}

void CDialogNpcScriptAnswerItemNew::Draw()
{
	if( int iLineCount = m_Script.GetStringCount() )
	{
		POINT ptDraw = m_sPosition;
		CJString* pJString;
		DWORD force_text_color = 0;
		if( m_highlight )
			force_text_color = D3DCOLOR_ARGB( 255, 255, 255, 0);

		for( int i = 0; i < iLineCount; ++i )
		{
			if( pJString = m_Script.GetString( i ) )
			{
				pJString->Draw( ptDraw.x , ptDraw.y,  true , force_text_color );
				ptDraw.y += pJString->GetStringHeight() + 5;
			}
		}
	}
}

unsigned CDialogNpcScriptAnswerItemNew::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
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

void CDialogNpcScriptAnswerItemNew::Update( POINT ptMouse )
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
