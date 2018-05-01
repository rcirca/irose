#include "stdafx.h"
#include "CNameBox.h"
#include "..\\Game.h"
#include "CTDrawImpl.h"
#include "IO_ImageRes.h"
#include "../../TGameCtrl/ResourceMgr.h"
#include "../Object.h"
#include "../JCommandState.h"
#include "../CClientStorage.h"
#include "../GameData/CParty.h"
#include "CClanMarkUserDefined.h"
#include "ClanMarkManager.h"
#include "CClanMarkView.h"




///이름 표시에 사용되는 칼라
const D3DCOLOR	g_dwVioletName		= D3DCOLOR_ARGB( 255, 224, 149, 255 );
const D3DCOLOR	g_dwPinkName		= D3DCOLOR_ARGB( 255, 255, 136, 200 );
const D3DCOLOR	g_dwRedName			= D3DCOLOR_ARGB( 255, 255, 113, 107 );
const D3DCOLOR	g_dwOrangeName		= D3DCOLOR_ARGB( 255, 255, 166, 107 );
const D3DCOLOR	g_dwYellowName  	= D3DCOLOR_ARGB( 255, 255, 228, 122 );
const D3DCOLOR	g_dwGreenName		= D3DCOLOR_ARGB( 255, 150, 255, 122 );
const D3DCOLOR	g_dwBlueName		= D3DCOLOR_ARGB( 255, 137, 243, 255 );
const D3DCOLOR	g_dwLightBlueName	= D3DCOLOR_ARGB( 255, 202, 243, 255 );
const D3DCOLOR	g_dwGrayName		= D3DCOLOR_ARGB( 255, 217,217,217 );



/// 클랜 레벨에 따른 표시 색상을 구하는 함수
inline DWORD GetClanNameColor( int iClanLevel )
{
	switch( iClanLevel )
	{
	case 1: return g_dwBlueName;
	case 2: return g_dwGreenName;
	case 3: return g_dwYellowName;
	case 4: return g_dwOrangeName;
	case 5: return g_dwRedName;
	case 6: return g_dwPinkName;
	case 7: return g_dwVioletName;
	default:
		break;
	}
	return g_dwGrayName;
}


D3DCOLOR g_dwNameColor[] = {
						g_dwRedName,
						g_dwYellowName,
						g_dwGreenName,
						g_dwBlueName,
						g_dwVioletName,
						g_dwGrayName
};


CNameBox::CNameBox()
{

}

CNameBox::~CNameBox()
{

}


void CNameBox::Draw(float x,float y,float z)
{
	// Transform
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, x - NAMEBOX_WIDTH/2, y - NAMEBOX_HEIGHT, z );

	::setTransformSprite( mat );

	RECT rt = { 2, 2, NAMEBOX_WIDTH, NAMEBOX_HEIGHT/2 };

	/// guild name
	if( m_strGuildName != "" )
		::drawFont( g_GameDATA.m_hFONT[ FONT_SMALL_BOLD ], true, &rt, g_dwYELLOW, DT_LEFT, (char*)m_strGuildName.c_str() );				

	/// name
	SetRect( &rt, 2, NAMEBOX_WIDTH, NAMEBOX_HEIGHT/2, NAMEBOX_HEIGHT/2 );
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_LEFT, (char*)m_strName.c_str() );				
	
}

