/*
	2004. 3. 22 추가 사항 - NPC의 인공지능에 의해서 실시간 수정되는 이벤트 값( m_nEventSTATUS ) 전송.
		. GSV_SET_EVENT_STATUS 패킷 추가.
		. gsv_NPC_CHAR 패킷의 멤버 변수 m_nEventSTATUS 추가.
		각 NPC의 고유 이벤트 데이타에서 m_nEventSTATUS 값을 참조하여 
		상황에 맞는 대화 연출및 퀘스트를 부여...

	2004. 3. 24 추가 사항 - 월드에서 관리되는 이벤트 오브젝트 객제 생성/삭제.
		. gsv_ADD_EVENTOBJ 패킷.

	2004. 3. 30 추가 사항 - NPC 스킬사용 패킷에 스킬 모션 번호 설정..
		. gsv_SELF_SKILL, gsv_TARGET_SKILL 패킷에 cNpcSkillMOTION 변수 추가
		. 사용자 스킬일 경우 이값을 무시, NPC 스킬일경우 패킷 크기가 +1 되면 cNpcSkillMOTION[0]에 모션 번호 들어감.

	2004. 3. 31 추가 사항 - gsv_DAMAGE_OF_SKILL 패킷 추가
		. gsv_EFFECT_OF_SKILL 패킷으로 스킬 적용상태, 데미지 둘중 하나의 값이 전달되었지만
		. gsv_DAMAGE_OF_SKILL 패킷은 데미지와 스킬 적용샅애가 같이 들어 있는 패킷..
		. 스킬 타입 7, 17번의 결과로 전송됨.

	2004. 3. 31 수정 사항.
		. gsv_EFFECT_OF_SKILL, gsv_DAMAGE_OF_SKILL 구조 변경.
		. 스킬 사용한 객체 중심에서, 스킬이 적용된 객체 중심으로...
		. 스킬에 적용된 객체는 gsv_EFFECT_OF_SKILL, gsv_DAMAGE_OF_SKILL의 패킷에 의해 적용되며,
		. 스킬을 사용한 객체는 위의 결과 패킷이 전송된후 gsv_RESULT_OF_SKILL이 전송된다.

	2004. 4. 1  추가 사항. 
		. tag_ADD_CHAR 팀번호 추가 CAI_OBJ::Is_ALLIED(상대팀번호)를 호출하여 true면 아군, false면 적군.

	2004. 4. 2  수정/추가 사항
		. tag_FIELDITEM구조 에서 아이템 남은 시간(m_wRemainTIME)을 제외시켜 tag_DROPITEM으로 이름 변경.
		. 일반 필드아이템일경우 gsv_ADD_FIELDITEM에 m_wRemainTIME 멤버 변수값으로 아이템 시간 설정.
		. 데미지 패킷의 tag_DROPITEM으로 생성한 아이템은 남은 시간을 ITEM_OBJ_LIVE_TIME으로 설정.
		. gsv_DAMAGE_OF_SKILL에 아이템 드롭시 tag_DROPITEM[0]에 값 들어감. 사용법은 일반 데미지 아이템과 동일

	2004. 4. 8  추가 사항
		. gsv_SKILL_LEARN_REPLY 패킷의 응답 종류에 RESULT_SKILL_LEARN_DELETE 추가..
		. 응답이 RESULT_SKILL_LEARN_DELETE 일경우 CUserDATA::Skill_DELETE(slot,skill)을 호출하여 삭제한다.

	2004. 4. 13 수정/추가 사항
		. tag_ADD_CHAR 멥버 m_btMoveMODE의 이동 상태값 추가..
		. 승용 아이템 교환시:: cli_ASSEMBLE_RIDE_ITEM, gsv_ASSEMBLE_RIDE_ITEM 패킷 사용.

	2004. 4. 16 수정 사항.
		. wsv_CHAR_LIST패킷에 tagBasicInfo대신 tagCHARINFO로 대체, m_nPartItemIDX 추가.
		. tag_ADD_CHAR 구조체에 소환된 케릭터 구분 기능 추가 ::
		  tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_LIFE_TIME 일경우 owner_obj_idx = (WORD)pAdjSTATUS[ btIDX++ ];
		  로 설정하면됨.
	    . CLI_APPRAISAL_REQ, GSV_APPRAISAL_REPLY 패킷 추가.

	2004. 4. 20 수정 사항.
		. gsv_MOVE 패킷의 m_bRunMODE => m_btMoveMODE ( tag_ADD_CHAR::m_btMoveMODE와 같은값 )
		. gsv_JOIN_ZONE패킷의 m_dwWorldTIME을 m_dwAccWorldTIME로 변경... 게임 서버의 누적 동작 월드 진행 시간

	2004. 4. 28 수정/추가 사항.
	    . gsv_RESULT_OF_SKILL의 m_nResultCNT를 m_nSKillIDX로 변경.
		. gsv_PARTY_LEVnEXP에 m_btFlagLevelUP 추가.
		. GSV_SKILL_START 패킷 추가 : 서버에서 스킬 케스팅 동작시 전송된다.

	2004. 4. 29 추가 사항.
		. cli_SET_WEIGHT_RATE추가, 클라이언트에서 아이템 소지량 체크해서 현재소지량/최대 소지량 비율이
		. 80%이상 뛰기 불등, 100%이상 이동 불능 판단한다.
		. cli_SET_WEIGHT_RATE받은 패킷은 서버에의해 주변에 gsv_SET_WEIGHT_RATE패킷으로 중계됨.
		. 아바타 조인시 패킷 gsv_AVT_CHAR에 m_btWeightRate에 현재 무계비율 입력되어 있음. 
		. cli_JOIN_ZONE패킷 안에 m_btWeightRate 추가.. 서버에 요청시 m_btWeightRate에 무게 비율을 넣어 전송;

	2004. 5. 1 수정 사항
		. gsv_TARGET_SKILL패킷에 속도보정을 위해 스킬 사용주체 좌표, 타겟 좌표 추가...
		. 추가 멤버: m_wSrvDIST, m_PosTO - gsv_ATTACK 패킷의 속도 보정과 동일하게 적용하여 사용.
		. 타겟 스킬의 경우 앞에서 멈칫하여 gsv_SKILL_START패킷을 기다리던 현상 수정될듯...
		. gsv_MOUSECMD, gsv_ATTACK멤버의 m_PosCUR대신 m_wSrvDIST로 변경.
		. cli_CANTMOVE, gsv_ADJUST_POS, cli_MOUSECMD, gsv_MOUSECMD, cli_JOIN_ZONE, gsv_AVT_CHAR Z값 추가.

	2004. 5. 11 수정 사항.
		. gsv_INVENTORY_DATA패킷에 m_WishITEMS[ MAX_WISH_ITEMS ] 추가, 사용 용도는 개인 상점 오픈시...
		. 개인 상점 거래용 패킷 cli_P_STORE_XXX, gsv_P_STORE_XXXX 추가.
		. cli_SET_WISHITEM 추가 상점등에서 구입 원하는 아이템을 설정할수 있다(인터넷 쇼핑몰처럼..)

	2004. 5. 13. 추가 사항.
		. cli_CHATROOM, wsv_CHATROOM, cli_CHATROOM_MSG, wsv_CHATROOM_MSG 패킷 추가
		. 기능: 월드서버에서 일반 채팅 서버처럼, 게임상에서 동작 - 채팅룸을 제공함.

	2004. 5. 17. 추가 사항.
		. cli_MEMO, wsv_MEMO패킷 추가 케릭터간 쪽지 보내기/받기 기능

	2004. 5. 18. 추가 사항.
		. CLI_MESSENGER, WSV_MESSENGER 패킷 추가..  친구 관리 패킷..
		. 1:1 채팅시는 cli_MESSENGER_CHAT, wsv_MESSENGER_CHAT패킷으로 송/수신 함.

	2004. 5. 31. 추가 사항
		. CLI_CHAR_CHANGE 추가.. 게임 플레이도중 케릭터 선택화면 이동 이동할때 게임서버에 전송..
		. 결과는 월드서버에서 WSV_CHAR_CHANGE 패킷이 전송된. 패킷 구조는 t_PACKETHEADER 사용

	2004. 6. 1.  추가 사항.
		. tag_ADD_CHAR 구조체에 소환된 케릭터에 사용된 소환 스킬 번호 추가 ::
		  기존 tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_DEC_LIFE_TIME 일경우 owner_obj_idx = (WORD)pAdjSTATUS[ btIDX++ ];
		  에서 owner_obj_idx != 0 이면 nUsedSummonSkillIDX = pAdjSTATUS[ btIDX ++ ]로 얻는다.
		. 소환된 몹의 특성치중 몇몇(MaxHP,공격력,방어력...) 은 소환에 사용된 스킬 번호로 얻을수 있다.

	2004. 6. 8.	 추가 사항.
		. 아이템 수리 요청,결과 패킷 ( 소모성아이템 사용시, NPC에 의한 수리시 ) 추가
		. 아이템 수명 변경및, 수리 결과 패킷은 기존의 gsv_SET_MONEYnINV, gsv_SET_INV_ONLY를 사용해
		. 패킷 타입만 변경되어 전송됨
		. 아이템 수명 변경 패킷 추가 : GSV_SET_ITEM_LIFE => 수명이 0으로 설정될 경우 UpdateAbility()를 호출해줘야 함

	2004. 6. 14. 수정 사항.
		. gsv_TOGGLE, gsv_EQUIP_ITEM, gsv_ASSEMBLE_RIDE_ITEM 패킷에 m_nRunSPEED[0]에 변경시 이동속도 추가...
		. if ( pPacket->m_HEADER.m_nSize == ( sizeof( gsv_XXX ) + sizeof(short) ) ) 이면 m_nRunSPEED[0] 값으로 이동 속도 적용.

	2004. 6. 16. 추가 사항.
		. GSV_P_STORE_MONEYnINV 패킷 타입 추가, 구조는 gsv_SET_MONEYnINV와 동일.
		. gsv_P_STORE_RESULT 결과에 RESULT_P_STORE_TRADE_PART 추가 거래시 일부 품목이 품절되거나 구입 희망 갯수보다
		. 작개 구입시 전송됨..

	2004. 6. 17. 추가 사항.
		. 부활시 패널티 경험치 적용방법 변경으로 gsv_JOIN_ZONE 패킷에 현재 경험치 m_lCurEXP 추가

	2004. 6. 28. 추가 사항.
		. CLI_CHANNEL_LIST_REQ 패킷 추가. srv_LOGIN_REPLY 패킷 수신후 원하는 월드서버의 ID를 전송하면
		. 월드 서버의 채널 서버리스트가 전송됨. 월드서버의 채널을 선택후 cli_SELECT_SERVER패킷에 채널 번호를
		. 추가해서 전송시켜야함.
	2004. 7. 8. 추가 사항.
		. CLI_CRAFT_ITEM_REQ, GSV_CRAFT_ITEM_REPLY 패킷 추가... 클라이언트 요청시에는 각각에 맞는 패킷에
		. 넣어 전송, 서버의 응답 패킷(GSV_CRAFT_ITEM_REPLY)은 공통, m_btRESULT의 값이 성공관련 값일경우에만
		. m_btOutCNT에 유효한 값이 들어있고, m_btRESULT이 실패관련일경우 참조하면 안됨(재련실패는예외-등급감소,소모된 아이템들어있음).
	2004. 7. 10. 추가 사항.
		. wsv_CHAT_ROOM_JOIN 패킷에 자신의 m_wUserID추가, 서버에서 CHAT_REPLY_ROOM_MAKED응답시
		. 방이름 앞에 m_wUserID(자신) 삽입.
		. gsv_CRAFT_ITEM_REPLY패킷중 m_btRESULT값이 CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED	일경우
		. m_sInvITEM[ m_btOutCNT-1 ].m_iQuantity에 성공도 계산된값이 들어있음(기획서참고)
	2004. 7. 20. 추가 사항.
		. gsv_AVT_CHAR에 ( tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_STORE_MODE ) 일경우 char szStoreTITLE[] 붙여 전송
		. GSV_SKILL_CANCEL 패킷 추가
	2004. 7. 26. 추가 사항.
		. gsv_PARTY_MEMBER에 파티의 현재 룰 추가.
		. cli_PARTY_ITEM, gsv_PARTY_ITEM 패킷 추가. 파티에서 아이템습득이 순서 분배일경우, 습득자와 아이템 정보들어 있음
	2004. 8. 8 수정사항.
		. MOB의 HP값을 short에서 int로 변경.
	2004. 8. 10 추가 사항.
		. tagCHARINFO 구조체에 m_dwRemainSEC멤버변수 추가, 값이 0이 아닐경우 삭제될때까지 남은 시간(초단위).
		. cli_DELETE_CHAR패킷에 m_bDelete 멤버변수 추가, 서버에 요청시 true면 삭제대기등록, false면 삭제대기 해제,
		. 응답 패킷으로 wsv_DELETE_CHAR 전송됨.
	2004. 8. 23 추가 사항.
		. gsv_SETEXP 패킷에 현재 스테미너 값 추가
	2004. 9. 16 추가 사항.
		. 파티 멥버 정보 tag_PARTY_MEMBER에 m_nSTAMINA 추가.
		. 추가로 파티 원이 스테미너가 < 3000 이하일경우 ... gsv_CHANGE_OBJIDX 패킷이 추가 전송됨
	2004. 10. 4 추가 사항.
		. gsv_SET_HPnMP패킷 추가 : 서버의 현재 hp, mp가 전송됨
	2004. 10. 5 추가 사항.
		. gsv_CHECK_NPC_EVENT패킷 추가, 기존 NPC죽였을때 클라이언트에서 NPC이벤트를 체크후 서버에 검증 요청하던것을
		. 서버가 클라이언트에게 검증요청 => 클라이언트 검증후 검증완료되면 => 서버에 검증요청 순서로 바꿈.
		. 마지막에 몹을 죽였던 사용자에게 들어가던 퀘스트아이템을 선빵때린 유저에게 들어가도록...
	2004. 10. 15 수정 / 추가 사항.
		. tag_ADD_CHAR 구조체의 char m_cTeamNO가 int m_iTeamNO로 변경
		. gsv_JOIN_ZONE 구조체에 자신의 팀번호 추가 int m_iTeamNO;
	2004. 10. 30 추가 사항.
		. 클랜 패킷 추가 CLI_CLAN_COMMAND, WSV_CLAN_COMMAND
		. 클랜관련 패킷은 모두 월드 서버에 전송함.
	2004. 11. 16 추가 / 수정.
		. CLI_ALLIED_CHAT, CLI_ALLIED_SHOUT, GSV_ALLIED_CHAT, GSV_ALLIED_SHOUT 추가
		. PvP 존에서 같은 팀번호에게만 채팅을 전송한다.
		. gsv_AVT_CHAR구조체의 m_dwSubFLAG추가 DataType.h의 FLAG_SUB_xxx 플레그 설정됨
		. tag_MY_CLAN구조체안의 m_btClanPOS(클랜내 직위)를 tag_CLAN_ID 구조체로 이동.
		. 서버에 보내는 클랜 명령에 GCMD_MEMBER_JOBnLEV추가 :: 자신의 직업/레벨을 다른 멤버에게 통보( 로그인후/레벨업후/전직후에 전송 )
	2004. 11. 23 수정.
		. tag_CLAN_INFO 삭제, tag_MY_CLAN 멥머 추가
	2004. 12. 1 추가
		. 10초 대기해야할경우 GSV_LOGOUT_REPLY 전송
	2004. 12. 16 추가
		. 제조/제련시 주위에 통보 패킷( cli_ITEM_RESULT_REPORT ) 추가, cli_CREATE_ITEM_EXP_REQ 패킷은 삭제
	2004. 12. 21 추가 
		. gsv_EFFECT_OF_SKILL에 스킬 시전자의 지력 m_nINT값 추가.
	2005. 01. 05 추가
		. CLI_SELECT_SERVER요청시 응답 결과 RESULT_SELECT_SERVER_NEED_CHARGE 추가 (무료사용자가 본섭에 접속시도할때)
		. 과금 관련 패킷 추가. gsv_BILLING_MESSAGE
	2005. 01. 11 수정
		. gsv_CHANGE_SKIN수정, gsv_SET_ABILITY패킷은 자신한테만 전달되고 주위에 통보 필요한 경우(머리털,얼굴,성별) gsv_CHANGE_SKIN로 전송됨
		. tagCHARINFO 서버에서 케릭터 리스트에 플레티넘케릭터 여부 필드 추가
		. wsv_CREATE_CHAR케릭터 생성 패킷에.... 플레티넘 케릭으로 생성된건지 여부...
		. gsv_BANK_LIST_REPLY패킷에 BANK_REPLY_PLATINUM 추가...플레티넘 창고리스트가 별도로 전송됨(아이템이 존제할경우)
	2005. 03. 23 추가
		. 클랜 마크 요청 패킷 cli_CLANMARK_REQ, wsv_CLANMARK_REPLY 추가
	2005. 06. 07 수정/추가
		. GSV_NPC_CHAR 패킷시 m_nCharIdx가 -(음수) 이면 숨김 상태이다.
		. gsv_SET_NPC_SHOW NPC의 숨김/보임 상태 설정.
	2005. 06. 16 추가
		. 기존 gsv_BILLING_MESSAGE에서 확장된 형태의 gsv_BILLING_MESSAGE_EXT 패킷 추가
		. defined FLAG_PAY_XXX 형태가 PAY_FLAG_국가_XXX 로 이름 변경
	2005. 06. 20 추가
		. 여신 소환 관련 패킷 GSV_GODDNESS_MODE 추가.
	2005. 06. 22 수정
		. cli_TELEPORT_REQ 패킷에 스피드핵을 방지하기 위해 서버와 비교할 좌표 클라이언트의 현재 좌표(m_PosCUR) 추가
	2005. 06. 23 추가
		. 파티 / 1:1 거래시 과금없을시 응답 추가
		. 소환수 명령 패킷 추가.( CLI_SUMMON_CMD, GSV_SUMMON_CMD )
	2005. 08. 22 추가
		. CLI_CART_RIDE / GSV_CART_RIDE 2인승카트 요청/응답 패킷 추가.
*/		  
#ifndef __NET_PROTOTYPE_H
#define __NET_PROTOTYPE_H
#ifdef	__SERVER
	#include "DataTYPE.h"
	#include "CUserDATA.h"
	#include "PacketHEADER.h"
	#include "classHASH.h"
#else
	#include "../common/DataTYPE.h"
	#include "../common/CUserDATA.h"
	#include "../util/PacketHEADER.h"
	#include "../util/classHASH.h"
#endif



#define	__APPLY_04_10_15_TEAMNO			// 04.10.15일자 팀번호 변경 패킷 적용할래 ?
#define	__INC_PLATINUM				// 유료화변경 적용여부..



#pragma warning( disable:4200 )
#pragma warning( disable:4201 )
#pragma pack (push, 1)

//-------------------------------------------------------------------------------------------------

#define	CLI_ALIVE					0x0700
#define	SRV_ERROR					0x0700

#define	SRV_ANNOUNCE_TEXT			0x0701
#define	GSV_ANNOUNCE_CHAT			0x0702

#define	CLI_ACCEPT_REQ				0x0703

#define	CLI_CHANNEL_LIST_REQ		0x0704
#define	LSV_CHANNEL_LIST_REPLY		0x0704

#define	CLI_CHECK_AUTH				0x0705
#define	SRV_CHECK_AUTH				0x0705

// 0x0706

#define	CLI_LOGOUT_REQ				0x0707
#define GSV_LOGOUT_REPLY			0x0707

#define CLI_LOGIN_REQ				0x0708
#define LSV_LOGIN_REPLY				0x0708
#define	GSV_LOGIN_REPLY				0x0709

#define	CLI_SELECT_SERVER			0x070a
#define	LSV_SELECT_SERVER			0x070a

/*
#define	CLI_JOIN_WORLD				0x070b
#define WSV_JOIN_WORLD				0x070c		// 월드 서버로 부터 접속 검증 여부...
*/

#define	CLI_JOIN_SERVER_REQ			0x070b		// 월드/존 서버로 접속을 요청한다.
#define	SRV_JOIN_SERVER_REPLY		0x070c


#define	GSV_GM_COMMAND				0x070d		// GM 명령어.

#define	GSV_SET_GLOBAL_VAR			0x070e		// 서버 변수 설정.
#define	GSV_SET_GLOBAL_FLAG			0x070f		// 서버 플레그 값...

#define	WSV_MOVE_SERVER				0x0711		// 존서버 이동해라...

#define	CLI_CHAR_LIST				0x0712
#define	WSV_CHAR_LIST				0x0712

