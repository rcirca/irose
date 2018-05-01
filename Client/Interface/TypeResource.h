#ifndef _TYPERESOURCE_
#define _TYPERESOURCE_

#include <map>
#include <string>

#include "..\\Common\\IO_STB.h"
#include "../GameCommon/StringManager.h"

// Resource String
#define RES_MINIMAP_ARROW			"3DData\\Control\\Res\\MiniMap_Arrow.tga"






#define STR_RESULT_SELECT_SERVER_FAILED					CStringManager::GetSingleton().GetString( 5 )
#define	STR_RESULT_SELECT_SERVER_FULL					CStringManager::GetSingleton().GetString( 6 )
#define	STR_RESULT_SELECT_SERVER_INVALID_CHANNEL		CStringManager::GetSingleton().GetString( 7 )
#define	STR_RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE		CStringManager::GetSingleton().GetString( 8 )
#define	STR_RESUTL_SELECT_SERVER_INVALID_AGE			CStringManager::GetSingleton().GetString( 9 )
#define STR_CANT_ADDITEM2BUYLIST						CStringManager::GetSingleton().GetString( 10 )
#define STR_FILTER_BADWORD								CStringManager::GetSingleton().GetString( 11 )
#define STR_MSG_IDENTIFY_COST							CStringManager::GetSingleton().GetString( 12 )
#define STR_NOTENOUGH_IDENTIFYCOST						CStringManager::GetSingleton().GetString( 13 )
#define STR_TIMEOUT										CStringManager::GetSingleton().GetString( 14 )
#define STR_MINUTE										CStringManager::GetSingleton().GetString( 15 )
#define STR_CANT_SEPARATE								CStringManager::GetSingleton().GetString( 16 )
#define STR_NOTSELECT_MAKEITEM							CStringManager::GetSingleton().GetString( 17 )
#define STR_NEED_ABILITY								CStringManager::GetSingleton().GetString( 18 )
#define STR_CRAFT_UPGRADE_SUCCESS						CStringManager::GetSingleton().GetString( 19 )
#define STR_CRAFT_UPGRADE_FAILED						CStringManager::GetSingleton().GetString( 20 )
#define STR_ITEM										CStringManager::GetSingleton().GetString( 21 )
#define STR_AVATARINFO									CStringManager::GetSingleton().GetString( 22 )
#define STR_SKILL										CStringManager::GetSingleton().GetString( 23 )
#define STR_QUEST										CStringManager::GetSingleton().GetString( 24 )
#define STR_COMMUNITY									CStringManager::GetSingleton().GetString( 25 )
#define STR_OPTION										CStringManager::GetSingleton().GetString( 26 )
#define STR_PARTY										CStringManager::GetSingleton().GetString( 27 )
#define STR_FAIL_IDENTIFY								CStringManager::GetSingleton().GetString( 28 )
#define STR_CANT_STORE_MONEY2BANK						CStringManager::GetSingleton().GetString( 29 )
#define STR_HOUR										CStringManager::GetSingleton().GetString( 30 )
#define STR_MONEY										CStringManager::GetSingleton().GetString( 31 )
#define STR_QUERY_SELLITEM2PRIVATESTORE					CStringManager::GetSingleton().GetString( 32 )
#define STR_QUERY_BUYITEMFROMPRIVATESTORE				CStringManager::GetSingleton().GetString( 33 )
#define STR_REQUIRE_SUMMONQUANTITY						CStringManager::GetSingleton().GetString( 34 )
#define STR_TOTAL_TRADEMONEY_PRIVATESTORE				CStringManager::GetSingleton().GetString( 35 )
#define STR_CLAN										CStringManager::GetSingleton().GetString( 36 )







#define STR_TOO_NEAR_NPC								CStringManager::GetSingleton().GetString( 40 )
#define STR_RESULT_LOGIN_REPLY_NO_REAL_NAME				CStringManager::GetSingleton().GetString( 41 )
#define	STR_NOT_ENOUGH_MONEY_PRIVATESTORE				CStringManager::GetSingleton().GetString( 42 )
#define	STR_CANTUSE_WARPSCROLL_CURRENTZONE				CStringManager::GetSingleton().GetString( 43 )


#define STR_CLAN_NAME									CStringManager::GetSingleton().GetString( 44 )
#define STR_CLAN_LEV									CStringManager::GetSingleton().GetString( 45 )
#define STR_CLAN_POINT									CStringManager::GetSingleton().GetString( 46 )
#define STR_CLAN_SLOGAN									CStringManager::GetSingleton().GetString( 47 )
#define STR_CLAN_MONEY									CStringManager::GetSingleton().GetString( 48 )
#define STR_CLAN_ALLYED									CStringManager::GetSingleton().GetString( 49 )
#define STR_CLAN_MEMBER_CLASS							CStringManager::GetSingleton().GetString( 50 )
#define STR_CLAN_MEMBER_CONTRIB							CStringManager::GetSingleton().GetString( 51 )
#define STR_CHANGE_TRADEITEM_IN_MYREADY					CStringManager::GetSingleton().GetString( 52 )///1:1 거래중 내가 확인 버튼을 누른 상태에서 상대방의 물품이 변경되었다.
#define STR_CLAN_MEMBER_COUNT							CStringManager::GetSingleton().GetString( 53 )
#define STR_CLAN_CANT_PROMOTE							CStringManager::GetSingleton().GetString( 54 )
#define STR_CLAN_ORGANIZED								CStringManager::GetSingleton().GetString( 55 )	
#define STR_CLAN_RESULT_CLAN_JOIN_OK					CStringManager::GetSingleton().GetString( 56 )	
#define STR_CLAN_RESULT_CLAN_DESTROY_OK					CStringManager::GetSingleton().GetString( 57 )	
#define STR_CLAN_RESULT_CLAN_CREATE_FAILED				CStringManager::GetSingleton().GetString( 58 )	
#define STR_CLAN_RESULT_CLAN_CREATE_DUP_NAME			CStringManager::GetSingleton().GetString( 59 )	
#define STR_CLAN_RESULT_CLAN_CREATE_NO_RIGHT			CStringManager::GetSingleton().GetString( 60 )	
#define STR_CLAN_RESULT_CLAN_DESTROYED					CStringManager::GetSingleton().GetString( 61 )	
#define STR_CLAN_RESULT_CLAN_DESTROY_FAILED				CStringManager::GetSingleton().GetString( 62 )	
#define STR_CLAN_RESULT_CLAN_DESTROY_NO_RIGHT			CStringManager::GetSingleton().GetString( 63 )	
#define STR_CLAN_RESULT_CLAN_JOIN						CStringManager::GetSingleton().GetString( 64 )	
#define STR_CLAN_RESULT_CLAN_JOIN_FAILED				CStringManager::GetSingleton().GetString( 65 )	
#define STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT				CStringManager::GetSingleton().GetString( 66 )	
#define STR_CLAN_RESULT_CLAN_JOIN_HAS_CLAN				CStringManager::GetSingleton().GetString( 67 )	
#define STR_CLAN_RESULT_CLAN_KICK						CStringManager::GetSingleton().GetString( 68 )	
#define STR_CLAN_RESULT_CLAN_KICK_ME					CStringManager::GetSingleton().GetString( 69 )	
#define STR_CLAN_RESULT_CLAN_QUIT						CStringManager::GetSingleton().GetString( 70 )	
#define STR_CLAN_GCMD_INVITE_REQ						CStringManager::GetSingleton().GetString( 71 )	
#define STR_CLAN_GCMD_INVITE_REPLY_NO					CStringManager::GetSingleton().GetString( 72 )	
#define STR_CLAN_RESULT_CLAN_POSITION					CStringManager::GetSingleton().GetString( 73 )	
#define STR_CLAN_CANT_DELETE_CLANMASTER					CStringManager::GetSingleton().GetString( 74 )	
#define STR_CLAN_CANT_QUIT_CLANMASTER					CStringManager::GetSingleton().GetString( 75 )	
#define STR_CLAN_NO_RIGHT								CStringManager::GetSingleton().GetString( 76 )	
#define STR_CLAN_RESULT_CLAN_CREATE_NO_CONDITION		CStringManager::GetSingleton().GetString( 77 )	
#define STR_CLAN_INPUT_SLOGAN							CStringManager::GetSingleton().GetString( 78 )	

#define STR_SKILLPOWER_EFFECT_0							CStringManager::GetSingleton().GetString( 80 )	///계산식에 따른 스킬파워에 영향을 미치는 것
#define STR_SKILLPOWER_EFFECT_1							CStringManager::GetSingleton().GetString( 81 )	
#define STR_SKILLPOWER_EFFECT_2							CStringManager::GetSingleton().GetString( 82 )	
#define STR_SKILLPOWER_EFFECT_3							CStringManager::GetSingleton().GetString( 83 )	

