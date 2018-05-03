#include "stdafx.h"
#include ".\cdialognpcscriptitem.h"
#include "../../../game.h"
#include "../../CTDrawImpl.h"
#include "../../IO_ImageRes.h"
//#include "../../../Util/Localizing.h"

#include "tgamectrl/resourcemgr.h"
//*-------------------------------------------------------------------------------/
// @param iIndex : 추가시 유의
//			0  - top일경우
//			-1 - bottom일경우
//			*  - 실제 script출력
//*-------------------------------------------------------------------------------/
//const int c_iBorderLeft = 25;
CDialogNpcScriptItem::CDialogNpcScriptItem( int iIndex,char* pszScript ,int iImageID, int iLineHeight, int iWidth, int iBorderWidth )
{
	SetWidth( iWidth );
	m_iModuleID = IMAGE_RES_UI;
	m_iImageID  = iImageID;
	m_iLineHeight  = iLineHeight;
	m_iBorderWidth = iBorderWidth;
	switch( iIndex )
	{
	case 0:
		m_iLineCount = 0;
		SetHeight( iLineHeight );
		break;
	case -1:
		m_iLineCount = 0;
		SetHeight( iLineHeight );
		break;
	default:
		{
			assert( pszScript );
			if( pszScript )
			{
				m_Script.SetSplitType( CJStringParser::SPLIT_WORD );
				m_Script.SetDefaultColor( g_dwBLACK );
				m_Script.SetString( pszScript, m_iWidth - m_iBorderWidth * 2 );

				m_iLineCount = m_Script.GetStringCount();
				if( m_iLineCount < 4 )
					m_iLineCount = 4;

				SetHeight( iLineHeight * m_iLineCount );
			}
			break;
		}
	}
}

CDialogNpcScriptItem::~CDialogNpcScriptItem(void)
{
	m_Script.Clear();
}

void CDialogNpcScriptItem::Draw()
{
	if( int iStringCount = m_Script.GetStringCount() )
	{
		POINT ptDraw = m_sPosition;
		CJString* pJString;
		
		for( int i = 0; i < m_iLineCount; ++i )
		{
			g_DrawImpl.Draw( ptDraw.x, ptDraw.y, m_iModuleID, m_iImageID );
			if( i < iStringCount )
			{
				if( pJString = m_Script.GetString( i ) )
					pJString->Draw( m_iBorderWidth, 0, false );
			}
			ptDraw.y += m_iLineHeight;
		}
	}
	else
	{
		g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iImageID );
	}

	//if( m_iLineCount )
	//{
	//	POINT ptDraw = m_sPosition;
	//	ptDraw.y += 20;
	//	for( int i = 1; i < m_iLineCount; ++i )
	//	{
	//		g_DrawImpl.Draw( ptDraw.x, ptDraw.y, m_iModuleID, m_iImageID );
	//		ptDraw.y += 20;
	//	}
	//}
	//g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iImageID );

	//m_Script.Draw();
}