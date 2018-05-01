
#ifndef _INTERFACE_TYPE_H
#define _INTERFACE_TYPE_H



/**
* 인터페이스 동작의 기본이 되는 Dialog들의 구분 Type
*
* @Author	최종진
* @Date		2005/9/5
*/

enum{
DLG_TYPE_NONE			= 0,
DLG_TYPE_MENU,			//1			//기본창 
DLG_TYPE_CHAT,			//2			//채팅창
DLG_TYPE_DIALOG,		//3			//npc 대화창, 기존질의창
DLG_TYPE_MAKE,			//4			//제조창
DLG_TYPE_COMMUNITY,		//5			//커뮤니티
DLG_TYPE_STORE,			//6			//상점
DLG_TYPE_DEAL,			//7			//거래창
DLG_TYPE_QUICKBAR,		//8			//퀵바 
DLG_TYPE_CHAR,			//9			//캐릭터창 
DLG_TYPE_MINIMAP,		//10		//미니맵창
DLG_TYPE_PARTYOPTION,	//11		//파티옵션창
DLG_TYPE_RESTART,		//12		//리스타트창
DLG_TYPE_SYSTEM,		//13		//시스템창
DLG_TYPE_QUEST,			//14		//퀘스트창
DLG_TYPE_EXCHANGE,		//15		//교환창
DLG_TYPE_SKILL,			//16		//스킬창
DLG_TYPE_SYSTEMMSG,		//17		//시스템 메세지창( 공지사항, 헬프...)
DLG_TYPE_N_INPUT,		//18		//숫자입력기
DLG_TYPE_MSGBOX,		//19		///공통 메세지 박스
DLG_TYPE_OPTION,		//20		//옵션 설정 창
DLG_TYPE_TARGET_MENU,	///21		///더이상 사용안하지만 인덱스 문제로 그대로 둔다.
DLG_TYPE_PARTY,			///22		///파티 관련 창
DLG_TYPE_BANK,			///23		///은행( 계정창고 )창
DLG_TYPE_HELP,			///24		///도움말창
DLG_TYPE_INFO,			///25	- 항상 화면에 보여지는 아바타의 정보창 
DLG_TYPE_MEMO,			///26	///PAT용 아이템 인벤토리&장착정보창
DLG_TYPE_ITEM,			///27
DLG_TYPE_PRIVATESTORE,	///28	- 자신이 개설한 개인상점
DLG_TYPE_AVATARSTORE,	///29	- 다른 사람이 개설한 개인상점을 이용시 사용되는 UI
DLG_TYPE_GOODS,			///30
DLG_TYPE_ADDFRIEND,		///31
DLG_TYPE_PRIVATECHAT,	///32
DLG_TYPE_CHATROOM,		///33
DLG_TYPE_MEMOVIEW,		///34
DLG_TYPE_SEPARATE,		///35
DLG_TYPE_UPGRADE,		///36
DLG_TYPE_CLAN,			/// 클랜 
DLG_TYPE_CLAN_ORGANIZE, /// 클랜 창설
DLG_TYPE_CLAN_NOTICE,	/// 공지 등록
DLG_TYPE_CHATFILTER,	///채팅 필터링 Setting 
DLG_TYPE_MSGBOX_SPECIAL,///특별한 메세지 박스 - 오직 한개만 화면에 띄울수 있다.
DLG_TYPE_SELECTEVENT,	
DLG_TYPE_EVENTDIALOG,	
DLG_TYPE_NOTIFY,			
DLG_TYPE_SKILLTREE,		/// 스킬 트리창
DLG_TYPE_DELIVERYSTORE,	///
DLG_TYPE_INPUTNAME,		/// 캐릭터 이름을 입력받는 창
DLG_TYPE_BANKWINDOW,	
DLG_TYPE_QUICKBAR_EXT,
DLG_TYPE_MAX
};
// *----------------------------------------------------------------* //
// *----------------------------------------------------------------* //

// 퀵슬롯에 등록되는 타입의 종류 
#define QUICK_TYPE_ITEM			1
#define QUICK_TYPE_ETC			2
// *----------------------------------------------------------------* //
#define	QUICKBAR_TYPE_NORMAL		0
#define	QUICKBAR_TYPE_EXTENSION		1

#endif