#define STR_CLAN_CANT_ENTRUST_TO_LOW_CLASS				CStringManager::GetSingleton().GetString( 90 )	
#define STR_CLAN_QUERY_ENTRUST							CStringManager::GetSingleton().GetString( 91 )	
#define STR_CLAN_QUERY_BAN								CStringManager::GetSingleton().GetString( 92 )	
#define STR_CLAN_QUERY_PROMOTE							CStringManager::GetSingleton().GetString( 93 )	
#define STR_CLAN_QUERY_DEMOTE							CStringManager::GetSingleton().GetString( 94 )	
#define STR_CLAN_QUERY_DISORGANIZE						CStringManager::GetSingleton().GetString( 95 )	
#define STR_CLAN_CANT_MORE_JOINMEMBER					CStringManager::GetSingleton().GetString( 96 )	
#define STR_CLAN_CANT_DISORGANIZE_MEMBERCOUNT			CStringManager::GetSingleton().GetString( 97 )	
#define STR_CLAN_CREATE_CONDITION						CStringManager::GetSingleton().GetString( 98 )	

////////////////////////////////////////////////////////////////////////////////////
///  전역 스트링 변수들
////////////////////////////////////////////////////////////////////////////////////
#define	STR_ITEM_TYPE						CStringManager::GetSingleton().GetString( 106 )
#define STR_WEIGHT							CStringManager::GetSingleton().GetString( 107 )			//무게
#define STR_ADD_ABILITY						CStringManager::GetSingleton().GetString( 108 )			//추가능력
#define STR_EQUIP_CONDITION					CStringManager::GetSingleton().GetString( 109 )			//장착조건
#define STR_ATTACK_RANGE					CStringManager::GetSingleton().GetString( 110 )			//공격거리
#define STR_ATTACH							CStringManager::GetSingleton().GetString( 111 )			//부착	
#define STR_DEAL_AT_STORE					CStringManager::GetSingleton().GetString( 112 )			//상점거래..		
#define STR_BUY_CATALOG						CStringManager::GetSingleton().GetString( 113 )			//구입목록
#define STR_SELL_CATALOG					CStringManager::GetSingleton().GetString( 114 )			//판매목록
#define STR_TRADE_PVP						CStringManager::GetSingleton().GetString( 115 )			//1:1거래
#define STR_STATUS_DLG						CStringManager::GetSingleton().GetString( 116 )			//상태창
#define STR_INVENTORY_DLG					CStringManager::GetSingleton().GetString( 117 )			//인벤토리창
#define STR_EQUIP_DLG						CStringManager::GetSingleton().GetString( 118 )			//장비창
#define STR_SKILL_DLG						CStringManager::GetSingleton().GetString( 119 )			//기술창
#define STR_QUEST_DLG						CStringManager::GetSingleton().GetString( 120 )			//퀘스트창
#define STR_MOVE_QUICKSLOT_DLG				CStringManager::GetSingleton().GetString( 121 )			//퀵바로 보내기
#define STR_MOVE_INVENTORY_DLG				CStringManager::GetSingleton().GetString( 122 )			//인벤토리창으로 보내기
#define STR_MOVE_EQUIP_DLG					CStringManager::GetSingleton().GetString( 123 )			//장비창으로 보내기
#define STR_STORE							CStringManager::GetSingleton().GetString( 124 )			//상점
#define STR_QUALITY							CStringManager::GetSingleton().GetString( 125 )			//품질
#define STR_HPRECOVERY						CStringManager::GetSingleton().GetString( 126 )			//HP회복
#define STR_NOT_ENOUGH_MANA					CStringManager::GetSingleton().GetString( 127 )			// 마나가 부족합니다.
#define STR_INVALID_TARGET					CStringManager::GetSingleton().GetString( 128 )			// 타겟이 유효하지 않습니다.
#define STR_WEAPON_MISMATCH					CStringManager::GetSingleton().GetString( 129 )			// 필요한 무기가 장비 되어 있지 않습니다.
#define STR_GET_EXP							CStringManager::GetSingleton().GetString( 130 )			// 경험치 %d을 얻었습니다.
#define STR_NOTIFY_01						CStringManager::GetSingleton().GetString( 131 )			// 더이상 아이템을 갖을수 없습니다.
#define STR_NOTIFY_02						CStringManager::GetSingleton().GetString( 132 )			// 소유권이 없습니다. 잠시후 획득하세요.
#define STR_NOTIFY_03						CStringManager::GetSingleton().GetString( 133 )			// <<공지 사항>>
#define STR_NOTIFY_04						CStringManager::GetSingleton().GetString( 134 )			// 현재 사용 대기중입니다.
#define STR_NOTIFY_05						CStringManager::GetSingleton().GetString( 135 )			// 유효하지 않은 타겟입니다.
#define STR_NOTIFY_06						CStringManager::GetSingleton().GetString( 136 )			// 조건이 맞지 않아 사용할수 없습니다.
#define ID_STR_DROP_MONEY					CStringManager::GetSingleton().GetString( 137 )			// 돈은 버리시겠습니까?
#define	ID_STR_DROP_ITEM					CStringManager::GetSingleton().GetString( 138 )			// 아이템을 버리시겠습니까?
#define STR_USE_REQUIRE						CStringManager::GetSingleton().GetString( 169 )			// 사용조건
#define STR_EQUIP_REQUIRE_JOB				CStringManager::GetSingleton().GetString( 170 )			// 장착직업
#define STR_MOVE_SPEED						CStringManager::GetSingleton().GetString( 171 )			// 이동속도
#define STR_CARRY_WEIGHT					CStringManager::GetSingleton().GetString( 172 )			// 휴대무게


#define FORMAT_STR_CANCEL_BLOCK_CHAT				CStringManager::GetSingleton().GetString( 173 )
#define	STR_CANCEL_BLOCK_CHAT						CStringManager::GetSingleton().GetString( 174 )
#define	FORMAT_STR_BLOCK_CHAT						CStringManager::GetSingleton().GetString( 175 )
#define STR_GM_BAN_USER								CStringManager::GetSingleton().GetString( 176 )
#define	FORMAT_STR_GM_BAN_USER						CStringManager::GetSingleton().GetString( 177 )
#define FORMAT_STR_RECEIVE_TRADE_REQ				CStringManager::GetSingleton().GetString( 178 ) 
#define STR_OTHER_CANCEL_TRADE						CStringManager::GetSingleton().GetString( 179 )
#define STR_NOT_ENOUGH_OTHER_INVENTORY_SPACE		CStringManager::GetSingleton().GetString( 180 )
#define STR_OTHER_CANT_RECEIVE_TRADE_REQ			CStringManager::GetSingleton().GetString( 181 )
#define STR_OTHER_REJECT_TRADE_REQ					CStringManager::GetSingleton().GetString( 182 )
#define STR_OTHER_IS_TOO_FAR_TO_TRADE				CStringManager::GetSingleton().GetString( 183 )
#define STR_NOT_FOUND_TRADE_OTHER					CStringManager::GetSingleton().GetString( 184 )
#define STR_STATE_BLOCK_CHAT						CStringManager::GetSingleton().GetString( 185 )
#define STR_REQ_TRADE								CStringManager::GetSingleton().GetString( 186 )
#define STR_NOT_ENOUGH_INVENTORY_SPACE				CStringManager::GetSingleton().GetString( 187 )
#define FORMAT_STR_FAIL_WHISPER						CStringManager::GetSingleton().GetString( 188 )
#define FORMAT_STR_WHISPER							CStringManager::GetSingleton().GetString( 189 )
///*-------------------------------------------------------------------------------*/
/// 로그인 관련 
#define STR_WAIT_LOGIN								CStringManager::GetSingleton().GetString( 190 )
#define STR_COMMON_ERROR							CStringManager::GetSingleton().GetString( 191 )
#define STR_NOT_FOUND_ACCOUNT						CStringManager::GetSingleton().GetString( 192 )
#define	STR_INVALID_PASSWORD						CStringManager::GetSingleton().GetString( 193 )
#define STR_ALREADY_LOGGEDIN						CStringManager::GetSingleton().GetString( 194 )
#define	STR_REFUSED_ACCOUNT							CStringManager::GetSingleton().GetString( 195 )
#define	STR_LOGIN_FAIL								CStringManager::GetSingleton().GetString( 196 )
#define STR_INSPECT_ALL_SEVER						CStringManager::GetSingleton().GetString( 197 )
#define	STR_WAIT_CONNECT_LOGIN_SERVER				CStringManager::GetSingleton().GetString( 198 )
#define	STR_NOT_ENOUGH_EQUIP_SPACE					CStringManager::GetSingleton().GetString( 199 )
#define	STR_EQUIPED_TWO_HAND_WEAPON					CStringManager::GetSingleton().GetString( 200 )

///아이템 사용 제한에 관한 스트링 
#define STR_DONT_SELL_ITEM							CStringManager::GetSingleton().GetString( 201 )	/// 팔수 없는 아이템입니다.
#define STR_DONT_DROP_ITEM							CStringManager::GetSingleton().GetString( 202 ) /// 버릴수 없는 아이템입니다.
#define STR_DONT_EXCHANGE_ITEM						CStringManager::GetSingleton().GetString( 203 ) /// 교환할수 없는 아이템입니다.