#include "../IO_Terrain.h"
/// @bug name 가 NULL 일때는 안그리게 했다. 이건?
void CNameBox::Draw( CObjCHAR* pCharOBJ, float x, float y, float z )
{	
	assert( pCharOBJ );
	if( pCharOBJ == NULL )
		return;

	char* pName = pCharOBJ->Get_NAME();

	if( pName == NULL )
		return;

	if( g_GameDATA.m_bFilmingMode )
		return;

	//---------------------------------------------------------------------------------
	/// 투명이나 위장상태일때는 아이디표시를 하지 않는다.
	//---------------------------------------------------------------------------------
	if( pCharOBJ->m_EndurancePack.GetStateFlag() & ( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT ) )
	{
		/// 같은팀이 아니면 완전히 안보이므로 아이디 표시도 하지 않는다.
		if( !(g_pAVATAR->Is_ALLIED( pCharOBJ ) || pCharOBJ->IsA( OBJ_USER )) )
			return;
	}

	bool bTargetObject = ( g_UserInputSystem.GetCurrentTarget() == pCharOBJ->Get_INDEX() )?true:false;

	switch ( pCharOBJ->Get_TYPE() )
	{
	case OBJ_MOB :
		{
			DrawMobName( x, y, z, pCharOBJ ,bTargetObject);
			break;
		}
	case OBJ_NPC :
		{
			if( !g_ClientStorage.IsShowNpcName() || 0 == getVisibility( pCharOBJ->GetZMODEL() ) )
				break;

			DrawNpcName( x, y, z, pCharOBJ, bTargetObject );
			break;
		}
	case OBJ_USER:
		{
			DrawMyName( x, y, z, pCharOBJ, bTargetObject );
			break;
		}
	case OBJ_AVATAR:
		{
			if( !g_ClientStorage.IsShowPcName() )
				break;

			if( ((CObjAVT*)pCharOBJ)->GetSpecialStateFLAG() & FLAG_SUB_HIDE )
				break;

			DrawAvatarName( x, y, z, pCharOBJ, bTargetObject );
			break;
		}
	default:
		{
			break;
		}
	}
}

/// 몹과 나의 레벨차에 의한 몹의 이름표시 색상 구하는 함수
DWORD CNameBox::GetTargetMobNameColor( int iAvatarLv, int iMobLv )
{
	DWORD dwColor = g_dwRED;
	int iDiffLv =  iAvatarLv - iMobLv;

	if( iDiffLv <= -23 )
		dwColor = g_dwVioletName;
	else if( iDiffLv <= -16 )
		dwColor = g_dwPinkName;
	else if( iDiffLv <= -10 )
		dwColor = g_dwRedName;
	else if( iDiffLv <= -4 )
		dwColor = g_dwOrangeName;
	else if( iDiffLv <= 3 )
		dwColor = g_dwYellowName;
	else if( iDiffLv <= 8 )
		dwColor = g_dwGreenName;
	else if( iDiffLv <= 14 )
		dwColor = g_dwBlueName;
	else if( iDiffLv <= 21 )
		dwColor = g_dwLightBlueName;
	else 
		dwColor = g_dwGrayName;

	return dwColor;
}