#define	CLI_CREATE_CHAR				0x0713
#define	WSV_CREATE_CHAR				0x0713

#define	CLI_DELETE_CHAR				0x0714
#define WSV_DELETE_CHAR				0x0714

#define	CLI_SELECT_CHAR				0x0715
#define	GSV_SELECT_CHAR				0x0715


#define	GSV_INVENTORY_DATA			0x0716
#define	GSV_SET_MONEYnINV			0x0717
#define	GSV_SET_INV_ONLY			0x0718

#define	GSV_SERVER_DATA				0x0719

#define	GSV_RELAY_REQ				0x071a	
#define	CLI_RELAY_REPLY				0x071a

#define	GSV_QUEST_DATA				0x071b

#define	CLI_CHAR_CHANGE				0x071c		// 케릭터 선택창으로 이동요청 :: 게임 서버로 패킷 전송해야함.
#define	WSV_CHAR_CHANGE				0x071c		// 이 패킷 받으면 CLI_CHAR_LIST을 서버에 요청 :: 월드 서버에서 전송되어짐

#define GSV_SET_MONEY_ONLY			0x071d		// 서버에서의 현재돈... 

#define	GSV_REWARD_MONEY			0x071e		// 돈이 보상 되었다... 구조는 gsv_SET_MONEY_ONLY
#define	GSV_REWARD_ITEM				0x071f		// 아이템 보상 되었다
#define	GSV_REWARD_ADD_ABILITY		0x0720		// 능력치 추가
#define	GSV_REWARD_SET_ABILITY		0x0721		// 능력치 설정

#define	CLI_LOGOUT_CANCEL			0x0722

#define	GSV_QUEST_ONLY				0x0723
#define	GSV_WISH_LIST				0x0724


#define	CLI_QUEST_REQ				0x0730
#define	GSV_QUEST_REPLY				0x0730

#define	GSV_CHECK_NPC_EVENT			0x0731

#define	GSV_CHEAT_CODE				0x0751


#define	CLI_JOIN_ZONE				0x0753
#define	GSV_JOIN_ZONE				0x0753

#define GSV_INIT_DATA				0x0754

#define	CLI_REVIVE_REQ				0x0755
#define	GSV_REVIVE_REPLY			0x0755

#define	CLI_SET_REVIVE_POS			0x0756

#define	CLI_SET_VAR_REQ				0x0757
#define	GSV_SET_VAR_REPLY			0x0757

#define CLI_CHAR_INFO_REQ			0x0761
#define	GSV_CHAR_INFO_REPLY			0x0761

#define	CLI_SET_WEIGHT_RATE			0x0762
#define	GSV_SET_WEIGHT_RATE			0x0762

#define	GSV_ADJUST_POS				0x0770


#define	CLI_CANTMOVE				0x0771
#define	GSV_CANTMOVE				0x0771

// #define	CLI_ATTACK_START			0x0772
// #define	GSV_ATTACK_START			0x0772
// #define	GSV_ATTACK_STOP				0x0773

#define	GSV_CHANGE_NPC				0x0774

#define	CLI_SUMMON_CMD				0x0775
#define	GSV_SUMMON_CMD				0x0775

// 0x77

#define	CLI_SET_MOTION				0x0781
#define	GSV_SET_MOTION				0x0781

#define	CLI_TOGGLE					0x0782
#define	GSV_TOGGLE					0x0782

#define	CLI_CHAT					0x0783
#define	GSV_CHAT					0x0783

#define	CLI_WHISPER					0x0784
#define	GSV_WHISPER					0x0784

#define	CLI_SHOUT					0x0785
#define	GSV_SHOUT					0x0785

#define	CLI_PARTY_CHAT				0x0786
#define GSV_PARTY_CHAT				0x0786

#define	CLI_CLAN_CHAT				0x0787
#define	WSV_CLAN_CHAT				0x0787

#define	CLI_ALLIED_CHAT				0x0788
#define GSV_ALLIED_CHAT				0x0788

#define	CLI_ALLIED_SHOUT			0x0789
#define	GSV_ALLIED_SHOUT			0x0789

#define	GSV_SET_EVENT_STATUS		0x0790

#define	GSV_NPC_CHAR				0x0791
#define	GSV_MOB_CHAR				0x0792
#define	GSV_AVT_CHAR				0x0793

#define GSV_SUB_OBJECT			    0x0794

#define CLI_SETPOS				    0x0795

#define	CLI_STOP					0x0796
#define	GSV_STOP					0x0796

#define GSV_MOVE					0x0797

#define	CLI_ATTACK					0x0798
#define GSV_ATTACK					0x0798

#define	CLI_DAMAGE					0x0799
#define GSV_DAMAGE					0x0799

#define	CLI_MOUSECMD				0x079a
#define	GSV_MOUSECMD				0x079a

#define	GSV_SETEXP					0x079b

#define GSV_LEVELUP					0x079e

#define	CLI_HP_REQ					0x079f
#define	GSV_HP_REPLY				0x079f

#define	GSV_SET_HPnMP				0x07a0

#define	CLI_STORE_TRADE_REQ			0x07a1
#define	GSV_STORE_TRADE_REPLY		0x07a1

//#define	CLI_CREATE_ITEM_EXP_REQ		0x07a2

#define	CLI_USE_ITEM				0x07a3
#define GSV_USE_ITEM				0x07a3

#define	CLI_DROP_ITEM				0x07a4

#define	CLI_EQUIP_ITEM				0x07a5
#define	GSV_EQUIP_ITEM				0x07a5

#define	GSV_ADD_FIELDITEM			0x07a6
#define	GSV_SUB_FIELDITEM			0x07a6

#define	CLI_GET_FIELDITEM_REQ		0x07a7
#define GSV_GET_FIELDITEM_REPLY		0x07a7

#define CLI_TELEPORT_REQ			0x07a8
#define GSV_TELEPORT_REPLY			0x07a8

#define	CLI_USE_BPOINT_REQ			0x07a9
#define GSV_USE_BPOINT_REPLY		0x07a9

#define	CLI_SET_HOTICON				0x07aa
#define	GSV_SET_HOTICON				0x07aa

#define	CLI_SET_BULLET				0x07ab
#define	GSV_SET_BULLET				0x07ab

//#define	CLI_CHANGE_SKIN				0x07ac
#define	GSV_CHANGE_SKIN				0x07ac

#define	CLI_BANK_LIST_REQ			0x07ad
#define	GSV_BANK_LIST_REPLY			0x07ad

#define	CLI_MOVE_ITEM				0x07ae
#define	GSV_MOVE_ITEM				0x07ae

#define	CLI_CREATE_ITEM_REQ			0x07af
#define	GSV_CREATE_ITEM_REPLY		0x07af


#define GSV_SKILL_LEARN_REPLY		0x07b0

#define	CLI_SKILL_LEVELUP_REQ		0x07b1
#define GSV_SKILL_LEVELUP_REPLY		0x07b1


#define	CLI_SELF_SKILL				0x07b2
#define	GSV_SELF_SKILL				0x07b2

#define	CLI_TARGET_SKILL			0x07b3
#define	GSV_TARGET_SKILL			0x07b3

#define	CLI_POSITION_SKILL			0x07b4
#define	GSV_POSITION_SKILL			0x07b4

#define	GSV_EFFECT_OF_SKILL			0x07b5
#define	GSV_DAMAGE_OF_SKILL			0x07b6

#define	GSV_CLEAR_STATUS			0x07b7
#define	GSV_SPEED_CHANGED			0x07b8

#define	GSV_RESULT_OF_SKILL			0x07b9

#define	CLI_APPRAISAL_REQ			0x07ba		// 아이템 감정 의뢰~~~
#define	GSV_APPRAISAL_REPLY			0x07ba

#define	GSV_SKILL_START				0x07bb		// 스킬 케스팅 스타트...

#define	CLI_CRAFT_ITEM_REQ			0x07bc
#define	GSV_CRAFT_ITEM_REPLY		0x07bc

#define	GSV_SKILL_CANCEL			0x07bd		// 시작했던 스킬 취소.
//	0x07be

#define	CLI_SET_WISHITEM			0x07bf

#define	CLI_TRADE_P2P				0x07c0		// 개인간 거래...
#define GSV_TRADE_P2P				0x07c0

#define	CLI_TRADE_P2P_ITEM			0x07c1		// 거래 아이템 추가 / 삭제.
#define	GSV_TRADE_P2P_ITEM			0x07c1

#define CLI_P_STORE_OPEN			0x07c2		// 개인 상점 개점 요청
#define GSV_P_STORE_OPENED			0x07c2

#define CLI_P_STORE_CLOSE			0x07c3		// 개인 상점 폐쇄 요청
#define	GSV_P_STORE_CLOSED			0x07c3

#define CLI_P_STORE_LIST_REQ		0x07c4		// 개인 상점 물건 리스트 요청
#define GSV_P_STORE_LIST_REPLY		0x07c4

#define CLI_P_STORE_BUY_REQ			0x07c5		// 개인 상점 물건 구매 요청
#define	CLI_P_STORE_SELL_REQ		0x07c6		// 개인 상점 물건 판매 요청

#define	GSV_P_STORE_RESULT			0x07c6
#define	GSV_P_STORE_MONEYnINV		0x07c7

#define	CLI_ASSEMBLE_RIDE_ITEM		0x07ca
#define	GSV_ASSEMBLE_RIDE_ITEM		0x07ca

#define	CLI_USE_ITEM_TO_REPAIR		0x07cb		// 소모성 수리 아이템으로 아이템 수리 요청
#define	GSV_USED_ITEM_TO_REPAIR		0x07cb		// gsv_SET_INV_ONLY

#define	CLI_REPAIR_FROM_NPC			0x07cd		// NPC를 통해 아이템 수리 요청
#define	GSV_REPAIRED_FROM_NPC		0x07cd		// gsv_SET_MONEYnINV

#define	GSV_SET_ITEM_LIFE			0x07ce		// 아이템의 수명이 변경됨

#define	CLI_PARTY_REQ				0x07d0
#define	GSV_PARTY_REQ				0x07d0

#define	CLI_PARTY_REPLY				0x07d1
#define	GSV_PARTY_REPLY				0x07d1

#define	GSV_PARTY_MEMBER			0x07d2		// 파티원 추가/삭제.
//#define	GSV_PARTY_INFO			0x07d3		// 파티 정보
#define	GSV_PARTY_ITEM				0x07d3		// 순서 분배시 아이템 습득 정보

#define	GSV_PARTY_LEVnEXP			0x07d4

#define	GSV_CHANGE_OBJIDX			0x07d5


#define	GSV_ADD_EVENTOBJ			0x07d6
// #define	GSV_SUB_EVENTOBJ			0x07d6

#define	CLI_PARTY_RULE				0x07d7
#define	GSV_PARTY_RULE				0x07d7

#define	CLI_ITEM_RESULT_REPORT		0x07d8
#define	GSV_ITEM_RESULT_REPORT		0x07d8

#define	CLI_MALL_ITEM_REQ			0x07d9
#define	GSV_MALL_ITEM_REPLY			0x07d9


#define	CLI_MOVE_ZULY				0x07da
#define	GSV_MOVE_ZULY				0x07da

#define	GSV_SET_NPC_SHOW			0x07db

#define	GSV_GODDNESS_MODE			0x07dc


#define	CLI_CART_RIDE				0x07dd
#define	GSV_CART_RIDE				0x07dd


#define	GSV_BILLING_MESSAGE_EXT		0x07de
#define	GSV_BILLING_MESSAGE			0x07df


#define CLI_CLAN_COMMAND			0x07e0
#define	WSV_CLAN_COMMAND			0x07e0

#define	CLI_MESSENGER				0x07e1
#define	WSV_MESSENGER				0x07e1

#define	CLI_MESSENGER_CHAT			0x07e2
#define	WSV_MESSENGER_CHAT			0x07e2
									   
#define	CLI_CHATROOM				0x07e3
#define	WSV_CHATROOM				0x07e3
									   
#define	CLI_CHATROOM_MSG			0x07e4
#define	WSV_CHATROOM_MSG			0x07e4

#define	CLI_MEMO					0x07e5
#define	WSV_MEMO					0x07e5

#define	CLI_CLANMARK_SET			0x07e6

#define	CLI_CLANMARK_REQ			0x07e7
#define	WSV_CLANMARK_REPLY			0x07e7

#define CLI_CLANMARK_REG_TIME		0x07e8
#define	WSV_CLANMARK_REG_TIME		0x07e8

#define	GSV_PATSTATE_CHANGE			0x07e9

#define	GSV_CHARSTATE_CHANGE		0x07ea


#define CLI_SCREEN_SHOT_TIME		0x07eb                          //스크린샷 찍을때 서버시간요청..(허재영 추가 2005.10.18)
#define GSV_SCREEN_SHOT_TIME		0x07eb                          //스크린샷 찍을때필요한 서버 시간전송.. .(허재영 추가 2005.10.18)

#define SRV_UPDATE_NAME				0x07ec
#define PXY_UPDATE_NAME				0x07ed
#define PXY_SET_RIGHTS				0x07ef


//-------------------------------------------------------------------------------------------------

struct cli_CHECK_AUTH : public t_PACKETHEADER {
	/*
		각 모듈별 데이타...
		필리핀의 경우 GG_AUTH_DATA 구조체
	*/
} ;
#define	AUTH_MODULE_nPROTECT	0xf1
struct srv_CHECK_AUTH : public t_PACKETHEADER {
	BYTE	m_btModuleTYPE;
	/*
		각 모듈별 데이타...
		필리핀의 경우 GG_AUTH_DATA 구조체
	*/
} ;

struct srv_ERROR : public t_PACKETHEADER {
	WORD	m_wErrorCODE;
	// szErrorMSG[]
} ;


struct lsv_ERROR : public srv_ERROR {
} ;
struct gsv_ERROR : public srv_ERROR {
} ;


struct gsv_GM_COMMAND : public t_PACKETHEADER {
	BYTE	m_btCMD;
	WORD	m_wBlockTIME;
} ;
#define	GM_CMD_SHUT		0x00		//	채팅 금지..
#define	GM_CMD_LOGOUT	0x01		//	접속 끊고 나가라...


struct cli_ACCEPT_REQ : public t_PACKETHEADER {
} ;

struct cli_LOGIN_REQ : public t_PACKETHEADER {
//	DWORD		m_dwBuildNo;							// 클라이언트 빌드 번호.
	union {
		BYTE	m_MD5Password[ 32 ];
		DWORD	m_dwMD5[ 8 ];
	} ;
    // szAccount[];
} ;
struct srv_LOGIN_REPLY : public t_PACKETHEADER {
    BYTE    m_btResult;
	WORD	m_wRight;
	WORD	m_wPayType;
    /*  m_cResult == RESULT_LOGIN_REPLY_OK 일경우
        struct {
            // szServerNAME[]
			DWORD dwServerID;
        } ;
    */
} ;

#define RESULT_LOGIN_REPLY_OK                   0x00
#define	RESULT_LOGIN_REPLY_KOREA_OK				RESULT_LOGIN_REPLY_OK

#define RESULT_LOGIN_REPLY_FAILED               0x01    // 오류
#define RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    0x02    // 계정 없다.
#define RESULT_LOGIN_REPLY_INVALID_PASSWORD     0x03    // 비번 오류
#define RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     0x04    // 이미 로그인 중이다
#define RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      0x05    // 서버에서 거부된 계정이다.
#define RESULT_LOGIN_REPLY_NEED_CHARGE          0x06    // 충전이 필요한다
#define	RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT	0x07	// 접속 권한이 없다.
#define	RESULT_LOGIN_REPLY_TOO_MANY_USER		0x08	// 접속 인원 제한
#define	RESULT_LOGIN_REPLY_NO_REAL_NAME			0x09
#define	RESULT_LOGIN_REPLY_INVALID_VERSION		0x0a	// 패치를 받아야 한다.
#define	RESULT_LOGIN_REPLY_OUT_OF_IP			0x0b	// 피씨방 접속 가능 ip 초과...

#define	RESULT_LOGIN_REPLY_TAIWAN_OK			0x10	// m_wPayType 값은 타이완 과금서버 체계
#define	RESULT_LOGIN_REPLY_TAIWAN_FAILED		0x11	// 타이완 과금서버 체계로 적용

#define	RESULT_LOGIN_REPLY_JAPAN_OK				0x18	// m_wPayType 값은 일본 과금서버 체계


/*
일본 과금 서버 RESULT_LOGIN_REPLY_JAPAN_OK일경우 m_wPayType의 값과
PAY_FLAG_JP_???? 와 & 연산해서 TRUE이면 결제것
예:: )
	if ( m_wPayType & PAY_FLAG_JP_EXTRA_CHAR ) {
		// 한국을 플레티넘 과금처럼 5개의 케릭터 운용가능.
	}
*/
/*
대만 과금 서버 RESULT_LOGIN_REPLY_TWAIN_OK일경우 m_wPayType의 값
0  :	// 금지
1	:	// 시간제 - 매 1 시간 단위로 6 포인트 삭감, 1시간을 채우지 못했을 경우, 1시간으로 계산한다.
2	:	// 월 정액제 - 매 30 일 단위로 350 포인트 삭감.
3	:	// 시간 정액제 - 매 12 시간 단위로 20 포인트 삭감.
4	:	// 3개월 정액제 - 매 90 일 단위로 888 포인트 삭감.
5	:	// 홍콩 월정액제
6	:	// 홍콩 3개월 정액제
7	:	// 홍콩 시간제
8	:	// 홍콩 시간 정액제
9	:	// 분 정량제 - 매 1분 단위로 0.25포인트 삭감.
10	:	// 시간 정량제  - 매 2시간 단위로 12 포인트 삭감.
11	:	// 10일 - 매 10일 단위로 150 포인트 삭감.
12	:	// 무료 체험
101:	// 블록 3일
102:	// 블록 7일
103:	// 블록 10일

대만 과금 서버 RESULT_LOGIN_REPLY_TWAIN_FAILED일경우 m_wPayType의 값
2	:	// 중복 로그인 (login result)	이미 로그인 된 계정
3	:	// 패스워드 에러 (login result)	계정 또는 패스워드 에러 또는 포인트 충전하지 않은 상태,  Joypark 회원 중심 참고.
4	:	// ObjnetObiect 구축 실패, Login Server등록 불가능 (Connection State Result)
5	:	// Login Server연결 중단 (Connection State Result)
6	:	// Login Server연결 실패 (Connection State Result)
7	:	// 없는 계정 (login result)	계정 또는 패스워드 에러 또는 포인트를 충전하지 않은 상태,  Joypark 회원 중심 참고. 
8	:	// 포인트 부족 (login result)	포인트 부족
9	:	// 시간 부족 (login result)	무료 체험기간 만기
10	:	// 없는 과금제도 (login result)	과금방식 문제
11	:	// 블록 (login result)	블록 중
12	:	// 블록３일 (login result)	블록 중
13	:	// 블록７일 (login result)	블록 중
14	:	// 블록 10 일 (login result)	블록 중
15	:	// 로그 아웃 상태 (logout result)
16	:	// 불확실한 에러 (데이터 베이스 에러 login, relogin, logout result)	불확실한 에러
17	:	// Ip중복 (홈 유저 대상 login, relogin, result) 
18	:	// 게임방 유효한 게임방 ip리스트 (사용 : 게임방 login, relogin, result)	
20	:	// 없는 계정 (relogin result)	계정 또는 패스워드 에러 도는 포인트를 충전하지 않은 상태, Joypark 회원 중심 참고.
21	:	// 중복 로그인 (relogin result)	이미 온라인 상황인 계정 
23	:	// 포인트 부족 (relogin result)	포인트 부족
24	:	// 시간 부족 (relogin result)	무료 체험기간 만기
25	:	// 부정확한 과금방식 (relogin result)	과금방식 문제 
26	:	// 블록 (relogin result)	블록 중
27	:	// 블록 ３일 (relogin result)	블록 중
28	:	// 블록 ７일 (relogin result)	블록 중
29	:	// 블록 10 일 (relogin result)	블록 중
30	:	// 같은 ip  n초내 중복 로그인 (login result)	마우스를 잠깐 쉬게 하고, 몇 초 후 다시 접속.
31	:	// Game Server등록 성공 (Connection State result)
32	:	// Game Server등록 ID중복(Connection State result)
33	:	// Game Server미 등록(login, relogin result)	서버 접속 불가능, 잠시후 다시 연결 시도.
34	:	// 유저 매일 접속 가능한 시간 초과.(login, relogin result)	접속시간 초과. 
35	:	// 무료 체험제 미 오픈 (login result)	이 서버는 무료 체험 계정 제공하지 않음.
101:	// Library버전과 Login Server 와 맞지 않음. (Connection State result)
*/


