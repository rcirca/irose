
#ifndef _CDIALOG_H
#define _CDIALOG_H

#include "Form.h"
#include "CCaption.h"

// 대화상자 
class CDlgBX : public CWndCtrl
{
protected:
	t_caption_bar	m_tCaption;
	
public:
	GameForm		m_ctrls;	
public:
	CDlgBX();
	~CDlgBX();
	
	bool Create(int iID,int iSX,int iSY,bool (*fpFunc)(int));
	bool Create(int iID,int iSX,int iSY,int iWidth,int iHeight,bool (*fpFunc)(int));
	void Release();

	virtual UINT MsgProc(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Draw();
	virtual void DrawToolTips() ;

	void   MoveWnd(int iPosX,int iPosY);

};

#endif