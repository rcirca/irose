#include "stdafx.h"
#include ".\cmakestate.h"
#include "../MakeDLG.h"
CMakeState::CMakeState(void)
{
	m_pParent = NULL;
}

CMakeState::~CMakeState(void)
{
}


void CMakeState::Draw()
{
	_ASSERT( 0 && "CMakeState::Draw()" );
	*(int*)0 = 10;
};
void CMakeState::Update( POINT ptMouse)
{
	_ASSERT( 0 && "CMakeState::Update" );
	*(int*)0 = 10;
}
void CMakeState::Show()
{
	_ASSERT( 0 && "CMakeState::Show" );
	*(int*)0 = 10;
}
void CMakeState::Hide()
{
	_ASSERT( 0 && "CMakeState::Hide" );
	*(int*)0 = 10;
}
void CMakeState::Init()
{
	_ASSERT( 0 && "CMakeState::Init" );
	*(int*)0 = 10;
}
void CMakeState::MoveWindow(POINT ptPos )
{
	_ASSERT( 0 && "CMakeState::MoveWindow" );
	*(int*)0 = 10;
}
unsigned int CMakeState::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	_ASSERT( 0 && "CMakeState::Process" );
	*(int*)0 = 10;
	return 0;
}
bool CMakeState::IsModal()
{
	_ASSERT( 0 && "CMakeState::IsModal() is Pure Virtual Function" );
	*(int*)0 = 10;
	return false;
}
