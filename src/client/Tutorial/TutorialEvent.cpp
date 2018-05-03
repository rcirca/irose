#include "stdafx.h"
#include ".\tutorialevent.h"
#include "../System/SystemProcScript.h"




CTutorialEvent::CTutorialEvent(void)
{
}

CTutorialEvent::~CTutorialEvent(void)
{
}

bool CTutorialEvent::Init()
{
	/// Init tutorial script
	CSystemProcScript::GetSingleton().CallLuaFunction( "InitTutorialEvent", ZZ_PARAM_END );
	
	return true;
}

void CTutorialEvent::Release()
{
	
}

///
/// Check all event condition.
/// some hard coded part...
///

void CTutorialEvent::CheckEvent()
{
	/// script check
	CSystemProcScript::GetSingleton().CallLuaFunction( "CheckTutorialEvent", ZZ_PARAM_END );
}