#ifndef _IT_MGR_H
#define _IT_MGR_H

#include "InterfaceType.h"



#include "dlgs/CMsgBox.h"
#include "command/CTCmdOpenDlg.h"

#include "../common/citem.h"
#include "../Game.h"


#include "TGameCtrl.h"
#include "TCommandQ.h"

#include <list>

struct t_PACKET;



class CBasePAN;
class CChatDLG;
class CMakeDLG;
class CDialogDlg;
class CInventoryDLG;
class CInventoryDLG;
class CDealDLG;
class CQuickBAR;
class CCharacterDLG;
class CMinimapDLG;
class CTargetObjectDLG;
class CRestartDLG;
class CSystemDLG;
class CSkillDLG;
class CExchangeDLG;
class CTDialog;
class CTDrawImpl;
class CTCommand;
class CTCmdNumberInput;
class CInventory;
class CSkillSlot;
class CAvatarInfoDlg;
class CPrivateChatDlg;
class CTCmdInputName;
class CITState;
class CTEditBox;
class CTDrawImpl;

typedef std::list< CTDialog* >			list_dlgs;
typedef list_dlgs::iterator				list_dlgs_itor;
typedef list_dlgs::reverse_iterator		list_dlgs_ritor;

class CReloadProcess;
class CIconDialog;
class CNotifyButtonDlg;

/**
* 캐릭터 선택후 게임을 나가거나 캐릭터 선택창으로 다시 이동하기 전까지의 인터페이스 동작 관리자
*  - WM_LBUTTONDOWN이 일어나거나 Show가 된Dlg는 리스트의 맨뒤로 옮겨서 그리기는 맨 마지막으로 하고 처리는 제일 먼저 하게 한다.
*
*
* @Author		최종진
*
* @Date			2005/9/5
*
*/
class IT_MGR
{
	/// IT_MGR의 멤버에 접근이 필요한 클래스들을 friend로 선언
	friend class CTCmdCreateMsgBox;				/// 메세지창을 새로 여는 TCommand
	friend class CTCmdDeleteMsgBox;				/// 메세지창을 닫는 TCommand
	friend class CTCmdClosePrivateChatDlg;		/// 1:1대화창을 닫는 TCommand
	friend class CTCmdOpenPrivateChatDlg;		/// 1:1대화창을 새로 TCommand

	/// IT_MGR의 State들
	friend class CITStateNormal;				
	friend class CITStateRepair;
	friend class CITStateAppraisal;
	friend class CITStateDead;
public:
	IT_MGR();
	~IT_MGR();

	enum{											/// 채팅 입력 방식 타입
		INPUTTYPE_NORMAL,							/// 채팅시마다 Enter를 입력하고 다시 엔터를 입력시 채팅모드는 종료된다.
		INPUTTYPE_AUTOENTER,						/// Show시에 해당 Dialog에 EditBox가 있다면 자동 입력된다.Enter후 자동으로 다시 Focus를 가진다.
	};

	enum{											/// 채팅글 타입	
		CHAT_TYPE_ALL,
		CHAT_TYPE_SHOUT,
		CHAT_TYPE_PARTY,
		CHAT_TYPE_WHISPER,
		CHAT_TYPE_NOTICE,
		CHAT_TYPE_SYSTEM,
		CHAT_TYPE_ERROR,
		CHAT_TYPE_QUEST,
		CHAT_TYPE_QUESTREWARD,
		CHAT_TYPE_CLAN,
		CHAT_TYPE_TRADE,
		CHAT_TYPE_ALLIED,
		CHAT_TYPE_FRIEND,
		CHAT_TYPE_MAX,
	};

	enum {											/// 상태 타입
		STATE_NORMAL,
		STATE_REPAIR,
		STATE_APPRAISAL,
		STATE_DEAD,
		STATE_WAITDISCONNECT,
		STATE_NPCDIALOG,
		STATE_RIDECART,
		STATE_MAX,
	};

	enum{
		QUERYTYPE_NPC,								/// NPC 대화
		QUERYTYPE_ITEM,								/// 아이템 사용시 사용되는 대화창
		QUERYTYPE_SELECT,							/// 선택문만이 나열되어 유저가 선택하는 대화창( 루나마을의 워프게이트 )
	};

	/// 게임내 모든 스트링 조합용 버퍼로 사용..
	char m_MessageBuf[ 1024 ];


