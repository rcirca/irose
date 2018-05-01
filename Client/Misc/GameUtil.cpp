#include "stdafx.h"
#include ".\gameutil.h"

CGameUtil::CGameUtil(void)
{
}

CGameUtil::~CGameUtil(void)
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 컬러 포맷 스트링으로부터 RGB값을 구한다.
//----------------------------------------------------------------------------------------------------

D3DXCOLOR CGameUtil::GetRGBFromString( unsigned int iColor )
{
	D3DXCOLOR Color = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	
	int temp = 0;
	int r = iColor/1000000;
	temp = iColor%1000000;
	int g = temp/1000;
	temp = iColor%1000;
	int b = temp;

	Color.r = (float)r/255.0f;
	Color.g = (float)g/255.0f;
	Color.b = (float)b/255.0f;

	return Color;
}
WORD CGameUtil::GetPrimaryLanguage()
{
	return PRIMARYLANGID( LOWORD( GetKeyboardLayout(0) ) );
}
WORD CGameUtil::GetSubLanguage()
{
	return SUBLANGID( LOWORD( GetKeyboardLayout(0) ) );
}

void CGameUtil::ConvertMoney2String( __int64 money, char* buffer, int buffer_size )
{
	char temp_buf[128];

	_i64toa( money, temp_buf, 10 );

	NUMBERFMT format = { 0, 0, 3, ".", ",", 1 };

	int ret = GetNumberFormat(NULL, NULL, temp_buf, &format, buffer, buffer_size);

	if( 0 == ret ) 
	{
		DWORD error = GetLastError();
		switch( error )
		{
		case ERROR_INSUFFICIENT_BUFFER:
			_ASSERT( 0 && "ERROR_INSUFFICIENT_BUFFER" );
			break;
		case ERROR_INVALID_FLAGS:
			_ASSERT( 0 && "ERROR_INVALID_FLAGS" );
			break;
		case ERROR_INVALID_PARAMETER:
			_ASSERT( 0 && "ERROR_INVALID_PARAMETER" );
			break;
		default:
			_ASSERT( 0 && "GetNumberFormat Error");
			break;
		}
	}
}
