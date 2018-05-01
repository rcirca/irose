#ifndef _COUNTRY_
#define _COUNTRY_

#include "Util/JSingleton.h"

//---------------------------------------------------------------------------------------
///
/// 각 국가별 정보를 제공..
///
//---------------------------------------------------------------------------------------
class CCountry : public CJSingleton< CCountry >
{
public:
	CCountry(void);
	~CCountry(void);

	bool CheckCountry();
	bool IsUseItemDelayNewVersion();
	bool IsApplyNewVersion();

	DWORD GetCountryCode();

	bool IsTAIWAN();
	bool IsROSE();
	bool IsPhilipplines();
	bool IsJapan();
	bool IsUSA();
	bool IsKorea();
	bool IsEU();

	bool IsCountry( int country );

	enum COUNTRY_TYPE
	{
		COUNTRY_KOR		= 0,
		COUNTRY_IROSE	= 1,
		COUNTRY_JPN		= 2,
		COUNTRY_CHA		= 3,
		COUNTRY_CHA_TW	= 4,
		COUNTRY_PH		= 5,
		COUNTRY_USA		= 6,
		COUNTRY_EU		= 7,
		COUNTRY_MAX,
	};

private:
	/*
	서버 실행시 사용되는 인자는 클라이언트랑 틀리다
	LANGUAGE_KOR = 0,
	LANGUAGE_USA = 1,
	LANGUAGE_JPN = 2,
	LANGUAGE_CHA_TRADITIONAL = 3,
	LANGUAGE_CHA_SIMPLE = 4,
	서버와의 혼란.. 차후에 맞추자.. 서버의 국가코드가 대만이 4번으로 되어있다.. 그러나 모든 스트링 테이블은 3번이..
	*/

	bool		m_bIsThatCountry[ COUNTRY_MAX ];
	DWORD		m_dwCountryCode;


};

bool IsTAIWAN ();
//bool IsIROSE ();

#endif //_COUNTRY_