struct cli_CHANNEL_LIST_REQ : public t_PACKETHEADER {
	DWORD	m_dwServerID;
} ;

struct tagCHANNEL_SRV {
	BYTE	m_btChannelNO;		// 보여줄 채널 슬롯 번호
	BYTE	m_btLowAGE;			// 접속 가능한 최저 연령 0 이면 최저 연령 제한 없음
	BYTE	m_btHighAGE;		// 접속 가능한 최고 연령 0 이면 최고 연령 제한 없음
	short	m_nUserPERCENT;		// 현재 접속되어 있는 사용자양...
	// char m_szChannenNAME[]	// m_szChannelsNAME == "" 이면 CHANNEL-채널번호 로 부혀함
} ;
struct lsv_CHANNEL_LIST_REPLY : public t_PACKETHEADER {
	DWORD			m_dwServerID;
	BYTE			m_btChannelCNT;
	/*
	{
		tagCHANNEL_SRV X m_btChannelCNT;
	}
	*/
} ;

struct cli_SELECT_SERVER : public t_PACKETHEADER {
	DWORD	m_dwServerID;
	BYTE	m_btChannelNO;
} ;
struct lsv_SELECT_SERVER : public t_PACKETHEADER {
	BYTE	m_btResult;
	DWORD	m_dwIDs[2];
	// szServerIP[];
	// WORD wServerPortNO;
} ;
#define	RESULT_SELECT_SERVER_OK					0x00
#define RESULT_SELECT_SERVER_FAILED				0x01
#define	RESULT_SELECT_SERVER_FULL				0x02	// 인원 꽉참
#define	RESULT_SELECT_SERVER_INVALID_CHANNEL	0x03	// 잘못된 채널 선택
#define	RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE	0x04	// 채널 서버 점검중
#define	RESUTL_SELECT_SERVER_INVALID_AGE		0x05	// 연령이 맞지 않는다..
#define	RESULT_SELECT_SERVER_NEED_CHARGE		0x06	// 과금이 안되어 있다


struct tagVAR_GLOBAL {
	short	m_nWorld_PRODUCT;					// 제조 관련
	DWORD	m_dwUpdateTIME;						// 갱신된 시간.
	short	m_nWorld_RATE;						// 경제 관련 :: 세계물가 80~140
	BYTE	m_btTOWN_RATE;						// 마을 물가					80~140
	BYTE	m_btItemRATE[ MAX_PRICE_TYPE ];		// 아이템별 물가				1~127
	DWORD	m_dwGlobalFLAGS;
} ;
struct gsv_SET_GLOBAL_VAR : public t_PACKETHEADER, public tagVAR_GLOBAL {
} ;

struct gsv_SET_GLOBAL_FLAG : public t_PACKETHEADER {
	DWORD	m_dwGlobalFLAGS;
} ;
#define	ZONE_FLAG_PK_ALLOWED		0x00000001

/*
struct cli_JOIN_WORLD : public t_PACKETHEADER {
	DWORD	m_dwID;
    BYTE	m_MD5Password[ 32 ];
} ;

// m_cResult 값은 lsv_LOGIN_REPLY와 같다.
struct wsv_JOIN_WORLD : public t_PACKETHEADER {
	BYTE	m_btResult;
	DWORD	m_dwID;
} ;
#define	RESULT_JOIN_WORLD_OK					0x00
#define	RESULT_JOIN_WORLD_FAILED				0x01
#define RESULT_JOIN_WORLD_TIME_OUT				0x02    // 시간 초과로 계정 없어져 다시 로그인 해야한다.
#define RESULT_JOIN_WORLD_INVALID_PASSWORD		0x03    // 비번 오류
#define RESULT_JOIN_WORLD_ALREADY_LOGGEDIN		0x04    // 이미 로그인 중이다
*/

// 월드/존 서버에 접속 요청
struct cli_JOIN_SERVER_REQ	: public t_PACKETHEADER {
	DWORD	m_dwID;
    BYTE	m_MD5Password[ 32 ];
} ;
struct srv_JOIN_SERVER_REPLY : public t_PACKETHEADER {
	BYTE	m_btResult;
	DWORD	m_dwID;
	DWORD	m_dwPayFLAG;
} ;
#define	RESULT_JOIN_SERVER_OK					0x00
#define	RESULT_JOIN_SERVER_FAILED				0x01
#define RESULT_JOIN_SERVER_TIME_OUT				0x02    // 시간 초과로 계정 없어져 다시 로그인 해야한다.
#define RESULT_JOIN_SERVER_INVALID_PASSWORD		0x03    // 비번 오류
#define RESULT_JOIN_SERVER_ALREADY_LOGGEDIN		0x04    // 이미 로그인 중이다


// 클라이언트에게 존서버 이동 접속하라고 통보...
struct wsv_MOVE_SERVER : public t_PACKETHEADER {
	WORD	m_wPortNO;
	DWORD	m_dwIDs[2];
	// szServerIP[]
} ;


struct cli_CHAR_LIST : public t_PACKETHEADER {
} ;


// 성별, 레벨,직업, 파트 인덱스...
struct tagCHARINFO {
	BYTE	m_btCharRACE;
	short	m_nLEVEL;
	short	m_nJOB;
	DWORD	m_dwRemainSEC;		// 0이 아닐경우 삭제될때까지 남은 시간(초단위)
#ifdef	__INC_PLATINUM
	BYTE	m_btIsPlatinumCHAR;	// 0이 아닌경우는 플레티넘일 경우만 선택 가능한 케릭이다.
#endif
} ;
struct wsv_CHAR_LIST : public t_PACKETHEADER {
	BYTE	m_btCharCNT;
	/*
	{
		tagCHARINFO m_CharINFO;
		tagPartITEM m_PartITEM    [ MAX_BODY_PART ];
		char		szCharName[]
	} x m_btCharCNT
	*/
} ;


struct cli_CREATE_CHAR : public t_PACKETHEADER {
	BYTE	m_btCharRACE;
	char	m_cBoneSTONE;
	char	m_cHairIDX;
	char	m_cFaceIDX;
	char	m_cWeaponTYPE;
	short	m_nZoneNO;
	// char szCharName[]
} ;
struct wsv_CREATE_CHAR : public t_PACKETHEADER {
	BYTE	m_btResult;
	BYTE	m_btIsPlatinumCHAR;
} ;
#define	RESULT_CREATE_CHAR_OK				0x00
#define	RESULT_CREATE_CHAR_FAILED			0x01
#define	RESULT_CREATE_CHAR_DUP_NAME			0x02
#define	RESULT_CREATE_CHAR_INVALID_VALUE	0x03
#define	RESULT_CREATE_CHAR_NO_MORE_SLOT		0x04	// 더이상 만들수 없다.
#define	RESULT_CREATE_CHAR_BLOCKED			0x05	// 서버에서 케릭터 생성이 금지되어 있는 상태다.

struct cli_DELETE_CHAR : public t_PACKETHEADER {
	BYTE	m_btCharNO;
	bool	m_bDelete;								// true면 삭제, false면 삭제대기 상태 케릭터 부활
	// char szCharName[]
} ;
struct wsv_DELETE_CHAR : public t_PACKETHEADER {
	DWORD	m_dwDelRemainTIME;						// 0이면 삭제 취소...
	// char szCharName[]
} ;




struct cli_LOGOUT_REQ : public t_PACKETHEADER {
} ;
struct gsv_LOGOUT_REPLY : public t_PACKETHEADER {
	WORD	m_wWaitSec;
} ;



struct gsv_INIT_DATA : public t_PACKETHEADER {
	int		m_iRandomSEED;
	WORD	m_wRandomINDEX;
} ;


struct cli_SELECT_CHAR : public t_PACKETHEADER {
	BYTE	m_btCharNO;
	BYTE	m_btRunMODE;
	BYTE	m_btRideMODE;
	// char szCharName[]
} ;

struct gsv_SELECT_CHAR : public t_PACKETHEADER {
	BYTE				m_btCharRACE;
	short				m_nZoneNO;
	tPOINTF				m_PosSTART;
	short				m_nReviveZoneNO;

	tagPartITEM			m_PartITEM[ MAX_BODY_PART   ];
//	tagPartITEM			m_RideITEM[ MAX_RIDING_PART ];

	tagBasicINFO		m_BasicINFO;	
	tagBasicAbility		m_BasicAbility;	
	tagGrowAbility		m_GrowAbility;	
	tagSkillAbility		m_Skill;
	CHotICONS			m_HotICONS;
	DWORD				m_dwUniqueTAG;
//	char				szCharName[];
} ;


struct gsv_INVENTORY_DATA : public t_PACKETHEADER {
	// 2004. 9.11 대체... CInventory	m_INV;
	struct {
		__int64		m_i64Money;
		tagBaseITEM	m_ItemLIST[ INVENTORY_TOTAL_SIZE ];
	} m_INV;
} ;

// 퀘스트 스위치 갯수 증가후 아래 2개의 패킷으로 분리후 사용안함.
struct gsv_QUEST_DATA : public t_PACKETHEADER {
	tagQuestData	m_Quests;
	tagWishLIST		m_WishLIST;
} ;

struct gsv_QUEST_ONLY : public t_PACKETHEADER {
	tagQuestData	m_Quests;
} ;
struct gsv_WISH_LIST : public t_PACKETHEADER {
	tagWishLIST		m_WishLIST;
} ;


struct cli_JOIN_ZONE : public t_PACKETHEADER {
	BYTE			m_btWeightRate;
	short			m_nPosZ;
} ;



struct gsv_JOIN_ZONE : public t_PACKETHEADER {
	WORD			m_wServerObjectIndex;
	short			m_nCurHP;
	short			m_nCurMP;
	__int64			m_lCurEXP;
	__int64			m_lPenalEXP;
	tagVAR_GLOBAL	m_VAR;
	DWORD			m_dwAccWorldTIME;		// 게임 서버 처음 동작후 누적된 월드 타임
#ifdef	__APPLY_04_10_15_TEAMNO
	int				m_iTeamNO;				// 서버에서 설정된 팀번호	:: 04.10.15 추가..
#endif

} ;


struct cli_SCREEN_SHOT_TIME:public t_PACKETHEADER
{
	WORD wCnt;
};

struct gsv_SCREEN_SHOT_TIME:public t_PACKETHEADER
{
	WORD wYear;
	BYTE	btMonth;
	BYTE	btDay;
	BYTE	btHour;
	BYTE  btMin;
};



struct cli_REVIVE_REQ : public t_PACKETHEADER {
	BYTE	m_btReviveTYPE;
} ;
#define	REVIVE_TYPE_REVIVE_POS		0x01
#define REVIVE_TYPE_SAVE_POS		0x02
#define	REVIVE_TYPE_START_POS		0x03
#define	REVIVE_TYPE_CURRENT_POS		0x04

struct gsv_REVIVE_REPLY : public t_PACKETHEADER {
	short	m_nZoneNO;
} ;

struct cli_SET_REVIVE_POS : public t_PACKETHEADER {
} ;

struct cli_SET_VAR_REQ : public t_PACKETHEADER {
	BYTE	m_btVarTYPE;
	int		m_iValue;
} ;
struct gsv_SET_VAR_REPLY : public cli_SET_VAR_REQ {
} ;
#define	REPLY_GSV_SET_VAR_FAIL_BIT	0x080 
// #define	REPLY_GSV_SET_JOB_xxx


#define	BIT_MOTION_STOP_CMD			0x080000
struct cli_SET_MOTION : public t_PACKETHEADER {
	short		m_nMotionNO;
	union {
		WORD	m_wValue;
		struct {
			WORD	m_wETC : 15;
			WORD	m_wIsSTOP :  1;
		} ;
	} ;
} ;
struct gsv_SET_MOTION : public cli_SET_MOTION {
	WORD		m_wObjectIDX;
} ;

struct cli_TOGGLE : public t_PACKETHEADER {
	BYTE	m_btTYPE;
} ;
#define	TOGGLE_TYPE_RUN		0x00
#define	TOGGLE_TYPE_SIT		0x01
#define	TOGGLE_TYPE_DRIVE	0x02
struct gsv_TOGGLE : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	BYTE	m_btTYPE;
	short	m_nRunSPEED[0];			// 팻 타고 내릴때만...
} ;

struct cli_CHAT : public t_PACKETHEADER {
	// szChatMSG[]
} ;
struct gsv_CHAT : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
} ;

struct cli_WHISPER : public t_PACKETHEADER {
	// szTargetAccount[]
	// szChatMSG[]
} ;
struct gsv_WHISPER : public t_PACKETHEADER {
	// szFromAccount[]
	// szChatMSG[]
} ;

struct cli_SHOUT : public cli_CHAT {
} ;
struct gsv_SHOUT : public t_PACKETHEADER {
} ;

struct cli_ALLIED_CHAT : public t_PACKETHEADER {
} ;
struct gsv_ALLIED_CHAT : public gsv_CHAT {
	int	m_iTeamNO;
	// szChatMSG[]
} ;

struct cli_ALLIED_SHOUT : public t_PACKETHEADER {
} ;
struct gsv_ALLIED_SHOUT : public t_PACKETHEADER {
	int	m_iTeamNO;
	// szFromAccount[]
	// szChatMSG[]
} ;


struct cli_PARTY_CHAT : public cli_CHAT {
} ;
struct gsv_PARTY_CHAT : public gsv_CHAT {
} ;

struct cli_CLAN_CHAT : public cli_CHAT {
} ;
struct wsv_CLAN_CHAT : public gsv_SHOUT {
} ;

struct gsv_SET_EVENT_STATUS : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nEventSTATUS;
} ;

#define	MOVE_MODE_WALK		0x00	// 걷기 모드
#define	MOVE_MODE_RUN		0x01	// 뛰기 모드
#define	MOVE_MODE_DRIVE		0x02	// 운전 모드 : 자신이 타고 있다.
#define	MOVE_MODE_RIDEON	0x04	// 승차 모드 : 남의것에 타고 있다.

struct tag_ADD_CHAR : public t_PACKETHEADER {
    WORD		m_wObjectIDX;
	tPOINTF		m_PosCUR;
	tPOINTF		m_PosTO;
	WORD		m_wCommand;
	WORD		m_wTargetOBJ;
	BYTE		m_btMoveMODE;					// 0:걷기, 1:뛰기, 2:내 승용아이템에 타고있다, 3: m_wTargetOBJ의 승용아이템에 타고있다.
	int			m_iHP;
#ifdef	__APPLY_04_10_15_TEAMNO
	int			m_iTeamNO;						//	char m_cTeamNO => int로 변경 04.10.15 수정...
#else
	char		m_cTeamNO;
#endif
	DWORD		m_dwStatusFALG;		
	/*
	지속 스킬에 의해 바뀐 값...	2004.1.4
	if ( m_dwStatusFALG & ( ING_FLAG_MAX_HP | ING_FLAG_xxx_MOV_SPEED | ING_FLAG_xxx_ATK_SPEED ) ) {
		각 구조체 뒤에 m_nAdjSTATUS[ xx ]붙어 전송됨.

		BYTE btIDX = 0;
  		if ( tag_ADD_CHAR.m_dwStatusFALG &  FLAG_ING_MAX_HP ) )
			inc_max_hp = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_INC_MOV_SPEED ) )
			inc_run_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_MOV_SPEED ) )
			dec_run_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_INC_ATK_SPEED ) )
			inc_atk_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_ATK_SPEED ) )
			dec_atk_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_LIFE_TIME ) )		// 2004. 4. 16 추가..
			owner_obj_idx = (WORD) pAdjSTATUS[ btIDX++ ];
	}
	*/
} ;

struct gsv_MOB_CHAR : public tag_ADD_CHAR {
	short			m_nCharIdx;					
	short			m_nQuestIDX;
	// short		m_nAdjSTATUS[];
} ;

// gsv_NPC_CHAR패킷에서 m_nCharIdx < 0 이면 숨김상태(클라이언트에서 랜더링하지 않음)
struct gsv_NPC_CHAR : public gsv_MOB_CHAR {
	float			m_fModelDIR;
	short			m_nEventSTATUS;				// 2004. 3. 22 추가.., NPC인공지능에 의해 내부적으로 바뀌는 변수값..
	// short		m_nAdjSTATUS[];
} ;


/// 현재 설정된 소모탄 
struct tagShotDATA {
	union {
		struct {
			WORD	m_cType			: 5;
			WORD	m_nItemNo		: 10;
		} ;
		WORD	m_wShotITEM;
	} ;
} ;

/*
	계산 방법 
	1. 공격속도 = 무기로 계산된값 + 패시브 보정값 + 지속 보정값
	2. 이동속도 = 서버에서 받은값(패시브포함) + 지속 보정값으로 
*/
struct tag_CLAN_ID {
	DWORD	m_dwClanID;
	union {
		DWORD	m_dwClanMARK;
		WORD	m_wClanMARK[2];
	} ;
	BYTE	m_btClanLEVEL;		// 클랜 레벨
	BYTE	m_btClanPOS;		// 클랜에서의 직위
} ;
struct gsv_AVT_CHAR : public tag_ADD_CHAR {
	BYTE		m_btCharRACE;
	short		m_nRunSpeed;						// 패시브에 의해 보정된 값까지, 지속에 의해 보정된값 제외 ..
	short		m_nPsvAtkSpeed;						// 패시브 값만...  기본속도, 지속에 의해 보정된값 제외 ..
	BYTE		m_btWeightRate;						// 현재소지량/최대소지량*100
	tagPartITEM m_PartITEM	  [ MAX_BODY_PART ];
	tagShotDATA	m_sShotItem	  [ MAX_SHOT_TYPE ];

	short		m_nJOB;
	BYTE		m_btLEVEL;

//	short		m_nRidingITEM [ MAX_RIDING_PART ];	// 승용 아이템 ..
	tagPartITEM	m_RidingITEM  [ MAX_RIDING_PART ];	// 승용 아이템 ..
	short		m_nPosZ;

	DWORD		m_dwSubFLAG;

	// char		szUserID[]
	// short	m_nAdjSTATUS[];
	// ( tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_STORE_MODE ) 일경우 char szStoreTITLE[] 붙음

	/*	클랜 정보 추가...
	tag_CLAN_ID	m_CLAN;	== NULL이 아니면 클랜정보 있음
	// char m_szClanNAME[];
	*/
} ;

struct gsv_SUB_OBJECT : public t_PACKETHEADER {
    WORD    m_wObjectIDX[ 1 ];
} ;

struct cli_SET_WEIGHT_RATE : public t_PACKETHEADER {
	BYTE	m_btWeightRate;
} ;

struct gsv_SET_WEIGHT_RATE : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	BYTE	m_btWeightRate;
} ;


struct gsv_ADJUST_POS : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	tPOINTF	m_PosCUR;
	short	m_nPosZ;
} ;

struct cli_CANTMOVE : public t_PACKETHEADER {
    tPOINTF m_PosCUR;
	short	m_nPosZ;
} ;
struct cli_SETPOS : public t_PACKETHEADER {
    tPOINTF m_PosCUR;
} ;


struct cli_STOP : public t_PACKETHEADER {
    tPOINTF m_PosCUR;
} ;

struct gsv_STOP : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
    tPOINTF m_PosCUR;
	short	m_nPosZ;
} ;


struct cli_MOUSECMD : public t_PACKETHEADER {
    WORD	m_wTargetObjectIDX;
    tPOINTF m_PosTO;
	short	m_nPosZ;
} ;

struct gsv_MOUSECMD : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;

//	tPOINTF	m_PosCUR;
	WORD	m_wSrvDIST;
    tPOINTF m_PosTO;

	short	m_nPosZ;
} ;


struct cli_ATTACK : public t_PACKETHEADER {
	WORD	m_wTargetObjectIndex;
} ;

struct gsv_ATTACK : public t_PACKETHEADER {
	WORD	m_wAtkObjIDX;
	WORD	m_wDefObjIDX;

//	tPOINTF	m_PosCUR;
	WORD	m_wSrvDIST;
    tPOINTF m_PosTO;
} ;

struct gsv_MOVE : public gsv_MOUSECMD {
	BYTE	m_btMoveMODE;		// tag_ADD_CHAR::m_btMoveMODE와 같은값
} ;


// tag_DROPITEM은 몹이 죽으면서 떨구는거...아이템 생성 시간은 고정이름 클라이어트가 ITEM_OBJ_LIVE_TIME 으로 설정.
struct tag_DROPITEM {
	tPOINTF		m_PosCUR;
	tagBaseITEM	m_ITEM;
	WORD		m_wServerItemIDX;
	WORD		m_wOwnerObjIDX;
	// WORD m_wRemainTIME;		// 삭제 :: ITEM_OBJ_LIVE_TIME로 설정.
} ;
struct gsv_ADD_FIELDITEM : public t_PACKETHEADER, public tag_DROPITEM {
	WORD	m_wRemainTIME;
} ;

