#include "stdafx.h"
#include "CDayNNightProc.h"
#include "Game.h"
#include "../Common/IO_STB.h"
#include <algorithm>
#include "IO_Effect.h"
#include "../Misc/GameUtil.h"
#include "BGMManager.h"


#define DAYENVIR_TICK	10000

CDayNNightProc g_DayNNightProc;


enum FOG_COLOR
{
	MORNING_FOG_COLOR = 0,
	DAY_FOG_COLOR = 1,
	EVENING_FOG_COLOR = 2,
	NIGHT_FOG_COLOR = 3,
	MAX_FOG_COLOR,
};

D3DXCOLOR s_FogColor[ MAX_FOG_COLOR ] = { D3DXCOLOR( 100, 100, 100, 255 ),
										D3DXCOLOR( 200, 200, 200, 255 ),
										D3DXCOLOR( 100, 100, 100, 255 ),
										D3DXCOLOR( 10, 10, 10, 255 ) };



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 전역공간 함수.. ( 퀘스트등에서 사용하기 위해서 필요 )
//----------------------------------------------------------------------------------------------------

DWORD Get_WorldPassTIME()
{
	return g_DayNNightProc.GetWorldTime();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 전역공간 함수.. ( 퀘스트등에서 사용하기 위해서 필요 )
//----------------------------------------------------------------------------------------------------

short Get_WorldTIME ()
{
	return ( g_DayNNightProc.GetWorldTime() % 8640 );
}











//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 건물의 이펙트들을 밤낮에 따라 보여줬다..사라졌다 시키기 위해서
//----------------------------------------------------------------------------------------------------

void ShowDayNNight( CEffect* pEffect )
{
	if( pEffect)
	{
		///::setVisibilityRecursive( pEffect->GetZNODE(), 1.0f );
		///pEffect->InsertToScene();
		HNODE hNode = pEffect->GetParentNODE();
		if( hNode )
			pEffect->LinkNODE( hNode );
		else
			assert( 0 && "Effect's parent node is null" );
	}
}

void HideDayNNight( CEffect* pEffect )
{
	if( pEffect)
	{
		///::setVisibilityRecursive( pEffect->GetZNODE(), 0.0f );
		///pEffect->RemoveFromScene();
		pEffect->UnlinkNODE();
	}
}





//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Constructor
//----------------------------------------------------------------------------------------------------

CDayNNightProc::CDayNNightProc()
{
	m_fTimeRateValue = 0.0f;

	/// 밤낮 변화 진행을 위한 시간변수들
	m_dwStartTime = 0;
	m_iCurrentState = DN_DAY;
	m_iChangeStep = 0;

	m_dwElapsedTime = 0;

	m_iDAYENVIR_TICK = DAYENVIR_TICK;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Destructor
//----------------------------------------------------------------------------------------------------

CDayNNightProc::~CDayNNightProc()
{
}


void CDayNNightProc::ResetDayEnviTick()
{
	m_iDAYENVIR_TICK = DAYENVIR_TICK;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 하츠력 년
//----------------------------------------------------------------------------------------------------

int CDayNNightProc::GetHeartsYear()
{
	return ( GetWorldTime() / 103680 );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 하츠력 월
//----------------------------------------------------------------------------------------------------

int	CDayNNightProc::GetHeartsMonth()
{
	int iMonth = ( GetWorldTime() - ( GetHeartsYear() * 103680 ) ) / 8640 + 1;

	return iMonth;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 하츠력 일
//----------------------------------------------------------------------------------------------------

int	CDayNNightProc::GetHeartsDay()
{
	int iDay = ( ( GetWorldTime() - ( GetHeartsYear() * 103680 ) - ( 8640 * ( GetHeartsMonth() - 1 ) ) ) ) / 160 + 1;

	return iDay;
}





//----------------------------------------------------------------------------------------------------
/// @param
/// @brief set world time
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::SetWorldTime( int iZoneNO, DWORD dwWorldTime )
{
	m_iZoneNO = iZoneNO;
	m_dwWorldTime = dwWorldTime;

	m_iZoneTimeCycle = ZONE_DAY_CYCLE( m_iZoneNO );

	if( m_iZoneTimeCycle != 0 )
	{		
		m_dwZoneTime = m_dwWorldTime % m_iZoneTimeCycle;
	}
	else
	{
		assert(0 && "ZoneTimeCycle is zero" );
		m_dwZoneTime = 0;
	}
	
	m_dwStartWorldTime = g_GameDATA.GetGameTime();
	m_dwElapsedWorldTime = 0;


	Log_String( LOG_NORMAL, "월드타임[%d]\n", dwWorldTime );
	Log_String( LOG_NORMAL, "존타임[%d]\n", m_dwZoneTime );

	
	GetCurrentTimeState();

	/// 초기 환경 설정..	
	switch( m_iCurrentTimeState )
	{
		case TIME_DN_MORNING:
			{
				m_iCurrentState = DN_DAY;
		
				m_dwAddiditionalTime = ( ( m_dwZoneTime - ZONE_MORNING_TIME( m_iZoneNO ) ) * 10000 );	 
				Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, ZONE_DAY_TIME( m_iZoneNO ) - ZONE_MORNING_TIME( m_iZoneNO ) );
			}			
			break;
		case TIME_DN_DAY:
			{
				m_iCurrentState = DN_DAY;
				m_dwAddiditionalTime = 0;	
				Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, 0 );
			}			
			break;

		case TIME_DN_EVENING:
			{
				m_iCurrentState = DN_NIGHT;
				m_dwAddiditionalTime = ( ( m_dwZoneTime - ZONE_EVENING_TIME( m_iZoneNO ) ) * 10000 );	
				Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, ZONE_NIGHT_TIME( m_iZoneNO ) - ZONE_EVENING_TIME( m_iZoneNO ) );
			}			
			break;
		case TIME_DN_NIGHT:
			{
				m_iCurrentState = DN_NIGHT;
				m_dwAddiditionalTime = 0;	
				Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, 0 );
			}			
			break;
	}	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 시간상태( 상태시간.. ) 을 구한다.
//----------------------------------------------------------------------------------------------------

int CDayNNightProc::GetCurrentTimeState()
{
	if( m_dwZoneTime > ZONE_NIGHT_TIME( m_iZoneNO ) )			/// 밤
	{
		m_iCurrentTimeState = TIME_DN_NIGHT;
	}else if( m_dwZoneTime > ZONE_EVENING_TIME( m_iZoneNO ) )	/// 저녁
	{
		m_iCurrentTimeState = TIME_DN_EVENING;
	}else if( m_dwZoneTime > ZONE_DAY_TIME( m_iZoneNO ) )		/// 낮
	{
		m_iCurrentTimeState = TIME_DN_DAY;
	}else if( m_dwZoneTime > ZONE_MORNING_TIME( m_iZoneNO ) )	/// 아침
	{
		m_iCurrentTimeState = TIME_DN_MORNING;
	}

	return m_iCurrentTimeState;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Check do update time environment?
//----------------------------------------------------------------------------------------------------

bool CDayNNightProc::CheckUpdateEnvironment()
{
	int iPrevState = m_iCurrentTimeState;

	GetCurrentTimeState();

	bool bChangeState = false;
	
	/// 밤에서 낮으로 변화
	if( iPrevState != m_iCurrentTimeState )
	{
		switch( m_iCurrentTimeState )
		{
			case TIME_DN_MORNING:
				{
					m_iCurrentState = DN_DAY;
					bChangeState = true;					
				}
				break;
			case TIME_DN_EVENING:
				{
					m_iCurrentState = DN_NIGHT;
					bChangeState = true;					
				}
				break;
		}			
	}

	if( bChangeState )
	{
		m_dwAddiditionalTime = 0;

		//CBGMManager::GetSingleton().ChangeBGMMusicWithDayAndNight( m_iZoneNO, ( m_iCurrentState == DN_NIGHT ) );

		return true;
	}
	
	return false;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 밤낮변화 래퍼..
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::SetGlobalIllumination( float fBlendRatio )
{
	/// Background color
	D3DXCOLOR BGPreColor;
	D3DXCOLOR BGCurrentColor;

	/// Character color
	D3DXCOLOR CharAPreColor;
	D3DXCOLOR CharACurrentColor;
	D3DXCOLOR CharDPreColor;
	D3DXCOLOR CharDCurrentColor;

	
	/// for color
	D3DXCOLOR FogPreColor;
	D3DXCOLOR FogCurrentColor;

	if( m_iCurrentState == DN_NIGHT )
	{
		::setSkyMaterialBlendRatio( findNode( "__XXX_sky_ma" ), 1.0f - fBlendRatio );

		FogPreColor		= s_FogColor[ DAY_FOG_COLOR ];
		FogCurrentColor = s_FogColor[ NIGHT_FOG_COLOR ];		
	}
	else
	{
		::setSkyMaterialBlendRatio( findNode( "__XXX_sky_ma" ), fBlendRatio );

		FogPreColor		= s_FogColor[ NIGHT_FOG_COLOR ];
		FogCurrentColor = s_FogColor[ DAY_FOG_COLOR ];		
	}

	float fFogR = ( ( FogPreColor.r * ( 1.0f - fBlendRatio ) ) + ( ( FogCurrentColor.r * fBlendRatio ) ) ) / 255.0f;
	float fFogG = ( ( FogPreColor.g * ( 1.0f - fBlendRatio ) ) + ( ( FogCurrentColor.g * fBlendRatio ) ) ) / 255.0f;
	float fFogB = ( ( FogPreColor.b * ( 1.0f - fBlendRatio ) ) + ( ( FogCurrentColor.b * fBlendRatio ) ) ) / 255.0f;

	::setFogColor( fFogR, fFogG, fFogB );

	/// Readability is best property.. !!
	/// 후반부 진행
	if( m_dwElapsedTime > ( m_iChangeStep * m_iDAYENVIR_TICK ) / 2 )
	{
		switch( m_iCurrentState )
		{
			case DN_DAY:
				{
					/// Background color
					BGPreColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_MORNING ) ;
					BGCurrentColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_DAY ) ;

					/// Character color
					CharAPreColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_MORNING ) ;
					CharACurrentColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_DAY ) ;

					CharDPreColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_MORNING ) ;
					CharDCurrentColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_DAY ) ;

					FogPreColor		= s_FogColor[ MORNING_FOG_COLOR ];
					FogCurrentColor = s_FogColor[ DAY_FOG_COLOR ];
				}
				break;
			case DN_NIGHT:
				{
					/// Background color
					BGPreColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_EVENING ) ;
					BGCurrentColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_NIGHT ) ;

					/// Character color
					CharAPreColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_EVENING ) ;
					CharACurrentColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_NIGHT ) ;

					CharDPreColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_EVENING ) ;
					CharDCurrentColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_NIGHT ) ;

					FogPreColor		= s_FogColor[ EVENING_FOG_COLOR ];
					FogCurrentColor = s_FogColor[ NIGHT_FOG_COLOR ];
				}
				break;
		}

		fBlendRatio -= 0.5f;
	}else
	{
		switch( m_iCurrentState )
		{
		case DN_DAY:
			{
				/// Background color
				BGPreColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_NIGHT ) ;
				BGCurrentColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_MORNING ) ;

				/// Character color
				CharAPreColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_NIGHT ) ;
				CharACurrentColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_MORNING ) ;

				CharDPreColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_NIGHT ) ;
				CharDCurrentColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_MORNING ) ;	

				FogPreColor		= s_FogColor[ NIGHT_FOG_COLOR ];
				FogCurrentColor = s_FogColor[ MORNING_FOG_COLOR ];
			}
			break;
		case DN_NIGHT:
			{
				/// Background color
				BGPreColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_DAY ) ;
				BGCurrentColor = GetColorStringFromSTB( BG_AMBIENT, COLOR_EVENING ) ;

				/// Character color
				CharAPreColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_DAY ) ;
				CharACurrentColor = GetColorStringFromSTB( CHAR_AMBIENT, COLOR_EVENING ) ;

				CharDPreColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_DAY ) ;
				CharDCurrentColor = GetColorStringFromSTB( CHAR_DIFFUSE, COLOR_EVENING ) ;	

				FogPreColor		= s_FogColor[ DAY_FOG_COLOR ];
				FogCurrentColor = s_FogColor[ EVENING_FOG_COLOR ];
			}
			break;
		}
	}	

	fBlendRatio *= 2.0f;

	/// 배경
	::setLight( findNode("light_01"), 
				"diffuse", 
				( BGPreColor.r * ( 1.0f - fBlendRatio ) ) + ( ( BGCurrentColor.r * fBlendRatio ) ),
				( BGPreColor.g * ( 1.0f - fBlendRatio ) ) + ( ( BGCurrentColor.g * fBlendRatio ) ),
				( BGPreColor.b * ( 1.0f - fBlendRatio ) ) + ( ( BGCurrentColor.b * fBlendRatio ) ) );

	/// 캐릭터	
	::setLight( findNode("light_02"), 
				"ambient", 
				( CharAPreColor.r * ( 1.0f - fBlendRatio ) ) + ( ( CharACurrentColor.r * fBlendRatio ) ),
				( CharAPreColor.g * ( 1.0f - fBlendRatio ) ) + ( ( CharACurrentColor.g * fBlendRatio ) ),
				( CharAPreColor.b * ( 1.0f - fBlendRatio ) ) + ( ( CharACurrentColor.b * fBlendRatio ) ) );
	/// 캐릭터	
	::setLight( findNode("light_02"), 
				"diffuse", 
				( CharDPreColor.r * ( 1.0f - fBlendRatio ) ) + ( ( CharDCurrentColor.r * fBlendRatio ) ),
				( CharDPreColor.g * ( 1.0f - fBlendRatio ) ) + ( ( CharDCurrentColor.g * fBlendRatio ) ),
				( CharDPreColor.b * ( 1.0f - fBlendRatio ) ) + ( ( CharDCurrentColor.b * fBlendRatio ) ) );	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief STB정보로 부터 RGB값 구함
