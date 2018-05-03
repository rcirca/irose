#ifndef _DAYNNIGHTPROC_
#define _DAYNNIGHTPROC_

#include <list>


class CEffect;

//----------------------------------------------------------------------------------------------------
/// @brief 현재 밤,낮 상태 타입
//----------------------------------------------------------------------------------------------------

enum enumDAYNNIGHT
{
	DN_DAY = 0,	
	DN_NIGHT,	
	DN_MAX,
};


//----------------------------------------------------------------------------------------------------
/// @brief 현재 시간타입..
//----------------------------------------------------------------------------------------------------

enum enumTIME_DAYNNIGHT
{
	TIME_DN_MORNING = 0,
	TIME_DN_DAY,
	TIME_DN_EVENING,
	TIME_DN_NIGHT,	
	TIME_DN_MAX,
};


//----------------------------------------------------------------------------------------------------
/// @brief 라이트 타입
//----------------------------------------------------------------------------------------------------

enum enumCOLORTYPE
{
	BG_AMBIENT = 0,
	CHAR_AMBIENT,
	CHAR_DIFFUSE,
};


//----------------------------------------------------------------------------------------------------
/// @brief 컬러 타입
//----------------------------------------------------------------------------------------------------

enum enumDAYCOLOR
{
	COLOR_MORNING = 0,
	COLOR_DAY,
	COLOR_EVENING,
	COLOR_NIGHT,
};



//----------------------------------------------------------------------------------------------------
/// @brief Managing day and night change..
//----------------------------------------------------------------------------------------------------

class CDayNNightProc
{
private:
	//----------------------------------------------------------------------------------------------------
	/// Effect list effected by changing day and night
	//----------------------------------------------------------------------------------------------------
	std::list< CEffect* >	m_EffectList;
	int a;


	//----------------------------------------------------------------------------------------------------
	/// 존별 시간관리..
	//----------------------------------------------------------------------------------------------------
	int						m_iZoneNO;
	DWORD					m_dwWorldTime;		/// 0 ~ 2880
	DWORD					m_dwZoneTime;		/// World time / zone time value
	int						m_iZoneTimeCycle;	/// World time per a day
	
	DWORD					m_dwStartWorldTime;
	DWORD					m_dwElapsedWorldTime;			/// 현재까지 진행된 월드 타임
		


	//----------------------------------------------------------------------------------------------------
	/// 현재 시간변화 수치
	//----------------------------------------------------------------------------------------------------
	float					m_fTimeRateValue;


	int						m_iDAYENVIR_TICK;

	//----------------------------------------------------------------------------------------------------
	/// 밤낮 변화 진행을 위한 시간변수들
	//----------------------------------------------------------------------------------------------------
	int						m_iChangeStep;					/// 변화 단계
	float					m_fChangeValue;					/// 실제 단계별 적용값	
	DWORD					m_dwStartTime;					/// 변화 시작 시간
	int						m_iCurrentState;				/// 현재 밤낮의 상태( 시간 별 )
	int						m_iCurrentTimeState;			/// 현재 밤낮의 상태( 상태 적용을 하는 실제.. 밤과 낮만 있음 )
	int						m_iCurrentSkyNO;				/// 현재 sky 인덱스
	DWORD					m_dwElapsedTime;				/// 진행된 시간

	float					m_fBlendFactor;					/// 블랜딩 비율

	DWORD					m_dwAddiditionalTime;			/// 시간 변화 중간에 존에 접속시 기존 흐른시간을 더해준다.

public:
	CDayNNightProc();
	~CDayNNightProc();

	//----------------------------------------------------------------------------------------------------
	/// 하츠력
	//----------------------------------------------------------------------------------------------------
	int						GetHeartsYear();
	int						GetHeartsMonth();
	int						GetHeartsDay();



	DWORD					GetWorldTime()			{ return m_dwWorldTime; }
	DWORD					GetElapsedWorldTime()	{ return m_dwElapsedWorldTime; }
	DWORD					GetZoneTime()			{ return m_dwZoneTime; }
	float					GetBlendFactor()		{ return m_fBlendFactor; }


	void					SetWorldTime( int iZoneNO, DWORD dwWorldTime );
	int						GetCurrentTimeState();
	bool					IsDay(){ if( m_iCurrentState == TIME_DN_MORNING || m_iCurrentState == TIME_DN_DAY ) return true; return false; }

	bool					CheckUpdateEnvironment();


	void					SetGlobalIllumination( float fBlendRatio );
	D3DXCOLOR				GetColorStringFromSTB( int iType, int iState );

	bool					PushEffect( CEffect* pEffect );
	void					DeleteEffectFromList( CEffect* pEffect );

	void					Start( int iCurrentSkyNO, int iCurrentState, int iChangeStep = 20 );
	void					Proc();

	void					ShowAllEffect( bool bShow );


	void					SetDayEnviTick( int iTick ){ m_iDAYENVIR_TICK = iTick; }
	void					ResetDayEnviTick();
};

extern CDayNNightProc		g_DayNNightProc;


extern DWORD Get_WorldPassTIME();
extern short Get_WorldTIME ();

#endif //_DAYNNIGHTPROC_
