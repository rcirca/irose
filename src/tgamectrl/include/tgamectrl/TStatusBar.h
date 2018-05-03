#ifndef _CTStatusBar_
#define _CTStatusBar_
#include "winctrl.h"
class CTImage;

/**
* 사용안함:CTDialog하단에 부착되는 Class
* @Author	최종진
* @Date		2005/8/30
*/
class TGAMECTRL_API CTStatusBar :	public CWinCtrl
{
public:
	CTStatusBar(void);
	virtual ~CTStatusBar(void);
	virtual void Draw();
	virtual void Update( POINT pt );
	virtual void MoveWindow( POINT pt );

	void	SetImage( CTImage* pImage );
protected:

	CTImage*	m_pImage;
};
#endif