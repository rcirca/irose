#ifndef _CTFONTIMPL_
#define _CTFONTIMPL_
#include "../../TGameCtrl/ITFont.h"


/**
* 인터페이스 라이브러리(TGameCtrl)에게 알려줄 ITFont의 Implemented Class
*  : TGameCtrl내부적으로 글자를 그릴때 이 인터페이스를 통해서 znzin의 해당 인터페이스를 호출한다. 
*
* @Author		최종진
*
* @Date			2005/9/5
*/

class CTFontImpl :	public ITFont
{
public:
	CTFontImpl(void);
	~CTFontImpl(void);

	virtual void Draw( int iFont, bool bUseSprite, RECT* pRect, D3DCOLOR Color, DWORD dwFormat, const char * pMsg );
	virtual void Draw( int iFont, bool bUseSprite, int x, int y, D3DCOLOR Color,  const char * pMsg );

	/// 해당 인덱스의 폰트를 실제 그릴시의 Height를 구하는 Method
	virtual int  GetFontHeight( int iFont );

	/// 해당 인덱스의 폰트로 실제 텍스트 출력시 너비와 높이를 구하는 Method
	virtual SIZE GetFontTextExtent ( int iFont, const char* pszText );
	
	/// 텍스트가 출력될 기준 위치를 변경하는 Method
	virtual void SetTransformSprite( float x, float y, float z );
};

extern CTFontImpl	g_FontImpl;
#endif