#include "stdafx.h"
#include ".\tutorialeventmanager.h"
#include "TutorialEventUtility.h"
#include "TutorialEventManager.h"
#include "../Game.h"
//---------------------------------------------------------------------------------------
///  Tutorial Image관리기능 추가			2005/5/29		nAvy
//---------------------------------------------------------------------------------------

CTutorialEventManager _TutorialEventManager;

CTutorialEventManager::CTutorialEventManager(void)
{
}

CTutorialEventManager::~CTutorialEventManager(void)
{
}

bool CTutorialEventManager::Init()
{	
	if( CTutorialEventUtility::GetSingleton().Init() == false )
		return false;

	/// Init tutorial event
	if( m_TutorialEvent.Init() == false )
		return false;

	/// Init Levelup event
	if( m_LevelUpEvent.Init() == false )
		return false;

	return true;
}

void CTutorialEventManager::Release()
{
	CTutorialEventUtility::GetSingleton().Release();

	m_TutorialEvent.Release();
	m_LevelUpEvent.Release();

	///기존에 draw 중이거나 삭제 대기중인 모든 이미지를 삭제한다.
	if( !m_tutorial_images.empty() )
	{
		stopSpriteSFX();///znzin
		S_TutorialImage item;
		while( !m_tutorial_images.empty() )
		{
			item = m_tutorial_images.front();
			unloadTexture( item.m_hNode );//znzin
			m_tutorial_images.pop_front();
		}
	}

}

void CTutorialEventManager::Proc()
{
	/// Check Tutorial event
	m_TutorialEvent.CheckEvent();
	ProcImage();
}

