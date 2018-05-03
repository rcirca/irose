#ifndef _CEventDialog_
#define _CEventDialog_

#include "tgamectrl/tdialog.h"
#include "tgamectrl/JStringParser.h"


/**
* 아이템사용시 열리는 대화창등 Event 로 열리는 다이얼로그
*	- 기본 동작은 CDialogDlg와 동일하다
* 
* @Warning		대만버젼과 국내버젼과는 표시 방식이 다르다(XML데이타가 틀리며 국가코드로 코드가 구분된다)
* @Author		최종진
* @Date			2005/9/14
*/

class CEventDialog : public CTDialog
{
public:
	CEventDialog(void);
	virtual ~CEventDialog(void);

	virtual void Hide();
	virtual void Draw();
	virtual bool Create( const char* IDD );
	virtual void Show();

	virtual unsigned Process(unsigned uiMsg, WPARAM wParam, LPARAM lParam );


	void SetScript( char* pszScript );
	void AddAnswerExample( char* pszScript, int iEventID, void (*fpEventHandle)(int iEventID) );
	void SetTargetNpcClientObjectIndex( short iIndex );
	int	 IsInValidShow();

protected:
	enum{
		IID_BG_IMAGE				= 1,
		IID_BTN_CLOSE				= 10,
		IID_ZLISTBOX_NPCSCRIPT		= 20,
		IID_ZLISTBOX_ANSWER_EXAMPLE = 21
	};


	short			m_nTargetClientIdx;
	std::string		m_strNpcName;
	std::string		m_strTempScript;
	HNODE			m_hNpcFace;
	int				m_widthNpcFace;
	int				m_heightNpcFace;

	CJStringParser	m_Script;


};
#endif