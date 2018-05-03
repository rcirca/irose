#ifndef _UICOMMAND_
#define _UICOMMAND_

#include "../../common/CItem.h"
#include "net_prototype.h"
#include "tgamectrl/tcommand.h"

class	CMakeDLG;
class	CItem;
/*--------------------------------------------------------------------------------------------------------*/
/// 인벤토리에서 제조창으로 아이템 이동
class CTCmdTakeInItem2MakeDlg : public CTCommand
{
public:
	CTCmdTakeInItem2MakeDlg(){}
	~CTCmdTakeInItem2MakeDlg( void ){}
	virtual bool Exec( CTObject* pObj );
};

/// 제조창에서 인벤토리로 아이템을 뺀다.
class CTCmdTakeOutItemFromMakeDlg : public CTCommand
{
public:
	CTCmdTakeOutItemFromMakeDlg(){}
	~CTCmdTakeOutItemFromMakeDlg( void ){}
	virtual bool Exec( CTObject* pObj );
};

/// 분리/분해창에서 아이템을 뺀다.
class CTCmdTakeOutItemFromSeparateDlg : public CTCommand
{
public:
	CTCmdTakeOutItemFromSeparateDlg(){}
	virtual ~CTCmdTakeOutItemFromSeparateDlg(){}
	virtual bool Exec( CTObject* pObj );
};

/// Pat아이템 장착
class CTCmdAssembleRideItem : public CTCommand
{
public:
	CTCmdAssembleRideItem(void){};
	virtual ~CTCmdAssembleRideItem( void ){}
	virtual bool Exec( CTObject* pObj );
};


/// Pat아이템 탈착
class CTCmdDisAssembleRideItem : public CTCommand
{
public:
	CTCmdDisAssembleRideItem(void){}
	virtual ~CTCmdDisAssembleRideItem(void){}
	virtual bool Exec( CTObject* pObj );

};
///*-----------------------------------------------------------------------------------------
/// 숫자 입력기 오픈
class CTCmdNumberInput;
class CTCmdOpenNumberInputDlg : public CTCommand
{
public:
	CTCmdOpenNumberInputDlg(void);
	virtual ~CTCmdOpenNumberInputDlg(void){}
	virtual bool Exec( CTObject* pObj );

	void	SetCommand( CTCmdNumberInput* pCmd );
	void	SetMaximum( __int64 i64Maximum );
protected:
	CTCmdNumberInput*		m_pCmd;
	__int64					m_i64Maximum;
};



