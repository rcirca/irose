#include "stdafx.h"
#include ".\tutorialeventutility.h"
#include "../System/SystemProcScript.h"
#include "../Object.h"
#include "../Interface/it_MGR.h"

CTutorialEventUtility _TutorialEventUtility;

CTutorialEventUtility::CTutorialEventUtility(void)
{
}

CTutorialEventUtility::~CTutorialEventUtility(void)
{
}

bool CTutorialEventUtility::Init()
{
	/// Load notify button table
	if( m_TblNotifyButtonEvent.Load ( "3DDATA\\STB\\EventButton.STB",		true, true ) == false )
		return false;

	return true;
}

void CTutorialEventUtility::Release()
{
	m_TblNotifyButtonEvent.Free();
}

/// 알림 버튼 이벤트를 생성한다.
bool CTutorialEventUtility::CreateNotifyEventButton( int iEventIdx )
{
	if( iEventIdx <= 0 )
		return false;

	if( iEventIdx >= m_TblNotifyButtonEvent.m_nDataCnt )
		return false;

	g_itMGR.AddNotifybutton( iEventIdx );


	return true;
}

//-------------------------------------------------------------------------------------------
/// EventButton.STB에 설정된 알림 버튼 이벤트를 실행한다.
//-------------------------------------------------------------------------------------------
bool CTutorialEventUtility::ExecNotifyEvent( int iEventIdx )
{
	if( iEventIdx <= 0 )
		return false;

	if( iEventIdx >= m_TblNotifyButtonEvent.m_nDataCnt )
		return false;

	CSystemProcScript::GetSingleton().CallLuaFunction( m_TblNotifyButtonEvent.m_ppDESC[ iEventIdx ],
														ZZ_PARAM_INT, g_pAVATAR->Get_INDEX(), ZZ_PARAM_END );

	return true;
}