struct gsv_SUB_FIELDITEM : public t_PACKETHEADER {
	WORD	m_wServerItemIDX;
} ;

struct cli_GET_FIELDITEM_REQ : public t_PACKETHEADER {
	WORD	m_wServerItemIDX;
} ;

struct gsv_GET_FIELDITEM_REPLY : public t_PACKETHEADER {
	WORD		m_wServerItemIDX;
	BYTE		m_btResult;
	short		m_nInventoryListNO;
	tagBaseITEM	m_ITEM;
} ;
#define	REPLY_GET_FIELDITEM_REPLY_OK			0x00	// 획득했다.
#define REPLY_GET_FIELDITEM_REPLY_NONE			0x01	// 사라진 아이템이다..
#define REPLY_GET_FIELDITEM_REPLY_NO_RIGHT		0x02	// 가져갈 권한이 없다.
#define	REPLY_GET_FIELDITEM_REPLY_TOO_MANY		0x03	// 더이상 아이템을 갖을수 없다.


struct cli_DAMAGE : public t_PACKETHEADER {
	WORD	m_wDefObjIDX;
	WORD	m_wRandomIDX;
} ;

union uniDAMAGE {
	WORD	m_wDamage;
	struct {
		WORD	m_wVALUE  : 11;
		WORD	m_wACTION :  5;
	} ;
} ;
struct gsv_DAMAGE : public t_PACKETHEADER {
	WORD			m_wAtkObjIDX;
	WORD			m_wDefObjIDX;

	uniDAMAGE		m_Damage;
	tag_DROPITEM	m_DropITEM[ 0 ];	// 죽는 데미지일경우에만 값이 들어 있다. 드롭된 아이템 인덱스
} ;
#define	MAX_DAMAGE			0x07ff

// gsv_DAMAGE::m_wDamage와 연산되는 값.
#define	DMG_BIT_DUMMY		0x0800		// 추가 데미지 붙은 거다 :: 자신이 때린경우 추가 데미지 계산을 해.
#define	DMG_BIT_IMMEDIATE	0x1000		// 패키 받고 즉각 처리 하면 된다:예)방패 뎀쥐 스킬에 의해 반사된 뎀쥐
#define	DMG_BIT_HITTED		0x2000		// 맞는 동작을 한다.
#define	DMG_BIT_CRITICAL	0x4000		// 크리티컬 데미지다
#define	DMG_BIT_DEAD		0x8000		// 죽었다.

// gsv_DAMAGE::m_wACTION이 갖는 값...
#define	DMG_ACT_RESERVED	0x01
#define	DMG_ACT_ATTACK		0x02
#define	DMG_ACT_HITTED		0x04
#define	DMG_ACT_CRITICAL	0x08
#define	DMG_ACT_DEAD		0x10

struct gsv_CHANGE_NPC : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nNPCNo;
} ;

struct cli_SUMMON_CMD : public t_PACKETHEADER {
	BYTE	m_btCMD;
} ;
#define	SUMMON_CMD_ATTACK		0x00
#define	SUMMON_CMD_DEFENSE		0x01
#define	SUMMON_CMD_STANDING		0x02


// 자신의 현재 경험치를 갱신한다.
struct gsv_SETEXP : public t_PACKETHEADER {
	__int64	m_lCurEXP;
	short	m_nCurSTAMINA;
	WORD	m_wFromObjIDX;
} ;

struct gsv_LEVELUP : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nCurLevel;
	__int64	m_lCurEXP;
	short	m_nBonusPoint;
	short	m_nSkillPoint;
} ;


struct cli_HP_REQ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
} ;
struct gsv_HP_REPLY : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	int		m_iHP;			// MOB hp를 전송하기는것이기 때문에...
} ;


struct gsv_SET_HPnMP : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nHP;
	short	m_nMP;
} ;


struct tag_BUY_ITEM {
	char	m_cTabNO;
	char	m_cColNO;
	WORD	m_wDupCNT;		// 중복된 아이템일경우 구매할 갯수
} ;
struct tag_SELL_ITEM {
	BYTE	m_btInvIDX;
	WORD	m_wDupCNT;		// 중복된 아이템일경우 판매할 갯수
} ;
// 상점에서 거래...
struct cli_STORE_TRADE_REQ : public t_PACKETHEADER {
	WORD	m_wNPCObjIdx;
	char	m_cBuyCNT;
	char	m_cSellCNT;
	DWORD	m_dwEconomyTIME;
/*
	tag_BUY_ITEM	x m_cBuyCNT
	tag_SELL_ITEM	x m_cSellCNT
*/
} ;

struct gsv_STORE_TRADE_REPLY : public t_PACKETHEADER {
	BYTE	m_btRESULT;
} ;
#define	STORE_TRADE_RESULT_PRICE_DIFF		0x01
#define	STORE_TRADE_RESULT_NPC_NOT_FOUND	0x02
#define	STORE_TRADE_RESULT_TOO_FAR			0x03
#define	STORE_TRADE_RESULT_OUT_OF_MONEY		0x04
#define	STORE_TRADE_RESULT_NOT_UNION_USER	0x05	// 같은 조합원이 아니다
#define	STORE_TRADE_RESULT_OUT_OF_POINT		0x06	// 조합 포인트가 모자르다

// 변경된 인벤토리 정보...
struct tag_SET_INVITEM {
	BYTE		m_btInvIDX;
	tagBaseITEM	m_ITEM;
} ;

// 변경된 인벤토리 정보가 들어감...
struct gsv_SET_MONEYnINV : public t_PACKETHEADER {
	__int64			m_i64Money;
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];		// 변경된 갯수 만큼 들어 있다...
} ;

struct gsv_SET_INV_ONLY : public t_PACKETHEADER {
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];		// 변경된 갯수 만큼 들어 있다...
} ;

struct gsv_SET_MONEY_ONLY : public t_PACKETHEADER {
	__int64			m_i64Money;
} ;

struct gsv_SET_ABILITY : public t_PACKETHEADER {
	WORD	m_wAbilityTYPE;
	int		m_iValue;
} ;

#define	SERVER_DATA_ECONOMY		0
#define	SERVER_DATA_NPC			1
struct gsv_SERVER_DATA : public t_PACKETHEADER {
	BYTE	m_btDataTYPE;
	BYTE	m_pDATA[0];
} ;

#define	RELAY_TYPE_RECALL	0x00
struct gsv_RELAY_REQ : public t_PACKETHEADER {
	WORD	m_wRelayTYPE;
	union {
		BYTE	m_pBODY[0];
		struct {
			short	m_nCallZoneNO;
			tPOINTF m_PosCALL;
		} ;
	} ;
} ;
struct cli_RELAY_REPLY : public gsv_RELAY_REQ {
} ;

struct cli_ITEM_SKILL : public t_PACKETHEADER {
	short	m_nInventoryIndex;
} ;

struct cli_USE_ITEM : public t_PACKETHEADER {
	short	m_nInventoryIndex;
	// WORD		m_wDestObjIDX;		:: 소모 아이템으로 타켓 스킬 사용시
	// tPOINTF	m_PosTARGET;		:: 소모 아이템으로 위치 스킬 사용시
} ;
struct gsv_USE_ITEM : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nUseItemNO;
	BYTE	m_btInvIDX[ 0 ];
} ;

struct cli_DROP_ITEM : public t_PACKETHEADER {
	BYTE	m_btInventoryIndex;
	DWORD	m_uiQuantity;	
} ;


// m_nEquipInvIDX는 받드시 값이 들어 있어야함.
// m_nWeaponInvIDX는 장비 탈거의 경우 0 !!
struct cli_EQUIP_ITEM : public t_PACKETHEADER {
	short	m_nEquipInvIDX;		// 장착창 인벤토리 번호 :: m_nEquipInvIDX >= 1 && m_nEquipInvIDX < MAX_EQUIP_IDX
	short	m_nWeaponInvIDX;	// 장비 아이템이 위치하고 있는 인벤토리 번호...
} ;


struct gsv_EQUIP_ITEM : public t_PACKETHEADER {
	WORD		m_wObjectIDX;
	short		m_nEquipIndex;
//	short		m_nItemNO;
	tagPartITEM m_PartITEM;
	short		m_nRunSPEED[ 0 ];	// 패시브 스킬에의해 보정된 값까지 : 지속에 의해 보정된값 제외 :: 팻 미 탑승 안했을때..
} ;


struct cli_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER {
	short	m_nRidingPartIDX;	// t_eRidePART의값.
	short	m_nRidingInvIDX;	// cli_EQUIP_ITEM::m_nWeaponInvIDX 처럼...
} ;

struct gsv_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER {
	WORD		m_wObjectIDX;
	short		m_nRidingPartIDX;
//	short		m_nItemNO;
	tagPartITEM	m_RideITEM;
	short		m_nRunSPEED[ 0 ];	// 패시브 스킬에의해 보정된 값까지 : 지속에 의해 보정된값 제외 :: 팻 탑승 했을때...
} ;


struct cli_USE_ITEM_TO_REPAIR : public t_PACKETHEADER {
	short	m_nUseItemInvIDX;			// 수리에 사용할 아이템의 인벤 번호
	short	m_nRepairTargetInvIDX;		// 수리될 아이템의 인벤 번호
} ;

struct cli_REPAIR_FROM_NPC : public t_PACKETHEADER {
	WORD	m_wNPCObjIDX;				// 수리 요청할 NPC 서버 인덱스
	short	m_nRepairTargetInvIDX;		// 수리될 아이템의 인벤 번호
} ;


struct gsv_SET_ITEM_LIFE : public t_PACKETHEADER {
	short	m_nInventoryIDX;			// 수명이 변경될 아이템의 인벤토리 번호
	short	m_nLife;					// 서버에서의 현재 수명
} ;


//struct cli_CHANGE_SKIN : public t_PACKETHEADER {
//	BYTE	m_btBodyIDX;
//	short	m_nItemNO;
//} ;
struct gsv_CHANGE_SKIN : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	WORD	m_wAbilityTYPE;
	int		m_iValue;
} ;

struct cli_MOVE_ITEM : public t_PACKETHEADER {
	BYTE		m_btMoveTYPE;
	BYTE		m_btFromIDX;
	tagBaseITEM	m_MoveITEM;						// <<-- 옮길 갯수만 있어도.. WORD m_wQuantity;
#ifdef	__INC_PLATINUM
	BYTE		m_btUseSpecialTAB;				// 값이 1이면 .. 프레티넘 서비스로 추가된 탭에 아이템 넣기..
#endif
} ;

#define	MOVE_ITEM_TYPE_INV2BANK		0x00		// ITEM :: Inventory --> Bank
#define	MOVE_ITEM_TYPE_BANK2INV		0x01		// ITEM :: Bank --> Inventory

// 패킷 사이즈 == gsv_MOVE_ITEM 이면				창고=>인벤토리 이동
// 패킷 사이즈 == gsv_MOVE_ITEM+sizeof(__int64)면	인벤=>창고, m_iCurMoney[0]에 돈들어 있음
struct gsv_MOVE_ITEM : public t_PACKETHEADER {
	short		m_nInvIDX;						// IDX == -1 실패
	short		m_nBankIDX;						// IDX == -1 실패

	tagBaseITEM	m_InvItem;
	tagBaseITEM	m_BankITEM;
	__int64		m_iCurMoney[0];					// 보관료 빠진 결과돈...
} ;

#define	MOVE_ZULY_TYPE_INV2BANK		0x10		// ZULY :: Inventory --> Bank
#define MOVE_ZULY_TYPE_BANK2INV		0x11		// ZULY :: Bank --> Inventory
struct cli_MOVE_ZULY : public t_PACKETHEADER {
	BYTE		m_btMoveTYPE;
	__int64		m_i64MoveZuly;					// 이동 할 줄리량
} ;
struct gsv_MOVE_ZULY : public t_PACKETHEADER {
	__int64		m_i64InvZuly;						// 이동후 소지한 줄리량
	__int64		m_i64BankZuly;					// 이동후 보관된 줄리량
} ;


#define	BANK_REQ_OPEN				0x00		// 창고 열때..
#define	BANK_REQ_CHANGE_PASSWORD	0x01		// 창고 비번 바꿀때...반드시 창고가 열려 있는 상태에서...
struct cli_BANK_LIST_REQ : public t_PACKETHEADER {
	BYTE	m_btREQ;
	char	m_szPassword[0];
} ;

#define	BANK_REPLY_INIT_DATA		0x00		// 창고 데이타 초기화 & 보여준다
#define	BANK_REPLY_SHOW_DATA		0x01		// 창고 보여 준다 m_btItemCNT == 0
#define	BANK_REPLY_NEED_PASSWORD	0x02		// 창고 비번이 필요..
#define	BANK_REPLY_CHANGED_PASSWORD	0x03		// 창고 비번이 변경됐음
#define	BANK_REPLY_INVALID_PASSWORD	0x04		// 창고 비번이 틀림
#define	BANK_REPLY_CLEARED_PASSWORD	0x05		// 창고 비번 삭제됨
#define	BANK_REPLY_PLATINUM			0x06		// 플레티넘 창고 내용( BANK_REPLY_INIT_DATA뒤에 이어서 전송됨 )
struct gsv_BANK_LIST_REPLY : public t_PACKETHEADER {
	BYTE			m_btREPLY;
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];			// 변경된 갯수 만큼 들어 있다...
} ;

struct cli_TELEPORT_REQ : public t_PACKETHEADER {
	short	m_nWarpIDX;
	tPOINTF	m_PosCUR;							// 스핵 방지용 :: 클라이언트의 현재 좌표를 주면 서버 좌표와 비교
} ;
struct gsv_TELEPORT_REPLY : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nZoneNO;
	tPOINTF	m_PosWARP;
	BYTE	m_btRunMODE;
	BYTE	m_btRideMODE;
} ;


// m_btAbilityNO >= 100	: 스킬 포인트 인덱스는 +100해서 전송...
struct cli_USE_BPOINT_REQ : public t_PACKETHEADER {
	BYTE	m_btAbilityNO;	
} ;
struct gsv_USE_BPOINT_REPLY : public t_PACKETHEADER {
	BYTE	m_btAbilityNO;
	short	m_nAbilityValue;
} ;


struct cli_SET_HOTICON : public t_PACKETHEADER {
	BYTE		m_btListIDX;
	tagHotICON	m_HotICON;
} ;
struct gsv_SET_HOTICON : public cli_SET_HOTICON {
} ;


struct cli_SET_BULLET : public t_PACKETHEADER {
	struct {
		WORD	m_wShotTYPE	: 2;
		WORD	m_wInventoryIDX : 14;
	} ;
} ;
struct gsv_SET_BULLET : public t_PACKETHEADER {
	WORD		m_wObjectIDX;
	tagShotDATA	m_sShot;		// m_sShot.m_wItemNO == 0 일경우 총알 다 썼다 !!!
} ;


struct gsv_SKILL_LEARN_REPLY : public t_PACKETHEADER {
	BYTE	m_btResult;
	BYTE	m_btSkillSLOT;
	short	m_nSkillIDX;
	short	m_nSkillPOINT;
} ;
#define	RESULT_SKILL_LEARN_FAILED			0x00		// 배우기 실패.
#define	RESULT_SKILL_LEARN_SUCCESS			0x01		// 배우기 성공.
#define	RESULT_SKILL_LEARN_NEED_JOB			0x02		// 직업이 일치하지 않는다.
#define	RESULT_SKILL_LEARN_NEED_SKILL		0x03		// 보유할 스킬이 필요한다.
#define	RESULT_SKILL_LEARN_NEED_ABILITY		0x04		// 능력치가 부족하다
#define	RESULT_SKILL_LEARN_OUTOFSLOT		0x05		// 더이상 스킬을 배울수 없다.
#define	RESULT_SKILL_LEARN_INVALID_SKILL	0x06		// 잘못된 스킬 번호다.
#define	RESULT_SKILL_LEARN_OUTOFPOINT		0x07		// 스킬 포인트 부족
#define	RESULT_SKILL_LEARN_DELETE			0x08		// m_btSkillSLOT의 m_nSkillIDX번 스킬을 삭제해라..


// 스킬 레벨업시...
struct cli_SKILL_LEVELUP_REQ : public t_PACKETHEADER {
	BYTE	m_btSkillSLOT;
	short	m_nNextLevelSkillIDX;
} ;
struct gsv_SKILL_LEVELUP_REPLY : public t_PACKETHEADER {
	BYTE	m_btResult;
	BYTE	m_btSkillSLOT;
	short	m_nSkillINDEX;
	short	m_nSkillPOINT;
} ;
#define	RESULT_SKILL_LEVELUP_SUCCESS		0x00
#define	RESULT_SKILL_LEVELUP_FAILED			0x01
#define	RESULT_SKILL_LEVELUP_OUTOFPOINT		0x02
#define	RESULT_SKILL_LEVELUP_NEED_ABILITY	0x03
#define	RESULT_SKILL_LEVELUP_NEED_JOB		0x04
#define	RESULT_SKILL_LEVELUP_NEED_SKILL		0x05
#define	RESULT_SKILL_LEVELUP_OUTOFZULY		0x06		// 05.05.25 줄리가 모자르다


/// 스킬이 즉시 적용 : 적용후 정지됨
struct cli_SELF_SKILL : public t_PACKETHEADER {
	BYTE	m_btSkillSLOT;
} ;
struct gsv_SELF_SKILL : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	short	m_nSkillIDX;
	char	cNpcSkillMOTION[ 0 ];
} ;

/// 스킬을 타겟에 적용 : 적용후 정지됨
struct cli_TARGET_SKILL : public t_PACKETHEADER {
	WORD	m_wDestObjIDX;
	BYTE	m_btSkillSLOT;
} ;
struct gsv_TARGET_SKILL : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;
	short	m_nSkillIDX;

	WORD	m_wSrvDIST;
	tPOINTF m_PosTO;				// 좌표 보정용	2004. 5. 1 추가

	char	cNpcSkillMOTION[ 0 ];
} ;


/// 스킬을 지역에 적용 : 적용후 정지됨
struct cli_POSITION_SKILL : public t_PACKETHEADER {
	BYTE	m_btSkillSLOT;
	tPOINTF	m_PosTARGET;
} ;
struct gsv_POSITION_SKILL : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nSkillIDX;
	tPOINTF	m_PosTARGET;
	char	cNpcSkillMOTION[ 0 ];
} ;

struct gsv_EFFECT_OF_SKILL : public t_PACKETHEADER {
	WORD			m_wObjectIDX;
	WORD			m_wSpellObjIDX;

// 홍근
#ifdef _GBC
	unsigned short	m_nSkillIDX;
	unsigned short	m_nINT;
	BYTE			m_btSuccessBITS;
#else
	union {
		struct {
			unsigned short	m_nSkillIDX		: 12;
			unsigned short	m_btSuccessBITS : 2;	// 성공여부
			BYTE			m_tmp1;
		} ;
		struct {
			BYTE			m_tmp2;
			unsigned short	m_tmp3			: 6;
			unsigned short	m_nINT			: 10;	// 시전자의 지력
		} ;
	} ;
#endif

} ;




struct gsv_DAMAGE_OF_SKILL : public gsv_EFFECT_OF_SKILL {
	union {
		uniDAMAGE	m_Damage;				// 피해가 있을경우 값이 들어 있음.
		WORD		m_wDamage;
	} ;
	tag_DROPITEM	m_DropITEM[ 0 ];		// 죽는 데미지일경우에만 값이 들어 있다. 드롭된 아이템 인덱스
} ;
struct gsv_RESULT_OF_SKILL : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nSkillIDX;
} ;

struct gsv_SKILL_START : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
} ;

#define	SKILL_CANCEL_NEED_ABILITY		0x01	// 능력치 부족
#define	SKILL_CANCEL_NEED_TARGET		0x02	// 케스팅 시도전 타겟이 없어졌다... MP소모 없음
#define	SKILL_CANCEL_TARGET_NOT_FOUND	0x03	// 케스팅후 실제동작전 타겟이 없어졌다... MP소모 있음
struct gsv_SKILL_CANCEL : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	BYTE	m_btWHY;
} ;

/*
	**	서버에서 패킷에 값이 입력되는 방법... AND 시킬 플레그는 DataType.h에 선언된 값의 크기 순으로
		m_gsv_CLEAR_STATUS.m_nAdjVALUE[x]에 값이 입력되어 있음.

	dwClearedFLAG = 클라이언트상태 & ~서버상태

	BYTE btIDX=0;	
	if ( dwClearedFLAG &  FLAG_ING_HP )
		clientHP = m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ];

	if ( dwClearedFLAG &  FLAG_ING_MP )
		clientMP = m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ];
*/
struct gsv_CLEAR_STATUS : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	DWORD	m_dwStatusFLAG;		// 삭제된후 남은 플레그...
	short	m_nAdjVALUE[0];		// 삭제된후 보정된 값...
} ;