/// 아이템창에서 다른 유저의 개인상점에 물풀을 팔때 사용되는 Command
class CTCmdDragItem2AvatarStoreDlg : public CTCmdOpenNumberInputDlg
{
public:
	CTCmdDragItem2AvatarStoreDlg(void){}
	virtual ~CTCmdDragItem2AvatarStoreDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 친구 등록 요청 수락
class CTCmdAcceptAddFriend : public CTCommand
{
public:
	CTCmdAcceptAddFriend( WORD wUserIdx, BYTE btStatus, const char* pszName );
	virtual ~CTCmdAcceptAddFriend(void){}
	virtual bool Exec(CTObject* pObj );

protected:
	WORD		m_wUserIdx;
	BYTE		m_btStatus;
	std::string m_strName;
};

/// 친구 등록 요청 거부
class CTCmdRejectAddFriend : public CTCommand
{
public:
	CTCmdRejectAddFriend( WORD wUserIdx , const char* pszName);
	virtual ~CTCmdRejectAddFriend(void){}
	virtual bool Exec( CTObject* pObj );

protected:
	WORD		m_wUserIdx;
	std::string m_strName;
};

/// 친구 리스트에서 친구 삭제
class CTCmdRemoveFriend : public CTCommand
{
public:
	CTCmdRemoveFriend( DWORD dwUserTag );
	virtual ~CTCmdRemoveFriend(){}
	virtual bool Exec( CTObject* pObj );

protected:
	DWORD	m_dwUserTag;
};
/// 퀘스트 포기
class CTCmdAbandonQuest : public CTCommand
{
public:
	CTCmdAbandonQuest( int iQuestSlotIdx,int iQuestID );
	virtual ~CTCmdAbandonQuest(){}
	virtual bool Exec( CTObject* pObj );

protected:
	int m_iQuestSlotIdx;
	int m_iQuestID;
};

/// 아이템을 버리는 패킷 전송
class CTCmdSendPacketDropItem : public CTCommand
{
public:
	CTCmdSendPacketDropItem( short nInvenIdx, int iQuantity );
	virtual ~CTCmdSendPacketDropItem(){}
	virtual bool Exec( CTObject* pObj );
protected:
	short	m_nInvenIdx;
	int		m_iQuantity;
};


/// Dialog Icon을 메뉴창에서 드래그시작
class CTCmdMoveDialogIcon2GroundFromMenu : public CTCommand
{
public:
	CTCmdMoveDialogIcon2GroundFromMenu(){}
	virtual ~CTCmdMoveDialogIcon2GroundFromMenu(){}
	virtual bool Exec( CTObject* pObj );
};

/// Dialog Icon을 바탕화면에서 드래그시작
class CTCmdMoveDialogIcon2Ground : public CTCommand
{
public:
	CTCmdMoveDialogIcon2Ground(){}
	virtual ~CTCmdMoveDialogIcon2Ground(){}
	virtual bool Exec( CTObject* pObj );
};

/// Dialog Icon을 바탕화면에서 퀵바에 등록
class CTCmdRegistDialogIcon2QuickBar : public CTCommand
{
public:
	CTCmdRegistDialogIcon2QuickBar(){}
	virtual ~CTCmdRegistDialogIcon2QuickBar(){}
	virtual bool Exec( CTObject* pObj );
};

/// Dilaog Icons을 It_Mgr의 Container에서 빼는 Command
class CTCmdRemoveDialogIcon : public CTCommand
{
public:
	CTCmdRemoveDialogIcon( int iDialogType );
	virtual ~CTCmdRemoveDialogIcon(){}
	virtual bool Exec( CTObject* pObj );
protected:
	int		m_iDialogType;

};

/// 해당 아이템을 Wish List에 추가하는 Command
class CTCmdAddItem2WishList : public CTCommand
{
public:
	CTCmdAddItem2WishList( tagITEM& Item );
	virtual ~CTCmdAddItem2WishList(){}
	virtual bool Exec( CTObject* pObj );
protected:
	tagITEM	m_Item;
};

/// 해당 아이템을 Wish List에 추가하는 Command
class CTCmdChangeStateUpgradeDlg : public CTCommand
{
public:
	CTCmdChangeStateUpgradeDlg( int iState );
	virtual ~CTCmdChangeStateUpgradeDlg(){};
	virtual bool Exec( CTObject* pObj );

protected:
	int m_iState;
};

/// ActionListener에서 직접 EndRepair/EndAppraisal를 호출시 에러남 .. 비동기적으로 끝내기 위한 커멘드
/// 수리 종료
class CTCmdEndRepair : public CTCommand
{
public:
	CTCmdEndRepair(){}
	virtual ~CTCmdEndRepair(){}
	virtual bool Exec( CTObject* pObj );
};

/// 아이템 감정 종료
class CTCmdEndAppraisal : public CTCommand
{
public:
	CTCmdEndAppraisal(){}
	virtual ~CTCmdEndAppraisal(){}
	virtual bool Exec( CTObject* pObj );
};

/// 감정요청
class CTCmdSendAppraisalReq : public CTCommand
{
public:
	CTCmdSendAppraisalReq( int iIndex );
	virtual ~CTCmdSendAppraisalReq(){}
	virtual bool Exec( CTObject* pObj );
protected:
	int		m_iIndex;
};

/// 다른 유저의 개인상점에서 파는 물품 리스트 요청
class CTCmdQuerySellItem2PrivateStore : public CTCommand
{
public:
	CTCmdQuerySellItem2PrivateStore( WORD wSellerSvrIdx, int iItemCount, tagSELL_ITEM& Item );
	virtual ~CTCmdQuerySellItem2PrivateStore(){}
	virtual bool Exec( CTObject* pObj );

protected:
	WORD			m_wSellerSvrIdx;
	int				m_iItemCount;
	tagSELL_ITEM	m_SellItem;
};

/// 다른 유저의 개인상점에서 사는 물품 리스트 요청
class CTCmdQueryBuyItemFromPrivateStore : public CTCommand
{
public:
	CTCmdQueryBuyItemFromPrivateStore( WORD wSellerSvrIdx, int iItemCount, tagPS_SLOT_ITEM& Item );
	virtual ~CTCmdQueryBuyItemFromPrivateStore(){}
	virtual bool Exec( CTObject* pObj );
protected:
	WORD			m_wSellerSvrIdx;
	int				m_iItemCount;
	tagPS_SLOT_ITEM m_BuyItem;
};

/// 클랜 가입 요청 승낙
class CTCmdAcceptReqJoinClan : public CTCommand
{
public:
	CTCmdAcceptReqJoinClan( const char* pszMasterName );
	virtual ~CTCmdAcceptReqJoinClan(){};
	virtual bool Exec( CTObject* pObj );
protected:
	std::string			m_strMasterName;
};

/// 클랜 가입 요청 거부
class CTCmdRejectReqJoinClan : public CTCommand
{
public:
	CTCmdRejectReqJoinClan( const char* pszMasterName );
	virtual ~CTCmdRejectReqJoinClan(){};
	virtual bool Exec( CTObject* pObj );
protected:
	std::string			m_strMasterName;
};

/// 서버로 클랜 명령어 전송
class CTCmdClanCommand : public CTCommand
{
public:
	CTCmdClanCommand( BYTE btCmd, const char* pszMsg );
	virtual ~CTCmdClanCommand(){}
	virtual bool Exec( CTObject* pObj );

protected:
	BYTE				m_btCmd;
	std::string			m_strMsg;
};

/// 서버와의 접속 종료 대기중 취소
class CTCmdCancelWaitDisconnect : public CTCommand
{
public:
	CTCmdCancelWaitDisconnect(){}
	virtual ~CTCmdCancelWaitDisconnect(){}
	virtual bool Exec( CTObject* pObj );
};

/// 이름 입력을 받을때 쓰이는 커맨드의 기본 클래스
class CTCmdInputName : public CTCommand
{
public:
	CTCmdInputName(){}
	virtual ~CTCmdInputName(){}
	void SetName( const char* pszName );
	virtual bool Exec( CTObject* pObj );
protected:
	std::string  m_name;
};

/// 몰아이템 창고에서 받을 사람이 존재하는지 서버에 의뢰요청
class CTCmdInputNameGiftReceiver : public CTCmdInputName
{
public:
	CTCmdInputNameGiftReceiver(){}
	virtual ~CTCmdInputNameGiftReceiver(){}
	virtual bool Exec( CTObject* pObj );
};

/// 몰아이템 창고의 아이템을 다른 유저에게 보낸다.
class CTCmdGiftMallItem : public CTCommand
{
public:
	CTCmdGiftMallItem( BYTE slotindex, const char* name );
	virtual ~CTCmdGiftMallItem(){}
	virtual bool Exec( CTObject* pObj );
protected:
	BYTE		m_slotindex;
	std::string m_name;
};

/// 클랜마크를 등록한다.( 서버로 )
class CTCmdRegisterClanMark : public CTCommand
{
public:
	CTCmdRegisterClanMark( int clan_id, const char* filename );
	virtual ~CTCmdRegisterClanMark(){}
	virtual bool Exec( CTObject* pObj );
protected:
	int		m_clan_id;
	std::string m_filename;
};

/// IT_MGR의 STATE를 변경한다.
class CTCmdChangeStateInterface : public CTCommand
{
public:
	CTCmdChangeStateInterface( int state );
	virtual ~CTCmdChangeStateInterface(){}
	virtual bool Exec( CTObject* pObj );
protected:
	int		m_state;
};

#endif