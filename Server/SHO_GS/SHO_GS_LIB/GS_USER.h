/**
 * \ingroup SHO_GS
 * \file	GS_USER.h
 * \brief	CObjAVT를 상속받으며 사용자의 패킷을 처리
 */
#ifndef __GS_USER_H
#define __GS_USER_H
#include "iocpSOCKET.h"
#include "CSLList.h"

#include "CObjCHAR.h"
#include "CObjNPC.h"
#include "CObjAVT.h"
#include "GS_Party.h"
#include "Calculation.h"

#ifdef	__INC_WORLD
	#include "CDLList.h"
	#include "CThreadMSGR.h"
#endif

#if defined(__N_PROTECT) && !defined(__NORTHUSA)
	#include "../../../GG_SDK/CSAuth2/include/ggsrv.h"
#endif

//---------------------------------------------------------------------------

extern CPacketCODEC *g_pPacketCODEC;

#define	SOCKET_KEEP_ALIVE_TIME	( 5 * 60 * 1000 )		// 5분

#define	PENALTY_EXP_TOWN	3	// 마을 부활 패널티 경험치 5%
#define	PENALTY_EXP_FIELD	5	// 필드 부활 패널치 경험치 8%


// #define	USER_STEP_CONNECT		0
// #define	USER_STEP_TO_DISCON		999

enum {
	CHEAT_INVALID=0,
	CHEAT_NOLOG=1,
	CHEAT_SEND,
	CHEAT_PROCED,
} ;

// #define	RIGHT_MM			0x00010
#define	RIGHT_TWG			0x00080			// 타이완 GM ( 256 과 512의 사이.. )
#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..

#define	IS_HACKING(pUSER, fn)		pUSER->IsHacking(fn, __FILE__, __LINE__)


#define	CHECK_ITEM_LOG_PRICE	10000		// 아이템 로그 남기는 장비 가격
#define	CHECK_ITEM_SN_PRICE		10000		// 아이템 SN 붙는 장비 가격
#define	CHECK_ITEM_MONEY		5000		// 로그 남기는 드롭 가격
#define	CHECK_ITEM_LOG_CNT		10			// 로그 남기는 드롭 갯수

#define	BIT_TRADE_READY			0x01	// 1 : 1 거래시
#define	BIT_TRADE_DONE			0x02	// 1 : 1 거래시
#define BIT_TRADE_P_STORE		0x04	// 개인 상점 거래시..
struct tagTradeITEM {
	short		m_nInvIDX;
	tagITEM		m_Item;
} ;

struct tagMallITEM {
	UINT64		m_ui64ID;
	tagBaseITEM	m_ITEM;
} ;
#define	MAX_MALL_ITEM_COUNT		48

struct tagMallDATA {
	tagMallITEM	m_ITEMS[ MAX_MALL_ITEM_COUNT ];
	t_HASHKEY	m_HashDestCHAR;
	CStrVAR		m_DestCHAR;
	CStrVAR		m_DestACCOUNT;
} ;

#define	MAX_USER_TITLE			50
/**
 * \ingroup SHO_GS_LIB
 * \class	classUSER
 * \author	wookSang.Jo
 * \brief	접속된 사용자의 네트웍 처리및 관리 클래스
 *			소켓클래스 iocpSOCKET, 사용자 케릭터 처리 클래스 CObjAVT 상속받음
 */
class classUSER : public iocpSOCKET, public CObjAVT
{
public:
#ifdef	__INC_WORLD
	float m_fPROPOINT[ 4 ];
	float m_fSUCPOINT[ 4 ];
#endif

private:
	CCriticalSection		m_csSrvRECV;
	CSLList< t_PACKET* >	m_SrvRecvQ;

    CStrVAR		m_Account;
	union {
		char		m_szMD5PassWD[ 33 ];
		DWORD		m_dwMD5PassWD[ 8 ];
	} ;


	short GuildCMD (char *szCMD);
	short Parse_CheatCODE (char *szCode);
	short Check_CheatCODE (char *szCode);

	bool  Do_SelfSKILL (short nSkillIDX);
	bool  Do_TargetSKILL (int iTargetObject, short nSkillIDX);

	bool  Is_SelfSKILL( short nSkillIDX );
	bool  Is_TargetSKILL( short nSkillIDX );

	bool  Use_pITEM( tagITEM *pITEM );
	bool  Use_InventoryITEM (t_PACKET *pPacket);

	classPACKET *Init_gsv_SET_MONEYnINV( WORD wType=GSV_SET_MONEYnINV );
	bool  Send_gsv_SET_MONEYnINV (classPACKET *pCPacket);

	bool  Send_gsv_USE_ITEM (short nItemNO, short nInvIDX);
	bool  Send_gsv_JOIN_ZONE (CZoneTHREAD *pZONE);
	bool  Send_gsv_LEVELUP (short nLevelDIFF);

	bool  Check_WarpPayment (short nZoneNO);
	short Proc_TELEPORT (short nZoneNO, tPOINTF &PosWARP, bool bSkipPayment=false);