//----------------------------------------------------------------------------------------------------

D3DXCOLOR CDayNNightProc::GetColorStringFromSTB( int iType, int iState )
{	
	unsigned int iColor = 0;
	switch( iType )
	{
		case BG_AMBIENT:
			iColor = SKY_AMBIENT( m_iCurrentSkyNO, iState );
			break;
		case CHAR_AMBIENT:
			iColor = SKY_CHAR_AMBIENT( m_iCurrentSkyNO, iState );
			break;
		case CHAR_DIFFUSE:
			iColor = SKY_CHAR_DIFFUSE( m_iCurrentSkyNO, iState );
			break;
	}

	return CGameUtil::GetRGBFromString( iColor );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 밤,낮 구별하는 이펙트 관리
//----------------------------------------------------------------------------------------------------

bool CDayNNightProc::PushEffect( CEffect* pEffect )
{
	if( pEffect == NULL )
		return false;

	/// 이미 들어가 있는 이펙트 이다.
	if( std::find( m_EffectList.begin(), m_EffectList.end(), pEffect ) != m_EffectList.end() )
		return false;

	m_EffectList.push_back( pEffect );

	if( IsDay() )
		pEffect->UnlinkNODE();

	return true;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 리스트로 부터 이펙트 제거
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::DeleteEffectFromList( CEffect* pEffect )
{
	m_EffectList.remove( pEffect );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 변화 시작
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::Start( int iCurrentSkyNO, int iCurrentState, int iChangeStep )
{
	m_iChangeStep = iChangeStep;							/// 변화시간( 초 )
	m_fChangeValue = (iChangeStep == 0)?1.0f:1.0f/(float)iChangeStep;				/// 초당 변화량( float )

	m_dwStartTime = g_GameDATA.GetGameTime();
	m_iCurrentState = iCurrentState;	
	m_iCurrentSkyNO = iCurrentSkyNO;
	m_dwElapsedTime = 0;		

	/// 현재 상태가 밤이라면
	if( m_iCurrentState == DN_NIGHT )
	{
		for_each( m_EffectList.begin(), m_EffectList.end(), ShowDayNNight );
	}else
	{	
		for_each( m_EffectList.begin(), m_EffectList.end(), HideDayNNight );
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 변화 진행
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::Proc()
{
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	/// zone time 증가..	
	DWORD dwTempTime = ( dwCurrentTime - m_dwStartWorldTime ) - m_dwElapsedWorldTime;
	if( dwTempTime > m_iDAYENVIR_TICK )			/// 10 초마다 1씩 증가.
	{
		m_dwElapsedWorldTime += ( dwTempTime - dwTempTime%m_iDAYENVIR_TICK );

		m_dwWorldTime += dwTempTime/m_iDAYENVIR_TICK;;
		m_dwZoneTime += dwTempTime/m_iDAYENVIR_TICK;
		if( m_dwZoneTime >= m_iZoneTimeCycle )
			m_dwZoneTime = 0;

		/// 시간에 따른 환경을 갱신해야된다면..
		if( CheckUpdateEnvironment() )
		{
			m_dwAddiditionalTime = 0;

			switch( m_iCurrentState )
			{
				case DN_DAY:
					Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, ZONE_DAY_TIME( m_iZoneNO ) - ZONE_MORNING_TIME( m_iZoneNO ) );
					break;
				case DN_NIGHT:
					Start( ZONE_BG_IMAGE( m_iZoneNO ), m_iCurrentState, ZONE_NIGHT_TIME( m_iZoneNO ) - ZONE_EVENING_TIME( m_iZoneNO ) );
					break;
			}			
		}
	}

	m_dwElapsedTime = ( dwCurrentTime - m_dwStartTime ) + m_dwAddiditionalTime;

	if( m_dwElapsedTime > ( m_iChangeStep * m_iDAYENVIR_TICK ) || m_iChangeStep == 0 )
		m_fBlendFactor = 1.0f;
	else
		m_fBlendFactor = (float)m_dwElapsedTime / (float)( m_iChangeStep * m_iDAYENVIR_TICK );

	SetGlobalIllumination( m_fBlendFactor );	

	///CBGMManager::GetSingleton().Proc();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 이펙트를 강제로 보이게 하거나 감춤
//----------------------------------------------------------------------------------------------------

void CDayNNightProc::ShowAllEffect( bool bShow )
{	
	if( bShow )
	{
		for_each( m_EffectList.begin(), m_EffectList.end(), ShowDayNNight );
	}else
	{	
		for_each( m_EffectList.begin(), m_EffectList.end(), HideDayNNight );
	}
}