struct gsv_SPEED_CHANGED : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	short	m_nRunSPEED;			// 패시브 상태를 포함, 지속 상태 제외
	short	m_nPsvAtkSPEED;			// 패시브 값만...
	BYTE	m_btWeightRate;			// 현재소지량/최대소지량*100
} ;


// 장비 아이템의 감정의뢰 :: m_nInventoryIndex의 아이템 감정 요청 ..
struct cli_APPRAISAL_REQ : public t_PACKETHEADER {			
	WORD	m_wInventoryIndex;
} ;
struct gsv_APPRAISAL_REPLY : public t_PACKETHEADER {
	struct {
		WORD	m_wInventoryIndex : 12;
		WORD	m_btResult : 4;
	} ;
} ;
#define	RESULT_APPRAISAL_REPLY_OK		0	// 성공
#define	RESULT_APPRAISAL_REPLY_FAILED	1	// 돈없다.


/// 거래 대상 m_wObjectIDX에게 응답을 보낸다.
struct cli_TRADE_P2P : public t_PACKETHEADER {
	BYTE	m_btRESULT;
	WORD	m_wObjectIDX;
	char	m_cTradeSLOT;
} ;
#define	RESULT_TRADE_REQUEST			0x00	// 거래 요구
#define	RESULT_TRADE_ACCEPT				0x01	// 거래 승낙
#define	RESULT_TRADE_REJECT				0x02	// 거래 거부
#define	RESULT_TRADE_CANCEL				0x03	// 거래 도중 취소
#define	RESULT_TRADE_CHECK_READY		0x04	// 거래 준비 완료
#define	RESULT_TRADE_UNCHECK_READY		0x05	// 거래 준비 취소
#define	RESULT_TRADE_DONE				0x06	// 거래 실행
#define	RESULT_TRADE_BUSY				0x07	// 거래를 할수 없다.
#define	RESULT_TRADE_TOO_FAR			0x08	// 거리가 멀다.	5m 이내..
#define	RESULT_TRADE_NOT_TARGET			0x09	// 대상이 없다.
#define	RESULT_TRADE_OUT_OF_INV			0x0a	// 상대방이 인벤토리가 부족하다.
#define	RESULT_TRADE_NO_CHARGE_TARGET	0x0b

/// 서버에서 전송된 m_wObjectIDX의 거래에 응답.
struct gsv_TRADE_P2P : public t_PACKETHEADER {
	BYTE	m_btRESULT;
	WORD	m_wObjectIDX;
	char	m_cTradeSLOT;
} ;


#define	MAX_TRADE_ITEM_SLOT		11
#define	TRADE_MONEY_SLOT_NO		(MAX_TRADE_ITEM_SLOT-1)
struct cli_TRADE_P2P_ITEM : public t_PACKETHEADER {
	char	m_cTradeSLOT;
	short	m_nInventoryIndex;				// 인벤토리 번호
	DWORD	m_uiQuantity;					// 수량
} ;
struct gsv_TRADE_P2P_ITEM : public t_PACKETHEADER {
	char		m_cTradeSLOT;
	tagBaseITEM	m_ITEM;
} ;


struct cli_SET_WISHITEM : public t_PACKETHEADER {
	BYTE		m_btWishSLOT;
	tagBaseITEM	m_ITEM;
} ;

// 개인 상점 관련....
#define	MAX_P_STORE_ITEM_SLOT	31

// 판매할 아이템
struct tagPS_ITEM {
	union {
		BYTE	m_btInvIDX;						// 판매시 인벤번호
		BYTE	m_btWishIDX;					// 구매시 WishList 슬롯 번호
	} ;
	tagBaseITEM	m_ITEM;							// 아이템( 중복갯수 아이템일경우 팔려는 갯수 포함)
	DWORD		m_dwPricePerEA;					// 1개당 판매 희망 가격
} ;

#define	STORE_MODE_TO_SELL		0x01
#define	STORE_MODE_TO_BUY		0x02
struct cli_P_STORE_OPEN : public t_PACKETHEADER {
	BYTE	m_btSellItemCNT;	// 판매 희망 아이템 등록 갯수
	BYTE	m_btWishItemCNT;	// 구입 희망 아이템 등록 갯수

	// 등록할 아이템
	tagPS_ITEM	m_ITEMs[ 0 ];	// 판매 3, 구매 2 일경우 0~2는 판매, 3~4는 구입희망 아이템
	// char	m_szStoreTITLE[0]
} ;
struct gsv_P_STORE_OPENED : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_szStoreTITLE[0];
} ;


struct cli_P_STORE_CLOSE : public t_PACKETHEADER {
} ;
struct gsv_P_STORE_CLOSED : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
} ;


struct tagPS_SLOT_PRICE {
	BYTE		m_btSLOT;
	tagBaseITEM	m_SlotITEM;
	DWORD		m_dwPRICE;
} ;
struct cli_P_STORE_LIST_REQ : public t_PACKETHEADER {		// 개인 상점 아이템 목록 요구
	WORD				m_wStoreObjectIDX;
} ;
struct gsv_P_STORE_LIST_REPLY : public t_PACKETHEADER {
	BYTE				m_btSellItemCNT;
	BYTE				m_btWishItemCNT;
	tagPS_SLOT_PRICE	m_SlotITEMs[0];
} ;


struct tagPS_SLOT_ITEM {
	BYTE		m_btSLOT;
	tagBaseITEM	m_SlotITEM;
} ;
// 개인 상점에 구입 요청
struct cli_P_STORE_BUY_REQ : public t_PACKETHEADER {
	WORD				m_wStoreObjectIDX;
	BYTE				m_btItemCNT;
	tagPS_SLOT_ITEM		m_BuyITEMs[0];
} ;

struct tagSELL_ITEM{
		BYTE		m_btInvIDX;			// 팔려는 아이템이 위치한 인벤토리 번호
		BYTE		m_btWishSLOT;		// 상점 주인이 원하는 아이템이 위치한 번호
		tagBaseITEM	m_SellITEM;			// 팔려는 아이템(중복갯수 아이템의 경우 팔려는 갯수조정 가능)
};
// 개인 상점에 판매 요청
struct cli_P_STORE_SELL_REQ : public t_PACKETHEADER {
	WORD			m_wStoreObjectIDX;
	BYTE			m_btItemCNT;
	tagSELL_ITEM	m_SellITEMs[0];
} ;

struct gsv_P_STORE_RESULT : public t_PACKETHEADER {
	WORD			m_wStoreObjectIDX;
	BYTE			m_btResult;
	BYTE			m_btItemCNT;
	tagPS_SLOT_ITEM m_UpdatedITEM[0];
} ;
#define	RESULT_P_STORE_CANCLED				0x01	// 거래 취소,
#define	RESULT_P_STORE_TRADE_SOLD_OUT		0x02	// 물건이 품절
#define	RESULT_P_STORE_TRADE_NO_MORE_NEED	0x03	// 상점 주인이 원하는 아이템을 누군가 먼저 팔았다.
#define RESULT_P_STORE_BOUGHT_PART			0x04	// 구입 완료...단, 일부 품목은 품절되어 구입 실패...
#define RESULT_P_STORE_BOUGHT_ALL			0x05	// 요청한 품목 전체 구입 완료 
#define RESULT_P_STORE_SOLD_PART			0x06	// 판매 완료...단, 일부 품목은 구입완료되어 판매 실패...
#define RESULT_P_STORE_SOLD_ALL				0x07	// 요청한 품목 전체 판매 완료

/*
	party structure ...
*/
struct cli_PARTY_REQ : public t_PACKETHEADER {
	BYTE	m_btREQUEST;
	DWORD	m_dwDestIDXorTAG;
} ;
struct gsv_PARTY_REQ : public t_PACKETHEADER {
	BYTE	m_btREQUEST;
	DWORD	m_dwFromIDXorTAG;
//	short	m_nZoneNO;
} ;
#define	PARTY_REQ_MAKE				0x00	// 파티 결성 요청 
#define	PARTY_REQ_JOIN				0x01	// 파티 참가 요청
#define	PARTY_REQ_LEFT				0x02	// 파티 탈퇴 요청
#define	PARTY_REQ_CHANGE_OWNER		0x03	// 파짱 변경 요청.

#define	PARTY_REQ_BAN				0x81	// 파티 강퇴 요청

struct cli_PARTY_REPLY : public t_PACKETHEADER {
	BYTE	m_btREPLY;
	DWORD	m_dwDestIDXorTAG;
} ;
struct gsv_PARTY_REPLY : public t_PACKETHEADER {
	BYTE	m_btREPLY;
	DWORD	m_dwFromIDXorTAG;
} ;
#define	PARTY_REPLY_NOT_FOUND				0x00	// 대상을 찾지 못했다.
#define	PARTY_REPLY_BUSY					0x01	// 대상이 파티에 참가 할수 없는 상태다.
#define	PARTY_REPLY_ACCEPT_MAKE				0x02
#define	PARTY_REPLY_ACCEPT_JOIN				0x03
#define	PARTY_REPLY_REJECT_JOIN				0x04	// 파티 결성/참가에 거절 ..
#define	PARTY_REPLY_DESTROY					0x05	// 파티 뽀개 졌다
#define PARTY_REPLY_FULL_MEMBERS			0x06	// 정원 만땅이다.
#define	PARTY_REPLY_INVALID_LEVEL			0x07	// 레벨이 맞지 않는다
#define	PARTY_REPLY_CHANGE_OWNER			0x08
#define	PARTY_REPLY_CHANGE_OWNERnDISCONN	0x09	// PARTY_REPLY_CHANGE_OWNER와 같다...단 기존 파짱을 disconnect된 상태로 변경.
#define	PAATY_REPLY_NO_CHARGE_TARGET		0x0a

#define	PARTY_REPLY_BAN						0x80
#define PARTY_REPLY_DISCONNECT				0x81	// 비정상 종료 됐다...
#define	PARTY_REPLY_REJOIN					0x82	// 비정상 종료후 재접했다.

/*
struct tag_PARTY_USER {		// 월드 서버에서 전송됨
	WORD	m_wUserTAG;
	//char	m_szCharName[]
} ;
struct wsv_PARTY_USER : public t_PACKETHEADER {
	char	m_cUserCNT;					// -1이면 파티원에서 제외, 양수 m_cType은 추가될 사용자수...
	WORD	m_wUserTAG[0];				// 파티원 제외일 경우 참조...
//	tag_PARTY_USER[ m_cUserCNT ];		// 파티원 추가일 경우 참조
} ;
*/
struct tag_PARTY_MEMBER {	// 존 서버에서 전송됨
	DWORD	m_dwUserTAG;
	WORD	m_wObjectIDX;
	short	m_nMaxHP;
	short	m_nHP;
	DWORD	m_dwStatusFALG;

	short	m_nCON;						// BYTE	m_btCON; //최대 스텟 범위가 300으로 증가됨 2004. 7. 21

	BYTE	m_btRecoverHP;				// item_recover_hp + passive_recover_hp
	BYTE	m_btRecoverMP;				// item_recover_mp + passive_recover_mp

	short	m_nSTAMINA;

	//char	m_szCharName[];				// ** PARTY_MEMBER_ADD일 경우만 값이 들어 있음
} ;
struct gsv_PARTY_MEMBER : public t_PACKETHEADER {
	BYTE	m_btPartyRULE;				// 파티 규칙(아이템분배, 경험치분배방식)
	char	m_cUserCNT;					// -1이면 파티원에서 제외, 양수 m_cType은 추가될 사용자수, ObjTAG...
	DWORD	m_dwObjectTAG[0];			// 파티원 제외일 경우 참조...
	/*
	tag_PARTY_MEMBER[ m_cUserCNT ];		// 파티원 추가일 경우 참조
	*/
} ;
#define	PARTY_MEMBER_SUB	(-1)

struct gsv_PARTY_LEVnEXP : public t_PACKETHEADER {
	BYTE				m_btLEVEL;
	struct {
		unsigned int	m_iEXP		 : 31;
		unsigned int	m_bitLevelUP : 1;
	} ;
} ;

struct gsv_CHANGE_OBJIDX : public t_PACKETHEADER {
	tag_PARTY_MEMBER	m_Member;
} ;

struct gsv_PARTY_ITEM : public t_PACKETHEADER {
	WORD		m_wObjectIDX;
	tagBaseITEM m_ITEM;
} ;

#define	BIT_PARTY_RULE_EXP_PER_PLAYER		0x001	// 경험치를 레벨비례로 나눈다...
#define	BIT_PARTY_RULE_ITEM_TO_ORDER		0x080	// 순서대로 우선권 부여...
struct cli_PARTY_RULE : public t_PACKETHEADER {
	BYTE	m_btPartyRUEL;
} ;
struct gsv_PARTY_RULE : public t_PACKETHEADER {
	BYTE	m_btPartyRULE;
} ;


struct gsv_ADD_EVENTOBJ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_cMapX;
	char	m_cMapY;
	WORD	m_wMapEventID;
	short	m_nEventSTATUS;
} ;
/*
struct gsv_SUB_EVENTOBJ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
} ;
*/

struct gsv_SET_NPC_SHOW : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	bool	m_bShow;						// m_bShow == true이면 보임/false이면 숨김
} ;


//--------------------------- CLAN 관련 패킷
#define	GCMD_CREATE				0x000		//길드생성, /guildcreate [길드명]
#define	GCMD_INFO				0x001		//길드정보, /ginfo - 길드에 대한 기본적인 정보
#define	GCMD_INVITE				0x002		//길드초대, /ginvite <플레이어> - 길드에 해당 플레이어 초대하기
#define	GCMD_REMOVE				0x003		//길드추방, /gremove <플레이어> - 길드에서 해당 플레이어 추방하기
#define	GCMD_PROMOTE			0x004		//길드승급, /gpromote <플레이어> - 해당 플레이어 길드 등급 올리기
#define	GCMD_DEMOTE				0x005		//길드강등, /gdemote <플레이어> - 해당 플레이어 길드 등급 내리기
#define	GCMD_MOTD				0x006		//길드공지, /gmotd <할말> - 오늘의 길드 메시지 정하기
#define	GCMD_QUIT				0x007		//길드탈퇴, /gquit - 길드에서 탈퇴하기
#define	GCMD_ROSTER				0x008		//길드목록, /groster - 전체 길드원 목록 보기
#define	GCMD_LEADER				0x009		//길드위임, /gleader <플레이어> - 다른 플레이어에게 길드장 위임하기 (길드장 전용)
#define	GCMD_DISBAND			0x00a		//길드해체, /gdisband - 길드 해체하기 (길드장 전용) 

#define GCMD_INVITE_REQ			0x00b		//나에게 클랜 가입을 요청했다. <요청자이름>
#define	GCMD_INVITE_REPLY_YES	0x00c		//클랜가입 승낙 <요청자이름>
#define	GCMD_INVITE_REPLY_NO	0x00d		//클랜가입 거절 <요청자이름>

#define	GCMD_SLOGAN				0x00e		//길드슬로건 /clan slotan 슬로건

#define	GCMD_MEMBER_JOBnLEV		0x00f		//자신의 직업/레벨을 다른 멤버에게 통보( 로그인후/레벨업후/전직후에 전송 )

//-- 서버 전용 타입...
#define	GCMD_LOGIN				0x0f0
#define	GCMD_LOGOUT				0x0f1
#define	GCMD_CHAT				0x0f2
#define	GCMD_ADJ_VAR			0x0f3
#define	GCMD_CLANMARK_SET		0x0f4
#define	GCMD_CLANMARK_GET		0x0f5
#define GCMD_CLANMARK_REGTIME	0x0f6


struct cli_CLAN_COMMAND : public t_PACKETHEADER {
	BYTE	m_btCMD;
} ;
struct cli_CLAN_CREATE : public cli_CLAN_COMMAND {
	WORD	m_wMarkIDX[2];
	// char	m_szName[];
	// char m_szDesc[];
} ;
struct cli_CLAN_MEMBER_JOBnLEV : public cli_CLAN_COMMAND {
	short	m_nLEVEL;
	short	m_nJOB;
} ;
struct wsv_CLAN_MEMBER_JOBnLEV : public t_PACKETHEADER {
	BYTE	m_btRESULT;
	short	m_nLEVEL;
	short	m_nJOB;
	// char m_szCharNAME[]
} ;

//struct tag_CLAN_INFO {
//	short		m_nClanLEVEL;
//	int			m_iClanSCORE;
//	int			m_iClanRATE;	// 창고효율
//	__int64		m_biClanMONEY;
//	char szName[];				이름
//	char szDesc[];				슬로건 없으면 NULL
//	char szAlliedGroup[]		동맹 이름 없으면 NULL
//} ;

#define	MAX_CLAN_SKILL_SLOT		20
#ifdef	MAX_CLAN_SKILL_SLOT
	struct tagClanSKILL {
		short	m_nSkillIDX;
		DWORD	m_dwExpiredAbsSEC;	// 사용 만료 시간
	//	short	m_nCount;			// 사용 가능 횟수 -> 누가 사용가능한 횟수?? 마스터만??
	} ;
	struct tagClanBIN {
		union {
			BYTE			m_pDATA[ 0 ];
			tagClanSKILL	m_SKILL[ MAX_CLAN_SKILL_SLOT ];
		} ;
	} ;

#endif

struct tag_MY_CLAN : public tag_CLAN_ID {
	int				m_iClanSCORE;		// 클랜 포인트
	int				m_iClanRATE;		// 창고효율
	__int64			m_biClanMONEY;
	short			m_nMemberCNT;		// 멤버 수
	int				m_iClanCONT;		// 클랜 기여도
#ifdef	MAX_CLAN_SKILL_SLOT
	tagClanBIN		m_ClanBIN;			// 클랜 바이너리 데이타..
#endif
	// char m_szClanName[];
	// char m_szClanDESC[];
	// char m_szClanMOTD[];
} ;

struct tag_CLAN_MEMBER {
	BYTE	m_btClanPOS;
	BYTE	m_btChannelNO;		// 값이 0xff이면 접속하지 않는 유저다...
	int		m_iClanCONTRIBUTE;
	short	m_nLEVEL;
	short	m_nJOB;
	// char m_szCharNAME[]
} ;
struct wsv_CLAN_COMMAND : public t_PACKETHEADER {
	BYTE	m_btRESULT;
	char	m_szStr[0];

	/*
	// m_btRESULT == RESULT_CLAN_MY_DATA
		tag_MY_CLAN	m_CLAN;
		// char m_szClanNAME[];
	*/

	/*
	// m_btRESULT == RESULT_CLAN_INFO 일경우
	struct tag_CLAN_INFO
	*/
	/*
	// m_btRESULT == RESULT_CLAN_ROSTER 일경우
	tag_CLAN_MEMBER	m_Members[ x ];
	*/
	/*
	// m_btRESULT == RESULT_CLAN_KICK, RESULT_CLAN_POSITION 일경우
	strujct {
		char *m_szMember;	탈퇴자
		char *m_szMaster;	탈퇴시킨자 == NULL일경우 스스로 나간것
	*/
	/*
	// m_btRESULT == RESULT_CLAN_JOINED
	struct {
		char *m_szMember;	신규 가입자
		char *m_szMaster;	가입시킨자
	} ;
	*/
	/*
	// m_btRESULT == RESULT_CLAN_SET :: 주변 사용자의 클랜 정보 설정
	struct {
		WORD		m_wObjIDX;
		tag_CLAN_ID	;
	} ;
	*/
} ;
struct wsv_RESULT_CLAN_SET : public t_PACKETHEADER {
	BYTE		m_btRESULT;
	WORD		m_wObjIDX;
	tag_CLAN_ID	m_ClanID;
} ;

#define	RESULT_CLAN_CREATE_OK			0x030		// 내가 창설 했다.
#define RESULT_CLAN_JOIN_OK				0x031		// 내가 신규 가입됐다
#define	RESULT_CLAN_DESTROY_OK			0x032
#define RESULT_CLAN_MY_DATA				0x033		// 이미 가입된 클랜이 있는 상태에서 나의 클랜 정보
#define	RESULT_CLAN_MOTD				0x034		// 클랜 공지 바뀌었다.
#define	RESULT_CLAN_SET					0x035		// 주변 사용자의 클랜 정보 설정
#define	RESULT_CLAN_SLOGAN				0x036		// 클랜 슬로건 바뀌었다

