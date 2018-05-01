#include "stdafx.h"
#include ".\cpartymember.h"
#include "../IO_ImageRes.h"
#include "../CTDrawImpl.h"
#include "../../Game.h"
#include "../../Object.h"
#include "../../CObjUSER.h"
#include "../../JCommandState.h"
#include "../CEnduranceProperty.h"

#include "ResourceMgr.h"

#include <algorithm>

const int   g_PartyTooFarDistance = 40 * 100;

///전체 박스의 Left-Top에서의 거리
const POINT g_ptOffsetHpGuage = { 7, 22 };

CPartyMember::CPartyMember(void)
{
	m_bSelected			= false;
	m_bLeader			= false;


	m_iGuageIdx			= 0;
	m_iGuageBGIdx		= 0;
	m_stName				= "NULL";

}

CPartyMember::~CPartyMember(void)
{
	m_listStatusIcon.clear();
}

///직업별 인덱스의 값이 Define 된곳 찾기
CPartyMember::CPartyMember(DWORD dwObjectTag, WORD wObjSvrIdx ,const char* pszName )
{
	assert( pszName );
	if( pszName == NULL )
		return;

	m_bSelected			= false;
	m_stName			= pszName;
	m_wObjSvrIdx		= wObjSvrIdx;
	m_dwObjectTag		= dwObjectTag;
	

	m_iGuageIdx			 = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI18_GUAGE_HP");
	m_iGuageBGIdx		 = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI18_GUAGE_HP_BASE");

	m_iWidth	= 220;
	m_iHeight	= 45;

}

void	CPartyMember::Draw()
{
	CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( m_wObjSvrIdx ), false );

	D3DCOLOR color =  D3DCOLOR_RGBA( 255,255,255,255 );
	if( pObjAVT == NULL )///같은 존에 없다면
	{
		color = D3DCOLOR_RGBA( 170,128,128,255 );
	}
	else ///같은 존에 있지만 멀다면
	{
		if(	g_pAVATAR->Get_DISTANCE( pObjAVT ) >= g_PartyTooFarDistance )
			color = D3DCOLOR_RGBA( 170,128,128,255 );
	}


	/// 선택 테두리
	for_each( m_listStatusIcon.begin(), m_listStatusIcon.end(), mem_fun_ref( &CIconStatus::Draw) );

	/// 이름
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_rcThis.left, (float)m_rcThis.top , 0.0f);
	::setTransformSprite( mat );

	if( !m_stName.empty())
	{
		if( m_bSelected )
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 0, 25, g_dwRED, m_stName.c_str() );		
		else
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 0, 25, g_dwWHITE, m_stName.c_str() );		
	}

	if( pObjAVT )
	{
		int iHP = pObjAVT->Get_HP();

		if( iHP < 0 ) 
			iHP = 0;

		short nWidth = 0;
		short nMaxWidth = 119;
		int iMaxHP = pObjAVT->Get_MaxHP();
		if( iMaxHP > 0 )
			nWidth = ( nMaxWidth * iHP) / iMaxHP ;

		if( nWidth > nMaxWidth )
			nWidth = nMaxWidth;

		g_DrawImpl.Draw( m_rcThis.right - nMaxWidth, m_rcThis.top + 25, IMAGE_RES_UI, m_iGuageBGIdx );
		g_DrawImpl.Draw( m_rcThis.right - nMaxWidth, m_rcThis.top + 25, nWidth, IMAGE_RES_UI,  m_iGuageIdx );

#ifdef _DEBUG
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 25, 0, g_dwWHITE, "%d / %d", iHP, iMaxHP );
#endif

	}


}

unsigned int CPartyMember::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };


	if( uiMsg == WM_LBUTTONDOWN && IsInside( ptMouse ) )
	{
		short nClientObjIdx = g_pObjMGR->Get_ClientObjectIndex( m_wObjSvrIdx );
		CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( nClientObjIdx, false );
		if( pObjAVT )
			g_UserInputSystem.SetCurrentTarget( nClientObjIdx );

		return uiMsg;
	}

	return 0;
}

bool CPartyMember::IsInside( POINT pt )
{
	return (PtInRect( &m_rcThis, pt ))?true:false;
}

void CPartyMember::SetPosition( POINT pt )
{
	m_rcThis.left = pt.x;
	m_rcThis.top  = pt.y;
	m_rcThis.right  = m_rcThis.left + m_iWidth;
	m_rcThis.bottom = m_rcThis.top + m_iHeight;

	///일정 간격으로 얼굴사진 아래서부터 왼쪽으로 상태 아이콘의 위치를 재정의해준다.
	ResetStatusIconsPosition();
}


bool CPartyMember::AddStatusIcon( DWORD dwFlag )
{
	std::map< int , int >::iterator iter;
	CIconStatus StatusIcon;
	CInfo	ToolTip;

	iter = CEndurancePack::m_StateFlagTable.find( dwFlag );
	if( iter == CEndurancePack::m_StateFlagTable.end() )
		return false;

	for( int i = 0; i < g_TblSTATE.m_nDataCnt; ++i )
	{
		if( STATE_TYPE( i ) == iter->second )
		{
			ToolTip.Clear();
			ToolTip.AddString( STATE_NAME( i ) );
			StatusIcon.SetToolTip( ToolTip );
			StatusIcon.SetGraphicID( IMAGE_RES_STATE_ICON, STATE_SYMBOL( i ) );
			m_listStatusIcon.push_back( StatusIcon );
			return true;
		}
	}
	return false;
}

void CPartyMember::ResetStatusIconsPosition()
{
	std::list<CIconStatus>::iterator iter;
	
	POINT pt = {  m_rcThis.left , m_rcThis.top };

	for( iter = m_listStatusIcon.begin(); iter != m_listStatusIcon.end(); ++iter )
	{
		iter->SetPosition( pt );
		pt.x += ( iter->GetWidth() + 2 );
	}
}

void CPartyMember::Update( POINT ptMouse )
{
	CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( m_wObjSvrIdx ), false );

	m_listStatusIcon.clear();

	if( pObjAVT )
	{
		DWORD dwStatusFlag = pObjAVT->m_EndurancePack.GetStateFlag();
		DWORD dwCompareFlag = 1;

		for( int i = 1; i < sizeof( DWORD ) * 8; ++i )
		{
			if( dwStatusFlag & dwCompareFlag )
			{
				AddStatusIcon( dwCompareFlag );
			}

			dwCompareFlag <<= 1;
		}
		ResetStatusIconsPosition();
	}
}

int CPartyMember::GetHeight()
{
	return m_iHeight;
}

DWORD CPartyMember::GetObjectTag()
{
	return m_dwObjectTag;
}

WORD CPartyMember::GetObjSvrIdx()
{
	return m_wObjSvrIdx;
}

void CPartyMember::SetObjSvrIdx( WORD wObjSvrIdx )
{
	m_wObjSvrIdx = wObjSvrIdx;
}
