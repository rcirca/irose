#ifndef _CTDRAWIMPL_
#define _CTDRAWIMPL_

#include "..\\..\\TGameCtrl\\ITDraw.h"
#include <D3DX9.h>

/**
* 인터페이스 라이브러리(TGameCtrl)에게 알려줄 ITDraw의 Implemented Class
*  : TGameCtrl내부적으로 2D이미지를 그릴때 이 인터페이스를 통해서 znzin의 해당 인터페이스를 호출한다. 
*
* @Author		이동현,최종진
*
* @Date			2005/9/5
*/
class CTDrawImpl : public ITDraw
{
public:
	CTDrawImpl();
	~CTDrawImpl();

	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID );
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, D3DCOLOR color );
	virtual void Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID );
	virtual void Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID, D3DCOLOR color );
	virtual void Draw( float iX, float iY, float iZ, int iModuleID, int iGraphicID );
	virtual void Draw( float iX, float iY, float iZ, int iModuleID, int iGraphicID , D3DCOLOR color);
	virtual void Draw( float iX, float iY, float iZ,int iWidth, int iModuleID, int iGraphicID );

	/// 가로, 세로 자동 확대.
	virtual void DrawFitW( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, D3DCOLOR color );
	virtual void DrawFitH( int iX, int iY, int iModuleID, int iGraphicID, int iHeight, D3DCOLOR color );
	virtual void DrawFit( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, int iHeight, D3DCOLOR color );

	virtual int GetScreenWidth();
	virtual int GetScreenHeight();

	/// 가로, 세로 확대시 
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, float fScaleWidth , float fScaleHeight , D3DCOLOR color );	
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, float fScaleWidth , float fScaleHeight , D3DCOLOR color );	

	/// Y축을 기준으로 회전시 
	virtual void DrawRotateY( float fX, float fY, float fZ, float fRotateY , int iModuleID, int iGraphicID );

	/// 진행 게이지(쿨타임)가 있는 아이콘
	void DrawCover( int iX, int iY, int iModuleID, int iGraphicID, D3DCOLOR color, D3DCOLOR coverColor, float value );
};


extern CTDrawImpl		g_DrawImpl;
#endif // _CTDRAWIMPL_