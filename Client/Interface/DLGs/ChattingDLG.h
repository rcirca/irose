
#ifndef _CHATTING_DLG_H
#define _CHATTING_DLG_H
#include <vector>
#include <queue>
#include "TDialog.h"
#include "IActionListener.h"

/// 0: false, 1: true, 2:true(can't change)
struct ChatFilter{
	BYTE	Filters[6];
};


/**
* 채팅 입/출력을 위한 다이얼로그
*	- 채팅명령어( 외치기등 )는 2바이트문자에서도 동작가능하도록 유니코드로 2개를 저장
*	- GM명령어중 채팅금지 기능 구현( 클라이언트에서만 구현되어 있으면 재 접속시 해당 기능정지 )
*
* @Todo			GM명령어중 채팅금지기능을 서버에서 구현하거나 저장후 다시 가져오는 방식으로 수정 필요
* @Author		최종진
* @Date			2005/9/12
*/
class CChatDLG : public CTDialog, public IActionListener
{

public:
	CChatDLG();
	virtual ~CChatDLG();

	virtual bool			Create( const char* IDD );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void			Update( POINT ptMouse );
	virtual void			Show();

	virtual unsigned		ActionPerformed( CActionEvent* e );


	ChatFilter& GetCurrentTabFilter( );
	void		SetCurrentTabFilter( ChatFilter& filter );


	enum{
		IID_BTN_FILTER = 10,
		IID_EDITBOX	= 15,

		IID_LISTBOX_ALL = 20,
		IID_SCROLLBAR_ALL = 21,

		IID_LISTBOX_WHISPER = 25,
		IID_SCROLLBAR_WHISPER = 26,

		IID_LISTBOX_TRADE	= 30,
		IID_SCROLLBAR_TRADE = 31,

		IID_LISTBOX_PARTY = 35,
		IID_SCROLLBAR_PARTY = 36,

		IID_LISTBOX_CLAN = 40,
		IID_SCROLLBAR_CLAN = 41,

		IID_LISTBOX_ALLIED = 45,
		IID_SCROLLBAR_ALLIED = 46,

		IID_RADIOBOX	= 50,
		IID_BTN_ALL		= 51,
		IID_BTN_WHISPER = 52,
		IID_BTN_TRADE   = 53,
		IID_BTN_PARTY	= 54,
		IID_BTN_CLAN	= 55,
		IID_BTN_ALLIED  = 56,


		IID_BTN_CAPTURE = 60,

		IDD_LISTBOX_TOP = 75,
		IDD_SCROLLBAR_TOP = 76,
		IDD_BTN_SCROLLDOWN = 100,
	};
	

	enum
	{
		FILTER_NORMAL,
		FILTER_SYSTEM,
		FILTER_TRADE,
		FILTER_PARTY,
		FILTER_CLAN,
		FILTER_WHISPER,
		FILTER_ALLIED,
	};

	void		SetChatBlock( DWORD BlockTime );

	void		SetChatUnBlock();

	void		AppendMsg( const char* pszMsg, DWORD color, int iType );
	void		AppendMsg2( const char* pszMsg, DWORD color, int iType );
	void		AppendMsg2System( const char* pszMsg, DWORD color);
	void		SendChatMsgRepeat();
	void		SetEnableShoutRestrict();									/// 외치기 시간 제한 설정
	void		SetDisableShoutRestrict();									/// 외치기 시간 제한 없앰

	int			GetLineCount();
	const char* GetLineString( int index );

protected:

	bool	On_LButtonUP( unsigned iProcID);
	void	OnLButtonDown( unsigned uiProcID, WPARAM wParam , LPARAM lParam );

	void	SendChatMsg( char* szMsg );


	enum{
		TAB_ALL = 1,
		TAB_WHISPER,
		TAB_TRADE,
		TAB_PARTY,
		TAB_CLAN,
		TAB_ALLIED,
		TAB_MAX
	};
	void	TabSelected( int iTab );




	enum{
		CHAT_NORMAL,
		CHAT_SHOUT,
		CHAT_WHISPER,
		CHAT_EXCHANGE,
		CHAT_PARTY,
		CHAT_OPEN_HELP,
		CHAT_CLAN,
		CHAT_TRADE,
		CHAT_ALLIED,
	};

	int		ChatParser( string stMsg, string& stRealMsg, string& stTargetID );
	
	bool	IsChatBlock();
	void	SetFocusEditBox( unsigned iID );
	int		GetActiveListBox();
	int		GetActiveScrollBar();
	void	AppendMsg2sys( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2All( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2Whisper( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2Trade( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2Party( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2Clan( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2Allied( const char* pszMsg, DWORD dwColor , int iFilterType );
	void	AppendMsg2ListBox( int iListBoxID, int iScrollBarID , const char* pszMsg, DWORD dwColor );

	void	ActiveListBoxMoveEnd();

protected:
	/// 도배 금지 기능에 사용되는 멤버들
	queue< DWORD >	m_PrevSendMessageTimes;
	int				m_iCheckChatCount;///3
	DWORD			m_dwCheckChatTime;///5 * 1000

	/// GM명령어중 채팅금지에 사용되는 멤버들
	bool			m_bChatBlock;				
	DWORD			m_dwChatBlockStartTime;
	DWORD			m_dwChatBlockTime;

	/// 외치기 시간 제한에 사용되는 멤버
	bool	m_bCheckShoutRestrict;
	DWORD   m_dwShoutRestrictTime;
	DWORD   m_dwPrevUseShoutTime;
	


	//DWORD   m_dwChatRestrictTime;	///사용안함
	//DWORD   m_dwPrevSendChatTime; ///사용안함

	std::string				m_strLastSendMsg;					/// 마지막 서버에 보낸 입력글 저장
	int						m_iSelectedTab;						/// 현재 탭 구분

	short					m_nChatLineCNT;						/// 채팅 라인수 
	static					int m_iCaptureCount;	

	vector< ChatFilter >	m_Filters;				


	/// 다국어 지원용 : 2바이트문자에서도 동작할수 있도록 유니코드로 등록
	std::vector< std::wstring >		m_ShoutCommands;			
	std::vector< std::wstring > 	m_TradeCommands;
	std::vector< std::wstring >		m_WhisperCommands;
	std::vector< std::wstring >		m_ClanCommands;
	std::vector< std::wstring >		m_PartyCommands;
	std::vector< std::wstring >		m_HelpCommands;
	std::vector< std::wstring >		m_AlliedCommands;
	std::vector< std::wstring >		m_Spaces;
};
#endif