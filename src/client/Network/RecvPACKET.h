/*
	$Header: /Client/Network/RecvPACKET.h 102   05-10-18 3:10p Young $
*/
#ifndef	__CRECVPACKET_H
#define	__CRECVPACKET_H
//-------------------------------------------------------------------------------------------------

class CRecvPACKET {
protected:
	t_PACKET *m_pRecvPacket;

	CStrVAR	m_WSV_IP;
	WORD	m_wWSV_PORT;
	DWORD	m_dwWSV_ID;

	CStrVAR	m_GSV_IP;
	WORD	m_wGSV_PORT;
	DWORD	m_dwGSV_IDs[2];

	virtual void DisconnectFromServer (short ) = 0{ *(int*)0 = 10; };

public   :

	bool Recv_tag_ADD_CHAR (short nCliObjIDX, tag_ADD_CHAR* tagAddChar );
	void Recv_tag_ADJ_STATUS (short& nPacketOffset, tag_ADD_CHAR* tagAddChar );



	CRecvPACKET ();
	virtual	~CRecvPACKET();

	void Recv_srv_ERROR ();
	void Recv_gsv_GM_COMMAND ();

	void Recv_gsv_SET_GLOBAL_VAR ();
	void Recv_gsv_SET_GLOVAL_FLAG();
	

	void Recv_srv_ANNOUNCE_TEXT ();
	void Recv_gsv_ANNOUNCE_CHAT ();

	bool Recv_lsv_LOGIN_REPLY ();
	
	void Recv_lsv_CHANNEL_LIST_REPLY ();
	int	 Recv_lsv_SELECT_SERVER ();

	int  Recv_srv_JOIN_SERVER_REPLY ();
	void Recv_gsv_INIT_DATA ();

	void Recv_wsv_CHAR_LIST ();
	void Recv_wsv_DELETE_CHAR ();
	bool Recv_wsv_CREATE_CHAR ();

	void Recv_wsv_MOVE_SERVER ();

	void Recv_gsv_JOIN_ZONE ();
	void Recv_gsv_SELECT_CHAR ();

	void Recv_gsv_INVENTORY_DATA ();
	void Recv_gsv_QUEST_DATA();

	void Recv_gsv_REVIVE_REPLY ();
	void Recv_gsv_SET_VAR_REPLY ();

	void Recv_gsv_CHEAT_CODE ();

	void Recv_gsv_SET_MOTION ();
	void Recv_gsv_TOGGLE ();
	void Recv_gsv_CHAT ();
	void Recv_gsv_WHISPER ();
	void Recv_gsv_SHOUT ();
	void Recv_gsv_PARTY_CHAT();

	void Recv_gsv_SET_NPC_SHOW ();

	void Recv_gsv_NPC_CHAR ();
	void Recv_gsv_MOB_CHAR ();
	void Recv_gsv_AVT_CHAR ();
	void Recv_gsv_SUB_OBJECT ();

	void Recv_gsv_SET_WEIGHT_RATE ();
	void Recv_gsv_ADJUST_POS ();

	void Recv_gsv_STOP ();
	void Recv_gsv_ATTACK ();
	void Recv_gsv_ATTACK_START ();

	void Recv_gsv_MOUSECMD ();
	void Recv_gsv_MOVE ();

	void Recv_gsv_DAMAGE ();

	void Recv_gsv_CHANGE_NPC ();
	
	void Recv_gsv_SETEXP ();
	void Recv_gsv_LEVELUP ();

	void Recv_gsv_HP_REPLY();

	void Recv_gsv_STORE_TRADE_REPLY ();

	void Recv_gsv_P_STORE_MONEYnINV();
	void Recv_gsv_SET_MONEYnINV ();
	void Recv_gsv_SET_INV_ONLY ();
	void Recv_gsv_USE_ITEM ();

	void Recv_gsv_CHANGE_SKIN ();
	void Recv_gsv_EQUIP_ITEM ();

	void Recv_gsv_ADD_FIELDITEM ();
	void Recv_gsv_GET_FIELDITEM_REPLY ();

	void Recv_gsv_TELEPORT_REPLY ();

	void Recv_gsv_SET_HOTICON ();

	void Recv_gsv_USE_BPOINT_REPLY ();

	void Recv_gsv_SKILL_LEARN_REPLY ();
	void Recv_gsv_SKILL_LEVELUP_REPLY ();

	void Recv_gsv_SELF_SKILL ();
	void Recv_gsv_TARGET_SKILL ();
	void Recv_gsv_POSITION_SKILL ();
	void Recv_gsv_SKILL_START();

	void Recv_gsv_SKILL_CANCEL();


	///
	/// result of skill
	///
	void Recv_gsv_EFFECT_OF_SKILL ();		
	void Recv_gsv_DAMAGE_OF_SKILL ();			// 7,17번 타입의 경우.
	void Recv_gsv_RESULT_OF_SKILL ();	
	

	void Recv_gsv_CLEAR_STATUS ();
	void Recv_gsv_SPEED_CHANGED ();

