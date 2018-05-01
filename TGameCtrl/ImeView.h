#ifndef _CIME_H_
#define _CIME_H_

#include "windows.h"
#include <imm.h>

#define IME_IN_COMPOSITION      1
#define IME_IN_CHOSECAND        2

#define MAX_LISTCAND 32
#define MAX_CHARS_PER_LINE		10
#define MAX_COMP_STRING_LEN		10
#define MAX_CHAR_100			MAX_PATH
#define DEFAULT_CAND_NUM_PER_PAGE 5

#define CANDWND_CLASS	"HanjaCandidate"

class CImeView
{
public:
	CImeView();           // protected constructor used by dynamic creation

	void Darw(HDC pdc,POINT * ppo);
	void PutString(LPSTR str);
	void OnChar(UINT nChar);
	bool IsYouMessage(UINT msg,WPARAM wparam,LPARAM lparam);

// Implementation

	virtual ~CImeView();
	void InitIme(HWND hwnd=NULL);
	inline BOOL Enter() {
		if(m_hWnd != NULL)
			return (m_hIMC = ImmGetContext(m_hWnd))?TRUE:FALSE;
		else return FALSE;
	};
	inline void Leave() {
		ImmReleaseContext(m_hWnd, m_hIMC);
	};
	inline BOOL Check() {
		return (!(m_property & IME_PROP_SPECIAL_UI) && (m_property & IME_PROP_AT_CARET));
	}
	BOOL IsProcessEvent();
	void SetState(BOOL fOpen);
	void ClearData();

	// for Message 
	LONG OnInputLangChange(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMESetContext(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMENotify(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEControl(WPARAM dwCommand, LPARAM dwData);

	// for IMEComposition
	BOOL SetCompositionFont(LPLOGFONT lf);
	void SetCompWndPos(POINT& pt);
	int	 GetCompCursorPos();

	virtual BOOL GetResultString();
	virtual BOOL GetCompString(LONG flag);
	virtual void ProcessResultString(LPSTR str);
	virtual void ProcessCompString(LPSTR str, LPSTR strAttr);
	virtual void IMEStartComposition(LONG dwData);
	virtual void IMEEndComposition(LONG dwData);

	// for candidate window
	BOOL OpenCandidate(LONG lParam);	// IMN_OPENCANDIDATE
	BOOL CloseCandidate(LONG lParam);	// IMN_CLOSECANDIDATE
	BOOL ChangeCandidate(LONG lParam);	// IMN_CHANGECANDIDATE
	BOOL SetOpenStatus();				// IMN_SETOPENSTATUS
	void CandPaint(HWND hwnd);
	BOOL DisplayCandStrings(HWND hwnd, LPCANDIDATELIST lpCandList);

	// friend function
	// variables
	//HWND	m_hKeyWnd;
	HKL		m_hKeyLayout;
    int		m_nState;					// Current Ime state.
    int		m_nCompLen;					// To save previous composition string length.
    DWORD	m_property;
    HWND	m_hwndCand[MAX_LISTCAND];
    LPCANDIDATELIST m_CandList[MAX_LISTCAND];
	int		m_charWidth;				// charwidth in Candidate Window, fixed Width
	int		m_charHeight;				// charHeight in Candidate Window
	// transient
	HIMC	m_hIMC;
	HWND    m_hWnd;
	char    m_Text[MAX_CHAR_100];
	int     m_TextCurCount;
	char    m_TextComp[3];
	
	// *-------------------------------------------------------------------------------* //
	// 새롭게 추가된 부분  
	// *-------------------------------------------------------------------------------* //
public:
	BOOL ChkKorean(LPSTR str);
	BOOL ChkKorean2(LPSTR str,int cnt);
	BOOL ChkEndChar();
	void ClearString();
	void ConvInputMode();
	// *------------------------------
};
extern CImeView			g_CIME;
#endif
