#include "stdafx.h"
#include ".\cicon.h"
#include "../CTDrawImpl.h"
#include "../it_mgr.h"
#include "../CToolTipMgr.h"
#include "../dlgs/subclass/CSlot.h"

#include "../../Common/IO_STB.h"
#include "../../Game.h"

#include "tgamectrl/tdialog.h"
#include "../../System/CGame.h"
CIcon::CIcon(void)
{
	ZeroMemory( &m_ptPosition, sizeof( m_ptPosition ) );
	m_bEnable			= true;
	//m_pParentDlg		= NULL;
	m_pCommand			= NULL;
	m_iWidth			= 40;
	m_iHeight			= 40;
	m_pSlot				= NULL;
	m_strString			= "CIcon";
	m_dwIconDrawColor	= D3DCOLOR_ARGB( 255,255,255,255 );
}

CIcon::~CIcon(void)
{
	m_pSlot = NULL;
}
void CIcon::SetDrawColor( DWORD color )
{
	m_dwIconDrawColor = color;
}

void CIcon::Draw()
{
	g_DrawImpl.Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, m_iModuleID, m_iIconGraphicID, m_dwIconDrawColor );
}

bool CIcon::IsInside( POINT pt )
{
	RECT rcThis = { m_ptPosition.x, m_ptPosition.y , m_ptPosition.x + m_iWidth, m_ptPosition.y + m_iHeight };
	return PtInRect( &rcThis, pt )? true:false; 
}

void CIcon::SetSlot( CSlot* pSlot )
{
	m_pSlot = pSlot;
}
void CIcon::SetCommand( CTCommand* pCommand )
{
	m_pCommand = pCommand;
}

void CIcon::ExecuteCommand()
{

}

CIcon* CIcon::Clone()
{
	return NULL;
}

void CIcon::Clear()
{
}
bool CIcon::IsEnable()
{
	return m_bEnable;
}
void CIcon::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	ToolTip.AddString( "CIcon" );
}

const char*	CIcon::GetName()
{
	return m_strString.c_str();
}

unsigned CIcon::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void CIcon::Update( POINT ptMouse )
{
	if( IsEnable() )
		m_dwIconDrawColor = D3DCOLOR_ARGB( 255,255,255,255 );
	else
		m_dwIconDrawColor = D3DCOLOR_ARGB( 255,128,128,128 );
}

CSlot*  CIcon::GetSlot()
{
	return m_pSlot;
}

/**
* @Warning Icon마다 리턴되는 값이 틀리다 
*		1. CIconStatus  - return 0
*		2. CIconDialog  - return DialogType;
*		3. CIconItem	- return Inventory Index
*		4. CIconSkill	- return SkillSlotIndex
*		5. CIconQuick	- return QuickSlotIndex;
*/
int CIcon::GetIndex()
{
	return 0;
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
CIconStatus::CIconStatus()
{
	m_iWidth			= 20;
	m_iHeight			= 20;
}

CIconStatus::CIconStatus( int iModuleID ,int iGraphicID )
{ 
	CIconStatus();
	SetGraphicID( iModuleID, iGraphicID );
}

void CIconStatus::SetGraphicID( int iModuleID, int iGraphicID )
{
	m_iModuleID  = iModuleID;
	m_iIconGraphicID = iGraphicID;
}

void CIconStatus::Draw()
{
	g_DrawImpl.Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, m_iModuleID, m_iIconGraphicID );
	POINT ptMouse;
	RECT  rcIcon = { m_ptPosition.x, m_ptPosition.y, m_ptPosition.x + m_iWidth, m_ptPosition.y + m_iHeight };
	CGame::GetInstance().Get_MousePos( ptMouse );
	if( !m_ToolTip.IsEmpty() && PtInRect( &rcIcon, ptMouse))
	{
		POINT ptDrawToolTip = { m_ptPosition.x, m_ptPosition.y - m_ToolTip.GetHeight() };
		m_ToolTip.SetPosition( ptDrawToolTip );
		CToolTipMgr::GetInstance().RegistInfo( m_ToolTip );
	}
}

int CIconStatus::GetIndex()
{
	return 0;
}