	bool  Send_gsv_ADJUST_POS (bool bOnlySelf=false);
	bool  Send_gsv_CREATE_ITEM_REPLY (BYTE btResult, short nStepORInvIDX=-1, float *pProcPOINT=NULL, tagITEM *pOutItem=NULL);
	bool  Send_gsv_STORE_TRADE_REPLY( BYTE btResult );
	bool  Send_gsv_QUEST_REPLY (BYTE btResult, BYTE btSlot, int iQuestID);
	bool  Send_gsv_SET_INV_ONLY (BYTE btInvIDX, tagITEM *pITEM, WORD wType=GSV_SET_INV_ONLY);
	bool  Send_gsv_SET_TWO_INV_ONLY (WORD wType, BYTE btInvIdx1, tagITEM *pITEM1, BYTE btInvIdx2, tagITEM *pITEM2);

	bool  Send_gsv_P_STORE_OPENED();
	bool  Send_gsv_LOGOUT_REPLY( WORD wWaitSec );

	bool  Send_gsv_CHARSTATE_CHANGE( DWORD dwFLAG );

	bool  Recv_cli_ALIVE ();
	bool  Recv_cli_STRESS_TEST( t_PACKET *pPacket );

	short Recv_cli_RELAY_REPLY( t_PACKET *pPacket );
	
    // bool Recv_cli_LOGIN_REQ( t_PACKET *pPacket );
    // int	Recv_cli_LOGOUT_REQ ();

	bool  Recv_cli_JOIN_SERVER_REQ( t_PACKET *pPacket );

    bool  Recv_cli_CREATE_CHAR( t_PACKET *pPacket );
    bool  Recv_cli_DELETE_CHAR( t_PACKET *pPacket );

	bool  Recv_cli_CHAR_LIST( t_PACKET *pPacket );

	bool  Recv_cli_SET_VAR_REQ( t_PACKET *pPacket );

	bool  Recv_cli_JOIN_ZONE ( t_PACKET *pPacket );
	short Recv_cli_REVIVE_REQ( BYTE btReviveTYPE, bool bApplyPenalty=false, bool bSkipPayment=false );
	bool  Recv_cli_SET_REVIVE_POS ();

	bool  Recv_cli_TOGGLE( t_PACKET *pPacket );
	bool  Recv_cli_SET_MOTION( t_PACKET *pPacket );

    short Recv_cli_CHAT( t_PACKET *pPacket );
	short Recv_cli_SHOUT( t_PACKET *pPacket );
    bool  Recv_cli_WHISPER( t_PACKET *pPacket );
	short Recv_cli_PARTY_CHAT( t_PACKET *pPacket );

	void LogCHAT( const char * szMSG, const char * pDestCHAR, const char * szMsgTYPE ); // 채팅을 파일에 로그로 남김 ( 대만일 경우에만 )

	short Recv_cli_ALLIED_CHAT( t_PACKET *pPacket );
	short Recv_cli_ALLIED_SHOUT( t_PACKET *pPacket );

    bool  Recv_cli_STOP( t_PACKET *pPacket );
    bool  Recv_cli_ATTACK( t_PACKET *pPacket );
    bool  Recv_cli_DAMAGE( t_PACKET *pPacket );
	bool  Recv_cli_MOUSECMD( t_PACKET *pPacket );

	bool  Recv_cli_SET_WEIGHT_RATE( BYTE btWeightRate/*t_PACKET *pPacket*/, bool bSend2Around=false );

	bool  Recv_cli_CANTMOVE( t_PACKET *pPacket );
	bool  Recv_cli_SETPOS( t_PACKET *pPacket );

	bool  Send_gsv_CHANGE_SKIN( WORD wAbilityTYPE, int iValue );

	bool  Recv_cli_DROP_ITEM( t_PACKET *pPacket );

	bool  Send_gsv_EQUIP_ITEM (short nEquipInvIDX, tagITEM *pEquipITEM);
	bool  Change_EQUIP_ITEM (short nEquipInvIDX, short nWeaponInvIDX);
	bool  Recv_cli_EQUIP_ITEM( t_PACKET *pPacket );

	bool  Recv_cli_ASSEMBLE_RIDE_ITEM( t_PACKET *pPacket );

	bool  Recv_cli_STORE_TRADE_REQ( t_PACKET *pPacket );

    bool  Recv_cli_USE_ITEM( t_PACKET *pPacket );

	bool  Recv_cli_SET_HOTICON( t_PACKET *pPacket );

	bool  Send_gsv_SET_BULLET( BYTE btShotTYPE );
	bool  Recv_cli_SET_BULLET( t_PACKET *pPacket );

	bool  Recv_cli_CREATE_ITEM_REQ( t_PACKET *pPacket );
//	bool  Recv_cli_CREATE_ITEM_EXP_REQ ();

	bool  Send_gsv_ITEM_RESULT_REPORT( BYTE btReport, BYTE btItemType, short nItemNo);
	bool  Recv_cli_ITEM_RESULT_REPORT( t_PACKET *pPacket );

	bool  Recv_cli_HP_REQ( t_PACKET *pPacket );

    bool  Recv_cli_GET_FIELDITEM_REQ( t_PACKET *pPacket );

	bool  Recv_cli_MOVE_ZULY( t_PACKET *pPacket );
	bool  Recv_cli_MOVE_ITEM( t_PACKET *pPacket );
	bool  Recv_cli_BANK_LIST_REQ( t_PACKET *pPacket );

    short Recv_cli_TELEPORT_REQ( t_PACKET *pPacket );

	bool  Recv_cli_USE_BPOINT_REQ( t_PACKET *pPacket );

	bool  Recv_cli_SKILL_LEVELUP_REQ( t_PACKET *pPacket );

