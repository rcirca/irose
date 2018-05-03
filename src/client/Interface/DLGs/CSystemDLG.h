#ifndef _SYSTEMDLG_
#define _SYSTEMDLG_

#include "tgamectrl/tdialog.h"

/**
* 게임종료및 캐릭터 선택창으로 이동선택을 위한 다이얼로그
*
* @Author		최종진
* @Date			2005/9/14
*/
class CSystemDLG  : public CTDialog
{
public:
	CSystemDLG();
	virtual ~CSystemDLG();

	unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);

protected:
	enum{
		IID_BTN_EXIT		= 10,
		IID_BTN_CONTINUE	= 11,
		IID_BTN_GOTO_SELECTAVT = 12,
		IID_BTN_CLOSE		= 13,
	};

	bool On_LButtonUP( unsigned iProcID);

};


#endif //_SYSTEMDLG_