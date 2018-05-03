#ifndef _CICON_
#define _CICON_


#include "../../Common/CItem.h"
#include "tgamectrl/tobject.h"
#include "../CInfo.h"
//----------------------------------------------------------------------------------------------------
/// 
///			CSkillSlot				CItemSlot 
///				↓						↓
///			  CSkill				  CItem
///			   ↓↑					   ↓↑
///			CSkillIcon ◁ CIcon ▷	CItemIcon
///				┃						┃
///				└--------------------------------> CSlot
///
//----------------------------------------------------------------------------------------------------


class CTCommand;
class CTDialog;
class CSlot;

/**
* 게임내의 모든 아이콘타입의 추상 클래스
* 기본적으로 랜더링메쏘드와 명령 실행 메쏘드, 그리고 업데이트 메쏘드를 가진다.
*
* @warning virtual Method는 항상 CIconQuick에서 재정의 해줄것
* @Author		최종진
* @Date			2005/9/6
*/
class CIcon : public CTObject
{
public:
	CIcon(void);
	virtual ~CIcon(void);


	//----------------------------------------------------------------------------------------------------
	/// Virtual methodes...
	//----------------------------------------------------------------------------------------------------
	virtual void	Draw();
	virtual void	ExecuteCommand();
	virtual CIcon*	Clone();
	virtual void	Clear();
	virtual void	GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType );
	virtual void	SetPosition( POINT pt ){ m_ptPosition = pt; }
	virtual bool	IsEnable();
	virtual const char*	GetName();
	virtual void	 Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int		 GetIndex();

	POINT	GetPosition(){ return m_ptPosition; }

	void	SetEnable(){ m_bEnable = true; }
	void	SetDisable(){ m_bEnable = false; }
	bool	IsInside( POINT pt );
	//void	SetParent( CTDialog* pDialog ){ m_pParentDlg = pDialog; }

	void	SetGraphicID( int iID ){ m_iIconGraphicID = iID; }
	void	SetGraphicModuleID( int iID ){ m_iModuleID = iID; }

	int		GetWidth(){ return m_iWidth; }
	int		GetHeight(){ return m_iHeight; }
	void	SetToolTip(CInfo& Info){ m_ToolTip = Info; }
	void	SetSlot( CSlot* pSlot );
	CSlot*	GetSlot();

	void	SetCommand( CTCommand* pCommand );
	void    SetDrawColor( DWORD color );

protected:
	POINT m_ptPosition;

	///Graphic
	int			m_iWidth;
	int			m_iHeight;
	int			m_iIconGraphicID;																/// 이미지 그래픽 ID
	int			m_iModuleID;																	/// 이미지 모듈 ID
	bool		m_bEnable;

	///Interface
	//CTDialog*	m_pParentDlg;
	CInfo		m_ToolTip;																		
	CSlot*		m_pSlot;																		/// Icon이 Slot에 Attach될경우 그 슬롯의 Pointer 저장
	CTCommand*	m_pCommand;																		/// Icon의 기본 실행 Command( 포인터만 가지고 있는다 )
	DWORD		m_dwIconDrawColor;																/// Icon을 Draw할때 Texture에 곱해질 색상값

};


/**
* 아바타의 상태를 표시할 아이콘
*	- 현재 파티창에서 파티원의 상태표시에만 사용된다.
*
* @Author		최종진
* @Date			2005/9/6
*/
class CIconStatus : public CIcon
{
public:
	CIconStatus();
	CIconStatus( int iModuleID, int iGraphicID );
	~CIconStatus(void){}

	virtual	void	Draw();
	virtual int		GetIndex();

	void SetGraphicID( int iModuleID, int iGraphicID );
};
#endif