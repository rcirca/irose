#include "stdafx.h"
#include ".\cclanmarkuserdefined.h"
#include "..\Game.h"


std::string CClanMarkUserDefined::ClanMarkFileDirectory = "clanmark\\";
std::string CClanMarkUserDefined::NewClanMarkFileName = ".\\Mark.bmp";
SIZE		CClanMarkUserDefined::ClanMarkSize        = { 20, 20 };


CClanMarkUserDefined::CClanMarkUserDefined(void)
{
	m_hNode				= NULL;		
	m_dwLastUsedTime	= 0;
	m_iRefCount			= 0;

	m_bLoaded			= false;
	m_wFileCRC			= 0;
	m_iClanID			= 0;
}

CClanMarkUserDefined::~CClanMarkUserDefined(void)
{
	Free();
}

void CClanMarkUserDefined::Free()
{
	m_strTextureName.clear();
	if( m_hNode )
	{
		unloadTexture( m_hNode );
		m_hNode				= NULL;
	}
	m_dwLastUsedTime	= 0;
}

void CClanMarkUserDefined::Draw( const D3DVECTOR& vDrawPos )
{
	Draw( vDrawPos.x, vDrawPos.y, vDrawPos.z );
}

void CClanMarkUserDefined::Draw( float x, float y, float z )
{
	if( m_hNode )
	{
		static RECT rcDraw = { 0,0,ClanMarkSize.cx, ClanMarkSize.cy };
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, x, y, z );
		::setTransformSprite( mat );
		::drawSprite( m_hNode, 
						&(rcDraw),
						NULL,
						&D3DXVECTOR3( 0, 0, 0 ),								
						D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
	}
	m_dwLastUsedTime = g_GameDATA.GetGameTime();
}

void CClanMarkUserDefined::SetNode( HNODE hNode )
{
	assert( m_hNode == NULL );
	m_hNode = hNode;
}

void CClanMarkUserDefined::SetName( const char* pszName )
{
	assert( m_strTextureName.empty() );
	assert( pszName );
	if( pszName )
		m_strTextureName = pszName;
}

const std::string& CClanMarkUserDefined::GetName()
{
	return m_strTextureName;
}

DWORD CClanMarkUserDefined::GetLastUsedTime()
{
	return m_dwLastUsedTime;
}
void	CClanMarkUserDefined::AddRef()
{
	m_iRefCount++;
}

void	CClanMarkUserDefined::Release()
{
	m_iRefCount--;
	assert( m_iRefCount >= 0 );
	if( m_iRefCount < 0 )
		m_iRefCount = 0;
}

int		CClanMarkUserDefined::GetRefCount()
{
	return m_iRefCount;
}

void CClanMarkUserDefined::GetClanMarkFileName( int server_id, int clan_id , std::string& filename )
{
	filename = CStr::Printf( "%s%d_%d.bmp",ClanMarkFileDirectory.c_str(), server_id, clan_id );
}