/// 스킬 습득에 관련한 스트링
#define STR_LEARN_SKILL_FAILED						CStringManager::GetSingleton().GetString( 204 ) /// 스킬 배우기 실패.
#define STR_LEARN_SKILL_JOB_FAILED					CStringManager::GetSingleton().GetString( 205 ) /// 직업이 맞지 않습니다.
#define STR_LEARN_SKILL_NEED_PRESKILL_FAILED		CStringManager::GetSingleton().GetString( 206 ) /// 필요 스킬이 없습니다.
#define STR_LEARN_SKILL_NEED_ABILITY_FAILED			CStringManager::GetSingleton().GetString( 207 ) /// 능력치가 부족합니다.
#define STR_LEARN_SKILL_SLOT_FULL_FAILED			CStringManager::GetSingleton().GetString( 208 ) /// 더이상 스킬을 배울수 없습니다.
#define STR_LEARN_SKILL_INVALID_SKILL_FAILED		CStringManager::GetSingleton().GetString( 209 ) /// 잘못된 스킬번호 입니다.

/// 명령, 스킬 사용시
#define STR_ACTION_COMMAND_STOP_STATE_FAILED		CStringManager::GetSingleton().GetString( 210 ) /// 정지상태 에서만 사용가능합니다.





///파티 관련 
#define FORMAT_STR_PARTY_MAKE_REQ					CStringManager::GetSingleton().GetString( 211 ) ///%s님이 파티 결성을 요청했습니다
#define FORMAT_STR_PARTY_INVITE						CStringManager::GetSingleton().GetString( 212 ) ///%s님이 파티에 초대하였습니다
#define FORMAT_STR_PARTY_CHANGE_LEADER				CStringManager::GetSingleton().GetString( 213 ) ///%s님이 파티의 리더가 되었습니다
#define	STR_PARTY_ENTRUSTED_LEADER					CStringManager::GetSingleton().GetString( 214 ) ///파티의 리더로 위임 받았습니다
#define STR_PARTY_BANNED							CStringManager::GetSingleton().GetString( 215 ) ///파티에서 강제 퇴장 당했습니다
#define FORMAT_STR_PARTY_BANNED						CStringManager::GetSingleton().GetString( 216 ) ///%s님이 파티에서 강제 퇴장 당했습니다
#define STR_PARTY_NOTFOUND_DEST						CStringManager::GetSingleton().GetString( 217 ) ///대상을 찾을수 없습니다
#define FORMAT_STR_PARTY_BUSY						CStringManager::GetSingleton().GetString( 218 ) ///%s님은 지금 요청을 받을 수 없는 상태입니다
#define STR_PARTY_MADE								CStringManager::GetSingleton().GetString( 219 ) ///새로운 파티가 결성되었습니다
#define FORMAT_STR_PARTY_REJECT_INVITE				CStringManager::GetSingleton().GetString( 220 ) ///%s님이 파티 요청을 거부했습니다
#define FORMAT_STR_PARTY_LEAVE						CStringManager::GetSingleton().GetString( 221 ) ///%s님이 파티에서 탈퇴했습니다
#define FORMAT_STR_PARTY_JOIN						CStringManager::GetSingleton().GetString( 222 ) ///%s님이 파티에 추가되었습니다

#define FORMAT_STR_PARTY_QUERY_ENTRUST				CStringManager::GetSingleton().GetString( 223 ) ///%s님을 파티리더로 위임하시겠습니까?
#define STR_PARTY_QUERY_LEAVE						CStringManager::GetSingleton().GetString( 224 ) ///파티에서 탈퇴하겠습니까?
#define FORMAT_STR_PARTY_QUERY_BAN					CStringManager::GetSingleton().GetString( 225 ) ///%s님을 강제로 퇴장 시키겠습니까?
#define FORMAT_STR_PARTY_LEVEL						CStringManager::GetSingleton().GetString( 226 ) ///파티레벨 %d

#define STR_SKILL_APPLY_FAILED						CStringManager::GetSingleton().GetString( 227 ) ///스킬 적용에 실패 했습니다.
#define STR_UNKNOWN_STATE_FLAG						CStringManager::GetSingleton().GetString( 228 ) ///알수 없는 상태 플래그.

#define STR_SEND									CStringManager::GetSingleton().GetString( 230 ) 
#define STR_DELETE									CStringManager::GetSingleton().GetString( 231 ) 

#define STR_CANT_CASTING_STATE						CStringManager::GetSingleton().GetString( 245 ) ///스킬을 사용할수 없는 상태입니다..
#define STR_NOT_ENOUGH_PROPERTY						CStringManager::GetSingleton().GetString( 246 ) ///필요한 에너지가 부족합니다.
#define STR_MISMATCH_CASTING_NEED_EQUIP				CStringManager::GetSingleton().GetString( 247 ) ///필요 장비가 부족합니다.

#define STR_DOING_SKILL_ACTION						CStringManager::GetSingleton().GetString( 248 ) ///필요 장비가 부족합니다.
#define STR_NOTIFY_CANT_DUPLICATE					CStringManager::GetSingleton().GetString( 249 ) ///중복 젹용 불가 스킬

#define STR_CANT_OPEN_UNIONSTORE					CStringManager::GetSingleton().GetString( 250 ) ///
#define STR_UNIONPOINT								CStringManager::GetSingleton().GetString( 251 ) ///조합포인트
#define STR_WANT_UNIONPOINT							CStringManager::GetSingleton().GetString( 252 ) ///조합포인트가 부족하다.
#define STR_ALLIED									CStringManager::GetSingleton().GetString( 253 ) ///조합포인트가 부족하다.


#define STR_CANT_EXITGAME							CStringManager::GetSingleton().GetString( 260 ) 
#define STR_WAITTIME_EXITGAME						CStringManager::GetSingleton().GetString( 261 ) 
#define STR_AVATARSTORE_CANT_SELL_OPTION			CStringManager::GetSingleton().GetString( 262 ) 
#define STR_AVATARSTORE_CANT_SELL_UPGRADE			CStringManager::GetSingleton().GetString( 263 ) 
#define STR_AVATARSTORE_CANT_SELL_DURABILITY		CStringManager::GetSingleton().GetString( 264 ) 
#define STR_AVATARSTORE_CANT_SELL_SOCKET			CStringManager::GetSingleton().GetString( 265 ) 
#define STR_AVATARSTORE_CANT_SELL_LIFE				CStringManager::GetSingleton().GetString( 266 ) 

#define STR_FUEL									CStringManager::GetSingleton().GetString( 270 ) 
#define STR_WASTE_FUEL								CStringManager::GetSingleton().GetString( 271 ) 
#define STR_RECOVERY_EXP							CStringManager::GetSingleton().GetString( 272 ) 
#define STR_SUMMON_CAPACITY							CStringManager::GetSingleton().GetString( 273 )


#define STR_NPC_TRADE_PRICE							CStringManager::GetSingleton().GetString( 275 )

#define STR_TARGET									CStringManager::GetSingleton().GetString( 307 )
#define STR_SUMMON_MOB								CStringManager::GetSingleton().GetString( 308 ) ///소환몹
#define STR_SHOOT_RANGE								CStringManager::GetSingleton().GetString( 309 ) ///사정거리
#define STR_APPLY_RANGE								CStringManager::GetSingleton().GetString( 310 ) ///적용범위
#define STR_RADIUS									CStringManager::GetSingleton().GetString( 311 ) ///반경
#define STR_EFFECT									CStringManager::GetSingleton().GetString( 312 ) ///효과
#define STR_LEVEL									CStringManager::GetSingleton().GetString( 313 ) ///레벨
#define STR_CONTINUE_TIME							CStringManager::GetSingleton().GetString( 314 ) ///지속시간
#define STR_SECOND									CStringManager::GetSingleton().GetString( 315 ) ///초
#define STR_NEXT_LEVEL								CStringManager::GetSingleton().GetString( 316 ) ///다음레벨
#define STR_SKILL_POWER								CStringManager::GetSingleton().GetString( 317 ) ///스킬파워
#define STR_SUCCESS_RATE							CStringManager::GetSingleton().GetString( 318 ) ///성공률
#define STR_CONSUME_ABILITY							CStringManager::GetSingleton().GetString( 319 ) ///소모능력
#define STR_REQUIRE_EQUIP							CStringManager::GetSingleton().GetString( 320 ) ///필요장비
#define STR_REQUIRE_JOB								CStringManager::GetSingleton().GetString( 321 ) ///사용직업
#define STR_REQUIRE_SKILL							CStringManager::GetSingleton().GetString( 322 ) ///필수스킬
#define STR_REQUIRE_ABILITY							CStringManager::GetSingleton().GetString( 323 ) ///조건능력


#define	STR_START_MAKE_ITEM							CStringManager::GetSingleton().GetString( 324 ) ///아이템 제조 시작
#define STR_NOT_ENOUGH_MATERIAL						CStringManager::GetSingleton().GetString( 325 ) ///재료가 부족합니다
#define STR_NOT_EXIST_MATERIAL						CStringManager::GetSingleton().GetString( 326 ) ///재료가 없습니다.
#define	STR_SUCCESS_MAKE_ITEM						CStringManager::GetSingleton().GetString( 327 ) ///아이템 제조에 성공했습니다
#define STR_FAIL_MAKE_ITEM							CStringManager::GetSingleton().GetString( 328 ) ///아이템 제조에 실패했습니다
#define STR_NOT_ENOUGH_MAKE_SKILL_LEVEL				CStringManager::GetSingleton().GetString( 329 ) ///제조 스킬 레벨이 부족합니다



