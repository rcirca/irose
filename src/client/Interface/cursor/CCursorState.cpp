#include "stdafx.h"
#include ".\ccursorstate.h"
#include "CCursor.h"
CCursorState::CCursorState(void)
{
	m_iState = CCursor::STATE_MAX;
}

CCursorState::~CCursorState(void)
{

}

int CCursorState::GetState()
{
	return m_iState;
}

void CCursorState::Enter()
{

}

void CCursorState::Leave()
{

}