	bool Init();
	void Free();
	UINT MsgProc(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	void Update();
	
	
	///코딩시에는 이곳에서 하고나서 안정화되면 CRecvPacket으로 이동시키자
	///bool PacketProc( t_PACKET * pRecvPacket );
	//void RegToolTip( const CInfo& Info );
	// *--------------------------------------------------------------* //
	void InitDLG();
	void FreeDLG();
	void DrawDLGs(POINT ptMouse);


	CChatDLG*			GetChatDLG();
	CDialogDlg*			GetQueryDLG();
	CQuickBAR*			GetQuickBAR();
#ifdef _NEW_UI
	CQuickBAR*			GetQuickBAR_EXT(); // 2nd skillbar
#endif
	CMinimapDLG*		GetMinimapDLG();
	CExchangeDLG*		GetExchangeDLG();
	CPrivateChatDlg*	GetPrivateChatDlg( DWORD dwUserTag );

	
	void				OpenPrivateChatDlg( DWORD dwUserTag, BYTE btStatus, const char* pszName );			/// 1:1 대화창 오픈


	// *------------------------------------------------------------------------------------------------------------------------------* //
	/// NPC 대화창 오픈
	bool	OpenQueryDLG(short nSX,short nSY,char* szQuery,short nMaxCharLen,int iEventID,void (*fpCloseProc)(int iEventID), int iEventOwner, int iEventDlgType );

	void	CloseQueryDlg();																				/// NPC대화창을 닫는다
	
	bool	QueryDLG_AppendExam(char* szExam,int iEventID,void (*fpExamEvent)(int iEventID), int iType );	/// NPC 대화창에서 선택문을 추가한다.
	// *------------------------------------------------------------------------------------------------------------------------------* //

	void	UpdateQuickSlot();																				/// CHotIconSlot 정보를 바탕으로 QuickDlg 의 슬롯들을 업데이트
	
	short	GetItemInventoryTAB(tagITEM sItem);																/// 아이템창에서 아이템이 들어갈 탭을 얻어온다 
	
	void	Close_store();																					/// NPC상점을 닫는다 



	void	ShowNumberInputDlg( CTCmdNumberInput* pCmd , CTObject* pCmdParam ,__int64 iMaxNumber = -1);		/// 숫자 입력창 열기
	
	bool	OpenMakeDlg( int iSkillSlotIdx );																/// 제조창 열기

	void	OpenDialogBySkill( int iSkillSlotIdx  , int iTargetDialogID );									/// 스킬로 다이얼로그를 열때 사용되는 Method

	bool	OpenInputNameDlg( CTCmdInputName* pCmd );														/// 이름 입력창 열기

	/*-----------------------------------------------------------------------------*/
	/// 메세지 박스 여는 methods
	void OpenMsgBox( const char* szMsg, 
		short int iButtonType = CMsgBox::BT_OK , bool	bModal = true,
		unsigned int iInvokerDlgID =  0, CTCommand* pCmdOk = NULL, CTCommand* pCmdCancel = NULL ,
		int iMsgType = 0
		);
	
	void	OpenMsgBox_CartRide( const char* szMsg, 
		short int iButtonType = CMsgBox::BT_OK , bool	bModal = true,
		unsigned int iInvokerDlgID =  0, CTCommand* pCmdOk = NULL, CTCommand* pCmdCancel = NULL ,
		int iMsgType = 0, WORD parm1 = 0, WORD parm2 = 0
		);

	void	OpenMsgBox2(CreateMsgBoxData& Data);

	/*---------------------------------------------------------------------------*/
	void		PostMsg2Dlg( int dialog_id, unsigned msg_id, unsigned param1, unsigned param2 );
	void		AppendChatMsg( const char* pszMsg, int iType ,DWORD forceapply_color = 0 );


	void		OpenDialog( int iDlgType, bool bToggle = true , int iPosX = -1, int iPosY = -1 );	/// 다이얼로그를 연다
	void		CloseDialog( int iDlgType );														/// 다이얼로그를 닫는다.

	CTDialog*	FindDlg( DWORD dwDlgType );												/// 해당 ID의 다이얼로그를 찾아서 포인터를 리턴
	
	CMsgBox*	FindMsgBox( int iMsgType );												/// 해당 타입의 메세지 박스의 포인터를 리턴

	int			IsDlgOpened( int iDlgID );												/// 해당 ID의 다이얼로그가 열려 있는가?
	bool		IsOpenedMsgBox( int iType );											/// 해당 타입의 메세지 박스가 1개 이상 열려 있는가?

	bool		AddTCommand( int iDlgID, CTCommand* pCmd );								/// Command를 Queue에 추가

	/*---------------------------------------------------------------------------*/
	void		InitInterfacePos();														/// 인터페이스 초기화, 해상도가 바뀌거나 인터페이스를 최초 위치로 옮기고 싶을때 사용하는 Method
	void		SetInterfacePosBySavedData();											///	저장되있던 Dialog Position이 있을경우 가져와서 Dialog의 위치들을 수정한다.
	void		SetInterfacePos_After();												/// 가장 마지막에 강제로 Pos 셋팅

	void		ServerDisconnected();													/// 서버와의 연결이 끊어졌을경우 처리하는 method
	
	
	bool		IsMouseOnInterface();													/// 현재 마우스가 인터페이스위에 있는가?

	/*---------------------------------------------------------------------------*/
	void		AddDialogIcon( int iDialogType );
	void		DelDialogIcon( int iDialogType );
	CIconDialog* FindDialogIcon( int iDialogType );

	void		SetDialogIconFromMenu( CIconDialog* pIcon );
	void		DelDialogiconFromMenu();
	void		SetDialogIconPosition( int iDialogType, POINT pt );

	/*---------------------------------------------------------------------------*/
	/// 상태 변경에 따른 인터페이스 변경 methods
	void		BeginRepair();															/// 수리 시작
	void		BeginAppraisal();														/// 아이템감정  시작
	void		EndRepair();															/// 수리상태 끝
	void		EndAppraisal();															/// 아이템감정 끝

	//void	BeginRepairItem();

	/*---------------------------------------------------------------------------*/
	int			GetState();																/// 현재 상태를 구하는 method
	void		ChangeState( int iIndex );												/// 상태 변경 method

	/*---------------------------------------------------------------------------*/
	void		CloseDialogAllExceptDefaultView();										/// 기본적으로 보이는 다이얼로그를 제외한 모든 다이얼로그를 닫는다.

	/*---------------------------------------------------------------------------*/
	void		ShowQuestStartMessage( int index );										/// 퀘스트 시작시 채팅창에 표시하는 method

	/*---------------------------------------------------------------------------*/
	void		AddNotifybutton( int iEventIndex );										/// 알림버튼을 추가한다.
	void		ClearNotifyButtons();													/// 모든 알림버튼을 닫는다.
	
	/*---------------------------------------------------------------------------*/
	CTEditBox*	GetFocusEditBox();														/// 현재 입력 포커스를 가진 TEditBox의 포인터를 구한다.

	/*---------------------------------------------------------------------------*/
	/// 전투시 혹은 비전투시에 서버와의 접속 종료를 지연하기 위한 Methods
	void		SetWaitDisconnectTime( int iSec );
	int			GetWaitDisconnectTime();

	void		SetWaitDisconnectType( int iType );
	int			GetWaitDisconnectType();
	/*---------------------------------------------------------------------------*/
	CReloadProcess*				GetReloadProcess();
	CTDrawImpl*					GetDrawImplPtr();

	void		RefreshDlg();															//  해상도 변경에 따른 위치 재조정.

private:
	void		ProcCommand();															/// Command Queue에 저장되어 있는 TCommand를 실행한다.

	void		AppendDlg(short nDlgType,CTDialog* pDLG, int iID = 0);					/// Dialog들을 보관하는 컨테이너에 추가
	void		InsertDlg(short nDlgType, CTDialog* pDlg,  int iNextID , int iID = 0 );	/// 컨테이너에서 특정 Dialog뒤에 해당 다이얼로그를 추가한다.

	void		MoveDlg2ListEnd( CTDialog* pDlg );										/// 특정 Dialog를 리스트의 맨뒤로 이동 ( 화면에서 제일위로 오게된다)
	void		MoveDlgIcon2ListEnd( CIconDialog* pIcon );								/// 특정 Dialog Icon을 리스트의 맨뒤로 이동 ( 화면에서 제일위로 오게된다)

	void		ProcessEscKeyDown();													/// ESC 키가 눌렸을때 처리하는 Method

private:
	list_dlgs		m_Dlgs;																/// 게임내 사용되는 인터페이스 다이얼로그들을 보관하는 컨테이너
	CTCommandQ		m_CommandQ;															/// Command Queue( 다이얼로그에 종속적인 Command는 각 다이얼로그가 가지고 있는다)

	/*---------------------------------------------------------------------------*/
	/// 사용자가 Esc키로 열려있는 창들의 상태를 저장(열린상태만)할수 있는 시스템을 위한 Member들	
	std::list< int >				m_listSavedDialog;									///Set에 들어갈수있는 Dialog들의 ID
	std::list< std::list< int > >	m_StackDialogSet;									///Set이 저장되어질 Stack;
	/*---------------------------------------------------------------------------*/

	std::list< CIconDialog* >		m_Icons;											/// 열린 다이얼로그들이 화면상에 아이콘화 되었을때 저장되는 컨테이너
	std::map< int, POINT >			m_ptIcons;											/// 아이콘화된 다이얼로그의 위치를 저장해두는 컨테이너

	CIconDialog*					m_pDialogIconFromMenu;								/// 메뉴창에서 버튼을 드래그할때 필요한 기본 아이콘을 보관한다.
	CNotifyButtonDlg*				m_pNotifyButtonDlg;									/// 알림버튼을 저장해둘 컨테이너 다이얼로그

	/*---------------------------------------------------------------------------*/
	//States
	CITState*	m_pCurrState;
	CITState*	m_pStates[ STATE_MAX ];
	/*---------------------------------------------------------------------------*/


	int			m_iWaitDisconnectTime;
	int			m_iWaitDisconnectType;													///0. Disconnect 1. Goto Select Avatar

	CReloadProcess*				m_pReloadProcess;										/// 아이템및 스킬의 쿨타임을 아이콘위에 표시하기 위한 멤버


};

extern IT_MGR			g_itMGR;

#define g_MsgBuf g_itMGR.m_MessageBuf


#endif