/// NPC는 나의 퀘스트 보유및 처리 결과에 따른 이모티콘을 추가로 표시한다.
void CNameBox::DrawNpcName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted )
{
	/// 이코티콘 이미지의 사이즈 상수
	const int		emoticon_width	= 72;
	const int		emoticon_height = 62;


	bool	bDrawQuestEmoticon = false;
	int		QuestEmoticonGid   = 0;

	CObjNPC* pNpc = (CObjNPC*)pCharOBJ;
	switch( pNpc->m_nQuestSignal )
	{
	case 1:
		bDrawQuestEmoticon = true;
		QuestEmoticonGid = CResourceMgr::GetInstance()->GetImageNID( 0, "QUEST_EMOTICON_POSSIBLE" );
		break;
	case 2:
		bDrawQuestEmoticon = true;
		QuestEmoticonGid = CResourceMgr::GetInstance()->GetImageNID( 0, "QUEST_EMOTICON_ING" );
		break;
	case 3:
		bDrawQuestEmoticon = true;
		QuestEmoticonGid = CResourceMgr::GetInstance()->GetImageNID( 0, "QUEST_EMOTICON_COMPLETE" );
		break;
	default:
		bDrawQuestEmoticon = false;
		break;
	}


	const char* pName = pCharOBJ->Get_NAME();
	assert( pName );

	DWORD dwColor = g_dwGREEN;	

	std::string strNpcFullName = pName;
	std::string strNpcJob;
	std::string strNpcName;
	
	D3DXMATRIX mat;	
	
	if( bDrawQuestEmoticon )
		g_DrawImpl.Draw( x - emoticon_width, y - emoticon_height, z, 0, QuestEmoticonGid ,D3DCOLOR_ARGB(255, 255, 255, 255));


	int iPos = strNpcFullName.find_first_of(']',0);
	if( iPos != string::npos )
	{
		strNpcJob  = strNpcFullName.substr( 0, iPos + 1);
		if( iPos < strNpcFullName.size() )
			strNpcName = strNpcFullName.substr( iPos + 1, strNpcFullName.size() - iPos - 1);


		D3DXMatrixTranslation( &mat, x - NAMEBOX_WIDTH/2, y - 60, z );
		::setTransformSprite( mat );			
		RECT rc = { 2, 2, NAMEBOX_WIDTH, 20 };

		if( !strNpcJob.empty() )
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rc, D3DCOLOR_ARGB(255, 255, 206, 174), DT_CENTER, strNpcJob.c_str() );

		if( !strNpcName.empty() )
		{
			SetRect(&rc, 2, 20, NAMEBOX_WIDTH, 40);
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rc, D3DCOLOR_ARGB(255, 231, 255, 174), DT_CENTER, strNpcName.c_str() );
		}

		if( bTargeted )
		{
			RECT rcDrawName;
			rcDrawName.left		= rc.left + x - NAMEBOX_WIDTH/ 2;
			rcDrawName.top		= rc.top  + y - 60;
			rcDrawName.right	= rcDrawName.left + NAMEBOX_WIDTH;
			rcDrawName.bottom	= rcDrawName.top + 18;

			DrawTargetMark( pCharOBJ, rcDrawName, z );
		}
	}
	else
	{
		D3DXMatrixTranslation( &mat, x - NAMEBOX_WIDTH/2, y - NAMEBOX_HEIGHT, z );
		::setTransformSprite( mat );			
		RECT rc = { 2, 2, NAMEBOX_WIDTH, NAMEBOX_HEIGHT/2 };
	
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rc, dwColor, DT_CENTER , strNpcFullName.c_str() );

		if( bTargeted )
		{
			RECT rcDrawName;
			rcDrawName.left		= rc.left + x - NAMEBOX_WIDTH/ 2 ;
			rcDrawName.top		= rc.top  + y - NAMEBOX_HEIGHT;
			rcDrawName.right	= rcDrawName.left + NAMEBOX_WIDTH;
			rcDrawName.bottom	= rcDrawName.top + NAMEBOX_HEIGHT / 2;

			DrawTargetMark( pCharOBJ,rcDrawName, z);
		}
	}
}

void CNameBox::DrawMobName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted   )
{
	DWORD dwColor = GetTargetMobNameColor( g_pAVATAR->Get_LEVEL(), pCharOBJ->Get_LEVEL() );

	/// 소환몹일경우에는 무조건 파란색
	if( pCharOBJ->m_EndurancePack.GetStateFlag() & FLAG_ING_DEC_LIFE_TIME )
		dwColor = g_dwBlueName;


	const char* pName = pCharOBJ->Get_NAME();
	int iWidthBackImage = 100;
	int iWidthGuage		= 100;

	if( bTargeted )
	{
		g_DrawImpl.Draw( x - iWidthBackImage / 2, y - NAMEBOX_HEIGHT / 2 + 4, z, IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_BACKGROUND" ));
		
		int iHP = pCharOBJ->Get_HP();

		if ( iHP < 0 ) 
			iHP = 0;

		int iMaxHP =  pCharOBJ->Get_MaxHP();
		int iWidth = 0;
		if( iMaxHP > 0 )
			iWidth = iWidthGuage * iHP  / iMaxHP;
			
		
		g_DrawImpl.Draw( x - iWidthGuage / 2 , y - NAMEBOX_HEIGHT / 2 + 4 , z, iWidth, IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_RED" ));


		char* pszMobName = pCharOBJ->Get_NAME();
		if( pszMobName )
		{
			SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], pszMobName );

			RECT rcDrawName;
			rcDrawName.left		= x - size.cx / 2 - 5;
			rcDrawName.top		= y - NAMEBOX_HEIGHT / 2 - 16;
			rcDrawName.right	= x + size.cx / 2 + 5;
			rcDrawName.bottom	= rcDrawName.top + size.cy;

			RECT rc = { iWidthGuage / 2 - size.cx / 2 - 5, -18, iWidthGuage / 2 + size.cx / 2 + 5, 0 };

			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rc, dwColor, DT_CENTER, pszMobName );

			DrawTargetMark( pCharOBJ, rcDrawName, z);
		}

	}
	else
	{

		SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], pCharOBJ->Get_NAME() );

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, x - size.cx / 2, y - NAMEBOX_HEIGHT / 2 + 4, z );
		::setTransformSprite( mat );	

		RECT rc = { 0, 0, size.cx, 18 };
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rc, dwColor, DT_CENTER, pCharOBJ->Get_NAME() );
	}
}

