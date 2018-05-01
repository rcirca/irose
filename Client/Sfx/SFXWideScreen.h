#ifndef _SFX_WIDE_SCREEN_
#define _SFX_WIDE_SCREEN_

#include "isfx.h"

//--------------------------------------------------------------------------------------
///
/// class CSFXWideScreen
/// 16:9 dramatic screen 
///
//--------------------------------------------------------------------------------------
class CSFXWideScreen :	public ISFX
{
private:
	HNODE				m_hSFXTexture;

	int					m_iWidth;
	int					m_iHeight;

public:
	CSFXWideScreen(void);
	~CSFXWideScreen(void);

	void							SetScreenSize( int iWidth, int iHeight );


	/*override*/bool				Init();
	/*override*/void				Release();

	/*override*/void				Update();
	/*override*/void				Draw();
};


#endif //_SFX_WIDE_SCREEN_