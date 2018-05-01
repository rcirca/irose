// CQuestDlg.h: interface for the CQuestDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CQUESTDLG_H__98E8A820_E90F_4103_91BE_1D58E5862D0D__INCLUDED_)
#define AFX_CQUESTDLG_H__98E8A820_E90F_4103_91BE_1D58E5862D0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDialog.h"
#include "../../Common/CUserData.h"
class CQuestListItem;
/**
* Quest 정보를 보여주는 다이얼로그
*
* @Warning		NPC 대화창이 열려있는 동안에는 포기를 할수 없도록 포기 버튼을 Disable시켜야한다.
* @Author		최종진
* @Date			2005/9/14
*/
class CQuestDlg : public CTDialog  
{
public:
	CQuestDlg();
	virtual ~CQuestDlg();

	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void	Update( POINT ptMouse );
	virtual void	Draw();
	virtual void    Show();
	virtual void    Hide();
	virtual void	MoveWindow( POINT pt );

	void UpdateQuestList();													/// 현재 수행중인 퀘스트 목록 갱신

	enum{

		IID_BTN_ABANDON		= 50,											/// 퀘스트 포기 버튼
		IID_BTN_CLOSE		= 10,
		IID_BTN_ICONIZE		= 11,
		IID_BTN_MINIMIZE	= 113,
		IID_BTN_MAXIMIZE	= 114,

		IID_ZLIST_QUEST		= 20,
		IID_ZLIST_SCROLLBAR = 21,
		IID_LIST_QUESTINFO	= 30,
		IID_ZLIST_ITEM		= 99,
		IID_PANE_QUESTLIST	= 100,
		IID_PANE_QUESTINFO	= 200

		//IID_LASTDRAWCTRL = 20,
	}; 
protected:

	bool On_LButtonUP( unsigned ID );  
	bool On_LButtonDN( unsigned ID );

	void DrawItems();														/// 해당 퀘스트용 아이템 Draw
	void UpdateQuestDescription();											/// 선택된 퀘스트의 설명 갱신
	int  GetSelectQuestIdx();												
	
	void Minimize();														/// 최소화
	void Maximize();														/// 최대화
	void ActiveListBoxMoveBegin();

	CWinCtrl* FindChildInPane( unsigned uiPaneID, unsigned uiChildID );
protected:
	///리스트 박스의 순서에 Quest Slot Idx를 넣어둔다.: 
	///CUserDATA에서 Quest Slot은 앞에 것이 지워져도 그 뒤에 것은 그 슬롯에 있는다.

	int					m_iList2QuestIdx[QUEST_PER_PLAYER];
	CQuestListItem*		m_pSelectedQuestItem;					/// 선택된 퀘스트아이템에 대한 포인터

	int					m_iPrevSelectedQuestID;					/// 이전에 선택되었던 퀘스트 인덱스
	bool				m_bMinimize;							/// 최소 or 최대

	
};

#endif // !defined(AFX_CQUESTDLG_H__98E8A820_E90F_4103_91BE_1D58E5862D0D__INCLUDED_)