void CNameBox::DrawAvatarName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted )
{
	DWORD dwColor = g_dwWHITE;
	
	const char* pName = pCharOBJ->Get_NAME();
	int pLevel = pCharOBJ->Get_LEVEL();
#if defined(FRAROSE)
	if(pLevel == 255)
		dwColor = 0xFFF7FF00;
	else if(pLevel >= 200)
		dwColor = 0xFFFF8400;
#endif
	if( g_GameDATA.m_iPvPState  )/// PVP 모드라면..
	{
		if( CUserInputState::IsEnemy( (CObjAVT*)pCharOBJ ) )/// 내가 아닌 아바타라면 팀판별..
			dwColor = g_dwRED;
	}

	if( pName && strlen( pName ) > 3 )
	{
		if( pName[ 0 ] == '[' && pName[ 1 ] == 'G' && pName[ 2 ] == 'M' && pName[3 ] == ']' )
		{
			dwColor = g_dwBLUE;
		}

		if( pName[ 0 ] == '[' && pName[ 1 ] == 'T' && pName[ 2 ] == 'M' && pName[3 ] == ']' )
		{
			dwColor = g_dwBLUE;
		}

		if (pName[ 0 ] == 'D' && pName[ 1 ] == 'a' && pName[ 2 ] == 'g' && pName[ 3 ] == 'n' && pName[ 4 ] == 'a' && pName[ 5 ] == 'r' && pName[ 6 ] == 'u' && pName[ 7 ] == 's')
		{
			dwColor = g_dwGREEN;
		}

		if (pName[ 0 ] == 'S' && pName[ 1 ] == 'h' && pName[ 2 ] == 'e' && pName[ 3 ] == 'r' && pName[ 4 ] == 'w' && pName[ 5 ] == 'i' && pName[ 6 ] == 'n')
		{
			dwColor = g_dwBLACK;
		}
#ifdef FRAROSE	
		if( pName[ 0 ] == '[' && pName[ 1 ] == 'D' && pName[ 2 ] == 'E' && pName[ 3 ] == 'V' && pName[ 4 ] == ']' )
		{
			dwColor = g_dwPINK;
		}

		if( pName[ 0 ] == '[' && pName[ 1 ] == 'E' && pName[ 2 ] == 'V' && pName[ 3 ] == 'E' && pName[ 4 ] == 'N' && pName[ 5 ] == 'T' && pName[ 6 ] == ']' )
		{
			dwColor = g_dwGREEN;
		}
		if( pName[ 0 ] == '[' && pName[ 1 ] == 'C' && pName[ 2 ] == 'H' && pName[ 3 ] == 'A' && pName[ 4 ] == 'M' && pName[ 5 ] == 'P' && pName[ 6 ] == 'I' && pName[ 7 ] == 'O' && pName[ 8 ] == 'N' && pName[ 9 ] == ']')
		{
			dwColor = 0xFFBB001C;
		}
#endif
	}




	SIZE size;

	if( g_ClientStorage.IsShowPartyMemberHpGuage() && CParty::GetInstance().IsPartyMember( g_pObjMGR->Get_ServerObjectIndex( pCharOBJ->Get_INDEX() ) ) )
	{
		int iWidthBackImage = 115;
		int iWidthGuage		= 115;
		int iHeightGuage	= 14;

		size.cx = iWidthBackImage;
		size.cy = iHeightGuage;

		float fGuageDrawX = x - iWidthBackImage / 2;
		float fGuageDrawY = y - NAMEBOX_HEIGHT  / 2 + 4;

		g_DrawImpl.Draw( fGuageDrawX, fGuageDrawY, z, 
						IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_BG_AVATAR" ),
						D3DCOLOR_ARGB( 128,255,255,255 )
					);

		int iHP = pCharOBJ->Get_HP();
		int iMaxHP = pCharOBJ->Get_MaxHP();

		if ( iHP < 0 ) 
			iHP = 0;

		int iWidth = 0;
		if( iMaxHP > 0 )
			iWidth = iWidthGuage * iHP / iMaxHP;

		g_DrawImpl.Draw( fGuageDrawX , fGuageDrawY , z, iWidth, IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_RED_AVATAR" ));

		RECT rt = { 0, 0, iWidthGuage, iHeightGuage };
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rt, dwColor, DT_CENTER , pName );
	}
	else
	{
		size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], pName );
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, x - size.cx / 2, y - NAMEBOX_HEIGHT / 2 + 4, z );
		::setTransformSprite( mat );	
		
		RECT rt = { 0, 0, size.cx , size.cy };

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rt, dwColor, DT_CENTER, pName );
	}

	if( bTargeted )
	{
		RECT rcDrawTargetMark;
		rcDrawTargetMark.left	= x - size.cx / 2 - 5;
		rcDrawTargetMark.top	= y - NAMEBOX_HEIGHT / 2 + 4;
		rcDrawTargetMark.right  = rcDrawTargetMark.left + size.cx + 10;
		rcDrawTargetMark.bottom	= rcDrawTargetMark.top	+ size.cy;

		DrawTargetMark( pCharOBJ, rcDrawTargetMark, z );
	}


	if( pCharOBJ->GetClanID() )
	{
		D3DVECTOR vDrawClanMark = GetClanMarkDrawPos( pCharOBJ , x, y, z);
		CClanMarkView::Draw( pCharOBJ, GetClanMarkDrawPos( pCharOBJ, x, y, z ) );

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, vDrawClanMark.x + 25, vDrawClanMark.y+5, z);
		::setTransformSprite( mat );

		drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 0, 0, GetClanNameColor( pCharOBJ->GetClanLevel()), pCharOBJ->GetClanName() );
	}
}

