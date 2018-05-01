#ifndef _CSelectEventDlg_
#define _CSelectEventDlg_

#include "TDialog.h"


/**
* 루나행성의 워프게이트 처럼 유저의 선택만을 입력받는 다이얼로그
*	- 기본 동작은 CDialogDlg와 동일하다
* 
* @Warning		대만버젼과 국내버젼과는 표시 방식이 다르다(XML데이타가 틀리며 국가코드로 코드가 구분된다)
* @Author		최종진
* @Date			2005/9/14
*/
class CSelectEventDlg :	public CTDialog
{
public:
	CSelectEventDlg(void);
	virtual ~CSelectEventDlg(void);
	virtual bool Create( const char* IDD );
	virtual void Hide();
	virtual void Show();
	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	void	SetTitle( const char* pszTitle );
	void	AddEvent( char* pszScript, int iEventID, void (*fpEventHandle)(int iEventID) );
	void	SetTargetClientObjectIndex( short iIndex );

private:
	enum{
		IID_BG_IMAGE		= 1,
		IID_BTN_CLOSE		= 10,
		IID_ZLISTBOX_EVENT	= 20,
	};


	short		m_nTargetClientIdx;
	std::string m_strTitle;
};
#endif