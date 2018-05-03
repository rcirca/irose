#ifndef _CDialogDlg_
#define _CDialogDlg_
#include "tgamectrl/tdialog.h"
#include "tgamectrl/JStringParser.h"

/**
* NPC 와의 대화시 사용되는 다이얼로그
*	
* @Warning		현재( 2005/9/12 ) 대만(New)버젼과 기존 버젼과 출력 방법이 틀리다.
* @Author		최종진
* @Date			2005/9/12
*/
class CDialogDlg : public CTDialog
{
public:
	CDialogDlg(void);
	virtual ~CDialogDlg(void);

	virtual bool Create( const char* IDD );
	virtual void Update( POINT ptMouse );
	virtual void Hide();
	virtual void Draw();
	virtual void Show();
	virtual unsigned Process(unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	void		SetScript( char* pszScript );															/// NPC 대사 
	void		AddAnswerExample( char* pszScript, int iEventID, void (*fpEventHandle)(int iEventID) );	/// 선택문 추가

	void		SetTargetNpcClientObjectIndex( short iIndex );		
	short		GetTargetNpcClientObjectIndex();					
	
	int			IsInValidShow();																		/// 화면에 보여질수 있는가?
	void		SetNpcFace( HNODE hNode_ );
	void		SetNpcName( std::string strName );
	void		SetNpctalkinterfaceHide( float fTime );
	float		GetNpctalkinterfaceHide();

protected:

	enum{
		IID_BG_IMAGE				= 1,
		IID_BTN_CLOSE				= 10,
		IID_ZLISTBOX_NPCSCRIPT		= 20,
		IID_ZLISTBOX_ANSWER_EXAMPLE = 21
	};

	short m_nTargetClientIdx;						/// NPC의 클라이언트 인덱스
	
	std::string m_strNpcName;						/// NPC이름
	std::string m_strTempScript;					/// 임시 변수
	
	HNODE		m_hNpcFace;							/// 대화창에 보여질 NPC의얼굴 텍스쳐 노드
	int			m_widthNpcFace;						/// 텍스쳐의 너비
	int			m_heightNpcFace;					/// 텍스쳐의 높이

	CJStringParser	m_Script;						/// NPC대사 

	float		m_fNpctalkinterfaceHideTime;
};
#endif