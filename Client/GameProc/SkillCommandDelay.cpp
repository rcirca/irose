#include "stdafx.h"
#include ".\skillcommanddelay.h"

#include "..\Game.h"


/// for singleton
CSkillCommandDelay _skillCommandDelay;


const int ANI_FRAME = 8; /// 0 ~ 8 ±îÁö 9 Àå

CSkillCommandDelay::CSkillCommandDelay(void) : m_dwCastingTimeDelay( 1000 )
{
	m_dwCastingStartTime	= g_GameDATA.GetGameTime();
	m_bCanCastSkill			= true;

	m_iProgressRatio		= 100;


	m_hCoverTexture			= NULL;
	m_iAnimationFrame		= 0;
}

CSkillCommandDelay::~CSkillCommandDelay(void)
{
}

bool CSkillCommandDelay::Init()
{
	if( m_hCoverTexture )
	{
		::unloadTexture( m_hCoverTexture );
		m_hCoverTexture = NULL;
	}

	m_hCoverTexture = ::loadTexture( "3DData\\Control\\Res\\skilldelay.dds", "3DData\\Control\\Res\\skilldelay.dds", 1, 0 ); 

	return true;
}

void CSkillCommandDelay::Release()
{
	if( m_hCoverTexture )
	{
		::unloadTexture( m_hCoverTexture );
		m_hCoverTexture = NULL;
	}
}

void CSkillCommandDelay::StartSkill()
{
	m_dwCastingStartTime = g_GameDATA.GetGameTime();
	m_iAnimationFrame = 0;
}

bool CSkillCommandDelay::CanCastSkill()
{
	return m_bCanCastSkill;	
}

void CSkillCommandDelay::Proc()
{
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	DWORD dwElapsedTime = ( dwCurrentTime - m_dwCastingStartTime );

	if( dwElapsedTime > m_dwCastingTimeDelay )
	{
		m_iProgressRatio = 100;
		m_bCanCastSkill = true;
	}else
	{
		m_iProgressRatio = ( dwElapsedTime * 100 ) / m_dwCastingTimeDelay;
		m_bCanCastSkill = false;
	}
}

void CSkillCommandDelay::Draw( int iX, int iY )
{
	if( m_bCanCastSkill )
		return;

	if( m_hCoverTexture == 0 )
	{
		return;
	}

	m_iAnimationFrame = ( ANI_FRAME * m_iProgressRatio ) / 100;

	int x = ( m_iAnimationFrame % 3 ) * 40;
	int y = ( m_iAnimationFrame / 3 ) * 40;

	RECT rt = { x, y, x + 40, y + 40 };

	////// Transform
	D3DXMATRIX mat;		
	D3DXMatrixTranslation( &mat, iX, iY , 0.0f );
	::setTransformSprite( mat );

	::drawSprite( m_hCoverTexture, 
					&rt,
					NULL,
					&D3DXVECTOR3( 0, 0, 0 ),								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}