#define	RESULT_CLAN_CREATE_FAILED		0x041
#define	RESULT_CLAN_CREATE_DUP_NAME		0x042
#define	RESULT_CLAN_CREATE_NO_RIGHT		0x043		// 창설 권한 없다.
#define	RESULT_CLAN_CREATE_NO_CONDITION	0x044		// 창설 조건 안맞는다.

#define	RESULT_CLAN_DESTROYED			0x051		// 클랜 뽀개 졌다..
#define	RESULT_CLAN_DESTROY_FAILED		0x052
#define	RESULT_CLAN_DESTROY_NO_RIGHT	0x053		// 뽀갤 권한 없다.

#define	RESULT_CLAN_JOIN_MEMBER			0x061		// 신규 가입했다 :: 전 클랜원 한테 전송..
#define	RESULT_CLAN_JOIN_FAILED			0x062
#define	RESULT_CLAN_JOIN_NO_RIGHT		0x063		// 가입 시킬 권한이 없다.
#define	RESULT_CLAN_JOIN_HAS_CLAN		0x064		// 다른 길드에 가입해 있다.
#define	RESULT_CLAN_JOIN_MEMBER_FULL	0x065		// 멤버 꽉찼다.

#define	RESULT_CLAN_INFO				0x071		// 클랜 정보
#define	RESULT_CLAN_ROSTER				0x072		// 클랜 리스트

#define	RESULT_CLAN_MEMBER_LOGIN		0x073		// 전체에게 보내지는 멤버 로그인
#define	RESULT_CLAN_MEMBER_LOGOUT		0x074		// 전체에게 보내지는 멤버 로그아웃
#define	RESULT_CLAN_MEMBER_POSITION		0x075		// 전체에게 보내지는 멤버 직위조절

#define	RESULT_CLAN_KICK				0x081		// 클랜에서 추방됨
#define	RESULT_CLAN_QUIT				0x082		// 소속 클랜에서 삭제됨( 클랜변수 초기화 )
#define	RESULT_CLAN_POSITION			0x083		// 클랜에서의 등급이 조정됐다.
#define	RESULT_CLAN_MEMBER_JOBnLEV		0x084		// 로그인된 멤버의 레벨/직업


// m_dwClanID의 클랜 마크를 서버에 요청
struct cli_CLANMARK_REQ	: public t_PACKETHEADER {
	DWORD	m_dwClanID;
} ;
// m_btMARK데이타를 자신의 클랜 마크로 설정
struct cli_CLANMARK_SET : public t_PACKETHEADER {
	WORD	m_wMarkCRC16;
//	BYTE	m_btMARK[ 0 ];
} ;
// m_dwClanID클랜의 클랜마크 데이타
#define	RESULT_CLANMARK_TOO_MANY_UPDATE		0x0001	// 마크 갱신은 일정시간 후에 해야 한다...
#define	RESULT_CLANMARK_DB_ERROR			0x0002	// 디비 갱신 오류
#define	RESULT_CLANMAKR_SP_ERROR			0x0003	// 디비 SP 오류
struct wsv_CLANMARK_REPLY : public t_PACKETHEADER {
	DWORD		m_dwClanID;
	union {
		WORD	m_wMarkCRC16;
		WORD	m_wFailedReason;	// m_dwClanID == 0 일경우 실패 원인 들어 있음.
	} ;
//	BYTE	m_btMARK[ 0 ];
} ;

struct cli_CLANMARK_REG_TIME : public t_PACKETHEADER {
} ;
struct wsv_CLANMARK_REG_TIME : public t_PACKETHEADER {
	WORD	m_wYear;
	BYTE	m_btMon;
	BYTE	m_btDay;
	BYTE	m_btHour;
	BYTE	m_btMin;
	BYTE	m_btSec;
} ;

struct gsv_CHEAT_CODE : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_szCheatCODE[0];
} ;


/*
	아이템 제조 관련...
*/
#define	CREATE_ITEM_STEP	4
struct cli_CREATE_ITEM_REQ : public t_PACKETHEADER {
	BYTE	m_btSkillSLOT;							// 사용할 제조 스킬
	char	m_cTargetItemTYPE;						// 제조할 아이템 타입( t_eITEM의 값 )
	short	m_nTargetItemNO;						// 제조할 아이템 번호
	short	m_nUseItemINV[ CREATE_ITEM_STEP ];		// 제조시 소모할 아이템 인벤토리 번호
//	short	m_nUseItemCNT[ CREATE_ITEM_STEP ];		// 제조시 소모할 아이템 갯수
} ;
struct gsv_CREATE_ITEM_REPLY : public t_PACKETHEADER {
	BYTE		m_btRESULT;								// 결과...
	short		m_nStepORInvIDX;						// 제조 실패시 실패된 스텝, 성공시 들어간 인벤토리번호
	short		m_nPRO_POINT[ CREATE_ITEM_STEP ];		// 진행 포인트
	tagBaseITEM	m_CreatedITEM;							// 제조 성공시 제조된 아이템
} ;
#define	RESULT_CREATE_ITEM_SUCCESS				0x00	// 성공
#define	RESULT_CREATE_ITEM_FAILED				0x01	// 실패
#define	RESULT_CREATE_ITEM_INVALID_CONDITION	0x02	// 마나가 부족하다.
#define	RESULT_CREATE_ITEM_NEED_ITEM			0x03	// 소모될 아이템이 부족하다
#define	RESULT_CREATE_ITEM_INVALID_ITEM			0x04	// 소모될 아이템이 부적합하다
#define	RESULT_CREATE_ITEM_NEED_SKILL_LEV		0x05	// 제조 스킬 레벨이 부족하다

//struct cli_CREATE_ITEM_EXP_REQ : public t_PACKETHEADER {
//} ;

#define	REPORT_ITEM_CREATE_START				0x00	// 아이템 제조 시작
#define	REPORT_ITEM_CREATE_SUCCESS				0x01	// 아이템 제조 성공
#define	REPORT_ITEM_CREATE_FAILED				0x02	// 아이템 제조 실패
#define	REPORT_ITEM_UPGRADE_START				0x03	// 아이템 제련 시작
#define	REPORT_ITEM_UPGRADE_SUCCESS				0x04	// 아이템 제련 성공
#define	REPORT_ITEM_UPGRADE_FAILED				0x05	// 아이템 제련 실패

struct cli_ITEM_RESULT_REPORT : public t_PACKETHEADER {
	BYTE	m_btREPORT;
	BYTE	m_btItemType;
	short	m_nItemNO;
} ;
struct gsv_ITEM_RESULT_REPORT : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	BYTE	m_btREPORT;
	BYTE	m_btItemType;
	short	m_nItemNO;
} ;

//---------------------------------------------------------
#define	CRAFT_GEMMING_REQ			0x01			// 재밍
#define CRAFT_BREAKUP_USE_SKILL		0x02			// 스킬로 분해
#define CRAFT_BREAKUP_FROM_NPC		0x03			// npc 통해 분해
#define	CRAFT_UPGRADE_USE_SKILL		0x04			// 스킬로 재련
#define	CRAFT_UPGRADE_FROM_NPC		0x05			// npc 통해 재련

struct cli_CRAFT_ITEM_REQ : public t_PACKETHEADER {
	BYTE	m_btTYPE;
} ;
struct cli_CRAFT_GEMMING_REQ : public cli_CRAFT_ITEM_REQ {
	BYTE	m_btEquipInvIDX;						// 장착된 무기 아이템 인벤 번호
	BYTE	m_btJemInvIDX;							// 보석 아이템 인벤 번호
} ;
struct cli_CRAFT_BREAKUP_REQ : public cli_CRAFT_ITEM_REQ {
	short	m_nSkillSLOTorNpcIDX;					// 사용 스킬슬롯 번호또는 npc번호
	BYTE	m_btTargetInvIDX;						// 분리할 아이템 인벤 번호
} ;
#define	UPGRADE_ITEM_STEP		3
struct cli_CRAFT_UPGRADE_REQ : public cli_CRAFT_ITEM_REQ {
	short	m_nSkillSLOTorNpcIDX;					// 사용 스킬슬롯 번호또는 npc번호
	BYTE	m_btTargetInvIDX;						// 재련할 아이템 인벤 번호
	BYTE	m_btUseItemINV[ UPGRADE_ITEM_STEP ];	// 재련시 소모할 아이템 인벤토리 번호
} ;


#define	CRAFT_GEMMING_SUCCESS		0x01
#define	CRAFT_GEMMING_NEED_SOCKET	0x02	// 소켓없다.
#define	CRAFT_GEMMING_USED_SOCKET	0x03	// 재밍된 경우등....

#define	CRAFT_BREAKUP_SUCCESS_GEM	0x04	// 보석 분리 성공
#define	CRAFT_BREAKUP_DEGRADE_GEM	0x05	// 보석 분리 성공, 보석등급 강등
#define	CRAFT_BREAKUP_CLEARED_GEM	0x06	// 보석 분리 성공, 보석 삭제됨

#define	CRAFE_BREAKUP_SUCCESS		0x07	// 아이템 분해 성공

#define	CRAFT_UPGRADE_SUCCESS		0x10	// 재련 성공
#define	CRAFT_UPGRADE_FAILED		0x11	// 재련 실패
#define	CRAFT_UPGRADE_INVALID_MAT	0x12	// 재료 아이템이 잘못됐다.

struct gsv_CRAFT_ITEM_REPLY : public t_PACKETHEADER {
	BYTE			m_btRESULT;
	BYTE			m_btOutCNT;			// 변경된 아이템 갯수
	tag_SET_INVITEM	m_sInvITEM[ 0 ];	// 변경된 갯수 만큼 들어 있다... 
										// 예외) CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED 일경우
										// m_sInvITEM[ m_btOutCNT-1 ].m_iQuantity에 성공도 계산된값이 들어있음
} ;


/*
///	퀘스트 관련 ...
*/
struct cli_QUEST_REQ : public t_PACKETHEADER {
	BYTE	m_btTYPE;
	BYTE	m_btQuestSLOT;
	union {
		int			m_iQuestID;
		t_HASHKEY	m_TriggerHash;
	} ;
} ;
#define	TYPE_QUEST_REQ_ADD						0x01
#define	TYPE_QUEST_REQ_DEL						0x02
#define	TYPE_QUEST_REQ_DO_TRIGGER				0x03


struct gsv_QUEST_REPLY : public t_PACKETHEADER {
	BYTE	m_btResult;
	BYTE	m_btQuestSLOT;
	union {
		int			m_iQuestID;
		t_HASHKEY	m_TriggerHash;
	} ;
} ;
#define	RESULT_QUEST_REPLY_ADD_SUCCESS			0x01
#define	RESULT_QUEST_REPLY_ADD_FAILED			0x02
#define	RESULT_QUEST_REPLY_DEL_SUCCESS			0x03
#define	RESULT_QUEST_REPLY_DEL_FAILED			0x04
#define	RESULT_QUEST_REPLY_TRIGGER_SUCCESS		0x05		// 이 패킷을 받으면 클라이언트에서도 보상을 적용한다.
#define	RESULT_QUEST_REPLY_TRIGGER_FAILED		0x06

// 서버가 클라이언트에게 NPC가 갖고 있는 이벤트를 체크해 봐라....
struct gsv_CHECK_NPC_EVENT : public t_PACKETHEADER {
	short	m_nNpcIDX;
} ;


//--------------------------- Messenger 관련 패킷
// 패킷 타입은 CLI_MESSENGER, WSV_MESSENGER로 통일, 세부 타입 m_btCMD에 의해 패킷 구조가 결정됨.
#define	FRIEND_STATUS_NORMAL		0x000		// 일반
#define FRIEND_STATUS_HUNT			0x001		// 사냥중
#define	FRIEND_STATUS_STORE			0x002		// 장사중
#define	FRIEND_STATUS_QUEST			0x003		// 퀘스트중
#define	FRIEND_STATUS_EAT			0x004		// 식사중
#define	FRIEND_STATUS_REST			0x005		// 휴식중
#define	FRIEND_STATUS_ONLINE		0x007		// 접속됨		: 친구목록의 m_dwUserTAG가 접속했다.
#define	FRIEND_STATUS_OFFLINE		0x008		// 접속끊김		: 친구목록의 m_dwUserTAG가 접속끊김
#define	FRIEND_STATUS_REFUSE_MSG	0x009		// 모든 메세지 거부

#define	FRIEND_STATUS_REFUSED		0x080		// 내 대화를 차단했다
#define	FRIEND_STATUS_DELETED		0x081		// 나를 친구목록에서 삭제했다


#define	MSGR_CMD_APPEND_REQ			0x001		// 친구 추가 요청	: 대상은 반드시 접속해 있어야 함
#define MSGR_CMD_APPEND_ACCEPT		0x002		// 친구 추가 요청 승낙
#define	MSGR_CMD_APPEND_REJECT		0x003		// 친구 추가 요청 거절
#define	MSGR_CMD_NOT_FOUND			0x004		// 친구 추가 실패(대상이 접속이 끊긴경우등...)
#define	MSGR_CMD_DELETE				0x005		// 친구 삭제		: 목록중에서 m_dwUserTAG를 삭제..
#define	MSGR_CMD_LIST				0x006		// 친구 리스트	: 친구목록 요구
#define	MSGR_CMD_REFUSE				0x007		// 메시지 차단	: 목록중에서 m_dwUserTAG의 메세지 거부
#define	MSGR_CMD_CHANGE_STATUS		0x008		// 상태 변경

#define	MSGR_CMD_LOGIN				0x080		// 서버에서만 사용
#define	MSGR_CMD_LOGOUT				0x081		// 서버에서만 사용


struct tag_MCMD_HEADER : public t_PACKETHEADER {
	BYTE	m_btCMD;
} ;
struct cli_MCMD_APPEND_REQ : public tag_MCMD_HEADER {		// 친구 추가 요청할때
	char	m_szName[0];
} ;
struct cli_MCMD_APPEND_REPLY : public tag_MCMD_HEADER {		// 친구 추가 요청에 대한 응답
	WORD	m_wUserIDX;
} ;
struct cli_MCMD_TAG : public tag_MCMD_HEADER {				// 삭제, 차단 요청
	DWORD	m_dwUserTAG;
} ;
struct cli_MCMD_STATUS_REQ : public tag_MCMD_HEADER {		// 내 상태 변경 요청
	BYTE	m_btStatus;
} ;


struct wsv_MCMD_APPEND_REQ : public tag_MCMD_HEADER {		// 친구 추가 요청받았을때
	WORD	m_wUserIDX;
	char	m_szName[0];
} ;
struct wsv_MCMD_APPEND_ACCEPT : public tag_MCMD_HEADER {	// 친구 추가 승낙일경우 리스트에 추가함
	DWORD	m_dwUserTAG;
	BYTE	m_btStatus;
	char	m_szName[0];
} ;
struct wsv_MCMD_STATUS_REPLY : public tag_MCMD_HEADER {		// 서버에서의 응답:: m_dwUserTAG상태 변경 통보
	DWORD	m_dwUserTAG;
	BYTE	m_btStatus;
} ;

struct wsv_MCMD_LIST : public tag_MCMD_HEADER {			// 친구 목록
	BYTE	m_btFriendCNT;
	/*
	{
		wsv_MCMD_STATUS_REPLY
		m_szName[]
	} X m_btFriendCNT
	*/
} ;





struct cli_MESSENGER_CHAT : public t_PACKETHEADER {
	DWORD	m_dwUserTAG;
	char	m_szMSG[0];
} ;
struct wsv_MESSENGER_CHAT : public t_PACKETHEADER {
	DWORD	m_dwUserTAG;
	char	m_szMSG[0];
} ;


//--------------------------- Chatting room 관련 패킷
#define	CHAT_REQ_ROOM_LIST		0x001
#define	CHAT_REQ_MAKE			0x002
#define	CHAT_REQ_JOIN			0x003
#define	CHAT_REQ_LEFT			0x004
#define	CHAT_REQ_KICK_USER		0x005

#define	MAX_CHAT_ROOM_USERS		64

struct tag_CHAT_HEADER : public t_PACKETHEADER {
	BYTE	m_btCMD;
} ;

struct cli_CHAT_ROOM_MAKE : public tag_CHAT_HEADER {
	BYTE	m_btRoomTYPE;
	BYTE	m_btMaxUSER;		// 최대 수용인원
	// szTitle[]
	// szPassword[]
} ;
struct cli_CHAT_ROOM_JOIN : public tag_CHAT_HEADER {
	BYTE	m_btRoomTYPE;
	WORD	m_wRoomID;
	// szPassword[]
} ;
struct cli_CHAT_ROOM_LIST : public tag_CHAT_HEADER {
	BYTE	m_btRoomTYPE;
	WORD	m_wFromRoomID;
} ;
struct cli_CHAT_ROOM_KICK : public tag_CHAT_HEADER {
	t_HASHKEY	m_HashUSER;
} ;

struct tag_CHAT_ROOM {
	BYTE	m_btRoomTYPE;
	short	m_nRoomIDX;
	char	m_cUserCNT;
	// szTitle[]
} ;
struct wsv_CHAT_ROOM_USER : public tag_CHAT_HEADER {
	WORD	m_wUserID;
	// szName[]
} ;
/*
struct wsv_CHAT_ROOM_JOIN : public tag_CHAT_HEADER {
	WORD	m_wUserID;	// 자신
	szRoomName[]		// 방이름
	{
		WORD	m_wUserID;
		szUserName[];
	} x ???
} ;
*/
struct wsv_CHAT_ROOM_LIST : public tag_CHAT_HEADER {
	char	m_cRoomCNT;
	// tag_CHAT_ROOM[]
} ;

#define	CHAT_REPLY_ROOM_MAKED		0x001	// 만들기 성공			m_wUserID(자신)
#define	CHAT_REPLY_MAKE_FAILED		0x002

#define	CHAT_REPLY_ROOM_JOINED		0x010	// 참가 성공			wsv_CHAT_ROOM_JOIN[]
#define	CHAT_REPLY_FULL_USERS		0x011	// 인원 만땅
#define	CHAT_REPLY_INVALIED_PASS	0x012	// 비번 틀림
#define CHAT_REPLY_ROOM_NOT_FOUND	0x013	// 방 없어 졌다
#define CHAT_REPLY_USER_ADD			0x014	// 사용자 추가			m_wUserID, szName[]
#define	CHAT_REPLY_KICKED			0x015	// 추방 당했다			m_wUserID
#define CHAT_REPLY_USER_LEFT		0x016	// 방에서 나갔다.		m_wUserID
#define	CHAT_REPLY_USER_LEFT_NAME	0x017	// 방에서 나갔다.		사용자 이름으로 탈퇴...

#define	CHAT_REPLY_ROOM_LIST		0x020
#define	CHAT_REPLY_ROOM_LIST_END	0x021

struct cli_CHATROOM_MSG : public t_PACKETHEADER {
	char m_szMSG[0];
} ;
struct wsv_CHATROOM_MSG : public t_PACKETHEADER {
	WORD m_wObjectID;
	char m_szMSG[0];
} ;


//--------------------------- 쪽지 관련 패킷
#define	MEMO_REQ_CONTENTS			0x001	// 수신된 전체 쪽지 내용 요구
#define MEMO_REQ_SEND				0x002	// 쪽지 전송 요구
#define MEMO_REQ_RECEIVED_CNT		0x003	// 보관된 쪽지 갯수 요구

struct cli_MEMO : public t_PACKETHEADER {
	BYTE m_btTYPE;
	/*
	struct tagReqMemoSend {
		// szTargetCHAR[]
		// szMemoContent[]			// 255자 미만
	} ;
	*/
} ;

#define	MEMO_REPLY_RECEIVED_CNT			0x001	// 보관된 쪽지 갯수
#define	MEMO_REPLY_CONTENTS				0x002	// 쪽지 내용들...
#define	MEMO_REPLY_SEND_OK				0x003	// 전송 성공
#define	MEMO_REPLY_SEND_INVALID_TARGET	0x004	// 대상 오류, 보내기 실패
#define	MEMO_REPLY_SEND_NOT_EXIST		0x005	// 대상이 존제하지 않는다.
#define	MEMO_REPLY_SEND_REFUSED			0x006	// 쪽지 수신 거부 상태로 보내기 실패
#define MEMO_REPLY_SEND_FULL_MEMO		0x007	// 상대방 수신함이 꽉차서 보내기 실패
#define MEMO_REPLY_SEND_INVALID_CONTENT	0x008	// 내용오류
struct wsv_MEMO : public t_PACKETHEADER {
	BYTE	m_btTYPE;
	short	m_nRecvCNT[0];						//  m_btTYPE == MEMO_REPLY_RECEIVED_CNT 일경우 받은 쪽지 갯수
	/*
	struct tagReplyMemoCONTENTS {
		// DWORD dwReceivedDATE			// classTIME::GetCurrentAbsSecond()함수로 얻어진 시간임.
		// szFrom[]						// 보낸이
		// szMemo[]						// 내용
	} ;
	*/
} ;


