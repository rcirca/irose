/*
	$Header: /Client/Network/SendPACKET.h 103   05-10-18 3:10p Young $
*/
#ifndef	__CSENDPACKET_H
#define	__CSENDPACKET_H
//-------------------------------------------------------------------------------------------------

typedef int tagPS_SellITEMs;


class CSendPACKET {
protected:
	unsigned char	m_pMD5Buff[ 32 ];
	t_PACKET	   *m_pSendPacket;
	
	virtual void Send_PACKET(t_PACKET *pSendPacket, bool bSendToWorld=false) = 0{ *(int*)0 = 10; }

public   :
	CSendPACKET ();
	virtual	~CSendPACKET();

	void Send_cli_HEADER (WORD wType, bool bSendToWorld );

	void Send_cli_LOGIN_REQ (char *szAccount, char *szPassword, bool bEncode );
	void Send_cli_LOGOUT_REQ ();
	void Send_cli_LOGOUT_CANCEL ();

	void Send_cli_CHAR_LIST ();
	void Send_cli_CHANNEL_LIST_REQ( int iServerID );
	void Send_cli_SELECT_SERVER( int iServerID , int iChannelNo);
//	void Send_cli_JOIN_WORLD (DWORD dwLSVID);
	void Send_cli_JOIN_SERVER_REQ (DWORD dwLSVID, bool bWorldServer=false);

	void Send_cli_CREATE_CHAR (char *szCharName, BYTE btCharRace, char cBoneStone, char cHairIdx, char cFaceIdx, char cWeaponType, char cZoneNo);
	void Send_cli_DELETE_CHAR (BYTE btCharNO,bool bDeleteOrRevive, char *szCharName);

	void Send_cli_SELECT_CHAR (BYTE btCharNO, char *szCharName);

	void Send_cli_JOIN_ZONE ( BYTE btWeightRate );
	void Send_cli_REVIVE_REQ (BYTE btReviveTYPE);
	void Send_cli_SET_REVIVE_POS ();

	void Send_cli_SET_VAR_REQ (BYTE btVarTYPE, int iValue);

	void Send_cli_SET_MOTION ( short nMotionNO, BYTE btStopCMD = 0, WORD wETC = 0 );
	void Send_cli_TOGGLE (short nToggleTYPE);

	void Send_cli_CHAT (char *szMsg);
	void Send_cli_WHISPER (char *szTargetAccount, char *szMsg);
	void Send_cli_SHOUT (char *szMsg);
	void Send_cli_PARTY_CHAT(char *szMsg);

	void Send_cli_STOP (D3DVECTOR &PosCUR);
	void Send_cli_ATTACK (int iClientTarget);
	void Send_cli_DAMAGE (CObjCHAR *pAtkOBJ, CObjCHAR *pDefOBJ, WORD wDamage);
	void Send_cli_MOUSECMD (int iClientTarget, D3DVECTOR &PosTO);
	void Send_cli_CANTMOVE ();

	void Send_cli_STORE_TRADE_REQ(WORD wNPCObjIDX, char cBuyCNT, char cSellCNT, tag_BUY_ITEM *pBuyITEMs, tag_SELL_ITEM *pSellITEMs);

	void Send_cli_SET_HOTICON (BYTE btListIDX, tagHotICON HotICON);
//	void Send_cli_CLICK_HOTICON (BYTE btListIDX);

	bool Send_cli_USE_ITEM (short nInventoryIndex, bool bTargetSkill, int iClientTarget);

	void Send_cli_CHANGE_SKIN (BYTE btBodyIDX, short nItemNO);

	void Send_cli_EQUIP_ITEM (short nEquipInvIDX, short nWeaponInvIDX);
	bool Send_cli_GET_FIELDITEM_REQ( CGameOBJ *pUSER, int iServerObject );

	// 필드에 아이템을 떨군다...
	void Send_cli_DROP_ITEM ( short nInventoryIndex, int iQuantity );

	void Send_cli_TELEPORT_REQ (CGameOBJ *pUSER, short nWarpIDX);

