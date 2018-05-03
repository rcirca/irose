#include "stdafx.h"
#include ".\cmakestatewait.h"
#include "../MakeDLG.h"
CMakeStateWait::CMakeStateWait( CMakeDLG* pParent)
{ 
	m_pParent = pParent; 
}
CMakeStateWait::~CMakeStateWait(void)
{
}

void CMakeStateWait::Draw()
{
	/// Guage ±×¸®±â
}

void CMakeStateWait::Init()
{
}

void CMakeStateWait::Update( POINT ptMouse )
{

}

void CMakeStateWait::Show()
{
	///Ignore
}

void CMakeStateWait::Hide()
{
	///Ignore
}

unsigned int CMakeStateWait::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	///Ignore
	return 0;
}

bool CMakeStateWait::IsModal()
{
	return true;
}