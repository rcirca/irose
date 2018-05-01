#ifndef _ITDRAW_
#define _ITDRAW_

/**
* Client가 상속받아 구현해야할 Draw Image Interface - TGameCtrl은 이 인터페이스를 통해서 Image를 Draw한다.
*
* @Warning	Client::CTDrawImpl - 현재 클라이언트에서 구현한 클래스
*
* @Author		2005/08/23 최종진
*/
class ITDraw
{

public:
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID ) = 0 ;
	virtual void Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID ) = 0 /*{ _ASSERT( 0 && "ITDraw::Draw2");}*/;
	virtual void Draw( float iX, float iY, float iZ, int iModuleID, int iGraphicID ) = 0 /*{ _ASSERT( 0 && "ITDraw::Draw3");}*/;

	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, float fScaleWidth , float fScaleHeight , D3DCOLOR color ) = 0;
	
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, float fScaleWidth , float fScaleHeight , D3DCOLOR color ) = 0;
	
	virtual void Draw( int iX, int iY, int iModuleID, int iGraphicID, D3DCOLOR color ) = 0;
	virtual void Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID, D3DCOLOR color ) = 0;

	virtual void DrawFitW( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, D3DCOLOR color ) = 0;
	virtual void DrawFitH( int iX, int iY, int iModuleID, int iGraphicID, int iHeight , D3DCOLOR color ) = 0;
	virtual void DrawFit( int iX, int iY, int iModuleID, int iGraphicID, int iWidth , int iHeight , D3DCOLOR color ) = 0;

};
#endif /// _ITDRAW_