#define	REQ_MALL_ITEM_LIST					0x01	// 리스트 요청
#define	REQ_MALL_ITEM_FIND_CHAR				0x02	// 케릭 이름 체크
#define	REQ_MALL_ITEM_BRING					0x03	// 꺼내오기
#define	REQ_MALL_ITEM_GIVE					0x04	// 선물하기
struct cli_MALL_ITEM_REQ : public t_PACKETHEADER {
	BYTE	m_btReqTYPE;
	short	m_nDupCnt;								// REQ_MALL_ITEM_BRING, REQ_MALL_ITEM_GIVE시에 갯수..
	BYTE	m_btInvIDX[0];							// REQ_MALL_ITEM_GIVE일경우 0~39 사이의 인벤토리 번호
	// char m_szCharName[]							// REQ_MALL_ITEM_BRING, REQ_MALL_ITEM_GIVE 요청시
	// char m_szDesc[]								// REQ_MALL_ITEM_GIVE 요청시 덧붙일말(선물할때 메세지)::최대 80자
} ;

#define	REPLY_MALL_ITEM_CHECK_CHAR_FOUND	0x01	// 대상 찾았다
#define	REPLY_MALL_ITEM_CHECK_CHAR_NONE		0x02	// 대상 없다.
#define	REPLY_MALL_ITEM_CHECK_CHAR_INVALID	0x03	// 자기 자신 계정등..잘못된 대상
#define	REPLY_MALL_ITEM_BRING_SUCCESS		0x04
#define	REPLY_MALL_ITEM_BRING_FAILED		0x05
#define	REPLY_MALL_ITME_GIVE_SUCCESS		0x06
#define	REPLY_MALL_ITEM_GIVE_FAILED			0x07
#define	REPLY_MALL_ITEM_LIST_START			0x08
#define	REPLY_MALL_ITEM_LIST_DATA			0x09
#define	REPLY_MALL_ITEM_LIST_END			0x0a
#define	REPLY_MALL_ITEM_NOT_FOUND			0x0b

struct gsv_MALL_ITEM_REPLY : public t_PACKETHEADER {
	BYTE				m_btReplyTYPE;
	BYTE				m_btCntOrIdx;				// 갯수 / 몰 인벤토리 인덱스

	/* MALL_ITEM_REQ_LIST 응답 :: m_nCntOrIdx 갯수만큼..
		m_btReplyTYPE == REPLY_MALL_ITEM_LIST_START
			m_btReplyTYPE == REPLY_MALL_ITEM_LIST_DATA
			{
				m_btCntOrIdx :: 갯수
				{
					tagBaseITEM
					szFrom[]		// 선물한 케릭
					if ( '\0' != szFrom[0] ) {
						// szDesc[]	// 선물할때 메세지
						// szTo[]	// 선물한 대상 케릭 이름
					}
				}
			}
		m_btReplyTYPE == REPLY_MALL_ITEM_LIST_END
	*/
	// REPLY_MALL_ITEM_BRING_SUCCESS응답 :: m_nCntOrIdx에는 몰 인벤토리 인덱스 m_BringITEM[0]에는 추가된 아이템
	union {
		tag_SET_INVITEM	m_BringITEM[0];				// MALL_ITEM_REQ_BRING에 대한 응답(변경된 인벤토리 정보) :: m_btCntOrIdx의 몰 인벤토리 아이템이 이동
	} ;
} ;


#define	BILLING_MSG_FREE_USER				0x00	//

//#define	BILLING_MSG_FEE_TYPE1				0x01	//  /요금: 귀하는 정액요금제 사용자 입니다. (2004년06월01일)
//#define	BILLING_MSG_FEE_TYPE2				0x02	//         귀하는 정량요금제 사용자 입니다. (18시간20분20초)

#define	BILLING_MSG_PAY_FU					0x01	// FU	로그인 된 사용자는 무료 아이디 사용자임을 나타낸다.
#define	BILLING_MSG_PAY_FA					0x02	// FA	로그인 된 사용자는 개인정액 사용자임을 나타낸다.
#define	BILLING_MSG_PAY_FAP					0x03	// FAP	로그인 된 사용자는 개인정액 사용자(플레티넘)임을 나타낸다.
#define	BILLING_MSG_PAY_FQ					0x04	// FQ	로그인 된 사용자는 개인정량 사용자임을 나타낸다.
#define	BILLING_MSG_PAY_GU					0x05	// GU	로그인 된 사용자는 게임방 사용자임을 나타낸다.
#define	BILLING_MSG_PAY_GQ					0x06	// GQ	게임방에서 로그인을 시도하였으나 게임방의 과금기간(시간)이 만료되어 개인정량으로 로그인 되었음을 나타낸다.
#define	BILLING_MSG_PAY_IQ					0x07	// IQ	게임방에서 로그인을 시도하였으나 이미 계약된 IP 숫자를 모두 사용하고 있어 개인정량으로 로그인 되었음을 나타낸다.


#define	BILLING_MSG_KICKOUT_CHEAT_DETECT	0x21	// 스핵등 치트 발견되서 종료
#define	BILLING_MSG_KICKOUT_ALREADY_LOGIN	0x22	// 이미 접속된 계정이라 접속 종료
#define	BILLING_MSG_KICKOUT_TIME_EXPIRED	0x23	// 과금 시간 만료
#define	BILLING_MSG_KICKOUT_DUP_LOGIN		0x24	// 다른 넘이 접속해서 접속 종료
#define BILLING_MSG_KICKOUT_NO_RIGHT		0x25	// 접속할 권한이 없다...
#define	BILLING_MSG_KICKOUT_OUT_OF_IP		0x26	// pc방에서 접속할수 있는 ip 갯수가 초과
#define	BILLING_MSG_KICKOUT_EXIST_ACCOUNT	0x27	// 접속되어 있는 계정을 짤랐다.

#define	BILLING_MSG_JPN_NEED_CHARGE			0x41	// 일본 과금 충전 필요!!! gsv_BILLING_MESSAGE2 패킷구조로 전송되고 m_dwPayFlag에 필요한 과금 플레그 셋팅되어 있음

//#define	BILLING_MSG_TIME_ALERT				0x31	// 남은 시간 통보
//#define	BILLING_MSG_TIME_EXPIRED			0x32	// <종료> 결제 시간이 다되어 게임을 종료합니다.

#define	BILLING_MSG_TYPE_SET_DATE			0x81	// 만료 일자 설정	szMsg = 200309101025
#define	BILLING_MSG_TYPE_SET_TIME			0x82	// 만료 시간 설정	szMsg = 남은분
#define	BILLING_MSG_TYPE_TIME_ALERT			0x83	// 남은 시간 경고	szMsg = 남은분

#define	BILLING_MSG_KICKOUT_TAIWAN			0xf0	// 대만 과금서버에서 짤라라 통보 받았을때..
#define	BILLING_MSG_TYPE_TAIWAN				0xf1	// 대만 과금서버에서 받은 메세지.. 보여주기만 하면 됨


struct gsv_BILLING_MESSAGE : public t_PACKETHEADER {
	BYTE	m_btTYPE;
	// char m_szMsg[];
} ;
struct gsv_BILLING_MESSAGE2: public t_PACKETHEADER {
	BYTE	m_btTYPE;
	char	m_cFunctionTYPE;						// 소분류( A ~ R )
	DWORD	m_dwPayFlag;
} ;

// 한국 과금 결제 비트~
#define	PAY_FLAG_KR_FU				(0x01 << BILLING_MSG_PAY_FU)	// FU	로그인 된 사용자는 무료 아이디 사용자임을 나타낸다.
#define	PAY_FLAG_KR_FA				(0x01 << BILLING_MSG_PAY_FA)	// FA	로그인 된 사용자는 개인정액 사용자임을 나타낸다.
#define	PAY_FLAG_KR_FAP				(0x01 << BILLING_MSG_PAY_FAP)	// FAP	로그인 된 사용자는 개인정액 사용자(플레티넘)임을 나타낸다.
#define	PAY_FLAG_KR_FQ				(0x01 << BILLING_MSG_PAY_FQ)	// FQ	로그인 된 사용자는 개인정량 사용자임을 나타낸다.
#define	PAY_FLAG_KR_GU				(0x01 << BILLING_MSG_PAY_GU)	// GU	로그인 된 사용자는 게임방 사용자임을 나타낸다.
#define	PAY_FLAG_KR_GQ				(0x01 << BILLING_MSG_PAY_GQ)	// GQ	게임방에서 로그인을 시도하였으나 게임방의 과금기간(시간)이 만료되어 개인정량으로 로그인 되었음을 나타낸다.
#define	PAY_FLAG_KR_IQ				(0x01 << BILLING_MSG_PAY_IQ)	// IQ	게임방에서 로그인을 시도하였으나 이미 계약된 IP 숫자를 모두 사용하고 있어 개인정량으로 로그인 되었음을 나타낸다.

// #define	PAY_FLAG_KR_PLATINUM		( FLAG_PAY_FAP|FLAG_PAY_GU )

// 일본 과금 결제 비트~
//#define	FLAG_JPAY_CHAT			(0x01 << 0)						// 0	Free Play 	캐릭작성, 채팅
#define	PAY_FLAG_JP_BATTLE			(0x01 << 0)	 					// 1	Entry	배틀, 베이식 필드
#define	PAY_FLAG_JP_COMMUNITY		(0x01 << 1)	 					// 2	Community	친구목록, 채팅방
#define	PAY_FLAG_JP_TRADE			(0x01 << 2)	 					// 3	Trade	거래, 상점
#define	PAY_FLAG_JP_STOCK_SPACE		(0x01 << 3)	 					// 4	Stock Space	뱅크
#define	PAY_FLAG_JP_EXTRA_STOCK		(0x01 << 4)	 					// 5	Extra Stock	추가뱅크 스페이스
#define	PAY_FLAG_JP_STARSHIP_PASS	(0x01 << 5)	 					// 6	Starship Pass	행성간 이동
#define	PAY_FLAG_JP_DUNGEON_ADV		(0x01 << 6)	 					// 7	Dungeon Adventure	던젼 필드
#define	PAY_FLAG_JP_EXTRA_CHAR		(0x01 << 7)	 					// 8	Extra Character	작성가능 캐릭 추가

#define	PAY_FLAG_JP_ALL				(0xff << 0)						//      모든 과금 결제~


// 과금형태에 따라 플레이 가능여부를 체크할 비트...
// 한국 과금중 FLAG_PAY_FAP,FLAG_PAY_GU의 경우는 아래 모든 비트 on 
// 이외 한국 과금은 PLAY_FLAG_EXTRA_STOCK, PLAY_FLAG_EXTRA_CHAR 비트 off
#define	PLAY_FLAG_BATTLE			(0x01 << 0)	 					// 1	Entry	배틀, 베이식 필드
#define	PLAY_FLAG_COMMUNITY			(0x01 << 1)	 					// 2	Community	친구목록, 채팅방
#define	PLAY_FLAG_TRADE				(0x01 << 2)	 					// 3	Trade	거래, 상점
#define	PLAY_FLAG_STOCK_SPACE		(0x01 << 3)	 					// 4	Stock Space	뱅크
#define	PLAY_FLAG_EXTRA_STOCK		(0x01 << 4)	 					// 5	Extra Stock	추가뱅크 스페이스
#define	PLAY_FLAG_STARSHIP_PASS		(0x01 << 5)	 					// 6	Starship Pass	행성간 이동
#define	PLAY_FLAG_DUNGEON_ADV		(0x01 << 6)	 					// 7	Dungeon Adventure	던젼 필드
#define	PLAY_FLAG_EXTRA_CHAR		(0x01 << 7)	 					// 8	Extra Character	작성가능 캐릭 추가

// 한국 기본 과금
#define	PLAY_FLAG_KOREA_DEFAULT		( PLAY_FLAG_BATTLE | PLAY_FLAG_COMMUNITY | PLAY_FLAG_TRADE | PLAY_FLAG_STOCK_SPACE | PLAY_FLAG_STARSHIP_PASS | PLAY_FLAG_DUNGEON_ADV )
#define	PLAY_FLAG_TAIWAN_DEFAULT	PLAY_FLAG_KOREA_DEFAULT


#define	EXT_BILLING_MSG_PAY_KOREA			0x1001	// 한국 과금 정보
#define	EXT_BILLING_MSG_PAY_JAPAN			0x1002	// 일본 과금 플래그
#define	EXT_BILLING_MSG_PAY_TAIWAN			0x1003	// 대만 과금 타입번호

#define	EXT_BILLING_MSG_TYPE_SET_DATE		0x1010	// 만료 일자 설정	m_dwPayType = m_dwPlayingFlag = 0	szMsg = 타입\t200309101025\t타입\t200309101025...
#define	EXT_BILLING_MSG_TYPE_SET_TIME		0x1011	// 만료 시간 설정	m_dwPayType = m_dwPlayingFlag = 0	szMsg = 타입\t남은분\t타입\t남은분...
#define EXT_BILLING_MSG_TYPE_TIME_EXPIRED	0x1012	// PayType=0이고 m_dwPlayingFlag[0]에 만료된 과금 플레그있음
#define EXT_BILLING_MSG_TYPE_TIME_ALERT		0x1013	// PayType=0이고 m_dwPlayingFlag[0]에 시간경고되는 과금 플레그있음

struct gsv_BILLING_MESSAGE_EXT : public t_PACKETHEADER {
	WORD	m_wMsgTYPE;
	DWORD	m_dwPayType;
	DWORD	m_dwPlayingFlag[ 4 ];	// 여유있게~ :: 32 * 4 = 128
	// char m_szMsg[];
} ;



//-------------------------------------------------------------------------------------------------
struct gsv_GODDNESS_MODE : public t_PACKETHEADER {
	BYTE	m_btOnOff;								// 적용/삭제
	WORD	m_wObjectIDX;							// 대상 서버 케릭터 인덱스
} ;

//-------------------------------------------------------------------------------------------------
struct gsv_PATSTATE_CHANGE : public t_PACKETHEADER { // GSV_PATSTATE_CHANGE
	BYTE	m_btOnOff;								// 카트 소환 가능/불가능(1/0) 
	DWORD	m_dwCoolTIME;							// 카트 소환 불가능 상태이면 여기에 쿨타임 값
	short	m_nMaxPatHP;							// 카트 체력
	WORD	m_wObjectIDX;							// 사용할 카트 체력 
} ;


//-------------------------------------------------------------------------------------------------
struct gsv_CHARSTATE_CHANGE : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	DWORD	m_dwFLAG;
};


struct cli_CART_RIDE : public t_PACKETHEADER {
	BYTE	m_btType;
	WORD	m_wOwnerObjIDX;				// 태울 아바타
	WORD	m_wGuestObjIDX;				// 탈 아바타
} ;
struct gsv_CART_RIDE : public cli_CART_RIDE {

} ;
#define	CART_RIDE_REQ				0x01	// 탈래 ?	m_wGuestObjIDX에게만 전송됨
#define	CART_RIDE_ACCEPT			0x02	// 탈께		주변 모든 케릭에 전송됨
#define	CART_RIDE_REFUSE			0x03	// 안탈래	m_wOwnerObjIDX에게만 전송됨
#define	CART_RIDE_OWNER_NOT_FOUND	0x04	// 태워 준다던 케릭이 사라졌다
#define	CART_RIDE_GUEST_NOT_FOUND	0x05	// 태우려던 케릭이 사라졌다

struct gsv_UPDATE_NAME : public t_PACKETHEADER {
	/*
		This update player's name
	*/
	short m_nCharID;
	// new name;
};

struct pxy_UPDATE_NAME : public t_PACKETHEADER {

};

struct pxy_SET_RIGHTS : public t_PACKETHEADER {
	DWORD wRIGHT;
};

//-------------------------------------------------------------------------------------------------

#if defined(__SERVER) && !defined(__SKIP_SRV_PROTOTYPE)
#include "SRV_Prototype.h"
#endif

//-------------------------------------------------------------------------------------------------
#define	__T_PACKET
struct t_PACKET {
	union {
		t_PACKETHEADER				m_HEADER;
		BYTE						m_pDATA[ MAX_PACKET_SIZE ];
		t_NETWORK_STATUS			m_NetSTATUS;

		cli_CHECK_AUTH				m_cli_CHECK_AUTH;
		srv_CHECK_AUTH				m_srv_CHECK_AUTH;

		lsv_ERROR					m_lsv_ERROR;
		gsv_ERROR					m_gsv_ERROR;

		gsv_GM_COMMAND				m_gsv_GM_COMMAND;

		gsv_SET_GLOBAL_VAR			m_gsv_SET_GLOBAL_VAR;
		gsv_SET_GLOBAL_FLAG			m_gsv_SET_GLOBAL_FLAG;

		cli_LOGOUT_REQ				m_cli_LOGOUT_REQ;
		gsv_LOGOUT_REPLY			m_gsv_LOGOUT_REPLY;

		// CLI <-> LSV
		cli_ACCEPT_REQ				m_cli_ACCEPT_REQ;
        cli_LOGIN_REQ				m_cli_LOGIN_REQ;
        srv_LOGIN_REPLY				m_srv_LOGIN_REPLY;

		cli_CHANNEL_LIST_REQ		m_cli_CHANNEL_LIST_REQ;
		lsv_CHANNEL_LIST_REPLY		m_lsv_CHANNEL_LIST_REPLY;

		cli_SELECT_SERVER			m_cli_SELECT_SERVER;
		lsv_SELECT_SERVER			m_lsv_SELECT_SERVER;


		// CLI <-> WSV/GSV
		cli_JOIN_SERVER_REQ			m_cli_JOIN_SERVER_REQ;
		srv_JOIN_SERVER_REPLY		m_srv_JOIN_SERVER_REPLY;
		/*
		cli_JOIN_WORLD				m_cli_JOIN_WORLD;
		wsv_JOIN_WORLD				m_wsv_JOIN_WORLD;
		*/

		// CLI <-> WSV
		wsv_MOVE_SERVER				m_wsv_MOVE_SERVER;

		cli_CHAR_LIST				m_cli_CHAR_LIST;
		wsv_CHAR_LIST				m_wsv_CHAR_LIST;

		cli_CREATE_CHAR				m_cli_CREATE_CHAR;
		wsv_CREATE_CHAR				m_wsv_CREATE_CHAR;

		cli_DELETE_CHAR				m_cli_DELETE_CHAR;
		wsv_DELETE_CHAR				m_wsv_DELETE_CHAR;

		gsv_INIT_DATA				m_gsv_INIT_DATA;

		cli_SELECT_CHAR				m_cli_SELECT_CHAR;
		gsv_SELECT_CHAR				m_gsv_SELECT_CHAR;


		// CLI <-> GSV
		gsv_SERVER_DATA				m_gsv_SERVER_DATA;
		gsv_RELAY_REQ				m_gsv_RELAY_REQ;
		cli_RELAY_REPLY				m_cli_RELAY_REPLY;

		cli_JOIN_ZONE				m_cli_JOIN_ZONE;
		gsv_JOIN_ZONE				m_gsv_JOIN_ZONE;

		cli_REVIVE_REQ				m_cli_REVIVE_REQ;
		gsv_REVIVE_REPLY			m_gsv_REVIVE_REPLY;
		cli_SET_REVIVE_POS			m_cli_SET_REVIVE_POS;

		cli_SET_VAR_REQ				m_cli_SET_VAR_REQ;
		gsv_SET_VAR_REPLY			m_gsv_SET_VAR_REPLY;

		gsv_INVENTORY_DATA			m_gsv_INVENTORY_DATA;
		gsv_QUEST_DATA				m_gsv_QUEST_DATA;

		gsv_QUEST_ONLY				m_gsv_QUEST_ONLY;
		gsv_WISH_LIST				m_gsv_WISH_LIST;

		gsv_CHEAT_CODE				m_gsv_CHEAT_CODE;

		cli_SET_MOTION				m_cli_SET_MOTION;
		gsv_SET_MOTION				m_gsv_SET_MOTION;

		cli_TOGGLE					m_cli_TOGGLE;
		gsv_TOGGLE					m_gsv_TOGGLE;

		cli_CHAT					m_cli_CHAT;
		gsv_CHAT					m_gsv_CHAT;

		cli_WHISPER					m_cli_WHISPER;
		gsv_WHISPER					m_gsv_WHISPER;

		cli_SHOUT					m_cli_SHOUT;
		gsv_SHOUT					m_gsv_SHOUT;

