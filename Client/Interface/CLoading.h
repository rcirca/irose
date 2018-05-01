#ifndef _LOADING_
#define _LOADING_
//#define __THREADED_LOADING

#include "../Util/JSingleton.h"
#include "../GameProc/LoadingImageManager.h"

/**
* 존간 워프시 로딩이미지를 보여주는 Class 
*	- CLoadingImageManager가 stb를 참조하여 보여주어야할 이미지를 바꾸어 준다.
*
* @Author	최종진
* @Date		2005/9/5
*/
class CLoading : public CJSingleton< CLoading >
{
	
	HNODE						m_hTexture;

	CLoadingImageManager		m_LoadingImageManager;

	/// m_LoadingImageManager 가 초기화 되었는가?
	bool						m_bInitLoadingImageManager;


public:
	CLoading();
	~CLoading();

	bool			InitLoadingImageManager();

	bool			LoadTexture( int iZoneNo = 9999, int iPlanetNO = 0 );
	void			Draw();
	void			UnloadTexture();
};

#define g_Loading CLoading::GetSingleton()

#endif //_LOADING_