void CTutorialEventManager::CheckLevelUpEvent( int iLevel )
{
	m_LevelUpEvent.CheckLevelEvent( iLevel );
}
//---------------------------------------------------------------------------------------------------------------------------------
/// 튜토리얼 이미지를 화면에 지정된 위치에 일정시간동안 보여주거나 큐에 저장한다.
/// @param const char* filename	: 이미지 화일네임( path/filename )
/// @param int		x			: 출력할 화면의 x좌표
/// @param int		y			: 출력할 화면의 y좌표
/// @param float	fade_time	: 출력중인 이미지가 화면에서 사라지기 시작하는 시간( max_time 보다는 작아야 한다 )
/// @param float	max_time	: 이미지가 화면에 표시될 총 시간
/// @param int		append_or_renewal	: 기존 출력중인 이미지가 있을경우 삭제하고 보여줄것인가? 출력이 끝난후 보여줄것인가?
//---------------------------------------------------------------------------------------------------------------------------------
void CTutorialEventManager::RegistImage( const char* filename, int x, int y, float fadein_endtime, float fadeout_starttime, float max_time, int append_or_renewal )
{
	assert( filename );
	if( NULL == filename )
		return;
	
	if( append_or_renewal )
	{
		S_TutorialImage newitem;

		newitem.m_drawstart_time	= 0;
		newitem.m_filename			= filename;
		newitem.m_displayscreen_x	= x;
		newitem.m_displayscreen_y	= y;
		newitem.m_fadein_endtime		= fadein_endtime; 
		newitem.m_fadeout_starttime		= fadeout_starttime;
		newitem.m_max_time			= max_time;

		if( LoadImage(  newitem ) )
		{
			ZZ_RECT		SrcRect = { 0, 0, newitem.m_width, newitem.m_height };
			ZZ_VECTOR   Center  = { 0, 0, 0};
			ZZ_VECTOR	Position = { (float)x , (float)y , 0 };		

			m_tutorial_images.push_back( newitem );
		}
	}
	else
	{
		S_TutorialImage item;

		///기존에 draw 중이거나 삭제 대기중인 모든 이미지를 삭제한다.
		if( !m_tutorial_images.empty() )
		{
			stopSpriteSFX();///znzin

			while( !m_tutorial_images.empty() )
			{
				item = m_tutorial_images.front();
				unloadTexture( item.m_hNode );//znzin
				m_tutorial_images.pop_front();
			}
		}

		/// znzin에 Draw 요청후 queue에 저장
		S_TutorialImage newitem;
		newitem.m_drawstart_time = 0;
		newitem.m_filename = filename;
		newitem.m_displayscreen_x = x;
		newitem.m_displayscreen_y = y;
		newitem.m_fadein_endtime  = fadein_endtime;
		newitem.m_fadeout_starttime = fadeout_starttime;
		newitem.m_max_time	= max_time;

		if( LoadImage( newitem ) )
		{
			ZZ_RECT		SrcRect = { 0, 0, newitem.m_width, newitem.m_height };
			ZZ_VECTOR   Center  = { 0, 0, 0};
			ZZ_VECTOR	Position = { (float)x , (float)y , 0 };		
			inputSpriteSFX( newitem.m_hNode, &SrcRect, &Center, &Position, D3DCOLOR_RGBA( 255, 255, 255, 255 ), fadein_endtime,fadeout_starttime, max_time );

			newitem.m_drawstart_time	= g_GameDATA.GetTime();
			m_tutorial_images.push_back( newitem );
		}
	}
}
//-----------------------------------------------------------------------------------------------------
/// @brief Tutorial Image를 Loading하고 queue에 push할 item를 리턴한다.
//-----------------------------------------------------------------------------------------------------
bool CTutorialEventManager::LoadImage( S_TutorialImage& newitem )
{

	setDelayedLoad(0);//znzin : delay loading off
	newitem.m_hNode = loadTexture ( newitem.m_filename.c_str(), newitem.m_filename.c_str(), 1,	0 );
	setDelayedLoad(1);//znzin : delayed loading on

	if( newitem.m_hNode )
	{
		getTextureSize( newitem.m_hNode, newitem.m_width, newitem.m_height );//znzin
		assert( newitem.m_width );
		assert( newitem.m_height );
		return true;
	}

	_RPT1( _CRT_ASSERT,"Tutorial Image loadTexture Error %s", newitem.m_filename.c_str() );
	return false;
}
//-----------------------------------------------------------------------------------------------------
/// @brief Tutorial Image관련 Update Method
///			- queue에 아직 Display되지 않은 Image가 있다면 Load, znzin에 요청
///			- queue의 front에 Display가 끝난 Image가 있다면 unload
//-----------------------------------------------------------------------------------------------------
void CTutorialEventManager::ProcImage()
{
	if( m_tutorial_images.empty() )	
		return;


	DWORD curr_time = g_GameDATA.GetTime();
	S_TutorialImage	front_item;

	front_item = m_tutorial_images.front();

	DWORD dwDrawEndExpectTime = (DWORD)(front_item.m_drawstart_time + front_item.m_max_time * 1000); 
	
	///Show 대기 중인 item이 있을경우
	if( front_item.m_drawstart_time == 0 )
	{
		if( front_item.m_drawstart_time == 0 )
		{
			m_tutorial_images.pop_front();

			ZZ_RECT		SrcRect = { 0, 0, front_item.m_width, front_item.m_height };
			ZZ_VECTOR   Center  = { 0, 0, 0};
			ZZ_VECTOR	Position = { (float)front_item.m_displayscreen_x , (float)front_item.m_displayscreen_y , 0 };
			inputSpriteSFX( front_item.m_hNode, &SrcRect, &Center, &Position, D3DCOLOR_RGBA( 255, 255, 255, 255 ), front_item.m_fadein_endtime,front_item.m_fadeout_starttime, front_item.m_max_time );
			front_item.m_drawstart_time	= g_GameDATA.GetTime();
			m_tutorial_images.push_front( front_item );
		}
	}
	else if( dwDrawEndExpectTime < curr_time )///Draw가 끝난 item이 있을경우
	{
		HNODE hCurrentTextureNode = getSpriteSFXCurrentTexNode();
		if( hCurrentTextureNode )
		{
			if( hCurrentTextureNode != front_item.m_hNode )
			{
				unloadTexture( front_item.m_hNode );//znzin
				m_tutorial_images.pop_front();
			}
		}
		else
		{
			unloadTexture( front_item.m_hNode );//znzin
			m_tutorial_images.pop_front();
		}
	}
}
