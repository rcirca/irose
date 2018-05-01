#ifndef _CChatFilterDlg_
#define _CChatFilterDlg_

#include "tdialog.h"
#include "Chattingdlg.h"


/**
* 채팅입력창에서 입력탭별로 보여질 채팅 타입을 선택할수 있는 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CChatFilterDlg : public CTDialog
{
public:
	CChatFilterDlg(void);
	virtual ~CChatFilterDlg(void);
	virtual void Show();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam , LPARAM lParam );

	ChatFilter m_filter;					/// 탭별 유저 선택필터 저장 변수									

protected:
	enum{
		CHECKBOX_ALL	= 10,
		CHECKBOX_SYSTEM = 11,
		CHECKBOX_TRADE  = 12,
		CHECKBOX_PARTY  = 13,
		CHECKBOX_CLAN   = 14,
		CHECKBOX_ALLIED = 15,
	};
};
#endif