	void Recv_gsv_TRADE_REQ ();
	void Recv_gsv_TRADE_REPLY ();

	void Recv_gsv_P_STORE_OPENED	();
	void Recv_gsv_P_STORE_CLOSED	();
	void Recv_gsv_P_STORE_LIST_REPLY ();
	void Recv_gsv_P_STORE_RESULT();
	void Recv_gsv_QUEST_REPLY ();

	///트레이드 관련
	void Recv_gsv_TRADE_P2P();
	void Recv_gsv_TRADE_P2P_ITEM();

	///파티 관련
	void Recv_gsv_PARTY_REQ();
	void Recv_gsv_PARTY_REPLY();
	void Recv_gsv_PARTY_MEMBER();
	void Recv_gsv_PARTY_LEVnEXP();
	void Recv_gsv_PARTY_ITEM();
	void Recv_gsv_PARTY_RULE();
	void Recv_gsv_CHANGE_OBJIDX();
	///제조관련
	void Recv_gsv_CREATE_ITEM_REPLY();
	///뱅크( 계정창고) 관련
	void Recv_gsv_BANK_LIST_REPLY();
	void Recv_gsv_MOVE_ITEM();
	///소모탄 관련
	void Recv_gsv_SET_BULLET();

	void Recv_gsv_SERVER_DATA();
	///PAT 관련
	void Recv_gsv_ASSEMBLE_RIDE_ITEM();


	/// 실시간 수정되는 이벤트 값( m_nEventSTATUS ) 전송
	void Recv_GSV_SET_EVENT_STATUS();


	void Recv_GSV_SET_ITEM_LIFE();

	void Recv_wsv_CHATROOM ();
	void Recv_tag_MCMD_HEADER ();

	void Recv_wsv_MESSENGER_CHAT ();
	void Recv_wsv_MEMO();

	void Recv_wsv_CHATROOM_MSG ();

	void Recv_gsv_USED_ITEM_TO_REPAIR();
	void Recv_gsv_REPAIRED_FROM_NPC();

	void Recv_gsv_APPRAISAL_REPLY();
	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief 재밍 재련관련
	///
	//----------------------------------------------------------------------------------------------------
	void Recv_gsv_CRAFT_ITEM_REPLY();
	void Recv_gsv_SET_MONEY_ONLY();
	void Recv_gsv_REWARD_MONEY();


	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief 퀘스트 보상관련
	///
	//----------------------------------------------------------------------------------------------------
	void Recv_gsv_REWARD_ITEM();
	void Recv_gsv_REWARD_ADD_ABILITY();
	void Recv_gsv_REWARD_SET_ABILITY();

	void Recv_wsv_CHAR_CHANGE();

	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief 이벤트 오브젝트 처리 관련
	///
	//----------------------------------------------------------------------------------------------------	
	void Recv_gsv_ADD_EVENTOBJ();


	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief HP, MP 물약 종료 패킷.
	///
	//----------------------------------------------------------------------------------------------------	
	void Recv_gsv_SET_HPnMP();


	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief Do_DeadEvent 에서 호출하던것을 서버에서 요청시 처리하게 변경..
	///
	//----------------------------------------------------------------------------------------------------
	void Recv_gsv_CHECK_NPC_EVENT();



	void Recv_wsv_CLAN_COMMAND();
	void Recv_wsv_CLAN_CHAT();
	void Recv_wsv_CLANMARK_REPLY();


	void Recv_gsv_ALLIED_CHAT();
	void Recv_gsv_ALLIED_SHOUT();

	void Recv_gsv_LOGOUT_REPLY();	
	void Recv_gsv_ITEM_RESULT_REPORT();



	void Recv_gsv_MALL_ITEM_REPLY();


	void Recv_gsv_BILLING_MESSAGE();

	void Recv_gsv_BILLING_MESSAGE_EXT();
	void Set_BILLING_MESSAGE_EXT_KOR_And_PH();		
	void Set_BILLING_MESSAGE_EXT_JPN();
	void Set_BILLING_MESSAGE_EXT_TW();
	void Set_BILLING_MESSAGE_EXT_Other();

	void Recv_wsv_CLANMARK_REG_TIME();

	void Recv_gsv_MOVE_ZULY();

	//----------------------------------------------------------------------------------------------------	
	///
	/// 박 지호    여신 소환 패킷처리 
	///
	//----------------------------------------------------------------------------------------------------	
	void Recv_gsv_GODDNESS_MODE();
	//----------------------------------------------------------------------------------------------------	
	/// 배틀카트 관련
	void Recv_gsv_PATSTATE_CHANGE();


	// 케릭터 상태 변환 (투명)
	void Recv_gsv_CHARSTATE_CHANGE();

	// 2인승 카트.
	void Recv_gsv_CART_RIDE();

	//스샷관련..
	void Recv_gsv_SCREEN_SHOT_TIME();

	// Updates a player name
	void Recv_gsv_UPDATE_NAME();
} ;

//-------------------------------------------------------------------------------------------------
#endif