///<- 2004 / 3 / 4 : 추가 nAvy
#define STR_BANK_NOT_ENOUGH_MONEY					CStringManager::GetSingleton().GetString( 330 ) ///보관료가 부족해서 더 이상 보관할 수 없습니다
#define STR_BANK_CANT_STORE_ITEM					CStringManager::GetSingleton().GetString( 331 ) ///보관할 수 없는 아이템입니다
#define STR_BANK_NOT_ENOUGH_SPACE					CStringManager::GetSingleton().GetString( 332 ) ///보관할 여유 공간이 없습니다

#define STR_BANK_STORAGE_FEE						CStringManager::GetSingleton().GetString( 333 ) ///보관료
#define STR_BANK_CANT_STORE							CStringManager::GetSingleton().GetString( 334 ) ///보관불가
#define STR_CANT_SELL								CStringManager::GetSingleton().GetString( 335 ) ///판매불가
#define STR_PRICE									CStringManager::GetSingleton().GetString( 336 ) ///가격
#define STR_SELL_PRICE								CStringManager::GetSingleton().GetString( 337 ) ///판매가격
#define STR_REQUIRE_POINT							CStringManager::GetSingleton().GetString( 338 ) ///필요포인트
#define STR_USING_MAKESKILL							CStringManager::GetSingleton().GetString( 339 ) ///제조스킬을 사용중입니다.
#define STR_CHANGE_PRICES							CStringManager::GetSingleton().GetString( 340 ) ///물가가 변동되었습니다.
#define STR_QUERY_STOP_TRADE						CStringManager::GetSingleton().GetString( 341 ) ///거래를 중단하시겠습니까?
#define STR_WHISPER									CStringManager::GetSingleton().GetString( 342 ) ///귓속말
#define STR_CANT_JOIN_PARTY_REASON_LEVEL			CStringManager::GetSingleton().GetString( 343 ) ///님은 레벨이 맞지 않아 파티에 들어올 수 없습니다
#define STR_BANK									CStringManager::GetSingleton().GetString( 344 ) ///창고
#define STR_NOT_ENOUGH_MONEY						CStringManager::GetSingleton().GetString( 345 ) ///돈이 부족합니다.
#define STR_ITEM_IN_TRADE_LIST						CStringManager::GetSingleton().GetString( 346 ) ///판매목록에 올려진 아이템입니다
#define STR_CANT_USEITEM_REASON_SYNCOPE				CStringManager::GetSingleton().GetString( 347 ) ///기절한 상태에서는 사용할수 없습니다
#define STR_INVALID_CHARACTER_NAME					CStringManager::GetSingleton().GetString( 348 ) ///이름에 허용되지 않는 글자가 있습니다
#define STR_FAIL_MAKE_NEW_CHARACTER					CStringManager::GetSingleton().GetString( 349 ) ///캐릭터 생성 실패
#define STR_REQUEST_SELECT_CHARACTER				CStringManager::GetSingleton().GetString( 350 ) ///캐릭터를 선택해 주세요
#define STR_QUERY_DELETE_CHARACTER					CStringManager::GetSingleton().GetString( 351 ) ///캐릭터를 지우시겠습니까?
#define STR_RECEIVE_CHARACTER_LIST					CStringManager::GetSingleton().GetString( 352 ) ///아바타 리스트를 받습니다
#define STR_SHOUT									CStringManager::GetSingleton().GetString( 353 ) ///외침
#define STR_FAIL_SKILL_LEVELUP						CStringManager::GetSingleton().GetString( 354 ) ///스킬 레벨업이 실패했습니다
#define STR_NOT_ENOUGH_SKILLPOINT					CStringManager::GetSingleton().GetString( 355 ) ///스킬포인트가 모자랍니다


#define TT_PARTY_ENTRUST							CStringManager::GetSingleton().GetString( 357 ) ///파티장 위임
#define TT_PARTY_LEAVE								CStringManager::GetSingleton().GetString( 358 ) ///파티 탈퇴
#define TT_PARTY_BAN								CStringManager::GetSingleton().GetString( 359 ) ///강퇴

#define TT_AVATAR_MENU_BTN_EXCHANGE					CStringManager::GetSingleton().GetString( 360 ) ///"교환 신청" 버튼
#define TT_AVATAR_MENU_BTN_PARTY					CStringManager::GetSingleton().GetString( 361 ) ///"파티 신청" 버튼
///->


#define TT_MINIMAP_BTN_ZOOM_MAX						CStringManager::GetSingleton().GetString( 139 )
#define TT_MINIMAP_BTN_ZOOM_MIN						CStringManager::GetSingleton().GetString( 140 )
#define TT_MINIMAP_BTN_SHOW_SHOW					CStringManager::GetSingleton().GetString( 141 )
#define TT_MINIMAP_BTN_SHOW_HIDE					CStringManager::GetSingleton().GetString( 142 )
#define TT_BASE_BTN_CHAR							CStringManager::GetSingleton().GetString( 143 )
#define TT_BASE_BTN_INVENTORY						CStringManager::GetSingleton().GetString( 144 )
#define TT_BASE_BTN_SKILL							CStringManager::GetSingleton().GetString( 145 )
#define TT_BASE_BTN_QUEST							CStringManager::GetSingleton().GetString( 146 )
#define TT_BASE_BTN_COMMUNITY						CStringManager::GetSingleton().GetString( 147 )
#define TT_BASE_BTN_OPTION							CStringManager::GetSingleton().GetString( 148 )
#define TT_BASE_BTN_SYSTEM							CStringManager::GetSingleton().GetString( 149 )
#define TT_CHAT_BTN_MAXIMIZE						CStringManager::GetSingleton().GetString( 150 )
#define TT_CHAT_BTN_MINIMIZE						CStringManager::GetSingleton().GetString( 151 )
#define TT_CHAT_BTN_FILTER							CStringManager::GetSingleton().GetString( 152 )
#define TT_QUICKBAR_BTN_NEXT						CStringManager::GetSingleton().GetString( 153 )
#define TT_QUICKBAR_BTN_ROTATE						CStringManager::GetSingleton().GetString( 154 )
#define TT_SKILL_BTN_LEVELUP						CStringManager::GetSingleton().GetString( 155 )
#define TT_QUEST_BTN_ABANDON						CStringManager::GetSingleton().GetString( 156 )
#define TT_ACTIVE_SKILL								CStringManager::GetSingleton().GetString( 157 )
#define TT_SLOT_HELMET								CStringManager::GetSingleton().GetString( 158 )
#define TT_SLOT_FACEITEM							CStringManager::GetSingleton().GetString( 159 )
#define TT_SLOT_ARMOR								CStringManager::GetSingleton().GetString( 160 )
#define TT_SLOT_GAUNTLET							CStringManager::GetSingleton().GetString( 161 )
#define TT_SLOT_BOOTS								CStringManager::GetSingleton().GetString( 162 )
#define TT_SLOT_KNAPSACK							CStringManager::GetSingleton().GetString( 163 )
#define TT_SLOT_WEAPON_R							CStringManager::GetSingleton().GetString( 164 )
#define TT_SLOT_WEAPON_L							CStringManager::GetSingleton().GetString( 165 )
#define TT_SLOT_RING								CStringManager::GetSingleton().GetString( 166 )
#define TT_SLOT_NECKLACE							CStringManager::GetSingleton().GetString( 167 )
#define TT_SLOT_EARING								CStringManager::GetSingleton().GetString( 168 )



#define STR_SKILL_TARGET_FILTER_NOTIFY_01			CStringManager::GetSingleton().GetString( 363 )			/// 같은 그룹원이 아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_02			CStringManager::GetSingleton().GetString( 364 )			/// 같은 길드원이 아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_03			CStringManager::GetSingleton().GetString( 365 )			/// 우호적인 플레이어가 아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_04			CStringManager::GetSingleton().GetString( 366 )			/// 몬스터가 아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_05			CStringManager::GetSingleton().GetString( 367 )			/// 적대관계의 플레이어가 아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_06			CStringManager::GetSingleton().GetString( 368 )			/// 플레이어가  아닙니다.
#define STR_SKILL_TARGET_FILTER_NOTIFY_07			CStringManager::GetSingleton().GetString( 369 )			/// 적용할수 있는 대상이 아닙니다.

#define STR_NOT_ENOUGH_CONDITION					CStringManager::GetSingleton().GetString( 370 )			/// 조건이 맞지 않아 사용할수 없습니다.
#define STR_SO_FAR_DISTANCE							CStringManager::GetSingleton().GetString( 371 )			/// 너무 멀리 떨어져 있습니다.
#define STR_CANT_SKILL_IN_AGIT						CStringManager::GetSingleton().GetString( 372 )			/// 아지트에선 사용할수 없습니다.


/*
371~380 아이템 타입 추가 및 예약
*/


///
/// PAT
///
#define STR_ERROR_NOT_ASSEMBLED						CStringManager::GetSingleton().GetString( 381 )			/// 조합이 되어있지 않습니다.
#define STR_PAT_ERROR_NOT_EQUIP_BODY				CStringManager::GetSingleton().GetString( 382 )			/// 바디부품을 먼저 장착하세요
#define STR_PAT_ERROR_NOT_EQUAL_CLASS				CStringManager::GetSingleton().GetString( 383 )			/// 바디와 부품종류가 틀립니다
#define STR_PAT_ERROR_NOT_VERSION					CStringManager::GetSingleton().GetString( 384 )			/// 부품의 버전수치가 바디보다 높습니다
#define STR_CLAN_LEVELUP							CStringManager::GetSingleton().GetString( 385 )	


