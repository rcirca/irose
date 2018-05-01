#include "stdafx.h"
#include ".\cclanmarkview.h"
#include "../CObjChar.h"
#include "CClanMarkUserDefined.h"
#include "../Game.h"
#include "CTDrawImpl.h"
#include "IO_ImageRes.h"
#include "ClanMarkManager.h"
#include "../System/CGame.h"

/// display namebox class
#define NAMEBOX_WIDTH	( 256 )
#define NAMEBOX_HEIGHT	48



CClanMarkView::CClanMarkView(void)
{

}

CClanMarkView::~CClanMarkView(void)
{

}

void CClanMarkView::Draw( CObjCHAR* pObjChar /*, const POINT& ptDraw*/ , const D3DVECTOR&  vDrawPos )
{
	assert( pObjChar );

	if( pObjChar->GetClanID() )
	{
		if( pObjChar->GetClanMarkBack() != 0 )
		{
			const int iMarkWidth = 20;		

			g_DrawImpl.Draw( vDrawPos.x, vDrawPos.y, vDrawPos.z, IMAGE_RES_CLANBACK,   pObjChar->GetClanMarkBack() );
			g_DrawImpl.Draw( vDrawPos.x, vDrawPos.y, vDrawPos.z, IMAGE_RES_CLANCENTER,   pObjChar->GetClanMarkCenter() );
		}
		else///유저등록 이미지 사용시
		{
			CClanMarkUserDefined* pUserDefinedClanMark = pObjChar->GetUserDefinedClanMark();
			
			if( pUserDefinedClanMark == NULL) 
			{
				/// 클랜마크 정보를 가지고 있지 않다면 관리자에게 요청을 한다.
				std::string file_name;
				CClanMarkUserDefined::GetClanMarkFileName( CGame::GetInstance().GetSelectedServerID(), pObjChar->GetClanID(), file_name );
				pUserDefinedClanMark = CClanMarkManager::GetSingleton().GetClanMark( file_name.c_str(), pObjChar->GetClanMarkCenter(), pObjChar->GetClanID());
				assert( pUserDefinedClanMark );
				pObjChar->SetUserDefinedClanMark( pUserDefinedClanMark );
			}			
			
			if( pUserDefinedClanMark )
				pUserDefinedClanMark->Draw( vDrawPos );
		}
		
		///drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 25, 5, 
		///			GetClanNameColor( pObjChar->GetClanLevel()), pszClanName );
	}
}