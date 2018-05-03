#ifndef _CTCheckBox_
#define _CTCheckBox_
#include "winctrl.h"

/**
* 체크박스 Class
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTCheckBox : public CWinCtrl
{
public:
	CTCheckBox(void);
	virtual ~CTCheckBox(void);
	bool		Create(int iScrX, int iScrY, int iWidth, int iHeight , int iModuleID, int iCheckImageID, int iUncheckImageID );
	virtual unsigned	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void		Draw();

	bool	IsCheck();
	void	SetCheck();
	void	SetUncheck();


protected:
	enum{
		CBS_UNCHECK,
		CBS_CHECK
	};


	int	m_iCheckState;					/// 체크 Yes_or_Not
	int	m_iModuleID;					/// 이미지들의 모듈 ID
	int m_iCheckImageID;				/// 체크 상태일경우 이미지 ID
	int m_iUncheckImageID;				/// 체크 안된 상태일경우 이미지 ID

};
#endif