///#define STR_CANT_CASTING_STATE_CART					CStringManager::GetSingleton().GetString( 456 )			/// 탑승중에는 시전할수 없습니다.


///386 ~ 407 : 능력속성이름 



/// 추가 경고 메세지들
#define STR_CANT_SUMMON_NPC							CStringManager::GetSingleton().GetString( 411 )
#define STR_CANT_DOUBLE_USE							CStringManager::GetSingleton().GetString( 412 )


/// 재밍 관련
#define STR_GEMMING_SUCCESS							CStringManager::GetSingleton().GetString( 413 )
#define STR_GEMMING_NEED_SOCKET						CStringManager::GetSingleton().GetString( 414 )

#define F_STR_SUMMON_TERMINATED						CStringManager::GetSingleton().GetString( 415 )//소환수 %s소멸
#define F_STR_GETTING_MONEY							CStringManager::GetSingleton().GetString( 416 )//돈 %원을 획득하였습니다
#define F_STR_GETTING_ITEMS							CStringManager::GetSingleton().GetString( 417 )//%s %d개 획득하였습니다
#define F_STR_GETTING_ITEM							CStringManager::GetSingleton().GetString( 418 )//%s 획득하였습니다
#define F_STR_QUEST_GETTING_MONEY					CStringManager::GetSingleton().GetString( 419 )//돈 %d원을 얻었습니다
#define F_STR_QUEST_GETTING_ITEMS					CStringManager::GetSingleton().GetString( 420 )//%s %d개를 얻었습니다
#define F_STR_QUEST_GETTING_ITEM					CStringManager::GetSingleton().GetString( 421 )//%s을 얻었습니다
#define F_STR_QUEST_SUBTRACT_MONEY					CStringManager::GetSingleton().GetString( 422 )//돈 %d원을 가져갔습니다
#define F_STR_QUEST_SUBTRACT_ITEMS					CStringManager::GetSingleton().GetString( 423 )//%s %d개를 가져갔습니다
#define F_STR_QUEST_SUBTRACT_ITEM					CStringManager::GetSingleton().GetString( 424 )//%s를 가져갔습니다
#define F_STR_SET_ABILITY							CStringManager::GetSingleton().GetString( 425 )//%s 능력치가 %d으로 변경되었습니다
#define F_STR_INCREASE_ABILITY						CStringManager::GetSingleton().GetString( 426 )//%s 능력치가 %d 상승했습니다
#define STR_CANT_USE_DRIVESKILL						CStringManager::GetSingleton().GetString( 427 )//드라이브 카트 스킬을 사용 할 수 없습니다
#define STR_PARTY_IS_FULL							CStringManager::GetSingleton().GetString( 428 )//더 이상 인원을 추가할 수 없습니다
#define STR_CANT_INVITE_TO_PARTY					CStringManager::GetSingleton().GetString( 429 )//파티초대는 파티리더만이 가능합니다
#define STR_REQUIRE_IDENTIFY						CStringManager::GetSingleton().GetString( 430 )//[아이템감정 필요)

#define STR_UNIT_PRICE								CStringManager::GetSingleton().GetString( 431 )//단가
#define STR_REPAIR_PRICE							CStringManager::GetSingleton().GetString( 432 )//수리비
#define STR_LIFE									CStringManager::GetSingleton().GetString( 433 )//수명
#define STR_DURABILITY								CStringManager::GetSingleton().GetString( 434 )//내구도
#define STR_NORMAL									CStringManager::GetSingleton().GetString( 435 )//보통
#define STR_FASTER									CStringManager::GetSingleton().GetString( 436 )//빠름
#define STR_SLOWER									CStringManager::GetSingleton().GetString( 437 )//느림
#define STR_CANT_EQUIP_USING_DRIVESKILL				CStringManager::GetSingleton().GetString( 438 )//PAT사용시에는 아이템을 장착할수 없습니다
#define STR_CANT_UNEQUIP_USING_DRIVESKILL			CStringManager::GetSingleton().GetString( 439 )//PAT사용시에는 아이템을 탈착할수 없습니다
#define STR_CANT_UNEQUIP_BULLET_USING_DRIVESKILL	CStringManager::GetSingleton().GetString( 440 )//PAT사용시에는 소모탄을 탈착할수 없습니다
#define STR_CANT_SUMMON_IN_PVPZONE					CStringManager::GetSingleton().GetString( 441 )//PVP 존에서는 소환몹을 소환할수 없습니다
#define STR_CANT_USESKILL_USING_DRIVESKILL			CStringManager::GetSingleton().GetString( 442 )//탑승중에는 시전할수 없습니다
#define STR_REMAIN_REMOVE_WAITTIME					CStringManager::GetSingleton().GetString( 443 )//남은 삭제대기시간
#define STR_DAY										CStringManager::GetSingleton().GetString( 444 )//일
#define STR_CANT_OPEN_PRIVATESTORE					CStringManager::GetSingleton().GetString( 445 )//개인상점을 개설 할 수 없습니다
#define STR_PRIVATESTORE							CStringManager::GetSingleton().GetString( 446 )//개인상점
#define STR_NONAME									CStringManager::GetSingleton().GetString( 447 )//이름없음
#define STR_ITEM_CANT_SEPARATED						CStringManager::GetSingleton().GetString( 448 )//분리/분해 할 수 없는 아이템입니다
#define STR_ITEM_CANT_UPGRADED						CStringManager::GetSingleton().GetString( 449 )//제련 할 수 없는 아이템입니다


/// 450 ~ 480 : 직업 이름

#define F_STR_QUERY_DROP_ITEM						CStringManager::GetSingleton().GetString( 481 )//%s를 버리시겠습니까?
#define TT_SELFTARGETING							CStringManager::GetSingleton().GetString( 482 )//클릭시 본인 타겟팅
#define STR_DUPLICATED_FRIENDNAME					CStringManager::GetSingleton().GetString( 483 )//친구목록에 이미 있는 이름입니다
#define F_STR_BANK_TITLE							CStringManager::GetSingleton().GetString( 484 )//%s'의 창고
#define F_STR_QUERY_DELETE_FRIEND					CStringManager::GetSingleton().GetString( 485 )//%s를 친구목록에서 삭제하시겠습니까?
#define F_STR_RESTRICT_SHOUT						CStringManager::GetSingleton().GetString( 486 )//외치기는 %d초에 한번만 사용할수 있습니다
#define STR_HELP									CStringManager::GetSingleton().GetString( 487 )//도움말
#define STR_CHANGED_PARTY_CONFIG					CStringManager::GetSingleton().GetString( 488 )//파티 설정이 변경되었습니다
#define STR_ITEM_PICKUP_RIGHT						CStringManager::GetSingleton().GetString( 489 )//아이템우선권
#define STR_PARTY_ITEM_PICKUP_SEQENCE				CStringManager::GetSingleton().GetString( 490 )//순서대로 획득
#define STR_PARTY_ITEM_PICKUP_RIGHT_ALL				CStringManager::GetSingleton().GetString( 491 )//파티원 전원 부여
#define STR_PARTY_EXP_LEVELRATE						CStringManager::GetSingleton().GetString( 492 )//레벨비례 분배
#define STR_PARTY_EXP_EQUALITY						CStringManager::GetSingleton().GetString( 493 )//균등하게 분배
#define STR_MYSTATE_IN_HUNT							CStringManager::GetSingleton().GetString( 494 )//사냥중
#define STR_MYSTATE_IN_TRADE						CStringManager::GetSingleton().GetString( 495 )//장사중
#define STR_MYSTATE_IN_QUEST						CStringManager::GetSingleton().GetString( 496 )//퀘스트중
#define STR_MYSTATE_IN_EAT							CStringManager::GetSingleton().GetString( 497 )//식사중
#define STR_MYSTATE_IN_BREAK						CStringManager::GetSingleton().GetString( 498 )//휴식중
#define STR_NOT_FOUND_FRIEND						CStringManager::GetSingleton().GetString( 499 )//상대방을 친구목록에서 찾을수 없습니다
#define STR_DEFAULT_PRIVATESTORE_TITLE				CStringManager::GetSingleton().GetString( 500 )//%s' 개인상점