	bool  Recv_cli_SELF_SKILL( t_PACKET *pPacket );
	bool  Recv_cli_TARGET_SKILL( t_PACKET *pPacket );
	bool  Recv_cli_POSITION_SKILL( t_PACKET *pPacket );

    bool  Recv_cli_TRADE_P2P( t_PACKET *pPacket );
	bool  Recv_cli_TRADE_P2P_ITEM( t_PACKET *pPacket );

	bool  Recv_cli_SET_WISHITEM( t_PACKET *pPacket );

	classPACKET* Init_gsv_P_STORE_RESULT( WORD wObjectIDX );
//	bool  Send_gsv_P_STORE_RESULT( classPACKET *pCPacket, BYTE btResult );
	bool  Send_gsv_P_STORE_RESULT( WORD wObjectIDX, BYTE btResult );

    bool  Recv_cli_P_STORE_OPEN( t_PACKET *pPacket );
    bool  Recv_cli_P_STORE_CLOSE( t_PACKET *pPacket );

    bool  Recv_cli_P_STORE_LIST_REQ( t_PACKET *pPacket );
    bool  Recv_cli_P_STORE_BUY_REQ( t_PACKET *pPacket );
	bool  Recv_cli_P_STORE_SELL_REQ( t_PACKET *pPacket );

	bool  Recv_cli_QUEST_REQ( t_PACKET *pPacket );

	/*
	bool  Recv_cli_ADD_QUEST( t_PACKET *pPacket );
	bool  Recv_cli_DEL_QUEST( t_PACKET *pPacket );
	bool  Recv_cli_QUEST_DATA_REQ( t_PACKET *pPacket );
	*/

	bool  Recv_cli_PARTY_REQ( t_PACKET *pPacket );
	bool  Recv_cli_PARTY_REPLY( t_PACKET *pPacket );

	bool  Recv_cli_APPRAISAL_REQ( t_PACKET *pPacket );

	bool  Recv_cli_USE_ITEM_TO_REPAIR( t_PACKET *pPacket );
	bool  Recv_cli_REPAIR_FROM_NPC( t_PACKET *pPacket );

	bool  Send_wsv_MESSENGER( BYTE btCMD, char *szStr );
	bool  Recv_cli_MESSENGER( t_PACKET *pPacket );

	bool  Send_tag_MCMD_HEADER( BYTE btCMD, char *szStr=NULL );
	bool  Recv_cli_MCMD_APPEND_REQ( t_PACKET *pPacket );

	bool  Recv_cli_MESSENGER_CHAT( t_PACKET *pPacket );

	bool         Send_gsv_CRAFT_ITEM_RESULT (BYTE btRESULT);
	classPACKET *Init_gsv_CRAFT_ITEM_REPLY ();
	void         Send_gsv_CRAFT_ITEM_REPLY( classPACKET *pCPacket, BYTE btRESULT, BYTE btOutCNT );

	bool  Proc_CRAFT_GEMMING_REQ( t_PACKET *pPacket );
	bool  Proc_CRAFT_BREAKUP_REQ( t_PACKET *pPacket, bool bUseMP=false );
	bool  Proc_CRAFT_UPGRADE_REQ( t_PACKET *pPacket, bool bUseMP=false );
	bool  Recv_cli_CRAFT_ITEM_REQ( t_PACKET *pPacket );
	bool  Recv_cli_PARTY_RULE( t_PACKET *pPacket );

	bool  Recv_cli_CLAN_COMMAND( t_PACKET *pPacket );
	bool  Recv_cli_CLAN_CHAT( t_PACKET *pPacket );

	bool  Recv_mon_SERVER_LIST_REQ( t_PACKET *pPacket );
	bool  Recv_mon_SERVER_STATUS_REQ( t_PACKET *pPacket );
	bool  Recv_mon_SERVER_ANNOUNCE( t_PACKET *pPacket );
	
