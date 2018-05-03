#ifndef _RESTARTDLG_
#define _RESTARTDLG_


#include "tgamectrl/tdialog.h"

/**
* 죽었을때 부활 지점을 선택하는 다이얼로그
*
* @Warning		대만버전에서는 현재 필드부활 선택을 할수 없다( XML에서 버튼부분을 뺐다 )
* @Author		최종진
* @Date			2005/9/14
*/
class CRestartDLG  : public CTDialog
{
public:
	CRestartDLG();
	~CRestartDLG();
	
	virtual void Show();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );

protected:

	enum
	{
		BTN_RESTART_SAVE_POSITION = 3,			/// 저장된 마을로 
		BTN_RESTART_CURRENT_FIELD,				/// 현재 필드에서 ( 대만버젼에서는 빠졌다: XML Date)
		BTN_RESTART_DUNGEON_GATE,				/// 던전입구로 ( 대만버젼에서 던전안에서 죽었을경우 던전입구에서 부활할수 있도록 버튼 추가 )
	};


	bool On_LButtonUP( unsigned iProcID);
};


#endif //_RESTARTDLG_