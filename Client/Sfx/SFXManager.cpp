#include "stdafx.h"
#include ".\sfxmanager.h"
#include "ISFX.h"
#include <functional>
#include <algorithm>

#include "SFXWideScreen.h"


/// for singleton
CSFXManager __sfxManager;


CSFXManager::CSFXManager(void)
{
}

CSFXManager::~CSFXManager(void)
{
}

void CSFXManager::Clear()
{
	std::list< ISFX* >::iterator iter;
	for( iter = m_SFXList.begin(); iter != m_SFXList.end(); )
	{
		(*iter)->Release();
		delete *iter;
		iter = m_SFXList.erase( iter );
	}
}
//-----------------------------------------------------------------------------------------
/// General purpose..
//-----------------------------------------------------------------------------------------

void CSFXManager::Update()
{
	std::for_each( m_SFXList.begin(), m_SFXList.end(), std::mem_fun< void, ISFX >( &(ISFX::Update ) ) ); 
}

void CSFXManager::Draw()
{
	std::for_each( m_SFXList.begin(), m_SFXList.end(), std::mem_fun< void, ISFX >( &(ISFX::Draw ) ) ); 
}

ISFX* CSFXManager::AddSFX( ISFX* pSFX )
{
	if( pSFX == NULL )
		return NULL;

	m_SFXList.push_back( pSFX );

	return pSFX;
}

bool CSFXManager::DeleteSFX( ISFX* pSFX )
{
	std::list< ISFX* >::iterator result = std::find( m_SFXList.begin(), m_SFXList.end(), pSFX );

	if( result != m_SFXList.end() )
	{
		ISFX* pSFX = *result;
		delete pSFX;

		m_SFXList.erase( result );
	}else
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------
/// find effect
//-----------------------------------------------------------------------------------------

ISFX* CSFXManager::FindSFXWithType( int iSFXType )
{
	std::list< ISFX* >::iterator begin = m_SFXList.begin();

	for(; begin != m_SFXList.end(); ++begin )
	{
		ISFX* pSFX = *begin;

		assert( pSFX != NULL );
		if( pSFX->GetType() == iSFXType )
		{
			return pSFX;
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------------------
/// Specific special effect
//-----------------------------------------------------------------------------------------

bool CSFXManager::StartWideScreenEffect()
{
	CSFXWideScreen*	pSFX = new CSFXWideScreen;

	pSFX->Init();
	pSFX->SetScreenSize(  g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() );
	this->AddSFX( pSFX );

	return true;
}

void CSFXManager::StopWideScreenEffect()
{
	ISFX* pSFX = FindSFXWithType( SFX_WIDE_SCREEN );

	pSFX->Release();

	if( pSFX )
	{
		DeleteSFX( pSFX );
	}
}