void CNameBox::DrawMyName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted )
{
	DWORD dwColor = g_dwWHITE;
	
	const char* pName = pCharOBJ->Get_NAME();
	int pLevel = pCharOBJ->Get_LEVEL();

	if( pName && strlen( pName ) > 3 )
	{
		if( pName[ 0 ] == '[' && pName[ 1 ] == 'G' && pName[ 2 ] == 'M' && pName[3 ] == ']' )
		{
			dwColor = g_dwBLUE;
		}

		if( pName[ 0 ] == '[' && pName[ 1 ] == 'T' && pName[ 2 ] == 'M' && pName[3 ] == ']' )
		{
			dwColor = g_dwBLUE;
		}

		if (pName[ 0 ] == 'D' && pName[ 1 ] == 'a' && pName[ 2 ] == 'g' && pName[ 3 ] == 'n' && pName[ 4 ] == 'a' && pName[ 5 ] == 'r' && pName[ 6 ] == 'u' && pName[ 7 ] == 's')
		{
			dwColor = g_dwGREEN;
		}

		if (pName[ 0 ] == 'S' && pName[ 1 ] == 'h' && pName[ 2 ] == 'e' && pName[ 3 ] == 'r' && pName[ 4 ] == 'w' && pName[ 5 ] == 'i' && pName[ 6 ] == 'n')
		{
			dwColor = g_dwBLACK;
		}
	}
	int iWidthBackImage = 115;
	int iWidthGuage		= 115;
	int iHeightGuage	= 14;

	float fGuageDrawX = x - iWidthBackImage / 2;
	float fGuageDrawY = y - NAMEBOX_HEIGHT  / 2 + 4;

	g_DrawImpl.Draw( fGuageDrawX, fGuageDrawY, z, 
					IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_BG_AVATAR" ),
					D3DCOLOR_ARGB( 128,255,255,255 )
				);
	
	int iHP = g_pAVATAR->Get_HP();
	if ( iHP < 0 ) 
		iHP = 0;

	int iWidth = iWidthGuage * g_pAVATAR->Get_PercentHP() / 100;

	g_DrawImpl.Draw( fGuageDrawX , fGuageDrawY , z, iWidth, IMAGE_RES_UI, CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_GUAGE_RED_AVATAR" ));

	RECT rt = { 0, 0, iWidthGuage, iHeightGuage };
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, &rt, dwColor, DT_CENTER , pName );

	

	//---------------------------------------------------------------------------------------------
	/// 클랜에 소속되어 있다면..
	if( g_pAVATAR->GetClanID() )
	{
		D3DVECTOR vDrawClanMark = GetClanMarkDrawPos( g_pAVATAR , x, y, z);
		CClanMarkView::Draw( g_pAVATAR, vDrawClanMark );

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, vDrawClanMark.x + 25, vDrawClanMark.y+5, 0.0f);
		::setTransformSprite( mat );

		drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 0, 0, GetClanNameColor( g_pAVATAR->GetClanLevel()), g_pAVATAR->GetClanName() );
	}

	if( bTargeted )
	{
		RECT rcDrawTargetMark = { fGuageDrawX, fGuageDrawY, fGuageDrawX + iWidthGuage, fGuageDrawY + iHeightGuage };
		DrawTargetMark( g_pAVATAR, rcDrawTargetMark, z );
	}
}