#define F_STR_QUERY_ABANDON_QUEST					CStringManager::GetSingleton().GetString( 501 )//%s 퀘스트를 포기하겠습니까?
#define STR_SEPARATE_EMPTY_TARGETITEM				CStringManager::GetSingleton().GetString( 502 )//분리/분해할 아이템이 없습니다
#define STR_NOTENOUGH_EMPTY_INVENTORYSLOT			CStringManager::GetSingleton().GetString( 503 )//인벤토리에 여유공간이 없습니다
#define F_STR_FRIEND_DELETED						CStringManager::GetSingleton().GetString( 504 )//%s이 나를 친구 목록에서 제외시켰습니다
#define F_STR_MESSANGER_BLOCKED						CStringManager::GetSingleton().GetString( 505 )//%s이 대화를 차단하였습니다
#define STR_REQUIRE_SKILLPOINT						CStringManager::GetSingleton().GetString( 506 )//필요 스킬포인트
#define STR_CANT_LEVELUP							CStringManager::GetSingleton().GetString( 507 )//더이상 레벨업이 되지 않습니다
#define STR_CANT_REPAIR_DURABILITY_IS_ZERO			CStringManager::GetSingleton().GetString( 508 )//내구도가 없어서 수리가 불가능합니다
//#define STR_INVALID_CHARACTER_NAME					CStringManager::GetSingleton().GetString( 509 )//이름에 허용되지 않는 글자가 있습니다
#define STR_LOGIN_REPLY_TOO_MANY_USER				CStringManager::GetSingleton().GetString( 510 )//최대 접속인원이 넘었습니다. 잠시후에 다시 이용해주시기 바랍니다
#define STR_LOGIN_REPLY_NO_RIGHT_TO_CONNECT			CStringManager::GetSingleton().GetString( 511 )//로즈온라인은 9월에 오픈 베타 테스트를 시작할 예정으로 현재는 접속하실수 없습니다
#define STR_QUERY_REVIVE_CHARACTER					CStringManager::GetSingleton().GetString( 512 )//캐릭터를 부활시키겠습니까?
#define STR_CANT_CREATE_MORE_CHARACTER				CStringManager::GetSingleton().GetString( 513 )//캐릭터를 더 이상 생성 할 수 없습니다
#define STR_ABSORPTION								CStringManager::GetSingleton().GetString( 514 )//흡수
#define STR_CHANGE_ABILITY							CStringManager::GetSingleton().GetString( 515 )//변경능력치
#define STR_STATE									CStringManager::GetSingleton().GetString( 516 )//상태
#define F_STR2_CHAT_BLOCKED_BY_GM					CStringManager::GetSingleton().GetString( 517 )//GM[%s)이 %d분간 채팅금지 시켰습니다
#define STR_OTHER_WHISPER_REJECT_STATE				CStringManager::GetSingleton().GetString( 518 )//상대방은 현재 귓속말 거부상태입니다
#define STR_WEIGHT_RATE								CStringManager::GetSingleton().GetString( 519 )//무게비율
#define STR_NO_PENALTY								CStringManager::GetSingleton().GetString( 520 )//제한없음

#define STR_CANT_ATTACK								CStringManager::GetSingleton().GetString( 521 )//공격 불가
#define STR_CANT_RUN								CStringManager::GetSingleton().GetString( 522 )//뛰기 금지
#define F_STR_SUCCESS_HUNT							CStringManager::GetSingleton().GetString( 523 )//%s 사냥에 성공했습니다
#define F_STR_WIN_PVP								CStringManager::GetSingleton().GetString( 524 )//%s와의 결투에서 이겼습니다
#define STR_SHIELD_COUNTERATTACK					CStringManager::GetSingleton().GetString( 525 )//방패 반격
#define STR_DROP_ITEM								CStringManager::GetSingleton().GetString( 526 )//아이템 드롭
#define STR_DROP_MONEY								CStringManager::GetSingleton().GetString( 527 )//돈 드롭
#define F_STR_SKILL_LEARNED							CStringManager::GetSingleton().GetString( 528 )//스킬 %s을 배웠습니다
#define STR_STORE_IS_CLOSED							CStringManager::GetSingleton().GetString( 529 )//상점이 닫혔습니다
#define STR_TRADE_IS_CANCEL							CStringManager::GetSingleton().GetString( 530 )//거래가 취소되었습니다
#define STR_ITEM_IS_SOLD							CStringManager::GetSingleton().GetString( 531 )//이미 팔린 아이템입니다
#define STR_ITEM_IS_BUY_ALL							CStringManager::GetSingleton().GetString( 532 )//더이상 사지 않는 아이템입니다

#define STR_BANK_REPLY_NEED_PASSWORD				CStringManager::GetSingleton().GetString( 533 )//창고 비밀번호를 입력하세요
#define STR_BANK_REPLY_CHANGED_PASSWORD				CStringManager::GetSingleton().GetString( 534 )//창고 비밀번호가 변경되었습니다
#define STR_BANK_REPLY_INVALID_PASSWORD				CStringManager::GetSingleton().GetString( 535 )//창고 비밀번호가 틀렸습니다
#define STR_BANK_REPLY_CLEARED_PASSWORD				CStringManager::GetSingleton().GetString( 536 )//창고 비밀번호가 삭제되었습니다
#define STR_BANK_MOVEITEM_FAILED					CStringManager::GetSingleton().GetString( 537 )//아이템 이동이 실패했습니다
#define STR_CHAT_REPLY_MAKE_FAILED					CStringManager::GetSingleton().GetString( 538 )//방만들기에 실패했습니다
#define STR_CHAT_REPLY_INVALIED_PASS				CStringManager::GetSingleton().GetString( 539 )//비밀번호가 틀립니다
#define STR_CHAT_REPLY_ROOM_NOT_FOUND				CStringManager::GetSingleton().GetString( 540 )//없어진 채팅방입니다


#define STR_CHAT_REPLY_FULL_USERS					CStringManager::GetSingleton().GetString( 541 )//더이상 참여할수 없습니다
#define STR_CHAT_REPLY_KICKED						CStringManager::GetSingleton().GetString( 542 )//채팅방에서 강퇴 당했습니다
#define F_STR_QUERY_APPEND_FRIEND_REQ				CStringManager::GetSingleton().GetString( 543 )//%s가 친구로 등록하려고 합니다. 승낙하시겠습니까?
#define F_STR_REJECT_APPEND_FRIEND_REQ				CStringManager::GetSingleton().GetString( 544 )//%s이 친구 추가 요청을 거절했습니다
#define F_STR_NOT_FOUNDORCONNECT_FRIEND				CStringManager::GetSingleton().GetString( 545 )//%s을 찾을수 없거나 접속중이 아닙니다
#define STR_OTHERSTATE_IS_REJCTMESSANGER			CStringManager::GetSingleton().GetString( 546 )//상대방은 현재 메신저 거부상태입니다
#define STR_MEMO_REPLY_SEND_NOT_EXISTORINVALID		CStringManager::GetSingleton().GetString( 547 )//대상을 찾을수 없습니다
#define STR_MEMO_REPLY_SEND_REFUSED					CStringManager::GetSingleton().GetString( 548 )//쪽지 수신 거부
#define STR_MEMO_REPLY_SEND_FULL_MEMO				CStringManager::GetSingleton().GetString( 549 )//상대방의 수신함이 꽉차서 더이상 받을수 없습니다
#define STR_MEMO_REPLY_SEND_INVALID_CONTENT			CStringManager::GetSingleton().GetString( 550 )//내용오류

#define STR_CRAFT_BREAKUP_SUCCESS_GEM				CStringManager::GetSingleton().GetString( 551 )//아이템 분리가 성공했습니다
#define STR_CRAFT_BREAKUP_DEGRADE_GEM				CStringManager::GetSingleton().GetString( 552 )//아이템 분리가 성공했습니다. 분리 과정에서 보석의 등급감소가 발생했습니다
#define STR_CRAFT_BREAKUP_CLEARED_GEM				CStringManager::GetSingleton().GetString( 553 )//아이템 분리가 성공했습니다. 분리 과정에서 보석이 손실되었습니다
#define STR_CRAFE_BREAKUP_SUCCESS					CStringManager::GetSingleton().GetString( 554 )//아이템 분해가 성공했습니다
#define STR_CRAFT_UPGRADE_INVALID_MAT				CStringManager::GetSingleton().GetString( 555 )//재료 아이템이 잘못됬습니다

#define F_STR_SET_MONEY_INC							CStringManager::GetSingleton().GetString( 556 )//돈이 %d증가하였습니다
#define F_STR_SET_MONEY_DEC							CStringManager::GetSingleton().GetString( 557 )//돈이 %d감소하였습니다
#define F_STR_REWARD_MONEY_INC						CStringManager::GetSingleton().GetString( 558 )//돈 %d을 받았습니다
#define F_STR_REWARD_MONEY_DEC						CStringManager::GetSingleton().GetString( 559 )//돈 %d을 가져갔습니다
#define F_STR_REWARD_ITEMS							CStringManager::GetSingleton().GetString( 560 )//%s %d개를 보상 받았습니다
#define F_STR_REWARD_ITEM							CStringManager::GetSingleton().GetString( 561 )//%s을 보상 받았습니다

/// 562 ~ 570 : 아이템 타입

#define STR_RIDE_PART_VER							CStringManager::GetSingleton().GetString( 571 )//부품번호
#define STR_RESULT_CREATE_CHAR_DUP_NAME				CStringManager::GetSingleton().GetString( 572 )//부품번호
#define STR_APPRAISAL_COST							CStringManager::GetSingleton().GetString( 573 )//부품번호
#define STR_COST									CStringManager::GetSingleton().GetString( 574 )//부품번호

#define STR_SERVERSTATE_SMOOTH						CStringManager::GetSingleton().GetString( 575 )
#define STR_SERVERSTATE_NORMAL						CStringManager::GetSingleton().GetString( 576 )
#define STR_SERVERSTATE_CONFUSION					CStringManager::GetSingleton().GetString( 577 )
#define STR_SERVERSTATE_EXCESS						CStringManager::GetSingleton().GetString( 578 )


