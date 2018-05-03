#ifndef _CTGUAGE_
#define _CTGUAGE_

#include "winctrl.h"
#include "ITFont.h"
#include "ITDraw.h"

/**
* 게이지 Class
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTGuage :	public CWinCtrl
{
public:
	CTGuage();
	~CTGuage(void);

	virtual void	Draw();
	bool Create( int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID ,int iBGraphicID, int iModuleID );
	
	void	SetText( const char* szTxt);
	const   char* GetText();

	void	SetValue( int iValue );
	int		GetValue();

	void	SetSizeFit( bool bFit );
	bool	GetSizeFit();
	
	void	SetGuageImageID( int iImageID );
	enum {
		UPDATE_DELETE,									// PARENT에게 지워달라고 요청 ( 사용에 주의 )
		UPDATE_NORMAL,		
		UPDATE_END										// UPDATE에서 할일을 다 끝냈다( PARENT가 알아서 다음에 해야할것들을 처리 )
	};

protected:
	///Image
	int				m_iBGImageID;
	int				m_iGuageImageID;
	int				m_iModuleID;
	///Data
	std::string		m_strText;


	///Guage를 그릴 Width
	int				m_iValue;							///현재 게이지가 그려져야할 %(0 ~ 100)
	bool			m_bSizeFit;							/// draw에서 사이즈 with, height엠 Fit.

	ITFont*			m_pFontImpl;
	ITDraw*			m_pDrawImpl;
};

#endif