		cli_PARTY_CHAT				m_cli_PARTY_CHAT;
		gsv_PARTY_CHAT				m_gsv_PARTY_CHAT;

		cli_CLAN_CHAT				m_cli_CLAN_CHAT;
		wsv_CLAN_CHAT				m_wsv_CLAN_CHAT;

		cli_ALLIED_CHAT				m_cli_ALLIED_CHAT;
		gsv_ALLIED_CHAT				m_gsv_ALLIED_CHAT;

		cli_ALLIED_SHOUT			m_cli_ALLIED_SHOUT;
		gsv_ALLIED_SHOUT			m_gsv_ALLIED_SHOUT;

		gsv_SET_EVENT_STATUS		m_gsv_SET_EVENT_STATUS;

		tag_ADD_CHAR				m_tag_ADD_CHAR;
		gsv_MOB_CHAR				m_gsv_MOB_CHAR;
		gsv_NPC_CHAR				m_gsv_NPC_CHAR;
		gsv_AVT_CHAR				m_gsv_AVT_CHAR;
		gsv_SUB_OBJECT				m_gsv_SUB_OBJECT;

		cli_SET_WEIGHT_RATE			m_cli_SET_WEIGHT_RATE;
		gsv_SET_WEIGHT_RATE			m_gsv_SET_WEIGHT_RATE;

		gsv_ADJUST_POS				m_gsv_ADJUST_POS;

		cli_CANTMOVE				m_cli_CANTMOVE;
		cli_SETPOS					m_cli_SETPOS;

		cli_STOP					m_cli_STOP;
		gsv_STOP					m_gsv_STOP;

		cli_ATTACK					m_cli_ATTACK;
		gsv_ATTACK					m_gsv_ATTACK;

		cli_DAMAGE					m_cli_DAMAGE;
		gsv_DAMAGE					m_gsv_DAMAGE;

		gsv_SETEXP					m_gsv_SETEXP;
		gsv_LEVELUP					m_gsv_LEVELUP;

		cli_HP_REQ					m_cli_HP_REQ;
		gsv_HP_REPLY				m_gsv_HP_REPLY;

		gsv_SET_HPnMP				m_gsv_SET_HPnMP;

		cli_MOUSECMD				m_cli_MOUSECMD;
		gsv_MOUSECMD				m_gsv_MOUSECMD;

		gsv_MOVE					m_gsv_MOVE;

		gsv_CHANGE_NPC				m_gsv_CHANGE_NPC;

		cli_STORE_TRADE_REQ			m_cli_STORE_TRADE_REQ;
		gsv_STORE_TRADE_REPLY		m_gsv_STORE_TRADE_REPLY;

		gsv_SET_MONEYnINV			m_gsv_SET_MONEYnINV;
		gsv_SET_INV_ONLY			m_gsv_SET_INV_ONLY;

		cli_USE_ITEM				m_cli_USE_ITEM;
		gsv_USE_ITEM				m_gsv_USE_ITEM;

		cli_EQUIP_ITEM				m_cli_EQUIP_ITEM;
		gsv_EQUIP_ITEM				m_gsv_EQUIP_ITEM;

		cli_ASSEMBLE_RIDE_ITEM		m_cli_ASSEMBLE_RIDE_ITEM;
		gsv_ASSEMBLE_RIDE_ITEM		m_gsv_ASSEMBLE_RIDE_ITEM;


		cli_USE_ITEM_TO_REPAIR		m_cli_USE_ITEM_TO_REPAIR;
		gsv_SET_INV_ONLY			m_gsv_USED_ITEM_TO_REPAIR;

		cli_REPAIR_FROM_NPC			m_cli_REPAIR_FROM_NPC;
		gsv_SET_MONEYnINV			m_gsv_REPAIRED_FROM_NPC;

		gsv_SET_ITEM_LIFE			m_gsv_SET_ITEM_LIFE;

		//cli_CHANGE_SKIN				m_cli_CHANGE_SKIN;
		gsv_CHANGE_SKIN				m_gsv_CHANGE_SKIN;

		cli_DROP_ITEM				m_cli_DROP_ITEM;

		cli_MOVE_ITEM				m_cli_MOVE_ITEM;
		gsv_MOVE_ITEM				m_gsv_MOVE_ITEM;

		cli_MOVE_ZULY				m_cli_MOVE_ZULY;
		gsv_MOVE_ZULY				m_gsv_MOVE_ZULY;

		cli_CREATE_ITEM_REQ			m_cli_CREATE_ITEM_REQ;
		gsv_CREATE_ITEM_REPLY		m_gsv_CREATE_ITEM_REPLY;

		//cli_CREATE_ITEM_EXP_REQ		m_cli_CREATE_ITEM_EXP_REQ;

		cli_ITEM_RESULT_REPORT		m_cli_ITEM_RESULT_REPORT;
		gsv_ITEM_RESULT_REPORT		m_gsv_ITEM_RESULT_REPORT;

		cli_BANK_LIST_REQ			m_cli_BANK_LIST_REQ;
		gsv_BANK_LIST_REPLY			m_gsv_BANK_LIST_REPLY;

		gsv_ADD_FIELDITEM			m_gsv_ADD_FIELDITEM;
		gsv_SUB_FIELDITEM			m_gsv_SUB_FIELDITEM;

		cli_GET_FIELDITEM_REQ		m_cli_GET_FIELDITEM_REQ;
		gsv_GET_FIELDITEM_REPLY		m_gsv_GET_FIELDITEM_REPLY;

		cli_TELEPORT_REQ			m_cli_TELEPORT_REQ;
		gsv_TELEPORT_REPLY			m_gsv_TELEPORT_REPLY;

		cli_USE_BPOINT_REQ			m_cli_USE_BPOINT_REQ;
		gsv_USE_BPOINT_REPLY		m_gsv_USE_BPOINT_REPLY;

		cli_SET_HOTICON				m_cli_SET_HOTICON;
		gsv_SET_HOTICON				m_gsv_SET_HOTICON;

		cli_SET_BULLET				m_cli_SET_BULLET;
		gsv_SET_BULLET				m_gsv_SET_BULLET;


		cli_TRADE_P2P				m_cli_TRADE_P2P;
		gsv_TRADE_P2P				m_gsv_TRADE_P2P;

		cli_TRADE_P2P_ITEM			m_cli_TRADE_P2P_ITEM;
		gsv_TRADE_P2P_ITEM			m_gsv_TRADE_P2P_ITEM;

		cli_SET_WISHITEM			m_cli_SET_WISHITEM;

		cli_P_STORE_OPEN			m_cli_P_STORE_OPEN;
		gsv_P_STORE_OPENED			m_gsv_P_STORE_OPENED;

		cli_P_STORE_CLOSE			m_cli_P_STORE_CLOSE;
		gsv_P_STORE_CLOSED			m_gsv_P_STORE_CLOSED;

		cli_P_STORE_LIST_REQ		m_cli_P_STORE_LIST_REQ;
		gsv_P_STORE_LIST_REPLY		m_gsv_P_STORE_LIST_REPLY;

		cli_P_STORE_BUY_REQ			m_cli_P_STORE_BUY_REQ;
		cli_P_STORE_SELL_REQ		m_cli_P_STORE_SELL_REQ;

		gsv_P_STORE_RESULT			m_gsv_P_STORE_RESULT;

		gsv_SKILL_LEARN_REPLY		m_gsv_SKILL_LEARN_REPLY;

		cli_SKILL_LEVELUP_REQ		m_cli_SKILL_LEVELUP_REQ;
		gsv_SKILL_LEVELUP_REPLY		m_gsv_SKILL_LEVELUP_REPLY;

		cli_SELF_SKILL				m_cli_SELF_SKILL;
		gsv_SELF_SKILL				m_gsv_SELF_SKILL;

		cli_TARGET_SKILL			m_cli_TARGET_SKILL;
		gsv_TARGET_SKILL			m_gsv_TARGET_SKILL;

		cli_POSITION_SKILL			m_cli_POSITION_SKILL;
		gsv_POSITION_SKILL			m_gsv_POSITION_SKILL;

		gsv_EFFECT_OF_SKILL			m_gsv_EFFECT_OF_SKILL;
		gsv_DAMAGE_OF_SKILL			m_gsv_DAMAGE_OF_SKILL;
		gsv_RESULT_OF_SKILL			m_gsv_RESULT_OF_SKILL;
		gsv_SKILL_START				m_gsv_SKILL_START;
	
		gsv_SKILL_CANCEL			m_gsv_SKILL_CANCEL;

		gsv_CLEAR_STATUS			m_gsv_CLEAR_STATUS;
		gsv_SPEED_CHANGED			m_gsv_SPEED_CHANGED;

		cli_APPRAISAL_REQ			m_cli_APPRAISAL_REQ;
		gsv_APPRAISAL_REPLY			m_gsv_APPRAISAL_REPLY;

		cli_QUEST_REQ				m_cli_QUEST_REQ;
		gsv_QUEST_REPLY				m_gsv_QUEST_REPLY;

		gsv_CHECK_NPC_EVENT			m_gsv_CHECK_NPC_EVENT;

		cli_PARTY_REQ				m_cli_PARTY_REQ;
		gsv_PARTY_REQ				m_gsv_PARTY_REQ;

		cli_PARTY_REPLY				m_cli_PARTY_REPLY;
		gsv_PARTY_REPLY				m_gsv_PARTY_REPLY;

//		wsv_PARTY_USER				m_wsv_PARTY_USER;

		gsv_PARTY_MEMBER			m_gsv_PARTY_MEMBER;
		gsv_PARTY_LEVnEXP			m_gsv_PARTY_LEVnEXP;
		gsv_CHANGE_OBJIDX			m_gsv_CHANGE_OBJIDX;
		gsv_PARTY_ITEM				m_gsv_PARTY_ITEM;
		cli_PARTY_RULE				m_cli_PARTY_RULE;
		gsv_PARTY_RULE				m_gsv_PARTY_RULE;


		gsv_ADD_EVENTOBJ			m_gsv_ADD_EVENTOBJ;
//		gsv_SUB_EVENTOBJ			m_gsv_SUB_EVENTOBJ;

		cli_CLAN_COMMAND			m_cli_CLAN_COMMAND;
		cli_CLAN_CREATE				m_cli_CLAN_CREATE;
		cli_CLAN_MEMBER_JOBnLEV		m_cli_CLAN_MEMBER_JOBnLEV;
		wsv_CLAN_MEMBER_JOBnLEV		m_wsv_CLAN_MEMBER_JOBnLEV;
		wsv_CLAN_COMMAND			m_wsv_CLAN_COMMAND;
		wsv_RESULT_CLAN_SET			m_wsv_RESULT_CLAN_SET;

		tag_CHAT_HEADER				m_tag_CHAT_HEADER;

		cli_CHAT_ROOM_MAKE			m_cli_CHAT_ROOM_MAKE;
		cli_CHAT_ROOM_JOIN			m_cli_CHAT_ROOM_JOIN;
		cli_CHAT_ROOM_LIST			m_cli_CHAT_ROOM_LIST;
		cli_CHAT_ROOM_KICK			m_cli_CHAT_ROOM_KICK;
									
		wsv_CHAT_ROOM_USER			m_wsv_CHAT_ROOM_USER;
		wsv_CHAT_ROOM_LIST			m_wsv_CHAT_ROOM_LIST;

		cli_CHATROOM_MSG			m_cli_CHATROOM_MSG;
		wsv_CHATROOM_MSG			m_wsv_CHATROOM_MSG;

		cli_MEMO					m_cli_MEMO;
		wsv_MEMO					m_wsv_MEMO;

		tag_MCMD_HEADER				m_tag_MCMD_HEADER;

		cli_MCMD_APPEND_REQ			m_cli_MCMD_APPEND_REQ;
		cli_MCMD_APPEND_REPLY		m_cli_MCMD_APPEND_REPLY;
		cli_MCMD_TAG				m_cli_MCMD_TAG;	
		cli_MCMD_STATUS_REQ			m_cli_MCMD_STATUS_REQ;
									
		wsv_MCMD_APPEND_REQ			m_wsv_MCMD_APPEND_REQ;
		wsv_MCMD_APPEND_ACCEPT		m_wsv_MCMD_APPEND_ACCEPT;
		wsv_MCMD_STATUS_REPLY		m_wsv_MCMD_STATUS_REPLY;
		wsv_MCMD_LIST				m_wsv_MCMD_LIST;	

		cli_MESSENGER_CHAT			m_cli_MESSENGER_CHAT;
		wsv_MESSENGER_CHAT			m_wsv_MESSENGER_CHAT;

		cli_CRAFT_ITEM_REQ			m_cli_CRAFT_ITEM_REQ;
		cli_CRAFT_GEMMING_REQ		m_cli_CRAFT_GEMMING_REQ;
		cli_CRAFT_BREAKUP_REQ		m_cli_CRAFT_BREAKUP_REQ;
		cli_CRAFT_UPGRADE_REQ		m_cli_CRAFT_UPGRADE_REQ;
									
		gsv_CRAFT_ITEM_REPLY		m_gsv_CRAFT_ITEM_REPLY;

		gsv_SET_MONEY_ONLY			m_gsv_SET_MONEY_ONLY;
		gsv_SET_ABILITY				m_gsv_SET_ABILITY;

		cli_MALL_ITEM_REQ			m_cli_MALL_ITEM_REQ;
		gsv_MALL_ITEM_REPLY			m_gsv_MALL_ITEM_REPLY;

		gsv_BILLING_MESSAGE			m_gsv_BILLING_MESSAGE;
		gsv_BILLING_MESSAGE_EXT		m_gsv_BILLING_MESSAGE_EXT;
		gsv_BILLING_MESSAGE2		m_gsv_BILLING_MESSAGE2;

		gsv_SET_NPC_SHOW			m_gsv_SET_NPC_SHOW;

		cli_CLANMARK_SET			m_cli_CLANMARK_SET;
		cli_CLANMARK_REQ			m_cli_CLANMARK_REQ;
		wsv_CLANMARK_REPLY			m_wsv_CLANMARK_REPLY;

		cli_CLANMARK_REG_TIME		m_cli_CLANMARK_REG_TIME;
		wsv_CLANMARK_REG_TIME		m_wsv_CLANMARK_REG_TIME;

		gsv_GODDNESS_MODE			m_gsv_GODDNESS_MODE;

		cli_SCREEN_SHOT_TIME		m_cli_SCREEN_SHOT_TIME;
		gsv_SCREEN_SHOT_TIME		m_gsv_SCREEN_SHOT_TIME;

		cli_SUMMON_CMD				m_cli_SUMMON_CMD;
		// gsv_SUMMON_CMD			m_gsv_SUMMON_CMD;
		gsv_PATSTATE_CHANGE			m_gsv_PATSTATE_CHANGE;

		gsv_CHARSTATE_CHANGE		m_gsv_CHARSTATE_CHANGE;
		cli_CART_RIDE				m_cli_CART_RIDE;
		gsv_CART_RIDE				m_gsv_CART_RIDE;

		gsv_UPDATE_NAME				m_gsv_UPDATE_NAME;
		pxy_UPDATE_NAME				m_pxy_UPDATE_NAME;
		pxy_SET_RIGHTS				m_pxy_SET_RIGHTS;

#if defined(__SERVER) && !defined(__SKIP_SRV_PROTOTYPE)
		gsv_LOG_SQL					m_gsv_LOG_SQL;

		zws_SERVER_INFO				m_zws_SERVER_INFO;

		zws_CONFIRM_ACCOUNT_REQ		m_zws_CONFIRM_ACCOUNT_REQ;
		wls_CONFIRM_ACCOUNT_REPLY	m_wls_CONFIRM_ACCOUNT_REPLY;

		zws_ADD_ACCOUNT				m_zws_ADD_ACCOUNT;
		zws_SUB_ACCOUNT				m_zws_SUB_ACCOUNT;
		zas_SUB_ACCOUNT				m_zas_SUB_ACCOUNT;
		zas_SERVER_TYPE				m_zas_SERVER_TYPE;

		zas_MESSAGE					m_zas_MESSAGE;
		zas_MESSAGE_EXT				m_zas_MESSAGE_EXT;

		zas_KICK_ACCOUNT			m_zas_KICK_ACCOUNT;
		zas_CHECK_ACCOUNT			m_zas_CHECK_ACCOUNT;

		wls_KICK_ACCOUNT			m_lws_KICK_ACCOUNT;

		gsv_BLOCK					m_gsv_BLOCK;

		lsv_CHECK_ACCOUNT			m_lsv_CHECK_ACCOUNT;

		gsv_CHANGE_CHAR				m_gsv_CHANGE_CHAR;

		srv_CHEAT					m_gsv_CHEAT_REQ;
		srv_CHEAT					m_wsv_CHEAT_REQ;

		srv_ACTIVE_MODE				m_srv_ACTIVE_MODE;
		srv_USER_LIMIT				m_srv_USER_LIMIT;
		wls_CHANNEL_LIST			m_wls_CHANNEL_LIST;

		wls_ACCOUNT_LIST			m_wls_ACCOUNT_LIST;
		zws_ACCOUNT_LIST			m_zws_ACCOUNT_LIST;

		srv_SET_WORLD_VAR			m_srv_SET_WORLD_VAR;

		mon_SERVER_STATUS_REQ		m_mon_SERVER_STATUS_REQ;
		srv_SERVER_LIST_REPLY		m_srv_SERVER_LIST_REPLY;
		wls_SERVER_STATUS_REPLY		m_wls_SERVER_STATUS_REPLY;
		gsv_SERVER_STATUS_REPLY		m_gsv_SERVER_STATUS_REPLY;

		ost_SERVER_USERLOGOUT		m_ost_SERVER_USERLOGOUT;
		gsv_SERVER_USERLOGOUT_REPLY m_gsv_SERVER_USERLOGOUT_REPLY;
		ost_SERVER_ZONEINFO			m_ost_SERVER_ZONEINFO;
		gsv_SERVER_ZONEINFO_REPLY	m_gsv_SERVER_ZONEINFO_REPLY;
		ost_SERVER_USERINFO			m_ost_SERVER_USERINFO;
		gsv_SERVER_USERINFO_REPLY	m_gsv_SERVER_USERINFO_REPLY;
		ost_SERVER_CHGUSER			m_ost_SERVER_CHGUSER;
		gsv_SERVER_CHGUSER_REPLY	m_gsv_SERVER_CHGUSER_REPLY;
		ost_SERVER_MOVECHR			m_ost_SERVER_MOVECHR;
		gsv_SERVER_MOVECHR_REPLY	m_gsv_SERVER_MOVECHR_REPLY;
		ost_SERVER_ZONEANNOUNCE		m_ost_SERVER_ZONEANNOUNCE;
		ost_SERVER_IPSEARCH			m_ost_SERVER_IPSEARCH;
		gsv_SERVER_IPSEARCH_REPLY	m_gsv_SERVER_IPSEARCH_REPLY;

		tag_ZWS_CLAN_PACKET			m_zws_CLAN_PACKET;
		zws_CREATE_CLAN				m_zws_CREATE_CLAN;
		zws_SET_USER_CLAN			m_zws_SET_USER_CLAN;
		zws_DEL_USER_CLAN			m_zws_DEL_USER_CLAN;

		gsv_ADJ_CLAN_VAR			m_gsv_ADJ_CLAN_VAR;
		wsv_SET_CLAN_VAR			m_wsv_SET_CLAN_VAR;

		bkd_MAKE_ACCOUNT			m_bkd_MAKE_ACCOUNT;
		bkd_MOD_ACCOUNT				m_bkd_MOD_ACCOUNT;
/*
		gsv_ADD_ZONE_LIST			m_gsv_ADD_ZONE_LIST;

		gsv_DEL_ZONE				m_gsv_DEL_ZONE;
		wsv_DEL_ZONE				m_wsv_DEL_ZONE;

		gsv_WARP_USER				m_gsv_WARP_USER;
		srv_DO_TRIGGER_NCPOBJ		m_srv_DO_TRIGGER_NPCOBJ;
		srv_DO_TRIGGER_EVENTOBJ		m_srv_DO_TRIGGER_EVENTOBJ;

		srv_SET_NPCOBJ_VAR			m_srv_SET_NPCOBJ_VAR;
		srv_SET_EVENTOBJ_VAR		m_srv_SET_EVENTOBJ_VAR;

		gsv_LEVEL_UP				m_gsv_LEVEL_UP;
		wsv_PARTY_CMD				m_wsv_PARTY_CMD;
	*/
#endif
	} ;
} ;

//-------------------------------------------------------------------------------------------------
#pragma pack (pop)
#pragma warning( default:4201 )
#pragma warning( default:4200 )
#endif	// __NET_PROTOTYPE_H