#define STR_SKILLTREE								CStringManager::GetSingleton().GetString( 580 )




#define STR_RECEIVER								CStringManager::GetSingleton().GetString( 581 )
#define STR_SENDING_ITEM							CStringManager::GetSingleton().GetString( 582 )
#define STR_REPLY_MALL_ITEM_CHECK_CHAR_NONE			CStringManager::GetSingleton().GetString( 583 )
#define STR_REPLY_MALL_ITEM_CHECK_CHAR_INVALID		CStringManager::GetSingleton().GetString( 584 )
#define STR_REPLY_MALL_ITME_GIVE_SUCCESS			CStringManager::GetSingleton().GetString( 585 )
#define STR_REPLY_MALL_ITEM_GIVE_FAILED				CStringManager::GetSingleton().GetString( 586 )
#define STR_REPLY_MALL_ITEM_BRING_FAILED			CStringManager::GetSingleton().GetString( 587 )
#define STR_SENDER									CStringManager::GetSingleton().GetString( 588 )
#define STR_GIFT_ITEM								CStringManager::GetSingleton().GetString( 589 )
#define STR_PLATINUM								CStringManager::GetSingleton().GetString( 590 )

#define STR_CANTPARTY_WITHOTHERTEAM_INPVPZONE		CStringManager::GetSingleton().GetString( 591 )

#define	STR_BILLING_MSG_FREE_USER				CStringManager::GetSingleton().GetString( 592 )//
#define	STR_BILLING_MSG_FEE_TYPE1				CStringManager::GetSingleton().GetString( 593 )//  /요금: 귀하는 정액요금제 사용자 입니다. (2004년06월01일)
#define	STR_BILLING_MSG_FEE_TYPE2				CStringManager::GetSingleton().GetString( 594 )//         귀하는 정량요금제 사용자 입니다. (18시간20분20초)
#define	STR_BILLING_MSG_TIME_ALERT				CStringManager::GetSingleton().GetString( 595 )	// 남은 시간 통보
#define	STR_BILLING_MSG_TIME_EXPIRED			CStringManager::GetSingleton().GetString( 596 )	// <종료> 결제 시간이 다되어 게임을 종료합니다.

#define	STR_BILLING_MSG_KICKOUT_CHEAT_DETECT	CStringManager::GetSingleton().GetString( 597 )	// 스핵등 치트 발견되서 종료
#define	STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN	CStringManager::GetSingleton().GetString( 598 ) // 이미 접속된 계정이라 접속 종료
#define	STR_BILLING_MSG_KICKOUT_TIME_EXPIRED	CStringManager::GetSingleton().GetString( 599 )	// 과금 시간 만료
#define	STR_BILLING_MSG_KICKOUT_DUP_LOGIN		CStringManager::GetSingleton().GetString( 600 )	// 다른 넘이 접속해서 접속 종료
#define STR_BILLING_MSG_KICKOUT_NO_RIGHT		CStringManager::GetSingleton().GetString( 601 )	// 접속할 권한이 없다...
#define	STR_BILLING_MSG_KICKOUT_OUT_OF_IP		CStringManager::GetSingleton().GetString( 602 )	// pc방에서 접속할수 있는 ip 갯수가 초과
#define	STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT	CStringManager::GetSingleton().GetString( 603 )	// 접속되어 있는 계정을 짤랐다.

#define	STR_BILLING_MSG_TYPE_SET_DATE			CStringManager::GetSingleton().GetString( 604 )	// 만료 일자 설정	szMsg = 200309101025
#define	STR_BILLING_MSG_TYPE_SET_TIME			CStringManager::GetSingleton().GetString( 605 )	// 만료 시간 설정	szMsg = 남은분
#define	STR_BILLING_MSG_TYPE_TIME_ALERT			CStringManager::GetSingleton().GetString( 606 )	// 남은 시간 경고	szMsg = 남은분

#define STR_RESULT_SELECT_SERVER_NEED_CHARGE	CStringManager::GetSingleton().GetString( 607 ) //사용시간이 만료되어 더 이상 접속하실수 없습니다
#define STR_ONLY_PLATINUM_TAB_BANK				CStringManager::GetSingleton().GetString( 608 )
#define STR_ONLY_PLATINUM_SELECT_CHAR			CStringManager::GetSingleton().GetString( 609 )
#define STR_SHOW_PARTYMEMBER_HPGUAGE			CStringManager::GetSingleton().GetString( 610 )


#define STR_PARTYLEVEL							CStringManager::GetSingleton().GetString( 611 )
#define STR_PARTYEXP_DISTRIBUTE_FORM			CStringManager::GetSingleton().GetString( 612 )
#define STR_OK									CStringManager::GetSingleton().GetString( 613 )
#define STR_DELIVERY_STORE						CStringManager::GetSingleton().GetString( 614 )

#define STR_OUTOFRANGE							CStringManager::GetSingleton().GetString( 615 )

#define FSTR_FRIEND_LOGIN						CStringManager::GetSingleton().GetString( 616 )
#define FSTR_FRIEND_LOGOUT						CStringManager::GetSingleton().GetString( 617 )
#define STR_EXPIRED_TIME						CStringManager::GetSingleton().GetString( 618 )
#define STR_ERROR_EQUIP_PAT_BODY_VER			CStringManager::GetSingleton().GetString( 619 )
#define STR_PREVIEW								CStringManager::GetSingleton().GetString( 620 )
#define STR_REGIST_CLANMARK						CStringManager::GetSingleton().GetString( 621 )
#define STR_WARN_BULLET_USEUP					CStringManager::GetSingleton().GetString( 622 )
#define STR_CLANMARK_HELP_HOMEPAGE				CStringManager::GetSingleton().GetString( 623 )
#define STR_CLANMARK_FILENAME					CStringManager::GetSingleton().GetString( 624 )
#define STR_CLANMARK_INVALID_SIZE				CStringManager::GetSingleton().GetString( 625 )
#define STR_CLANMARK_FILE_NOTFOUND				CStringManager::GetSingleton().GetString( 626 )
#define STR_CLANMARK_INVALID_FORMAT				CStringManager::GetSingleton().GetString( 627 )
#define STR_CLANMARK_FILE_NOTFOUNT_REGISTER		CStringManager::GetSingleton().GetString( 628 )
#define STR_CLANMARK_INVALID_COLOR				CStringManager::GetSingleton().GetString( 629 )
#define STR_CLANMARK_REGISTER_SUCCESS			CStringManager::GetSingleton().GetString( 630 )

#define STR_SUCCESS_EXCHANGE					CStringManager::GetSingleton().GetString( 631 )
#define STR_CLANMARK_UPDATE_ERROR				CStringManager::GetSingleton().GetString( 632 )
#define STR_QUERY_CHANGE_CLANMARK				CStringManager::GetSingleton().GetString( 633 )
#define STR_CLANMARK_REGISTER_TIME				CStringManager::GetSingleton().GetString( 634 )

#define STR_CHAT_RESTRICT						CStringManager::GetSingleton().GetString( 635 )
#define STR_SAVED_CHAT							CStringManager::GetSingleton().GetString( 636 )
#define STR_CRASH_MSG							CStringManager::GetSingleton().GetString( 637 )


#define STR_BANK_WITHDRAW_FAIL					CStringManager::GetSingleton().GetString( 638 )
#define STR_BANK_SAVE_FAIL						CStringManager::GetSingleton().GetString( 639 )
#define STR_QUERY_BANK_SAVE						CStringManager::GetSingleton().GetString( 640 )
#define STR_QUERY_BANK_WITHDRAW					CStringManager::GetSingleton().GetString( 641 )

#define STR_SAVE								CStringManager::GetSingleton().GetString( 642 )
#define STR_WITHDRAW							CStringManager::GetSingleton().GetString( 643 )
#define STR_QUERY_REGIST_WISHLIST				CStringManager::GetSingleton().GetString( 644 )

#define STR_ENTERCHATTING_MODE					CStringManager::GetSingleton().GetString( 645 )
#define STR_NORMALCHATTING_MODE					CStringManager::GetSingleton().GetString( 646 )
#define STR_REQUIRE_SP							CStringManager::GetSingleton().GetString( 647 )
#define STR_REQUIRE_JULY						CStringManager::GetSingleton().GetString( 648 )
#define STR_REQUIRE_LEVEL						CStringManager::GetSingleton().GetString( 649 )
#define STR_CONSUME_JULY						CStringManager::GetSingleton().GetString( 650 )
#define STR_CONSUME_SP							CStringManager::GetSingleton().GetString( 651 )
#define STR_EARN_SP								CStringManager::GetSingleton().GetString( 652 )
#define STR_SERVER_EXAMINE						CStringManager::GetSingleton().GetString( 653 )
#define STR_SERVER_INFO							CStringManager::GetSingleton().GetString( 654 )



