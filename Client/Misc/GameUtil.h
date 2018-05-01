#ifndef _GAME_UTIL_
#define _GAME_UTIL_


//----------------------------------------------------------------------------------------------------
/// 게임에 필요한 잡다한 유틸 함수들
/// class CGameUtil
//----------------------------------------------------------------------------------------------------

class CGameUtil
{
public:
	CGameUtil(void);
	~CGameUtil(void);


	static D3DXCOLOR	GetRGBFromString( unsigned int iColor );
	static WORD			GetPrimaryLanguage();
	static WORD			GetSubLanguage();
	static void			ConvertMoney2String( __int64 money, char* buffer, int buffer_size );

};

#endif //_GAME_UTIL_
