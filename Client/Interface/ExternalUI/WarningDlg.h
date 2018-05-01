#pragma once

#include "CExternalUI.h"
#include "../../../TGameCtrl/TDialog.h"

class CWarningDlg: public CTDialog
{
private:
	///
	///	Observer class
	///
	CExternalUIManager*		m_pEUIManager;

public:
	CWarningDlg(void);
	~CWarningDlg(void);

	void SetEUIObserver( CExternalUIManager* pObserver );

	virtual UINT	Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    Create( const char* szIDD );
	virtual void	Show();
	virtual void	Hide();
	virtual void	Draw();
	virtual void    MsgBoxProc(  unsigned int iMsgType, short int iButton );


	enum
	{	
		IID_IMG_BG			= 0,
		IID_IMG_USA_SERVER	= 3,
		IID_IMG_TALA_SERVER	= 4,
		IID_BTN_OK			= 5,
		IID_BTN_CANCEL		= 6,
		IID_LISTBOX_MESSAGE = 10,
		IID_LISTBOX_MESSAGE_SCROLLBAR = 11,
	};

protected:

	void OnLButtonUp( unsigned iProcID );
	void OnLButtonDown( unsigned iProcID );		
};
