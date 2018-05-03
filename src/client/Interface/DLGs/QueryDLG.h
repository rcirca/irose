
#ifndef _QUERY_DLG_H
#define _QUERY_DLG_H

//#include "CommonDLG.h"
#include "../../../TGameCtrl/TDialog.h"
// *--------------------------------------------------------------------* //
// 질의 윈도우 
// *--------------------------------------------------------------------* //
#define MAX_QUESTION_LN		3

typedef struct {
	
	char		m_szQuestion[MAX_QUESTION_LN][MAX_PATH];
}t_question_page;		//질문페이지 

typedef struct {
	int					m_iEventID;
	short				m_nCurPageNo;			//현재페이지번호 
	short				m_nPageCNT;				//페이지의 개수 
	t_question_page*	m_pQuestionPage;		//질문페이지
}t_query_question;		//질문

struct t_query_exam {
	char		*m_pExam;							//예시 
	int			m_iEventID;							//이벤트의 아이디
	short		m_nLnCNT;							//라인 아이디(2줄이상되는 것들때문에) 
	void		(*fpEventHandle)(int iEventID);		//예시 선택시 처리 함수 
	t_query_exam ()		{ m_pExam=NULL;	}
};			//예시

typedef vector<t_query_exam>			vec_query_exam;
typedef vector<t_query_exam>::iterator	vec_query_exam_itr;

enum {
	QY_BTN_PREV,	//0,이전
	QY_BTN_NEXT,	//1,다음
	QY_BTN_MAX_CNT	
};

class CQueryDLG : public CTDialog
{
private:
	short					m_nTargetClientIdx;
public:
	short					m_nMaxLength;		//한라인에 들어갈 최대 문자수
	short					m_nLastPosY;		//질문이 마지막으로 찍힌 좌표 
	
	int						m_iFontNo_q;		//폰트번호-질문 
	D3DCOLOR				m_dCOL_q;			//글씨색-질문 
	
	int						m_iFontNo_e;		//폰트번호-예시 
	D3DCOLOR				m_dCOL_e;			//글씨색-예시
	t_query_question		m_sQuestion;		//질문


	char					*m_pQuery;	
	short					m_nMaxLN;

	short					m_nExamCNT;			//등록된 질문의 예문개수 
	short					m_nMaxExamCNT;		//최대로 들어갈 예문개수 
	short					m_nLastExamID;		//부여해줄 예문의 아이디(2라인 이상되는것들때문에)
	vec_query_exam			m_vecEXAM;			//질문에 등록된 예문들

	void					(*fpClose)(int iEventID);		//X버튼으로 클로즈시 처리할 함수 
public:
	CQueryDLG();
	~CQueryDLG();

	virtual void Draw();
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Update( POINT ptMouse );
	virtual int	 IsInValidShow();

	bool OpenDLG(short nSX,short nSY,char* szQuery,short nMaxCharLength,int iEventID,void (*fpCloseProc)(int iEventID));
	void Close();

	void DrawQuery();
	//질문
	bool AppendQuestion(char* szQuestion,int iEventID);
	//질문삭제
	void ReleaseQuestion();
	//예제
	bool AppendExam(char* szExam,int iEventID,void (*fpExamEvent)(int iEventID));
	void Free_EXAM();
	void ReleaseExam();
	//마우스가 선택한 예제 번호를 얻어온다 
	short getMouseSelectLN();
	
	//예제선택처리 
	void ProcessEvent(short nNum);
	//마우스 클릭 이벤트 처리 
	bool  MouseClickEventProc();
	//키이벤트 처리 
	bool  KeyDownEventProc(WPARAM wParam);

		
	// *---------------------------------------------------------------* //

	bool On_LButtonDN(unsigned iProcID);
	bool On_KeyDown(WPARAM wParam);
};





#endif