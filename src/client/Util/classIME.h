
#ifndef __CLASS_IME_H
#define __CLASS_IME_H

//-------------------------------------------------------------------------------------------------

#include <IMM.h>

#define IME_IN_COMPOSITION			1
#define IME_IN_CHOSECAND			2

#define MAX_LISTCAND				32
#define MAX_CHARS_PER_LINE			10
#define MAX_COMP_STRING_LEN			10
#define DEFAULT_CAND_NUM_PER_PAGE	5

#define MAX_TEXT_LENGTH				255

#define CANDWND_CLASS	"HanjaCandidate"

class classIME 
{
public :// private :
	void			PutString(LPSTR str);
	int				OnChar(UINT nChar);

	inline bool		Enter();
	inline void		Leave();
	inline bool		Check();

	bool			IsProcessEvent();
	void			SetState(bool fOpen);
	void			ClearData();

	// for Message 
	long			OnInputLangChange		(WPARAM dwCommand, LPARAM dwData);
	long			OnIMESetContext			(WPARAM dwCommand, LPARAM dwData);
	long			OnIMEStartComposition	(WPARAM dwCommand, LPARAM dwData);
	long			OnIMEComposition		(WPARAM dwCommand, LPARAM dwData);
	long			OnIMEEndComposition		(WPARAM dwCommand, LPARAM dwData);
	long			OnIMECompositionFull	(WPARAM dwCommand, LPARAM dwData);
	long			OnIMENotify				(WPARAM dwCommand, LPARAM dwData);
	long			OnIMEControl			(WPARAM dwCommand, LPARAM dwData);

	// for IMEComposition
	bool			SetCompositionFont	(LPLOGFONT lf);
	void			SetCompWndPos		(POINT& pt);
	int				GetCompCursorPos();

	virtual bool	GetResultString();
	virtual bool	GetCompString(long flag);
	virtual void	ProcessResultString(LPSTR str);
	virtual void	ProcessCompString(LPSTR str, LPSTR strAttr);
	virtual void	IMEStartComposition(long dwData);
	virtual void	IMEEndComposition(long dwData);

	// for candidate window
	bool			OpenCandidate(long lParam);	// IMN_OPENCANDIDATE
	bool			CloseCandidate(long lParam);	// IMN_CLOSECANDIDATE
	bool			ChangeCandidate(long lParam);	// IMN_CHANGECANDIDATE
	bool			SetOpenStatus();				// IMN_SETOPENSTATUS
	void			CandPaint(HWND hwnd);
	bool			DisplayCandStrings(HWND hwnd, LPCANDIDATELIST lpCandList);

	// friend function
	// variables
	//HWND	m_hKeyWnd;
	HKL				m_hKeyLayout;
    int				m_nState;					// Current Ime state.
    int				m_nCompLen;					// To save previous composition string length.
    DWORD			m_property;
    HWND			m_hwndCand[ MAX_LISTCAND ];
    LPCANDIDATELIST m_CandList[ MAX_LISTCAND ];
	// transient
	HIMC			m_hIMC;
	HWND			m_hWnd;
	short			m_nMaxTextLength;

public :
	int				m_charWidth;				// charwidth in Candidate Window, fixed Width
	int				m_charHeight;				// charHeight in Candidate Window
	char		   *m_pText;					// [ MAX_CHAR_100 ];
	int				m_TextCurCount;
	char			m_TextComp[ 3 ];

	classIME();
	~classIME();
	void			InitIme (HWND hWnd=NULL, short nMaxTextLen=MAX_TEXT_LENGTH)	{	Create (hWnd,nMaxTextLen);	}

	int				ProcIMEMessage (UINT msg,WPARAM wparam,LPARAM lparam);

	
	// *-------------------------------------------------------------------------------* //
	// 새롭게 추가된 부분  
	// *-------------------------------------------------------------------------------* //
private :
	bool			m_bActive;

public  :
	bool	Create (HWND hWnd=NULL, short nMaxTextLen=MAX_TEXT_LENGTH);
	void	Clear ();
	bool	IsActive()					{	return m_bActive;		}
	void	SetActive (bool bActive)	{	m_bActive = bActive;	}

	bool	ChkKorean	(LPSTR str);
	bool	ChkKorean2	(LPSTR str,int cnt);
	bool	ChkEndChar	();
	void	ClearString ();

	void	ToggleInputMode(bool bHangul);
};


#define CIME_SKIP		0
#define CIME_HANDLED	1
#define CIME_RETURN		2
#define CIME_CANCEL		3
#define CIME_WMCHAR		4

//-------------------------------------------------------------------------------------------------
#endif