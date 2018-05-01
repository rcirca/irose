#include "stdafx.h"
#include ".\ciconquick.h"

CIconQuick::CIconQuick( CIcon* pIcon )
{
	assert( pIcon );
	m_pIcon = pIcon;
}

CIconQuick::~CIconQuick(void)
{
	delete m_pIcon;
}

void CIconQuick::Draw()
{
	assert( m_pIcon );
	m_pIcon->Draw();
}

void CIconQuick::Update( POINT ptMouse )
{
	assert( m_pIcon );
	m_pIcon->Update( ptMouse );
}

void CIconQuick::ExecuteCommand()
{
	assert( m_pIcon );
	m_pIcon->ExecuteCommand();
}

CIcon*	CIconQuick::Clone()
{
	assert( m_pIcon );
	CIconQuick* pIcon = new CIconQuick( m_pIcon->Clone()  );
	pIcon->SetQuickBarSlotIndex( m_iQuickBarSlotIndex );
	return pIcon;
}

void	CIconQuick::Clear()
{
	assert( m_pIcon );
	m_pIcon->Clear();
}

void	CIconQuick::SetQuickBarSlotIndex( int iIndex )
{
	m_iQuickBarSlotIndex = iIndex;
}

int	CIconQuick::GetQuickBarSlotIndex()
{
	return m_iQuickBarSlotIndex;
}

void	CIconQuick::SetPosition( POINT pt )
{
	assert( m_pIcon );
	m_pIcon->SetPosition( pt );
}

const	CIcon*	CIconQuick::GetIcon()
{
	return m_pIcon;
}

void	CIconQuick::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	assert( m_pIcon );
	m_pIcon->GetToolTip( ToolTip, dwDialogType, dwType );
}

int CIconQuick::GetIndex()
{
	return m_iQuickBarSlotIndex;
}