	// 보너스 포인트를 이용해서 기본 능력치를 향상 시킨다...
	void Send_cli_USE_BPOINT_REQ (BYTE btAbilityTYPE);

	void Send_cli_TRADE_REQ (void);
	void Send_cli_TRADE_REPLY (void);

	/// 서버에 해당 오브젝트 HP 정보 요구
	void Send_cli_HP_REQ( int iClientTarget ); 


	void Send_cli_P_STORE_OPEN ( BYTE btSellItemCount,BYTE btBuyItemCount, tagPS_ITEM* SellItems, tagPS_ITEM* BuyItems, char* pszTitle );
	void Send_cli_P_STORE_CLOSE (void);
	void Send_cli_P_STORE_LIST_REQ ( WORD wStoreObjectIDX );
	void Send_cli_P_STORE_BUY_REQ ( WORD wStoreObjectIDX, int iItemCount, tagPS_SLOT_ITEM* pBuyItems );
	void Send_cli_P_STORE_SELL_REQ( WORD wStoreObjectIDX, int iItemCount, tagSELL_ITEM*	pSellItems );
	void Send_cli_SKILL_LEARN_REQ(short nSkillIDX);
	void Send_cli_SKILL_LEVELUP_REQ (BYTE btSkillSLOT, short nNextLevelSkillIDX=0);
	void Send_cli_SELF_SKILL (BYTE btSkillSLOT);
	void Send_cli_TARGET_SKILL (int iClientTarget, BYTE btSkillSLOT);
	void Send_cli_POSITION_SKILL (D3DVECTOR &PosTO, BYTE btSkillSLOT);

	void Send_cli_QUEST_REQ (BYTE btReqTYPE, BYTE btQuestSLOT, int iQuestID, char *szQuestTriggerName=NULL);

	///트레이드 관련 Method
	bool Send_cli_TRADE_P2P( WORD wServerIdx, BYTE btResult ,char cSlotIdx = 0 );
	void Send_cli_TRADE_P2P_ITEM( char	cTradeSLOT,short nInventoryIndex, DWORD	iQuantity );	
	///파티 관련
	void Send_cli_PARTY_REQ(BYTE btRequest, DWORD dwDestIDXorTAG );
	void Send_cli_PARTY_REPLY(BYTE btRequest, DWORD dwDestIDXorTAG );
	void Send_cli_PARTY_RULE( BYTE btRule );
	///제조 관련
	void Send_cli_CREATE_ITEM_REQ( BYTE btSkillSLOT, char cTargetItemTYPE, short nTargetItemNO, short* pnUseItemINV );
//	void Send_cli_CREATE_ITEM_EXP_REQ();


	void Send_cli_ITEM_RESULT_REPORT( BYTE btREPORT, BYTE btItemType, short nItemNo );

	///창고 관련
	void Send_cli_MOVE_ITEM( BYTE btMoveTYPE,BYTE btFromIDX,tagITEM& MoveITEM ,bool bPlatinum );
	void Send_cli_BANK_LIST_REQ( BYTE btREQ, char* pszPassword );
	///소모탄 관련
	void Send_cli_SET_BULLET( BYTE btShotType, short nInvenIdx );

	///PAT 관련
	void Send_cli_ASSEMBLE_RIDE_ITEM( short nPartIdx, short nInvenIdx );

	void Send_cli_SET_WEIGHT_RATE( BYTE btWeightRate );
	void Send_cli_SET_WISHITEM( int iIndex, tagITEM& Item );
 
	void Send_cli_MCMD_APPEND_REQ (char *szName);
	void Send_cli_MCMD_APPEND_REPLY (BYTE btCMD, WORD wUserIDX, char* pszName );
	void Send_cli_MCMD_TAG (BYTE btCMD, DWORD dwUserTAG);
	void Send_cli_MCMD_STATUS_REQ (BYTE btStatus);

	void Send_cli_MESSENGER_CHAT( DWORD dwUserTag, char* pszMsg );

