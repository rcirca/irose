#include "stdafx.h"
#include ".\ccursor.h"
#include "CCursorStateDx.h"
#include "CCursorStateWindow.h"
#include "CCursorStateSprite.h"

CCursor::CCursor(void)
{
	m_pCurrState = NULL;
	for( int i = 0 ; i < STATE_MAX; ++i )
		m_pCursorState[i] = NULL;
}

CCursor::~CCursor(void)
{

}

CCursor& CCursor::GetInstance()
{
	static CCursor s_Instance;
	return s_Instance;
}

void CCursor::Initialize()
{
	m_pCursorState[ STATE_DX ]		= new CCursorStateDx;
	m_pCursorState[ STATE_SPRITE ]	= new CCursorStateSprite;
	m_pCursorState[ STATE_WINDOW ]	= new CCursorStateWindow;

	m_pCurrState = m_pCursorState[ STATE_DX ];
}

void CCursor::Finalize()
{
	m_pCurrState = NULL;
	for( int i = 0 ; i < STATE_MAX; ++i )
		SAFE_DELETE( m_pCursorState[i] );
}

void CCursor::Draw( POINT ptMouse )
{
	assert( m_pCurrState );
	if( m_pCurrState )
		m_pCurrState->Draw( ptMouse );
}

void CCursor::ChangeNextState()
{
	assert( m_pCurrState );
	if( m_pCurrState )
	{
		m_pCurrState->Leave();
		
		int iCurrState = m_pCurrState->GetState();
		if( iCurrState >= STATE_MAX - 1 )
			m_pCurrState = m_pCursorState[ 0 ];
		else
			m_pCurrState = m_pCursorState[ iCurrState + 1 ];
		m_pCurrState->Enter();

		RefreshCursor();
	}
}

int CCursor::SetCursorType( int iType, CInfo* pMouseInfo )
{
	if( iType < 0 || iType >= CURSOR_MAX )
		return 0;

	m_MouseInfo.Clear();

	if( pMouseInfo )
		m_MouseInfo = *pMouseInfo;

	m_iType =iType;

	assert( m_pCurrState );
	
	if( m_pCurrState )
		return  m_pCurrState->SetCursorType( iType );

	return 0;
}

CInfo&	CCursor::GetMouseInfo()
{
	return m_MouseInfo;
}

int CCursor::RefreshCursor()
{
	assert( m_pCurrState );
	
	if( m_pCurrState )
		return SetCursorType( m_iType );

	return 0;
}
int CCursor::GetCurrCursorType()
{
	return m_iType;
}

void CCursor::ReloadCursor()
{
	for( int i = 0 ; i < STATE_MAX; ++i )
		m_pCursorState[i]->ReloadCursor();

}