#define STR_JP_PAY_FREEPLAY						CStringManager::GetSingleton().GetString( 655 )
#define STR_JP_PAY_ENTRY						CStringManager::GetSingleton().GetString( 656 )
#define STR_JP_PAY_COMMUNITY					CStringManager::GetSingleton().GetString( 657 )
#define STR_JP_PAY_TRADE						CStringManager::GetSingleton().GetString( 658 )
#define STR_JP_PAY_STOCKSPACE					CStringManager::GetSingleton().GetString( 659 )
#define STR_JP_PAY_EXTRASTOCK					CStringManager::GetSingleton().GetString( 660 )
#define STR_JP_PAY_STARSHIPPASS					CStringManager::GetSingleton().GetString( 661 )
#define STR_JP_PAY_DUNGEONADVENTURE				CStringManager::GetSingleton().GetString( 662 )
#define STR_JP_PAY_EXTRACHARACTER				CStringManager::GetSingleton().GetString( 663 )

#define STR_JP_BILL_CANT_ATTACK					CStringManager::GetSingleton().GetString( 664 )
#define STR_JP_BILL_CANT_INVITE_PARTY			CStringManager::GetSingleton().GetString( 665 )
#define STR_JP_BILL_CANT_INVITED_PARTY			CStringManager::GetSingleton().GetString( 666 )
#define STR_JP_BILL_CANT_USE_CLANAGIT			CStringManager::GetSingleton().GetString( 667 )
#define STR_JP_BILL_CANT_USE_CLANSKILL			CStringManager::GetSingleton().GetString( 668 )
#define STR_JP_BILL_CANT_USE_CLANCHAT			CStringManager::GetSingleton().GetString( 669 )
#define STR_JP_BILL_CANT_JOIN_CLAN				CStringManager::GetSingleton().GetString( 670 )
#define STR_JP_BILL_CANT_MOVE_PLANET			CStringManager::GetSingleton().GetString( 671 )
#define STR_JP_BILL_CANT_USE_FREINDLIST			CStringManager::GetSingleton().GetString( 672 )
#define STR_JP_BILL_CANT_SEND_MSG				CStringManager::GetSingleton().GetString( 673 )
#define STR_JP_BILL_CANT_USE_CHATROOM			CStringManager::GetSingleton().GetString( 674 )
#define STR_JP_BILL_CANT_TRADE					CStringManager::GetSingleton().GetString( 675 )
#define STR_JP_BILL_CANT_TRADE_DONT_PAY_TARGET	CStringManager::GetSingleton().GetString( 676 )
#define STR_JP_BILL_CANT_OPEN_PRIVATESTORE		CStringManager::GetSingleton().GetString( 677 )
#define STR_JP_BILL_CANT_USE_BANK				CStringManager::GetSingleton().GetString( 678 )
#define STR_JP_BILL_CANT_USE_BANK_MONEY			CStringManager::GetSingleton().GetString( 679 )
#define STR_JP_BILL_CANT_USE_BANK_EXTRASPACE	CStringManager::GetSingleton().GetString( 680 )
#define STR_JP_BILL_CANT_MOVE_PLANET2			CStringManager::GetSingleton().GetString( 681 )
#define STR_JP_BILL_CANT_ENTER_DUNGEON			CStringManager::GetSingleton().GetString( 682 )
#define STR_JP_BILL_CANT_SELECT_CHARACTER		CStringManager::GetSingleton().GetString( 683 )

#define STR_JP_PAY_TYPE							CStringManager::GetSingleton().GetString( 684 )
#define STR_JP_PAY_EXPIRE_DATE					CStringManager::GetSingleton().GetString( 685 )
#define STR_JP_PAY_REMAIN_TIME					CStringManager::GetSingleton().GetString( 686 )
#define STR_JP_PAY_EXPIRE_ALERT					CStringManager::GetSingleton().GetString( 687 )
#define STR_JP_PAY_REMAIN_TIME_MESSAGE			CStringManager::GetSingleton().GetString( 688 )
#define STR_JP_PAY_EXPIRE						CStringManager::GetSingleton().GetString( 689 )

#define STR_CANT_USE_CART						CStringManager::GetSingleton().GetString( 690 )
#define STR_CANT_USE_CASTLEGEAR					CStringManager::GetSingleton().GetString( 691 )
#define STR_CHANGED_STOREDTOWN_POS				CStringManager::GetSingleton().GetString( 692 )
#define STR_RECOVERD_STOREDTOWN_POS				CStringManager::GetSingleton().GetString( 693 )


//TAIWAN BILLING MSG
#define STR_TW_BILL_TYPE_1HOUR					CStringManager::GetSingleton().GetString( 695 )
#define STR_TW_BILL_TYPE_MONTH					CStringManager::GetSingleton().GetString( 696 )
#define STR_TW_BILL_TYPE_12HOUR					CStringManager::GetSingleton().GetString( 697 )
#define STR_TW_BILL_TYPE_3MONTH					CStringManager::GetSingleton().GetString( 698 )
					
#define STR_TW_BILL_TYPE_HONGKONG_MONTH			CStringManager::GetSingleton().GetString( 699 )
#define STR_TW_BILL_TYPE_HONGKONG_3MONTH		CStringManager::GetSingleton().GetString( 600 )
#define STR_TW_BILL_TYPE_HONGKONG_HOUR			CStringManager::GetSingleton().GetString( 701 )
#define STR_TW_BILL_TYPE_HONGKONG_12HOUR		CStringManager::GetSingleton().GetString( 702 )

#define STR_TW_BILL_TYPE_MINUTE					CStringManager::GetSingleton().GetString( 703 )
#define STR_TW_BILL_TYPE_2HOUR					CStringManager::GetSingleton().GetString( 704 )
#define STR_TW_BILL_TYPE_10DAY					CStringManager::GetSingleton().GetString( 705 )
#define STR_TW_BILL_TYPE_FREE					CStringManager::GetSingleton().GetString( 706 )

#define STR_TW_BILL_TYPE_BLOCK_3DAY				CStringManager::GetSingleton().GetString( 707 )
#define STR_TW_BILL_TYPE_BLOCK_7DAY				CStringManager::GetSingleton().GetString( 708 )
#define STR_TW_BILL_TYPE_BLOCK_10DAY			CStringManager::GetSingleton().GetString( 709 )

//Cart coupe(^^?) msg
#define STR_MUST_BOARD_CART_ABLE_TO_USE			CStringManager::GetSingleton().GetString( 710 )
#define STR_AFFIX_RUMBLE						CStringManager::GetSingleton().GetString( 711 )
#define STR_CANT_BOARD_CONDITION				CStringManager::GetSingleton().GetString( 712 )
#define STR_CANT_TOO_HEAVY						CStringManager::GetSingleton().GetString( 713 )
#define STR_WANT_BOARDS							CStringManager::GetSingleton().GetString( 714 )
#define STR_REFUSE_BOARD						CStringManager::GetSingleton().GetString( 715 )
#define STR_BOARDING_CANT_USE					CStringManager::GetSingleton().GetString( 716 )
#define STR_CANT_BOARD							CStringManager::GetSingleton().GetString( 717 )


//kor, ph billing msg
#define STR_FREE_ACCOUNT_USER					CStringManager::GetSingleton().GetString( 718 )
#define STR_PAID_DAYBASED_PLAN					CStringManager::GetSingleton().GetString( 719 )
#define STR_PAID_DAYBASED_PLAN_PLATINUM			CStringManager::GetSingleton().GetString( 720 )

#define STR_PAID_TIMEBASED_PLAN					CStringManager::GetSingleton().GetString( 721 )
#define STR_PAID_INTERNET_CAFE					CStringManager::GetSingleton().GetString( 722 )
#define STR_PAID_USER_PLAN_IC_TIME_END			CStringManager::GetSingleton().GetString( 723 )
#define STR_PAID_USER_PLAN_IC_IP_FULL			CStringManager::GetSingleton().GetString( 724 )

#define STR_GAME_TIME_LEFT_MINS					CStringManager::GetSingleton().GetString( 725 )

#define STR_BILL_AT_ROSEONLINE_HOMEPAGE			CStringManager::GetSingleton().GetString( 726 )
#define STR_INSUFFICIENCY_IP					CStringManager::GetSingleton().GetString( 727 )

#define STR_GETTING_ITEM_PARTY					CStringManager::GetSingleton().GetString( 728 ) //%s님이 %s 아이템을 획득하였습니다
#define STR_GETTING_ITEMS_PARTY					CStringManager::GetSingleton().GetString( 729 ) //%s님이 %s %d개 획득하였습니다
#define STR_GETTING_MONEY_PARTY					CStringManager::GetSingleton().GetString( 730 ) //%s님이 돈 %원을 획득하였습니다

#define STR_CANT_REFAIR_PAT_ENGINE				CStringManager::GetSingleton().GetString( 731 ) //팻 엔진은 수리 할 수 없습니다. 엔진 연료를 충전해야 합니다.

///////////////////////////////////////////////////////////////////////////////////////
/// sound
///////////////////////////////////////////////////////////////////////////////////////

#define SOUND_EQUIP_ITEM							401
#define SOUND_EQUIP_ITEM_LEATHER					402
#define SOUND_EQUIP_ITEM_METAL						403

#define SOUND_WALK_AVT01							651






#define SOUND_LEVEL_UP								16
#define SOUND_BUTTON_CLICK01						1

#endif // _TYPERESOURCE_