	void Send_tag_CHAT_HEADER (BYTE btRoomCMD);
	void Send_cli_CHAT_ROOM_MAKE (BYTE btRoomTYPE, BYTE btMaxUSER, char *szRoomTitle, char *szPasswd);
	void Send_cli_CHAT_ROOM_JOIN (BYTE btRoomTYPE, WORD wRoomID, char *szPasswd);
	void Send_cli_CHAT_ROOM_KICK ( t_HASHKEY HashUSER );
	void Send_cli_CHAT_ROOM_LIST (BYTE btRoomTYPE, WORD wFromRoomID);

	void Send_cli_MEMO( char* pszName, char* pszMemo );
	void Send_cli_MEMO_REQ();
	void Send_cli_MEMO_CNT_REQ();

	void Send_cli_CHATROOM_MSG( char* pszMsg );


	void Send_cli_USE_ITEM_TO_REPAIR( short nUsedItemInvenIdx, short nTargetItemInvenIdx );
	void Send_cli_REPAIR_FROM_NPC( WORD wNpcSvrIdx, short nTargetItemInvenIdx );

	//----------------------------------------------------------------------------------------------------	
	///
	/// @brief 재밍 재련관련
	///
	//----------------------------------------------------------------------------------------------------
	void Send_cli_CRAFT_GEMMING_REQ( BYTE btEquipInvIDX, BYTE btGemInvIDX );	
	void Send_cli_CRAFT_BREAKUP_REQ( BYTE btType, short nSkillSLOTorNpcIDX, BYTE btTargetInvIDX );	
	void Send_cli_CRAFT_UPGRADE_REQ( BYTE btType, short nSkillSLOTorNpcIDX, BYTE btTargetInvIDX, BYTE btUseItemINV[ UPGRADE_ITEM_STEP ] );



	void Send_cli_CHAR_CHANGE();

	void Send_cli_APPRAISAL_REQ( int iInventoryIndex );


	void Send_cli_CLAN_COMMAND( BYTE btCmd , char* pszMsg);
	void Send_cli_CLAN_CREATE( WORD wMarkBack, WORD wMarkCenter, char* pszName, char* pszSlogan );
	void Send_cli_CLAN_CHAT( char *szMsg );

	void Send_cli_CLANMARK_REQ( int iClanID );
	void Send_cli_CLANMARK_SET( unsigned char* pDATA, int iSize , WORD crc16 );



	void Send_cli_ALLIED_CHAT( char* pszMsg );
	void Send_cli_ALLIED_SHOUT( char* pszMsg );

	void Send_cli_CLAN_MEMBER_JOBnLEV( short nLev, short nJob );




	void Send_cli_MALL_ITEM_LIST_REQ();
	void Send_cli_MAIL_ITEM_FIND_CHAR( char* pszName );
	void Send_cli_MALL_ITEM_GIVE( BYTE slotindex, char* pszName );
	void Send_cli_MALL_ITEM_BRING( BYTE slotindex, short nDupCount );


	void Send_cli_CLANMARK_REG_TIME();

	void Send_cli_MOVE_ZULY_INV2BANK( __int64 i64MoveZuly );
	void Send_cli_MOVE_ZULY_BANK2INV( __int64 i64MoveZuly );

	void Send_cli_CART_RIDE( BYTE bType, WORD wOwnerObjIDX_, WORD wGuestObjIDX_ );

//---스샷관련...--------------------------------------------------------------------------------------
	void Send_cli_SCREEN_SHOT_TIME();
	void Send_cli_UPDATE_NAME( char* szName );
	void Send_cli_SET_RIGHTS( DWORD dwRight );
//
#ifdef	__VIRTUAL_SERVER
	void Send_gsv_ADD_CHAR (int iObjectIndex, short nCharIdx, tPOINTF &PosSET);
	void Send_gsv_ADD_USER (CObjAVT *pAvtCHAR);
	void Send_gsv_ADD_FIELDITEM (int iObjectIndex, tagITEM &sITEM);
#endif
} ;


//-------------------------------------------------------------------------------------------------
#endif