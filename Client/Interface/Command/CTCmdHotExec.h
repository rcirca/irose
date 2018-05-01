#ifndef _CTCMDHOTEXEC_
#define _CTCMDHOTEXEC_
#include "tcommand.h"

class CTObject;

/// 클라이언트 종료 
class CTCmdExit : public CTCommand
{
public:
	CTCmdExit(void);
	~CTCmdExit(void);
	virtual bool	Exec( CTObject* );
};

/// 서버 디스컨넥트 시 재로그인( __SRVDISCONNECTRELOGIN 전처리기가 선언되어있을 경우만 작동)
class CTCmdReLogin : public CTCommand
{
public:
	CTCmdReLogin(void);
	~CTCmdReLogin(void);
	virtual bool	Exec( CTObject* );
};

/// 특정 Dialog 닫기
class CTCmdHideDialog : public CTCommand
{
public:
	CTCmdHideDialog( void ){}
	CTCmdHideDialog( int iDialogID ){ m_iDialogID = iDialogID; }
	~CTCmdHideDialog( void ){}
	virtual bool Exec( CTObject* );
protected:
	int m_iDialogID;
};

/// NPC상점에서의 거래 종료
class CTCmdCloseStore : public CTCommand
{
public:
	CTCmdCloseStore( void ){}
	~CTCmdCloseStore(void){}
	virtual bool Exec( CTObject* );
};

/// 1:1 거래 요청 승낙
class CTCmdAcceptTradeReq : public CTCommand
{
public:
	CTCmdAcceptTradeReq( void ){}
	CTCmdAcceptTradeReq( WORD  w ){ m_wReqServerObjectIdx = w; }
	~CTCmdAcceptTradeReq(void){}
	virtual bool Exec( CTObject* );
protected:
	WORD m_wReqServerObjectIdx;
};

/// 1:1 거래 요청 거부 
class CTCmdRejectTradeReq : public CTCommand
{
public:
	CTCmdRejectTradeReq(void){}
	CTCmdRejectTradeReq( WORD w ){ m_wReqServerObjectIdx = w; }
	~CTCmdRejectTradeReq(void){}
	virtual bool Exec( CTObject* );

protected:
	WORD m_wReqServerObjectIdx;
};

/// 파티 요청을 한다.
class CTCmdSendPacketPartyReq : public CTCommand
{
public:
	CTCmdSendPacketPartyReq( void ){}
	CTCmdSendPacketPartyReq( BYTE btRequest, DWORD dwObjectIDXorTAG )
	{
		m_btRequest = btRequest; 
		m_dwObjectIDXorTAG = dwObjectIDXorTAG; 
	}
	~CTCmdSendPacketPartyReq( void ){}
	virtual bool	Exec( CTObject* );

protected:
	BYTE m_btRequest;
	DWORD m_dwObjectIDXorTAG;
};

/// 파티 요청에 대한 응답을 보낸다.
class CTCmdSendPacketPartyReply : public CTCommand
{
public:
	CTCmdSendPacketPartyReply( void ){}
	CTCmdSendPacketPartyReply( BYTE btReply, WORD wObjSvrIdx ){ m_btReply = btReply; m_wObjSvrIdx = wObjSvrIdx; }
	~CTCmdSendPacketPartyReply( void ){}
	virtual bool	Exec( CTObject* );
protected:
	BYTE m_btReply;
	WORD m_wObjSvrIdx;
};

/// 파티 초대 승낙
class CTCmdAcceptPartyJoin : public CTCommand
{
public:
	CTCmdAcceptPartyJoin( void ){}
	CTCmdAcceptPartyJoin( WORD wDestObjSvrIdx ){ m_wDestObjSvrIdx = wDestObjSvrIdx; }
	~CTCmdAcceptPartyJoin( void ){}
	virtual bool Exec( CTObject* );

protected:
	WORD m_wDestObjSvrIdx;
};

/// 파티 초대 거부
class CTCmdRejectPartyJoin : public CTCommand
{
public:
	CTCmdRejectPartyJoin( void ){}
	CTCmdRejectPartyJoin( WORD wDestObjSvrIdx ){ m_wDestObjSvrIdx = wDestObjSvrIdx; }
	~CTCmdRejectPartyJoin( void ){}
	virtual bool Exec( CTObject* );
protected:
	WORD m_wDestObjSvrIdx;
};

/// 제조 Dialog의 상태 변경
class CTCmdChangeState : public CTCommand
{
public:
	CTCmdChangeState(int i){ m_iState = i; }
	~CTCmdChangeState(void){};
	virtual bool Exec( CTObject* );
protected:
	int			m_iState;
};

/// 파티를 떠난다.
class CTCmdLeaveParty : public CTCommand
{
public:
	CTCmdLeaveParty(void){}
	virtual ~CTCmdLeaveParty(void){}
	virtual bool Exec( CTObject* );
};


// 2인승 카트
class CTCmdAcceptCartRide : public CTCommand
{
public:
	CTCmdAcceptCartRide( void ){}
	CTCmdAcceptCartRide( WORD wOwnerIDX, WORD wGuestIDX ){ m_wOwnerIDX = wOwnerIDX;	m_wGuestIDX = wGuestIDX;  }
	~CTCmdAcceptCartRide( void ){}
	virtual bool Exec( CTObject* );

protected:
	WORD m_wOwnerIDX;
	WORD m_wGuestIDX;
};

// 2인승 카트
class CTCmdRejectCartRide : public CTCommand
{
public:
	CTCmdRejectCartRide( void ){}
	CTCmdRejectCartRide( WORD wOwnerIDX, WORD wGuestIDX ){ m_wOwnerIDX = wOwnerIDX;	m_wGuestIDX = wGuestIDX;  }
	~CTCmdRejectCartRide( void ){}
	virtual bool Exec( CTObject* );

protected:
	WORD m_wOwnerIDX;
	WORD m_wGuestIDX;
};


#endif