#ifndef	__CKEYBOARD_H
#define	__CKEYBOARD_H
#include "util\WndMsgQ.h"
#include "util\classIME.h"
//-------------------------------------------------------------------------------------------------
// #define	__USE_IME
class CKeyboard : public CWndMsgQ
{
private :
	void On_WM_KEYUP	(WPARAM wParam, LPARAM lParam);
	void On_WM_KEYDOWN	(WPARAM wParam, LPARAM lParam);
	void On_WM_CHAR		(WPARAM wParam, LPARAM lParam);

	CKeyboard ();
	~CKeyboard ()	{	;	}
	static CKeyboard* m_pInstance;



public  :
#ifdef	__USE_IME
	bool IME_WndMSG (UINT uiMsg, WPARAM wParam, LPARAM lParam);
	classIME	m_IME;
#endif

	static CKeyboard* Instance ();
	void Destroy ()				{	SAFE_DELETE( m_pInstance );	}

	bool Add_WndMSG (UINT uiMsg, WPARAM wParam, LPARAM lParam);

	void Init (void);
	void Proc (void);
	void SetEnable(){ m_bEnable = true; }
	void SetDisable(){ m_bEnable = false; }
	bool IsEnable(){ return m_bEnable; }

protected:
	bool	m_bEnable;
	
} ;
extern CKeyboard *g_pCKeybd;

//-------------------------------------------------------------------------------------------------
#endif
