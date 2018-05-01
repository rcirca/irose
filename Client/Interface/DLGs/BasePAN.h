
#ifndef _BASE_PAN_H
#define _BASE_PAN_H


#include "../../../TGameCtrl\\TDialog.h"
// *--------------------------------------------------------------------* //
// 기본판낼창 
// *--------------------------------------------------------------------* //

enum basePAN {
	BASE_IMG			= 1,		//1,판낼
	BASE_BTN_CHAR_OPEN,				//2,캐릭터정보버튼
	BASE_BTN_INV_OPEN,				//3.인벤토리오픈 
	BASE_BTN_SKILL_OPEN,			//4.스킬창오픈 
	BASE_BTN_QUEST_OPEN,			//5.퀘스트창오픈 
	BASE_BTN_COMMUNITY_OPEN,		//6.커뮤니티오픈 
	BASE_BTN_OPTION_OPEN,			//7.시스템메뉴오픈 
	BASE_BTN_SYS_OPEN,				//8.시스템메뉴오픈 
	BASE_MAX_CTRL_CNT		
};

class CBasePAN : public CTDialog
{
public:
	CBasePAN();
	~CBasePAN();


	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void Update( POINT ptMouse );

private:
	bool On_LButtonUP( unsigned iProcID);

};



#endif