	/// 유저 로그아웃
	bool  Recv_ost_SERVER_USERLOGOUT( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_USERLOGOUT_REPLY( const char * szAccount , bool bLogOuted  );

	bool  Recv_ost_SERVER_ZONEINFO( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_ZONEINFO_REPLY( void );

	bool  Recv_ost_SERVER_USERINFO( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_USERINFO_REPLY( const char * szACCOUNT, classUSER * pUSER );

	bool  Recv_ost_SERVER_CHGUSER( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_CHGUSER_REPLY( classUSER * pUSER, DWORD dwSTATUS, DWORD dwCMD );

	bool  Recv_ost_SERVER_MOVECHR( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_MOVECHR_REPLY( classUSER * pUSER, short nZoneNO, short nX, short nY );

	bool  Recv_ost_SERVER_ZONEANNOUNCE( t_PACKET *pPacket );

	bool  Recv_ost_SERVER_IPSEARCH( t_PACKET* pPacket );
	bool  Send_gsv_SERVER_IPSEARCH_REPLY( classUSER * pUSER );

	bool  Recv_cli_MALL_ITEM_REQ( t_PACKET *pPacket );

	bool Recv_cli_CLANMARK_SET( t_PACKET *pPacket );
	bool Recv_cli_CLANMARK_REQ( t_PACKET *pPacket );
	bool Recv_cli_CLANMARK_REG_TIME( t_PACKET *pPacket );

	bool Recv_cli_CART_RIDE( t_PACKET *pPacket );

public :
	bool Send_gsv_CART_RIDE( BYTE btType, WORD wSourObjIdx, WORD wDestObjIDX, bool bSendToSector=false );

	bool Send_wsv_CLANMARK_REPLY( DWORD dwClanID, WORD wMarkCRC, BYTE *pMarkData, short nMarkDataLen );

	void Save_ItemToFILED (tagITEM &sDropITEM, int iRemainTime=ITEM_OBJ_DROP_TIME);
	bool Pick_ITEM( CObjITEM *pITEM );

	bool Send_gsv_SET_HPnMP (BYTE btApply);
	bool Send_wsv_CHATROOM (BYTE btCMD, WORD wUserID, char *szSTR);

	bool Recv_cli_SELECT_CHAR( t_PACKET *pPacket, bool bFirstZONE, BYTE btRunMODE, BYTE btRideMODE  );
	bool Send_gsv_GM_COMMAND( char *szAccount, BYTE btCMD, WORD wBlockTIME=1);
	bool Send_gsv_TELEPORT_REPLY (tPOINTF &PosWARP, short nZoneNO);

	bool Send_gsv_INVENTORYnQUEST_DATA (void);

	bool Send_srv_JOIN_SERVER_REPLY  (t_PACKET *pRecvPket, char *szAccount);

	bool Send_gsv_CHECK_NPC_EVENT( short nNpcIDX );

	bool Send_gsv_SETEXP (WORD wFromObjIDX);
	bool Send_gsv_HP_REPLY (int iObjectIDX, int iHP);
	bool Send_gsv_WHISPER (char *szFromAccount, char *szMessage);

	BYTE Send_gsv_SKILL_LEARN_REPLY (short nSkillIDX, bool bCheckCOND=true);

	bool Send_gsv_TRADE_P2P (int iObjectIDX, BYTE btResult, char cTradeSLOT=-1);
	bool Send_gsv_TRADE_P2P_ITEM (char cTradeSLOT, tagITEM &sITEM);

	bool Send_gsv_BANK_ITEM_LIST (bool bNewBank=false);
	bool Send_gsv_BANK_LIST_REPLY (BYTE btReply);
	bool Send_gsv_PARTY_REQ(int iObjectIDX, BYTE btResult);
	bool Send_gsv_PARTY_REPLY(int iObjectIDX, BYTE btResult);
	bool Send_gsv_RELAY_REQ( WORD wRelayTYPE, short nZoneGOTO, tPOINTF &PosGOTO );

	bool Send_gsv_SET_MONEYONLY (WORD wType);
	bool Send_BulltCOUNT ();
	bool Send_gsv_SET_ITEM_LIFE (short nInvIDX, short nLife);
	bool Send_gsv_SPEED_CHANGED (bool bUpdateSpeed=true);
	bool Do_QuestTRIGGER( t_HASHKEY HashTRIGGER, short nSelectReward=-1 );

	bool Send_wsv_CLAN_COMMAND( BYTE btCMD, ... );
	bool Send_wsv_RESULT_CLAN_SET (char *szClanName=NULL);

	bool Send_gsv_BILLING_MESSAGE( BYTE btMsgType, char *szMsg );
	bool Send_gsv_BILLING_MESSAGE_EXT( WORD wMsgType, DWORD dwPayType, DWORD *pPlayFlag, char *szMsg );
	bool Send_gsv_BILLING_MESSAGE2( BYTE btType, char cFunctionType, DWORD dwPayFlag );

	bool Send_srv_ERROR ( WORD wErrCODE );
	bool Check_ItemEventMSG( tagITEM &sITEM );

	bool Send_srv_CHECK_AUTH ();
	bool Recv_cli_CHECK_AUTH ( t_PACKET *pPacket );

	bool Recv_cli_SUMMON_CMD( t_PACKET *pPacket );

	bool Recv_cli_SCREEN_SHOT_TIME(t_PACKET *pPacket);
	bool Send_gsv_SCREEN_SHOT_TIME();

	bool Recv_cli_UPDATE_NAME(t_PACKET *pPacket);
	bool Send_gsv_UPDATE_NAME();

	bool Recv_cli_SET_RIGHTS(t_PACKET *pPacket);

	//----------------------- virtual function inherit from CUserDATA
	bool Add_MoneyNSend( int iAmount, WORD wType=GSV_SET_MONEY_ONLY )	
	{	
		if ( iAmount ) {
			this->Add_CurMONEY( iAmount );
			return this->Send_gsv_SET_MONEYONLY( wType );
		}
		return true;
	}
	bool Add_ExpNSend( int iExp )
	{	
		if ( iExp ) 
			this->Add_EXP( iExp, false, 0 );
		return true;
	}
	bool Add_ItemNSend( tagITEM &sITEM )
	{
		this->Set_ItemSN( sITEM );		// 퀘스트 보상에 의해...

		short nInvIDX = this->Add_ITEM( sITEM );
		if ( nInvIDX >= MAX_EQUIP_IDX ) {
			// 들어갔다..
			this->Check_ItemEventMSG( sITEM );
			return this->Send_gsv_SET_INV_ONLY( (BYTE)nInvIDX, &sITEM, GSV_REWARD_ITEM );
		} else {
			// 퀘스트에서 부여되는 아이템... 바닦에 떨굼
			this->Save_ItemToFILED( sITEM );
		}

		return false;
	}
	bool Add_SkillNSend( short nSkillIDX )
	{
		return ( RESULT_SKILL_LEARN_SUCCESS == Send_gsv_SKILL_LEARN_REPLY( nSkillIDX, false ) );
	}
	bool Sub_SkillNSend( short nSkillIDX )
	{
		short nSkillSLOT = this->Skill_FindLearnedSlot( nSkillIDX );
		if ( nSkillSLOT >= 0 ) {
			classPACKET *pCPacket = Packet_AllocNLock ();
			if ( !pCPacket )
				return false;

			pCPacket->m_HEADER.m_wType = GSV_SKILL_LEARN_REPLY;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_SKILL_LEARN_REPLY );

			pCPacket->m_gsv_SKILL_LEARN_REPLY.m_btResult    = RESULT_SKILL_LEARN_DELETE;
			pCPacket->m_gsv_SKILL_LEARN_REPLY.m_btSkillSLOT = (BYTE)nSkillSLOT;
			pCPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX   = nSkillIDX;

			this->SendPacket( pCPacket );
			Packet_ReleaseNUnlock( pCPacket );

			this->Skill_DELETE( nSkillSLOT, nSkillIDX );
		}
		return false;
	}

	bool Send_gsv_SET_ABILITY( WORD wPacketType, WORD wAbilityType, int iValue )
	{
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = wPacketType;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_ABILITY );

		pCPacket->m_gsv_SET_ABILITY.m_wAbilityTYPE  = wAbilityType;
		pCPacket->m_gsv_SET_ABILITY.m_iValue		= iValue;

		this->SendPacket( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );

		// 변경시 주위에 통보 필요한것들....
		switch( wAbilityType ) {
			case AT_FACE		:	
				this->m_sFaceIDX.m_nItemNo = iValue;
				this->Send_gsv_CHANGE_SKIN( wAbilityType, iValue );
				break;
			case AT_HAIR		:
				this->m_sHairIDX.m_nItemNo = iValue;
			case AT_SEX			:	
				this->Send_gsv_CHANGE_SKIN( wAbilityType, iValue );
				break;
		}
		return true;
	}
	void Add_AbilityValueNSend( WORD wType, int iValue )
	{	
		this->Add_AbilityValue( wType, iValue);
		this->Send_gsv_SET_ABILITY( GSV_REWARD_ADD_ABILITY, wType, iValue );
	}
	bool Set_AbilityValueNSend( WORD wType, int iValue )
	{	
		this->Set_AbilityValue(wType, iValue);
		return this->Send_gsv_SET_ABILITY( GSV_REWARD_SET_ABILITY, wType, iValue );
	}

	bool Quest_CHANGE_SPEED()
	{
		return this->Send_gsv_SPEED_CHANGED ();
	}


	bool Quest_IsPartyLEADER ()
	{
		if ( this->GetPARTY() ) {
			return ( this == this->GetPARTY()->GetPartyOWNER() );
		}
		return false;
	}
	int	Quest_GetPartyLEVEL ()
	{	
		return this->GetPARTY() ? this->GetPARTY()->GetPartyLEV() : -1;	
	}
	int	Quest_PartyMemberCNT()
	{
		if ( this->GetPARTY() ) {
			return ( this->GetPARTY()->GetMemberCNT() );
		}
		return 0;
	}

	int	 Quest_GetZoneNO()									{	return m_nZoneNO;	}
	int	 Quest_DistanceFrom(int iX, int iY, int iZ)			
	{	
		tPOINTF Pos;
		Pos.x = (float)iX;
		Pos.y = (float)iY;
		return this->Get_DISTANCE( Pos );
	}
	bool Reward_WARP( int iZoneNO, tPOINTF &PosGOTO);	

	void SetCur_JOB (short nValue)
	{	
		this->m_BasicINFO.m_nClass=nValue;
		this->UpdateAbility ();
		// 전직후 maxhp, maxmp가 틀려진다.
		// 파티중일경우는 파티원에거 전달...
		if ( this->GetPARTY() ) {
			this->GetPARTY()->Change_ObjectIDX( this );
		}
	}

	bool Set_TeamNoFromUNIQUE ();
	bool Set_TeamNoFromClanIDX ();
	bool Set_TeamNoFromPartyIDX ();
	bool Set_RevivePOS( int iXPos, int iYPos );

	char*Get_StoreTITLE ()									{	return this->m_szUserTITLE;		}

	short			m_nZoneNO;

	DWORD			m_dwLSID;
	DWORD			m_dwWSID;
	DWORD			m_dwDBID;	// db에 저장된 값은 2147483647까지..
	DWORD			m_dwItemSN;

	t_HASHKEY		m_HashACCOUNT;
	t_HASHKEY		m_HashCHAR;
	DWORD			m_dwRIGHT;
	DWORD			m_dwPayFLAG;

//	short			m_nProcSTEP;
	CStrVAR			m_BankPASSWORD;

	short			m_nReviveZoneNO;
	tPOINTF			m_PosREVIVE;

	BYTE			m_btTradeBIT;
	int				m_iTradeUserIDX;
	tagTradeITEM	m_TradeITEM[ MAX_TRADE_ITEM_SLOT ];		// 거래시 상대방에게 건내줄 아이템
	short			m_nCreateItemEXP;
	int				m_iLastEventNpcIDX;

	// PVP등에서 사용되는 임시 현재존 부활위치...
	bool			m_bSetImmediateRevivePOS;
	tPOINTF			m_PosImmediateRivive;

	bool			m_bKickOutFromGUMS;
	BYTE			m_btWishLogOutMODE;
	DWORD			m_dwTimeToLogOUT;
	DWORD			m_dwLastSkillActiveTIME;
	DWORD			m_dwLastSkillSpellTIME[ MAX_LEARNED_SKILL_CNT ];
	DWORD			m_dwLastSkillGroupSpeelTIME[ MAX_SKILL_RELOAD_TYPE ];

	char			m_szUserTITLE[ MAX_USER_TITLE+1 ];
	struct tagUserSTORE {
		bool	m_bActive;

		BYTE	m_btSellCNT;									// 등록된 판매 품목 갯수
		short	m_nInvIDX			[ MAX_P_STORE_ITEM_SLOT ];	// 판매시 팔려는 아이템의 인벤토리 위치.
		tagITEM	m_SellITEM			[ MAX_P_STORE_ITEM_SLOT ];	// 판매, 구입 희망 아이템(갯수 포함)
		DWORD	m_dwSellPricePerEA	[ MAX_P_STORE_ITEM_SLOT ];	// 1개당 희망 가격

		BYTE	m_btWishCNT;									// 구입 희만 품목 갯수
		BYTE	m_btWishIDX			[ MAX_P_STORE_ITEM_SLOT ];	// 구입 하려는 아이템의 슬롯 번호
		tagITEM	m_WishITEM			[ MAX_P_STORE_ITEM_SLOT ];	// 구입 희망 아이템(갯수 포함)
		DWORD	m_dwWishPricePerEA	[ MAX_P_STORE_ITEM_SLOT ];	// 1개당 희망 가격
		BYTE	m_btWishIdx2StoreIDX[ MAX_P_STORE_ITEM_SLOT ];
	} m_STORE;

	tagMallDATA	m_MALL;

	bool	Check_TradeITEM ();
	void	RemoveTradeItemFromINV (classUSER *pTradeUSER, classPACKET *pCPacket);
	void	AddTradeItemToINV (classUSER *pTradeUSER/*tagTradeITEM *pTradeITEM*/, classPACKET *pCPacket);


	#define	BANK_UNLOADED	0x00	// 적재 되지 않았다
	#define	BANK_LOADED		0x01	// 적재 됐다
	#define	BANK_CHANGED	0x02	// 은행 내용이 바뀌었다.
	BYTE		m_btBankData;

	BYTE		m_btShotTYPE;
	tagITEM	   *m_pShotITEM;

	#define	LOGOUT_MODE_LEFT		0x01
	#define	LOGOUT_MODE_WARP		0x02
	#define	LOGOUT_MODE_CHARLIST	0x03
	#define LOGOUT_MODE_NET_ERROR	0x04

	BYTE		m_btPlatinumCHAR;
	BYTE		m_btLogOutMODE;
	DWORD		m_dwBackUpTIME;
	DWORD		m_dwLoginTIME;
	__int64		m_i64StartMoney;
	int			m_iClanCreateMoney;

	bool		m_bInAGIT;

	CParty							 *m_pPartyBUFF;
#ifdef	__USE_ARRAY_PARTY_USER
	short							  m_nPartyPOS;
#else
x	//CDLList< tagPartyUSER >::tagNODE *m_pPartyNODE;
#endif

	classUSER ();
	virtual ~classUSER ();

    inline bool SendPacket( classPACKET *pCPacket )			{   return this->Send_Start( pCPacket );			}
		   bool SendPacketToPARTY( classPACKET *pCPacket )
		   {
				if ( this->GetPARTY() )
					return m_pPartyBUFF->SendToPartyMembers( pCPacket );
				return this->Send_Start( pCPacket );
		   }
		   bool	SendPacketIfPARTY( classPACKET *pCPacket )
		   {	
				if ( this->GetPARTY() )
					return m_pPartyBUFF->SendToPartyMembers( pCPacket );
				return true;
		   }
		   /*
		   bool SendPacketToPartyExecpNearUSER( classPACKET *pCPacket )
		   {
			   if ( this->GetPARTY() )
					return m_pPartyBUFF->SendToPartyMembersExecptNEAR( this, pCPacket );
			   return true;
		   }
		   */
		   bool	SendPacketToPartyExceptME( classPACKET *pCPacket )
		   {
			    if ( this->GetPARTY() ) 
					return m_pPartyBUFF->SendToPartyMembersExceptME( this, pCPacket );
			   return true;	
		   }

		   bool SendPacketToTARGET( CObjCHAR *pAtkCHAR, classPACKET *pCPacket )	
		   {
			   if ( NULL == pAtkCHAR->GetPARTY() || pAtkCHAR->GetPARTY() != this->GetPARTY() ) {
				   // 때린 사람 한테 전송... 파티가 없거나, 같은 파티원이 아니면...
				   pAtkCHAR->SendPacket( pCPacket );
			   }

			   // 맞은 사람한테 전송...
			   return this->SendPacketToPARTY( pCPacket );
		   }

	char *Get_ACCOUNT()										{	return m_Account.Get();							}
	WORD  Get_AccountLEN()									{	return m_Account.BuffLength();					}
	void  Set_ACCOUNT(char *szAccount)						{	m_Account.Set(szAccount);						}
	char *Get_MD5PW()										{	return m_szMD5PassWD;							}

	bool Check_PartyJoinLEVEL (int iUserLEV, int iAverageLEV, int iPartyLEV)
	{
		if ( iUserLEV < (int)(iAverageLEV * 0.92f) - 6 || 
			 iUserLEV > (int)(iAverageLEV * 1.08 + 6 + iPartyLEV / 6 ) ) {
			return false;
		}

		return true;
	}

#ifdef	__INC_WORLD
	int	m_iChatRoomID;
	CDLList<classUSER*>::tagNODE *m_pNodeChatROOM;
#endif

#if defined(__N_PROTECT) && !defined(__NORTHUSA)
	CCSAuth2		m_CSA;		// n-Protect검증용
	DWORD			m_dwCSARecvTime;
	DWORD			m_dwCSASendTime;
#endif

	DWORD			m_dwCoolTIME[ MAX_USEITEM_COOLTIME_TYPE ];

	void InitUSER ()
	{
		::ZeroMemory( m_dwCoolTIME, sizeof(m_dwCoolTIME) );

	#ifdef	__INC_WORLD
		m_iChatRoomID = 0;
	#endif

	#if defined(__N_PROTECT) && !defined(__NORTHUSA)
		m_CSA.Init();
		m_dwCSARecvTime = m_dwCSASendTime = 0;
	#endif

		CObjTARGET::Set_TargetIDX( 0 );

		this->Set_ACCOUNT( NULL );
		m_HashACCOUNT = 0;
		this->m_BankPASSWORD.Set( NULL );

		this->Set_NAME( NULL );
		m_HashCHAR = 0;

		this->Clear_SummonCNT ();		// 소환된 갯수 0으로..

		m_IngSTATUS.Reset(true );

		m_iRecvSeqNO = 0;

		m_dwRIGHT = 0;
		m_dwPayFLAG = 0;
		m_bRunMODE  = true;
		m_HashKeyIP = 0;

		m_btWeightRate = 0;

		m_nZoneNO = 0;

		m_dwWSID = 0;
		m_dwLSID = 0;

		m_btTradeBIT = 0;
		m_iTradeUserIDX = 0;
		m_pPartyBUFF = NULL;

		m_btLogOutMODE = 0;
		m_dwBackUpTIME = 0;

		m_btBankData = BANK_UNLOADED;
		m_pShotITEM = NULL;

		m_iLastEventNpcIDX = 0;
		m_nCreateItemEXP = 0;
		m_iTeamNO = TEAMNO_USER;

		m_btRideMODE = 0;
		m_iLinkedCartObjIDX = 0;

		::ZeroMemory( &m_STORE, sizeof(tagUserSTORE) );
		m_szUserTITLE[ 0 ] = 0;

		m_bSetImmediateRevivePOS = false;
		m_i64StartMoney = 0;

		ClanINIT ();
		m_iClanCreateMoney = 0;

		m_bKickOutFromGUMS = false;
		m_btWishLogOutMODE = 0;
		m_dwTimeToLogOUT   = 0;
		m_dwAttackTIME     = 0;

		m_dwLastSkillActiveTIME = 0;
		::ZeroMemory( m_dwLastSkillSpellTIME,		sizeof(m_dwLastSkillSpellTIME) );
		::ZeroMemory( m_dwLastSkillGroupSpeelTIME,	sizeof(m_dwLastSkillGroupSpeelTIME) );

		m_iAppliedPenaltyEXP = 0;
		m_btPlatinumCHAR = 0;

		::ZeroMemory( &m_MALL, sizeof(tagMallDATA) );

		m_bInAGIT = false;
		m_dwGoddnessTIME = 0;
		m_btSummonCMD = 0;

		m_nPatHP_Mode = 0;
	}
	void FreeUSER ()
	{
		CSLList< t_PACKET* >::tagNODE *pNode;
		m_csSrvRECV.Lock ();
			pNode = m_SrvRecvQ.GetHeadNode();
			while( pNode ) {
				delete[] pNode->m_VALUE;
				m_SrvRecvQ.DeleteHead ();
				pNode = m_SrvRecvQ.GetHeadNode();
			}
		m_csSrvRECV.Unlock ();

		m_Account.Set( NULL );
	}

	//----------------------- virtual function inherit from iocpSOCKET
	bool Recv_Done (tagIO_DATA *pRecvDATA);

	bool HandleWorldPACKET (void);
    bool HandlePACKET( t_PACKETHEADER *pPacket );

	int  Proc_ZonePACKET( t_PACKET *pPacket );
	bool IsHacking (char *szDesc, char *szFile, int iLine);

public :	
	//----------------------- virtual function inherit from CObjCHAR
	void		Add_EXP (__int64 iExp, bool bApplyStamina/*=true*/, WORD wFromObjIDX);
	bool		Dead (CObjCHAR *pKiller);
	CParty*		GetPARTY ()				{	return m_pPartyBUFF;	}

	void		CloseNETWORK ()			{	this->CloseSocket();	}
	DWORD		Get_RIGHT()				{	return this->m_dwRIGHT;	}
	t_HASHKEY	Get_CharHASH ()			{	return m_HashCHAR;		}		

	bool	Chk_ShotITEM (BYTE btShotCnt)
	{	
		if ( this->m_pShotITEM ) {
			if ( this->m_pShotITEM->GetQuantity() < btShotCnt ) {
				// 총알이 모자르다..
				return false;
			}
		}
		return true;	
	}

	bool	Dec_ShotITEM (BYTE btShotCnt)
	{	
		if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN && this->m_pShotITEM ) {
			if ( this->m_pShotITEM->GetQuantity() < btShotCnt ) {
				// 총알이 모자르다..
				return false;
			}

			m_pShotITEM->m_uiQuantity -= btShotCnt;
			if ( m_pShotITEM->GetQuantity() <= 0 ) {
				// 아이템 삭제...
				m_pShotITEM->m_uiQuantity = 0;
				m_pShotITEM->Clear ();

				// 총알 없다...공격력 재계산.
				this->Cal_ATTACK ();

				return this->Send_gsv_SET_BULLET( this->m_btShotTYPE );
			} else 
			if ( 0 == ( m_pShotITEM->GetQuantity() & 0x0f ) ) {
				this->Send_BulltCOUNT ();
			}
		}

		return true;	
	}
	void	Set_ShotITEM( t_eSHOT eShotTYPE )
	{
		// 현재 장착된 오른손 무기의 필요 총알 번호 얻기..
		if ( eShotTYPE < MAX_SHOT_TYPE ) {
			// 현재 소모탄 설정...
			this->m_btShotTYPE = eShotTYPE;
			this->m_pShotITEM = &this->m_Inventory.m_ItemSHOT[ eShotTYPE ];

			// 총알 없다...공격력 재계산.
			this->Cal_ATTACK ();
		} else {
			this->m_pShotITEM = NULL;
		}
	}
	void	Set_ShotITEM ()
	{
		// 현재 장착된 오른손 무기의 필요 총알 번호 얻기..
		t_eSHOT eShotTYPE = this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_R ].GetShotTYPE();
		this->Set_ShotITEM( eShotTYPE );
	}
	void	Set_ItemSN( tagITEM &sInITEM )
	{
		if ( 0 == sInITEM.m_iSN && !sInITEM.IsEnableDupCNT() ) {
			if ( ITEM_BASE_PRICE(sInITEM.GetTYPE(), sInITEM.GetItemNO() ) >= CHECK_ITEM_SN_PRICE ) {
				sInITEM.m_dwSN[0] =   this->m_dwDBID;
				sInITEM.m_dwSN[1] = ++this->m_dwItemSN;
			}
		}
	}

	bool	CheckClanCreateCondition (char cStep);

	bool    Send_gsv_ADJ_CLAN_VAR (BYTE btVarType, int iValue);
	void	IncClanLEVEL ();
	void	AddClanSCORE(int iScore);
	void	AddClanCONTRIBUTE(int iCon);
	void	AddClanMONEY(int iMoney);

	BYTE	FindClanSKILL(short nSkillNo1, short nSkillNo2);
	bool	AddClanSKILL (short nSkillNo);
	bool	DelClanSKILL (short nSkillNo);

	bool Add_SrvRecvPacket( t_PACKET *pPacket );
	int  ProcLogOUT ();
	int	 Proc (void);

private:
	int			m_iRecvSeqNO;

#ifndef	__STRESS_TEST
	WORD  E_SendP (t_PACKETHEADER *pPacket)
	{
		return g_pPacketCODEC->Encode_SendServerPACKET( pPacket );
	}
	WORD  D_RecvH (t_PACKETHEADER *pPacket)
	{
		return g_pPacketCODEC->Decode_RecvClientHEADER( pPacket, m_iRecvSeqNO );
	}
	short D_RecvB (t_PACKETHEADER *pPacket)
	{
		return g_pPacketCODEC->Decode_RecvClientBODY( pPacket );
	}
	WORD  P_Length(t_PACKETHEADER *pPacket)
	{	
		return g_pPacketCODEC->GetDecodedPacketLength( pPacket );
	}
#endif

	DWORD A_Cheater ();
	DWORD B_Cheater ();
	DWORD C_Cheater ();
	DWORD GM_Cheater ();
	DWORD TWGM_Cheater ();

	short Cheat_where (CStrVAR *pStrVAR, char *pArg1/*szCharName*/, char *pArg2/*szAccount*/, char *szCode );
	short Cheat_account (char *pArg1, char *szCode );
	short Cheat_move (char *pArg1, char *pArg2, char *szCode );
	short Cheat_mm (short nZoneNO, char *pArg2, char *pArg3);
	short Cheat_npc ( CStrVAR *pStrVAR, CObjNPC *pNPC, int iNpcIDX, char *pArg2, char *pArg3, char *pArg4 );
	short Cheat_add ( char *pArg1, char *pArg2, char *pArg3, char *szCode );
	short Cheat_del ( CStrVAR *pStrVAR, char *pArg1, char *pArg2, char *pArg3 );
	short Cheat_get ( CStrVAR *pStrVAR, char *pArg1, char *pArg2, char *szCode );
	short Cheat_out ( char *pArg1, char *pArg2, char *szCode );
	short Cheat_shut ( char *pArg1, char *pArg2, char *pArg3, char *szCode );
	short Cheat_call ( char *pArg1, char *pArg2, char *szCode );
	short Cheat_item ( char *pArg1, char *pArg2, char *pArg3, char *pArg4 );
	short Cheat_mon ( char *pArg1, char *pArg2 );
	short Cheat_mon2 ( char *pArg1, char *pArg2, char *pArg3, char *pArg4 );
	short Cheat_damage( char *pArg1, char *pArg2, char *pArg3 );
	short Cheat_quest( CStrVAR *pStrVAR, char *pArg1 );
	short Cheat_set ( classUSER *pUSER, char *pArg1, char *pArg2, char *pArg3 );
	short Cheat_toggle ( CStrVAR *pStrVAR, char *pArg1 );
	short Cheat_regen ( CStrVAR *pStrVAR, char *pArg1, char *pArg2 );
	short Cheat_speed ( char * pArg1 );
} ;

//---------------------------------------------------------------------------
#endif
