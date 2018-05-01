#include "stdafx.h"
#include ".\chelpmgr.h"
#include "it_mgr.h"
#include "dlgs/CSystemMsgDlg.h"
#include "interfacetype.h"
#include "../Common/IO_STB.h"
#include "../Object.h"
#include "../GameCommon/StringManager.h"

CHelpMgr	g_HelpMgr;



inline		short GetHelpLowestLimitLv( int i )	{ return g_TblHELP.m_ppVALUE[i][1].GetSHORT(); }
inline		short GetHelpUpperLimitLv( int i )	{ return g_TblHELP.m_ppVALUE[i][2].GetSHORT(); }
inline		short GetHelpFontColor( int i )		{ return g_TblHELP.m_ppVALUE[i][3].GetSHORT(); }

/// STB에서의 시간은 초단위 , 프로그램에서 사용하는것은 ms단위
inline		short GetHelpContinueTime( int i )  { return g_TblHELP.m_ppVALUE[i][4].GetSHORT() * 1000;  }

inline		char* GetHelpMsg( int i )			{ return (char*)CStringManager::GetSingleton().GetTipContent(i); }
inline		char* GetHelpTitle( int i )			{ return (char*)CStringManager::GetSingleton().GetTipHeader(i); }

const	short	MaxFontColor = 8;
const   int	    ChangeTipTimeSpace = 5000;///이번 메세지가 다 보이고 나서 다음메세지가 보일때까지의 시간

inline		D3DCOLOR GetColor( short i )
{ 
	if( i < 0 || i >=  MaxFontColor ) 
		i = 0;
	return g_dwCOLOR[i];
}

CHelpMgr::CHelpMgr(void)
{
	m_iPrevShowHelpID	= 0;
	m_dwSetMsgTimeSpace	= 15000;///15초 간격으로 새로운 메세지를 보낸다.
	m_dwSetMsgTime		= g_GameDATA.GetGameTime();
	m_bShowTip			= true;
}

CHelpMgr::~CHelpMgr(void)
{

}

void CHelpMgr::SetNextHelpMsg()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SYSTEMMSG );
	if( pDlg )
	{
		CSystemMsgDlg* pMsgDlg = (CSystemMsgDlg*) pDlg;

		int iNextShowHelpID = GetNextIndex( m_iPrevShowHelpID );

		if( iNextShowHelpID <= 0 || iNextShowHelpID >= g_TblHELP.m_nDataCnt )
			return;

		char* szTitle = GetHelpTitle( iNextShowHelpID );
		char* szMsg   = GetHelpMsg( iNextShowHelpID );

		if( szTitle == NULL || szMsg == NULL )
		{
			if( m_HelpData.empty() )
				return ;
			
			ITER_MAP_HELP iter  = m_HelpData.begin();
			iNextShowHelpID		= iter->first;

			szTitle = GetHelpTitle( iNextShowHelpID );
			szMsg   = GetHelpMsg( iNextShowHelpID );
			if( szTitle == NULL || szMsg == NULL )
				return;
		}

		HNODE hFont   = g_GameDATA.m_hFONT[ FONT_NORMAL ];
		D3DCOLOR color = GetColor( GetHelpFontColor( iNextShowHelpID));

		//if( strcmp( szTitle, "[공지]" ) == 0 )
		//{
		//	hFont = g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ];
		//	color = g_dwWHITE;
		//}

		if( pMsgDlg->SetMessage(  szTitle, szMsg,	CSystemMsgDlg::MSG_TYPE_HELP , color , GetHelpContinueTime( iNextShowHelpID ) , hFont ))
		{
			m_iPrevShowHelpID = iNextShowHelpID;
			///다음 Tip을 보여주기까지의 시간
			m_dwSetMsgTimeSpace = GetHelpContinueTime( iNextShowHelpID ) + ChangeTipTimeSpace;
		}
	}
}

void CHelpMgr::UpdateFrame()
{
	if( !m_bShowTip ) return;
	if( m_HelpData.empty() ) return;

	DWORD dwNowTime = g_GameDATA.GetGameTime();
	if( dwNowTime - m_dwSetMsgTime >= m_dwSetMsgTimeSpace )
	{
		SetNextHelpMsg();
		m_dwSetMsgTime = dwNowTime;
	}
}
int CHelpMgr::GetFirstIndex()
{
	if( m_HelpData.empty() )
		return -1;
	ITER_MAP_HELP iter;
	iter = m_HelpData.begin();
	return iter->first;
}

int CHelpMgr::GetLastIndex()
{
	if( m_HelpData.empty() )
		return -1;
	RITER_MAP_HELP riter;
	riter = m_HelpData.rbegin();
	return riter->first;
}

int CHelpMgr::GetNextIndex( int iPrevIndex )
{
	if( m_HelpData.empty() )
		return -1;


	ITER_MAP_HELP iter;
	iter = m_HelpData.find( iPrevIndex );
	if( iter == m_HelpData.end() )
	{
		iter = m_HelpData.begin();
	}
	else
	{
		iter++;
		if( iter == m_HelpData.end() )
			iter = m_HelpData.begin();
	}

	return iter->first;
}

void CHelpMgr::Update()
{
	if( g_pAVATAR == NULL )
		return;
	
	m_HelpData.clear();

	int iAvatarLv = g_pAVATAR->Get_LEVEL();

	short iLowestLv = 0;
	short iUpperLv  = 0;
	for( int i = 0; i < g_TblHELP.m_nDataCnt; ++i )
	{
		iLowestLv = GetHelpLowestLimitLv(i);
		iUpperLv  = GetHelpUpperLimitLv(i);

		///예약된 row: 무시한다.
		if( iLowestLv <= 0 || iUpperLv <= 0 )
			continue;

		if( iLowestLv <= iAvatarLv &&  iUpperLv >= iAvatarLv )
			m_HelpData.insert( MAP_HELP::value_type( i, make_pair( iLowestLv, iUpperLv )));
	}
}