void CNameBox::DrawTargetMark( CObjCHAR* pChar, RECT& rcDrawName , float z)
{
	assert( pChar );
	if( pChar == NULL ) return;

	int iTargetMarkImageID  = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"UI00_TARGETMARK" );
	int iTargetMarkWidth    = 16;
	int iTargetMarkHeight   = 15;
	int iModuleID			= IMAGE_RES_UI;

	if( pChar->IsA( OBJ_MOB ) && NPC_TYPE( pChar->Get_CharNO() ) )
	{
		iTargetMarkImageID  = NPC_TYPE( pChar->Get_CharNO() );
		iTargetMarkWidth    = 19;
		iTargetMarkHeight   = 19;
		iModuleID			= IMAGE_RES_TARGETMARK;
	}

	float fTargetMarkDrawY  = (float)( rcDrawName.top + ( rcDrawName.bottom - rcDrawName.top ) / 2  - iTargetMarkHeight/2);

	g_DrawImpl.Draw( (float)( rcDrawName.left - iTargetMarkWidth),
						fTargetMarkDrawY, 
						z, 
						iModuleID, 
						iTargetMarkImageID );

	g_DrawImpl.DrawRotateY( (float)( rcDrawName.right + iTargetMarkWidth),
					fTargetMarkDrawY, 
					(float)z, 
					3.14f,
					iModuleID, 
					iTargetMarkImageID );
}

/// 캐릭터의 위치와 이름으로 클랜마크를 그릴 위치를 구한다.
D3DVECTOR	CNameBox::GetClanMarkDrawPos( CObjCHAR* pChar, float x, float y, float z )
{
	assert( pChar );

	const char* pszClanName = pChar->GetClanName();
	SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], pszClanName );		
	
	D3DVECTOR vDrawPos;

	vDrawPos.x = x - size.cx / 2 - 25;
	vDrawPos.y = y - NAMEBOX_HEIGHT / 2 - 20;
	vDrawPos.z = z;

	return vDrawPos;
}