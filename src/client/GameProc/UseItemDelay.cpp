#include "stdafx.h"
#include ".\useitemdelay.h"
#include "../Game.h"

//USEITEM
CUseItemDelay	g_UseItemDelay,
				g_CurUseItemDelayTick,
				g_SoloUseItemDelayTick;

//SKILL
CUseItemDelay	g_UseSkillDelay,
				g_CurSkillDelayTick,
				g_SoloSkillDelayTick;

////----------------------------------------------------------------------------------------------------
///// @param
///// @brief
////----------------------------------------------------------------------------------------------------
//CUseItemDelay::CUseItemDelay(void)
//{
//	for( int i = 0; i < MAX_DELAYTICK_TYPE ; i++ )
//	{
//		m_UseItemDelay[ i ] = 0;
//	}
//}
//
////----------------------------------------------------------------------------------------------------
///// @param
///// @brief
////----------------------------------------------------------------------------------------------------
//
//CUseItemDelay::~CUseItemDelay(void)
//{
//}
//
////----------------------------------------------------------------------------------------------------
///// @param
///// @brief
////----------------------------------------------------------------------------------------------------
//
//void CUseItemDelay::Clear()
//{
//	for( int i = 0; i < MAX_DELAYTICK_TYPE ; i++ )
//	{
//		m_UseItemDelay[ i ] = 0;
//	}
//}
//
////----------------------------------------------------------------------------------------------------
///// @param
///// @brief
////----------------------------------------------------------------------------------------------------
//
//void CUseItemDelay::Proc()
//{
//	for( int i = 0; i < MAX_DELAYTICK_TYPE ; i++ )
//	{
//		m_UseItemDelay[ i ] -= g_GameDATA.GetElapsedFrameTime();
//		if( m_UseItemDelay[ i ] < 0 )
//			m_UseItemDelay[ i ] = 0;
//	}
//}


//생성자 
CUseItemDelay::CUseItemDelay()
{

	m_nCount	= 0;

}

//소멸자 
CUseItemDelay::~CUseItemDelay()
{

	Release();

}

//자원 해제 
void CUseItemDelay::Release(void)
{

	m_nCount	= 0;
	m_UseItemDelay.clear();

}

void CUseItemDelay::SetUseItemDelay(int iUseItemType, float iDelayTime)
{

	m_UseItemDelay[iUseItemType] = iDelayTime;

	m_nCount = m_UseItemDelay.size();

}


float	CUseItemDelay::GetUseItemDelay(int iUseItemType)
{

	USERITEMDELAYITR itr = m_UseItemDelay.find(iUseItemType);

	if(itr == m_UseItemDelay.end())
		return 0;

	return itr->second;

}



void CUseItemDelay::Proc(void)
{

	for(USERITEMDELAYITR itr = m_UseItemDelay.begin();itr != m_UseItemDelay.end();itr++)
	{

		if(itr->second <= 0.0f)
			continue;
		

		itr->second-=(float)g_GameDATA.GetElapsedFrameTime();
		
		if(itr->second < 0.0f)
			itr->second = 0.0f;
	}

}




