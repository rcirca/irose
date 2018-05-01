/*
	** 모델 데이타 text 작성법.
	numobj 1
	obj 1
		mesh 		3Ddata\????.ZMS		-- 메쉬 파일
		mat 		3Ddata\????.tga		-- 텍스쳐 파일
		isskin   	0					-- skinning		on:1, off:0		*주의 bonenumber 같이 사용안된다 !
		alpha 		0					-- alpha		on:1, off:0
		twoside 	0					-- twoside		on:1, off:0
		alphatest	1                   -- 알파테스트 여부. on:1, off:0.              2003/10/10 추가됨.
		alpharef	128                 -- 알파테스트시 비교값. 0-255.                2003/10/10 추가됨.
		ztest		1                   -- Z테스트.  on:1, off:0.                     2003/10/10 추가됨.
		zwrite		1                   -- Z값 쓰기. on:1, off:0.                     2003/10/10 추가됨.
		blendtype	0                   -- 블렌딩타입. 기본:0, 밝게:1.                2003/10/10 추가됨.
		specular	0                   -- 스페큘러 여부. on:1, off:0.				  2003/10/10 추가됨.
		alphavalue 1.0				-- 재질 기본 전체 알파. 기본값 1.0		2004/07/23 추가됨.
		glowtype 0					-- 재질 글로우 타입 기본값 0					2004/07/25 추가됨.
		glowcolor 0.5 0.5 0.5    -- 글로우 컬러. 기본 1, 1, 1. 범위 0에서 1사이. 2004/07/22 추가됨.
		pos	0.0 0.0 0.0					-- export에서 기록되는 정보 
		rot -0.xxx 0.xxx -0.xxx -0.xxx	-- export에서 기록되는 정보 
		scale 1.0 1.0 1.0				-- export에서 기록되는 정보 
		linkdummy	?					-- 장착 아이템의 경우 위치할 더미 번호.
		bonenumber	?					-- 위치할 본의 번호				*주의 isskin과 같이 사용안된다 !
		parent		0					
		collision	4					-- 충돌 체크, 필드 아이템, 건물 나무등은 4입력  풀, 나무잎등은 필요 없음.

		mob_stop	3DData\???.ZMO		-- 몬스터 모델중 메쉬 에니메이션이되는 경우 각 행동에 맞는 모션 파일을 기록	
		mob_move	3DData\???.ZMO		-- 별도의 모션 파일이 지정이 필요 없을 경우는 없는 추가하지 않는다.
		mob_attack	3DData\???.ZMO      -- 모션이 지정되지 않으면 정지 모션이 사용된다.
		mob_die		3DData\???.ZMO
		mob_hit		3DData\???.ZMO
		mob_etc		3DData\???.ZMO

		avt_walk	3DData\???.ZMO		-- 아바타 모델중 메쉬 에니메이션이 되는것은 각 동작에 맞는 모션 파일을 기록.
		avt_attack  3DData\???.ZMO		-- 몬스터 모션 설정과 동일.
		avt_stop1	3DData\???.ZMO
		avt_stop2	3DData\???.ZMO
		avt_run		3DData\???.ZMO
		avt_sitting	3DData\???.ZMO
		avt_sit		3DData\???.ZMO
		avt_standup	3DData\???.ZMO
		avt_stop3	3DData\???.ZMO
		avt_hit		3DData\???.ZMO
		avt_fall	3DData\???.ZMO
		avt_die		3DData\???.ZMO
		avt_raise	3DData\???.ZMO


	numpoint 1
	point 1
		effect		3DDATA\????.eft		-- 포인트에 링크될 효과 파일 없으면 공백으로...
		parent 1						-- 포인트가 위치한 obj 번호
		pos	0.0 0.0 0.0					-- export에서 기록되는 정보 
		rot -0.xxx 0.xxx -0.xxx -0.xxx	-- export에서 기록되는 정보 
		scale 1.0 1.0 1.0				-- export에서 기록되는 정보 


	케릭터 :
		isskin 과 bonenumber 가 동시에 1이 되면 않된다.

	무기 :
		-- 무기가 장창될 linkdummy를 반드시 기록한다.


	건물 & 지형물 :
		-- 충돌 체크가 필요한 부분은 collision 을 기록한다.
		-- 나뭇잎등 케릭터 위쪽에 위치하는것들의 경우 collision을 0으로 하거나 삭제한다.

	필드 아이템 :
		-- 반드시 collision 값을 4로 설정한다.
*/
/*
	** 캐릭터 더미 포인트 순서.
		0 : 오른손
		1 : 왼손
		2 : 왼손 방패
		3 : 등장비
		4 : 입
		5 : 눈
		6 : 모자
		7 : 타격 지점
*/

/*
	** 무기 포인트 순서 ( 효과 장착 포인트는 3개를 만듦)
		0 ~ 1 : 칼일 경우 잔상이 그려질 두점 
		0 : 원거리 무기 총알 발사 위치.
		1 ~ 나머지 : 기타 효과 장착 위치. 칼일경우 2번 포인트 부터.
*/

/*
	** 아바타 본 인덱스.
		0 :
		1 :
		2 :
		3 :
		4 : 머리
		5 :
		6 :
*/

/*
// 아이템 소분류표 !!!
가면     (111)	
안경     (112)	
기타     (113)	
전투모    (121)	
마법모    (122)	
일반모자  (123)	
머리장식 (124)	
전투복   (131)	
마법옷   (132)	
일반옷   (133)	
건들렛    (141)	
마법장갑  (142)	
일반장갑  (143)	
전투화    (151)	
마법부츠  (152)	
일반신발 (153)	
등방어구 (161)	
배낭     (162)	
날개     (163)	
화살통   (164)	
총알통   (165)	
폭탄통   (166)	
목걸이   (171)	비어있는 반지에 보석을 붙여서 장착시킨다.
반지     (172)	
한손검   (211)	데이터 파일에서는 1번 ~ 100번까지
한손 둔기 (212)	
양손검    (221)	크기가 큰 무기 종류
데이터 파일에서는 101번 ~ 200번까지
창        (222)	
양손 도끼 (223)	
활        (231)	원거리 공격용 무기
데이터 파일에서는 201번 ~ 300번까지
총        (232)	
마법지팡이 (241)	마법용 무기나 도구
데이터 파일에서는 301번 ~ 400번까지
마법 도구  (242)	
카타르    (251)	오른손,왼손에 모두 쌍으로 장착되는 무기.
데이터 파일에서는 401번 ~ 500번까지
이도류    (252)	
쌍총     (253)	
방패      (261)	* 한손 무기를 장착했을 때, 다른 손에 보조 장비를 장착할 수 있다. * 보조 장비는 아바타의 왼손에 장착된다.
보조도구  (262)	* 팬시상품, 마법보조 도구.
약품      (311)	체력회복약, 근력강화제, DHA보강제
음식      (312)	돼지바베큐, 통닭, 주먹밥
마법문서  (313)	1회용 마법을 발동시키는 아이템
스킬교본  (314)	새로운 스킬을 배우게 해주는 아이템
보석      (411)	다이아몬드, 루비 등..
예술품    (412)	도자기, 그림, 조각상 등..
금속      (421)	철, 구리, 은 등..
마금속    (422)	마법의힘이 결합된 특수 금속
석재      (423)	대리석, 기와, 벽돌 등..
목재      (424)	판자, 각목 등..
가죽      (425)	소가죽, 양가죽 등..
옷감      (426)	무명천, 면, 비단, 모직 등..
연금재료  (427)	마금속, 보석가공등에 사용
화학품    (428)	약품, 음식 등을 만들 때 사용.
글리세린, 소다, 마그네슘, 화약, 마법가루
공산품    (429)	열쇠, 인형, 종이, 고무, 유리병, 나무상자, 
마법잉크(색깔 구분), 빈 마법책
자연물    (430)	철광석, 석탄, 고령토, 샘물,석유,화석 등..
나무토막, 버섯, 잎사귀, 개미꼬리,달팽이알, 가재껍질, 생수 등
퀘스트 물품(441)	 왕의옥쇠, 마스터키, 감옥열쇠
인증서     (442)	 보안관 신분증, 국왕 초대장
*/


#ifndef	__DATATYPE_H
#define	__DATATYPE_H
//-------------------------------------------------------------------------------------------------

#define	BONE_IDX_PELVIS		0
#define	BONE_IDX_HEAD		4

#define DUMMY_IDX_R_HAND	0
#define	DUMMY_IDX_L_HAND	1
#define DUMMY_IDX_L_SHIELD	2
#define DUMMY_IDX_BACK		3
#define DUMMY_IDX_MOUSE		4
#define DUMMY_IDX_EYES		5
#define	DUMMY_IDX_CAP		6

//-------------------------------------------------------------------------------------------------

#ifndef	__POINT16
#define	__POINT16
struct tPOINT16 {
	short	m_nX;
	short	m_nY;
} ;
#endif


#ifndef	__POINTF
#define	__POINTF
typedef	CVec2 tPOINTF;
/*
struct tPOINTF {
	float	m_fX;
	float	m_fY;
*/
#endif

#ifndef	__SERVER
	#define	NAME_MESH			"@MS%03d"
	#define	NAME_MOTION			"@AM%03d"
	#define	NAME_SKEL			"@SK%03d"
	#define	NAME_MATERIAL		"@MT%03d"
	#define	NAME_TERRAIN_MAT	"#TR%03d"
	#define	NAME_FIXEDOBJ		"@FM%03dp%d"
	#define	NAME_CNSTOBJ		"@CN%03dp%d"
	#define	NAME_TREEOBJ		"@TE%03dp%d"
	#define	NAME_COLLISIONOBJ	"@COLL%03dp%d"
	#define	NAME_ITEMOBJ		"@FI%03dp%d"
	#define	NAME_MOB_MODEL		"@MB%s-%d"				// create order
	#define	NAME_PARTICLE		"@PT%d"
	#define	NAME_BULLET			"@BU%d"

	typedef	LPDIRECT3DDEVICE9			LPD3DDEVICE;
	typedef	IDirect3DTexture9			ID3DTexture;
	typedef	LPDIRECT3DTEXTURE9			LPD3DTEXTURE;
	typedef	D3DMATERIAL9				D3DMATERIAL;
	typedef	LPDIRECT3DVERTEXBUFFER9		LPD3DVERTEXBUFFER;

	typedef	D3DVECTOR					t_POSITION;
#else
	typedef	tPOINTF						t_POSITION;
#endif


#define	MIN_PRICE_TYPE		1
#define	MAX_PRICE_TYPE		11


#ifndef	__ENUM_CHARPART
#define	__ENUM_CHARPART


#define FACE_EXPRESSION_COUNT	7
#define HAIR_TYPE_COUNT			5


//*************************************************************************************************
/// 보여주기 위한 모델 번호..
enum t_CharPART {
	BODY_PART_FACE = 0,						// 0 link to bone
	BODY_PART_HAIR = 1,
	BODY_PART_HELMET = 2,

	BODY_PART_ARMOR = 3,					// 3 skinning
	BODY_PART_GAUNTLET = 4,
	BODY_PART_BOOTS = 5,

	BODY_PART_GOGGLE = 6,					// 6 link to dummy
	BODY_PART_FACE_ITEM=BODY_PART_GOGGLE,	// 6 link to dummy
	BODY_PART_KNAPSACK = 7,

	// 오른손, 왼손 순서 주의 !!!
	BODY_PART_WEAPON_R = 8,					// 8 link to dummy	
	BODY_PART_WEAPON_L = 9,	

	MAX_BODY_PART = 10
} ;
#endif


//*************************************************************************************************
enum t_ObjTAG {
	OBJ_NULL = 0,
	OBJ_MORPH,
	OBJ_ITEM,
	OBJ_COLLISION,
	OBJ_GROUND,
	OBJ_CNST,
	OBJ_NPC,		// OBJ_NPC >= character object ...
	OBJ_MOB,
	OBJ_AVATAR,
	OBJ_USER,
	OBJ_CART,
	OBJ_CGEAR,
	OBJ_EVENTOBJECT,
	OBJ_MAX_TYPE
} ;


//*************************************************************************************************
/// 착용하고 있는 아이템...
enum t_EquipINDEX {
	EQUIP_IDX_NULL = 0,

	EQUIP_IDX_FACE_ITEM,
	EQUIP_IDX_HELMET,
	EQUIP_IDX_ARMOR,
	EQUIP_IDX_KNAPSACK,
	EQUIP_IDX_GAUNTLET,
	EQUIP_IDX_BOOTS,	// 5
	EQUIP_IDX_WEAPON_R=7,
	EQUIP_IDX_WEAPON_L,	
	EQUIP_IDX_NECKLACE,
	EQUIP_IDX_RING,
	EQUIP_IDX_EARRING,	// 2003. 10. 27 : 장착가능 반지가 2개...

	MAX_EQUIP_IDX		// 11
} ;

enum t_eSHOT {
	SHOT_TYPE_ARROW=0,			// 화살용
	SHOT_TYPE_BULLET,			// 총/쌍권총
	SHOT_TYPE_THROW,			// 투척기용
	MAX_SHOT_TYPE = 3
} ;


///
/// cart PET_TYPE == bone type
///
enum PET_TYPE
{
	PET_TYPE_CART01 = 21,
	PET_TYPE_CASTLE_GEAR01 = 31,
#ifdef FRAROSE
	PET_TYPE_MOUNT01 = 41,
	PET_TYPE_MOUNT02,
	PET_TYPE_MOUNT03,
	PET_TYPE_MOUNT04,
	PET_TYPE_MOUNT05,
	PET_TYPE_MOUNT06,
	PET_TYPE_MOUNT07,
	PET_TYPE_MOUNT08,
	PET_TYPE_MOUNT09,
	PET_TYPE_MOUNT10,
	PET_TYPE_MOUNT11,
	PET_TYPE_MOUNT12,
	PET_TYPE_MOUNT13,
	PET_TYPE_MOUNT14,
	PET_TYPE_MOUNT15,
	PET_TYPE_MOUNT16,
	PET_TYPE_MOUNT17,
	PET_TYPE_MOUNT18,
	PET_TYPE_MOUNT19,
	PET_TYPE_MOUNT20,
#endif
	PET_TYPE_MAX,
};

enum t_eRidePART 
{
	RIDE_PART_BODY = 0,
	RIDE_PART_ENGINE,
	RIDE_PART_LEG,
#ifndef __KCHS_BATTLECART__		// __OLD_DATA_COMPATIBLE__
	RIDE_PART_ARMS,
#else
	RIDE_PART_ABIL,
	RIDE_PART_ARMS,
#endif
	MAX_RIDING_PART
} ;



enum t_InvTYPE {
	INV_WEAPON = 0,
	INV_USE,
	INV_ETC,
	INV_RIDING,
	MAX_INV_TYPE
} ;
#define	INVENTORY_PAGE_SIZE		( 5 * 6 )
#define	INVENTORY_TOTAL_SIZE	( MAX_EQUIP_IDX + MAX_SHOT_TYPE + MAX_INV_TYPE*INVENTORY_PAGE_SIZE + MAX_RIDING_PART )
#define	INVENTORY_USE_ITEM0		( MAX_EQUIP_IDX + INVENTORY_PAGE_SIZE )				/// 소모 아이템 시작 인벤 번호
#define INVENTORY_ETC_ITEM0		( INVENTORY_USE_ITEM0  + INVENTORY_PAGE_SIZE )		/// 기타 이이템 시작 인벤 번호
#define	INVENTORY_SHOT_ITEM0	( MAX_EQUIP_IDX + MAX_INV_TYPE*INVENTORY_PAGE_SIZE )
#define	INVENTORY_RIDE_ITEM0	( INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )

#ifndef __SERVER///.nAvy.
#define INVENTORY_ITEM_INDEX_0		MAX_EQUIP_IDX	///게임상 인벤토리의 첫번째 INDEX NO
#define INVENTORY_ITEM_INDEX_LAST	MAX_EQUIP_IDX + MAX_INV_TYPE*INVENTORY_PAGE_SIZE - 1///게임상 인벤토리의 마지막 INDEX NO
#endif

enum t_eItemCLASS {
	JEWEL_ITEM_RING			= 171,		// 반지 
	JEWEL_ITEM_NECKLACE		= 172,		// 목걸이
	JEWEL_ITEM_EARRING		= 173,		// 귀걸이

	WEAPON_ITEM_USE_ARROW		= 231,		// SHOT_TYPE_ARROW 소모하는 무기
	WEAPON_ITEM_USE_ARROW2		= 271,		// SHOT_TYPE_ARROW 소모하는 무기(한손원거리무기)
	WEAPON_ITEM_USE_BULLET		= 232,		// SHOT_TYPE_BULLET 소모하는 무기
	WEAPON_ITEM_USE_THROW		= 233,		// SHOT_TYPE_THROW 소모하는 무기
	WEAPON_ITEM_USE_BULLET2		= 253,		// SHOT_TYPE_BULLET 소모하는 무기
	WEAPON_ITEM_NOT_USE_BULLET	= 242,	// SHOT_TYPE_BULLET 소모하는 무기


	WEAPON_ITEM_SHIELD			= 261,		// 방패 아이템


	USE_ITEM_SKILL_DOING		= 313,		// 소모성 아이템으로 스킬 사용
	USE_ITEM_SKILL_LEARN		= 314,		// 소모성 아이템으로 스킬 배움

	USE_ITEM_REPAIR_ITEM		= 315,		// 소모성 아이템으로 장비 수리
	USE_ITEM_EVENT_ITEM			= 316,		// 축하 이벤트 스크롤..
	USE_ITEM_FUEL				= 317,		// 연료 아이템
	USE_ITEM_VOLATILITY_ITEM	= 320,		// 습득시 바로 소모되는 아이템

	USE_ITEM_MAINTAIN_ITEM		= 321,		// 지속 시간으로 유지되는 아이템(창고무료서비스,창고확장...)

	TUNING_PART_BODY_CART			= 511,	// 튜닝부품 - 카트 바디
	TUNING_PART_BODY_CASTLEGEAR		= 512,	// 튜닝부품 - 케슬기어 바디
	TUNING_PART_ENGINE_CART			= 521,	// 튜닝부품 - 카트 엔진
	TUNING_PART_ENGINE_CASTLEGEAR	= 522,	// 튜팅부품 - 캐슬기어 엔진
	TUNING_PART_WEAPON_CASTLEGEAR	= 552,
} ;


//*************************************************************************************************
/// 아이템의 중분류 순서 --> STB 순서 !!.
enum t_eITEM {
	ITEM_TYPE_FACE_ITEM = 1,		// 1	LIST_FACEITEM.stb	얼굴 장식	
	ITEM_TYPE_HELMET,				// 2	LIST_CAP.stb
	ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
	ITEM_TYPE_GAUNTLET,				// 4	LIST_ARMS.stb
	ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
	ITEM_TYPE_KNAPSACK,				// 6	LIST_BACK.stb
	ITEM_TYPE_JEWEL,				// 7	LIST_JEWEL.stb		장신구 : 목걸이 반지

	ITEM_TYPE_WEAPON,				// 8	LIST_WEAPON.stb		무기
	ITEM_TYPE_SUBWPN,				// 9	LIST_SUBWPN.stb

	ITEM_TYPE_USE,					// 10	LIST_USEITEM.stb	소모

	ITEM_TYPE_ETC,					// 11
	ITEM_TYPE_GEM = ITEM_TYPE_ETC,	// 11	LIST_JEMITEM.stb	기타 : 보석
	ITEM_TYPE_NATURAL,				// 12	LIST_NATURAL.stb
	ITEM_TYPE_QUEST,				// 13	LIST_QUESTITEM.stb
	ITEM_TYPE_RIDE_PART=14,

	ITEM_TYPE_MONEY = 0x1f			// 31	max item type value !!!
} ;

//*************************************************************************************************
/// NPC 모션 인덱스
enum {
	MOB_ANI_STOP = 0,
//	MOB_ANI_STOP2,
	MOB_ANI_MOVE,
	MOB_ANI_ATTACK,
	MOB_ANI_HIT,
	MOB_ANI_DIE,
	MOB_ANI_RUN,
	MOB_ANI_CASTION01,
	MOB_ANI_SKILL_ACTION01,
	MOB_ANI_CASTION02,
	MOB_ANI_SKILL_ACTION02,
	MOB_ANI_ETC,

	MAX_MOB_ANI
} ;

//*************************************************************************************************
/// 아바타 모션 인덱스 
enum {
	AVT_ANI_STOP1	= 0,
	AVT_ANI_STOP2	= 1,
	AVT_ANI_WALK	= 2,
	AVT_ANI_RUN		= 3,
	AVT_ANI_SITTING	= 4,
	AVT_ANI_SIT		= 5,
	AVT_ANI_STANDUP = 6,
	AVT_ANI_STOP3	= 7,
	AVT_ANI_ATTACK	= 8,	
	AVT_ANI_ATTACK2	= 9,
	AVT_ANI_ATTACK3	= 10,

	AVT_ANI_HIT		= 11,
	AVT_ANI_FALL	= 12,
	AVT_ANI_DIE		= 13,
	AVT_ANI_RAISE	= 14,
	AVT_ANI_JUMP1	= 15,
	AVT_ANI_JUMP2	= 16,

	AVT_ANI_PICKITEM = 17,
	MAX_AVT_ANI
} ;


//*************************************************************************************************
/// 아바타 변수 지칭하는 인뎃스..
enum t_VarINDEX {
	SV_SEX = 0,
	SV_BIRTH,
	SV_CLASS,
	SV_UNION,
	SV_RANK,
	SV_FAME,
	SV_STR,
	SV_DEX,
	SV_INT,
	SV_CON,
	SV_CHA,
	SV_SEN,
	SV_EXP,
	SV_LEVEL,
	SV_POINT,
} ;


//*************************************************************************************************
/// 아바타 능력치 구분
enum t_AbilityINDEX {
	AT_SEX = 2,			// 남0,여1
	AT_BIRTHSTONE = 3,	// 탄생성
	AT_CLASS = 4,		// 직업
	AT_UNION = 5,		// 소속
	AT_RANK = 6,		// 계급
	AT_FAME = 7,		// 명성
	AT_FACE = 8,		// 얼굴모양
	AT_HAIR,

	AT_STR = 10,		// 기본 능력치
	AT_DEX,
	AT_INT,
	AT_CON,
	AT_CHARM,
	AT_SENSE = 15,

	AT_HP,
	AT_MP,
	AT_ATK,
	AT_DEF,
	AT_HIT,
	AT_RES = 21,
	AT_AVOID,
	AT_SPEED,
	AT_ATK_SPD,
	AT_WEIGHT,
	AT_CRITICAL = 26,
	AT_RECOVER_HP,
	AT_RECOVER_MP,

	AT_SAVE_MP = 29,

	AT_EXP = 30,
	AT_LEVEL,
	AT_BONUSPOINT = 32,

	AT_PK_FLAG = 33,
	AT_TEAM_NO = 34,

	AT_HEAD_SIZE,
	AT_BODY_SIZE,
	AT_SKILLPOINT = 37,

	AT_MAX_HP = 38,		// 2004. 2. 3 추가 
	AT_MAX_MP,

	AT_MONEY = 40,

	/// 패시브 스킬 능력치..
	AT_PSV_ATK_POW_NO_WEAPON	= 41,
	AT_PSV_ATK_POW_1HAND_WPN	= 42,	// 공격력 : 한손	<< AT_PSV_ATK_POW_SWORD 검 에서 한손으로 변경 2004. 2. 26 by icarus
	AT_PSV_ATK_POW_2HAND_WPN	= 43,	//			양손	<< AT_PSV_ATK_POW_AXE	둔기에서 양손을 변경
	AT_PSV_ATK_POW_BOW			= 44,
	AT_PSV_ATK_POW_GUN			= 45,
	AT_PSV_ATK_POW_STAFF_WAND	= 46,
	AT_PSV_ATK_POW_AUTO_BOW		= 47,	// 석궁
	AT_PSV_ATK_POW_KATAR_PAIR	= 48,	// 이도류

	AT_PSV_ATK_SPD_BOW			= 49,
	AT_PSV_ATK_SPD_GUN			= 50,
	AT_PSV_ATK_SPD_PAIR			= 51,	// 이도류

	AT_PSV_MOV_SPD				= 52,
	AT_PSV_DEF_POW,
	AT_PSV_MAX_HP,
	AT_PSV_MAX_MP				= 55,
	AT_PSV_RECOVER_HP,
	AT_PSV_RECOVER_MP,
	AT_PSV_WEIGHT				= 58,

	AT_PSV_BUYING				= 59,	// 구입 가격 흥정 기술
	AT_PSV_SELLING				= 60,	// 팔기 가격 흥정 기술

	AT_PSV_SAVE_MP				= 61,	// MP 절감

	AT_PSV_SUMMON_MOB_CNT		= 62,	// 소환가능 몹수
	AT_PSV_DROP_RATE			= 63,	// 드롭 확률

	AT_AFTER_PASSIVE_SKILL		= 71,						// 패시브 스킬 보정능력치 이후...
	AT_RACE						= AT_AFTER_PASSIVE_SKILL,	// 종족 init_avatar.stb 라인번호 / 2

	// 이후 2004.5.27 추가
	AT_DROP_RATE				= 72,	// 드롭 성공률
	AT_FAME_G					= 73,
	AT_FAME_B					= 74,

	AT_CURRENT_PLANET			= 75,	// 현재 플레이하고 있는 행성 번호

	AT_STAMINA					= 76,	// 스테미너
	AT_FUEL						= 77,	// 현재 연료..

	AT_IMMUNITY,

	AT_UNION_POINT1				= 81,	// 조합별 포인트
	AT_UNION_POINT2,
	AT_UNION_POINT3,
	AT_UNION_POINT4,
	AT_UNION_POINT5,
	AT_UNION_POINT6,
	AT_UNION_POINT7,
	AT_UNION_POINT8,
	AT_UNION_POINT9,
	AT_UNION_POINT10			= 90,

	AT_GUILD_NO					= 91,	// 길드 번호
	AT_GUILD_SCORE,						// 길드 클랜 점수
	AT_GUILD_POS,						// 길드 직위

	AT_MAINTAIN_ABILITY			= 94,
	AT_BANK_FREE				= AT_MAINTAIN_ABILITY,	// 창고 무료 
	AT_BANK_ADDON				= 95,					// 창고 확장
	AT_STORE_SKIN				= 96,					// 창고 모양
	AT_PATHP					= 97,

	AT_PSV_RES,
	AT_PSV_HIT,
	AT_PSV_CRITICAL,
	AT_PSV_AVOID,
	AT_PSV_SHIELD_DEF,
	AT_PSV_IMMUNITY,
	AT_AFTER_PASSIVE_SKILL_2ND,

#ifdef FRAROSE
	AT_EXP_BOOST				= 106,
	AT_DROP_BOOST				= 107,
	AT_EXP_NONE					= 108,
#endif
	AT_MAX
} ;


//*************************************************************************************************
/// 포인터 사용가능한 능력치들..
enum t_BasicAbilityINDEX {
	BA_STR=0,	// 			short	m_nSTR;			// 근력 1~200
	BA_DEX,		// short	m_nDEX;			// 민첩 1~200
	BA_INT,		// short	m_nINT;			// 지력 1~200
	BA_CON,		// short	m_nCON;			// concentration 집중력 1~200
	BA_CHARM,	// short	m_nCHARM;		// 매력 1~200
	BA_SENSE,	// short	m_nSENSE;		// 감각 1~200

	BA_MAX
} ;


//*************************************************************************************************
// 아바타 직업 구분표.		//
#define	CLASS_VISITOR		0	// 0~9 방문객

#define	CLASS_SOLDIER_111	111	// 1차 솔져
#define	CLASS_SOLDIER_121	121	// 2차 나이트
#define	CLASS_SOLDIER_122	122	// 2차 챔프
#define	CLASS_SOLDIER_131	131	// 3차 미정...
#define	CLASS_SOLDIER_132	132	// 3차 미정...

#define	CLASS_MAGICIAN_211	211	// 1차 뮤즈
#define	CLASS_MAGICIAN_221	221	// 2차 메지션
#define	CLASS_MAGICIAN_222	222	// 2차 클러릭
#define	CLASS_MAGICIAN_231	231	// 3차 미정..
#define	CLASS_MAGICIAN_232	232	// 3차 미정..

#define	CLASS_MIXER_311		311	// 1차 호커
#define	CLASS_MIXER_321		321	// 2차 레이더
#define	CLASS_MIXER_322		322	// 2차 스카우트
#define	CLASS_MIXER_331		331	// 3차 미정
#define	CLASS_MIXER_332		333	// 3차 미정

#define CLASS_MERCHANT_411	411	// 1차 딜러
#define CLASS_MERCHANT_421	421	// 2차 부즈주아
#define CLASS_MERCHANT_422	422	// 2차 아티쟌
#define CLASS_MERCHANT_431	431	// 3차 미정
#define CLASS_MERCHANT_432	432	// 3차 미정


//*************************************************************************************************
/// 지속 상태 유형.
enum eING_TYPE {
	ING_NULL=0,

	ING_PROC = 1,
	ING_INC_HP = ING_PROC,				// 처리 : hp 변화
	ING_INC_MP,							// 처리 : mp 변화
	ING_POISONED,						// 처리 : 중독

	ING_INC_MAX_HP=4,					// 유지 : 최대 hp
	ING_INC_MAX_MP,						// 유지 : 최대 mp

	ING_CHECK_START = 6,

	ING_INC_MOV_SPD=ING_CHECK_START,	// 유지 : 이속/공속
	ING_DEC_MOV_SPD,					// 유지 : 이속/공속
	ING_INC_ATK_SPD=8,
	ING_DEC_ATK_SPD,
	ING_INC_APOWER=10,					// 유지 : 공격력
	ING_DEC_APOWER,
	ING_INC_DPOWER=12,					// 유지 : 방어력
	ING_DEC_DPOWER,	
	ING_INC_RES=14,						// 유지 : 항마력
	ING_DEC_RES,
	ING_INC_HIT=16,						// 유지 : 명중
	ING_DEC_HIT,
	ING_INC_CRITICAL=18,				// 유지 : 크리티컬
	ING_DEC_CRITICAL,
	ING_INC_AVOID=20,					// 유지 : 회피력 
	ING_DEC_AVOID,
	ING_DUMB=22,						// 유지 : 벙어리 : skill stb 5번 컬럼이 7~14인 스킬 사용 못함.
	ING_SLEEP,							// 유지 : 수면 : 명령 불능 상태로.. 공격 받으면 풀림
	ING_FAINTING,						// 유지 : 기절 : 수면과 같으나 공격 받아도 안풀림
	ING_DISGUISE,						// 유지 : 위장 : 
	ING_TRANSPARENT,					// 유지 : 투명 :
	ING_SHIELD_DAMAGE=27,				// 유지 : 방패 데미지
	ING_DUMMY_DAMAGE=28,				// 유지 : 추가 데미지

	ING_CHECK_END = ING_DUMMY_DAMAGE,

	ING_DEC_LIFE_TIME=29,				// 처리 : 소환된 케릭의 생명 시간...	추가: 2004. 4. 6 

	ING_CLEAR_GOOD,						// 즉각 : 유리 상태 해지
	ING_CLEAR_BAD,						// 즉각 : 불리 상태 해지
	ING_CLEAR_ALL,						// 즉각 : 모든 상태 해지
	ING_CLEAR_INVISIBLE,				// 즉각 : 투명상태 해지(ING_DISGUISE,ING_TRANSPARENT) 

	ING_TAUNT,							// 유지 : 타운트
	ING_REVIVE,							// 유지 : 부활

	ING_MAX_STATUS
} ;


//*************************************************************************************************
/// 지속 상태 유지 플레그 
#define	FLAG_ING_HP					0x00000001
#define	FLAG_ING_MP					0x00000002
#define	FLAG_ING_POISONED			0x00000004

#define	FLAG_ING_MAX_HP				0x00000010
#define	FLAG_ING_MAX_MP				0x00000020
#define	FLAG_ING_INC_MOV_SPEED		0x00000040
#define	FLAG_ING_DEC_MOV_SPEED		0x00000080
#define	FLAG_ING_INC_ATK_SPEED		0x00000100
#define	FLAG_ING_DEC_ATK_SPEED		0x00000200
#define	FLAG_ING_INC_APOWER			0x00000400
#define	FLAG_ING_DEC_APOWER			0x00000800
#define	FLAG_ING_INC_DPOWER			0x00001000
#define	FLAG_ING_DEC_DPOWER			0x00002000
#define	FLAG_ING_INC_RES			0x00004000
#define	FLAG_ING_DEC_RES			0x00008000
#define	FLAG_ING_INC_HIT			0x00010000
#define FLAG_ING_DEC_HIT			0x00020000
#define	FLAG_ING_INC_CRITICAL		0x00040000
#define	FLAG_ING_DEC_CRITICAL		0x00080000
#define	FLAG_ING_INC_AVOID			0x00100000
#define	FLAG_ING_DEC_AVOID			0x00200000
#define	FLAG_ING_DUMB				0x00400000	
#define	FLAG_ING_SLEEP				0x00800000
#define	FLAG_ING_FAINTING			0x01000000
#define	FLAG_ING_DISGUISE			0x02000000
#define	FLAG_ING_TRANSPARENT		0x04000000
#define	FLAG_ING_SHIELD_DAMAGE		0x08000000
#define	FLAG_ING_DUMMY_DAMAGE		0x10000000

#define	FLAG_ING_DEC_LIFE_TIME		0x20000000		/// 소환된 케릭으로 초당 HP감소..
#define	FLAG_ING_REVIVE				0x40000000		/// 부활...
#define	FLAG_ING_TAUNT				0x80000000		/// 타운트 스킬 설정됨

/// 기절 상태 조사시... 이동,공격, 장비 탈부착, 소모 아이템 사용, 교환 할수 없다.
#define FLAG_ING_IGNORE_ALL			( FLAG_ING_SLEEP | FLAG_ING_FAINTING )

// 투명 상태 
#define	FLAG_ING_INVISIBLE			( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT )

#define	FLAG_ING_PROC_PER_TICK		( FLAG_ING_DEC_LIFE_TIME | FLAG_ING_POISONED )	/// 매 틱마다 처리
#define	FLAG_ING_CHECK_PER_TICK		( 0xfffffff0 )									/// 매 틱마다 유지 시간만 체크하면됨 

#define	FLAG_ING_BAD	(	FLAG_ING_POISONED		| FLAG_ING_DEC_MOV_SPEED| FLAG_ING_DEC_ATK_SPEED	\
						  | FLAG_ING_DEC_APOWER		| FLAG_ING_DEC_DPOWER	| FLAG_ING_DEC_RES			\
						  | FLAG_ING_DEC_HIT		| FLAG_ING_DEC_CRITICAL | FLAG_ING_DEC_AVOID		\
						  |	FLAG_ING_DUMB			| FLAG_ING_SLEEP		| FLAG_ING_FAINTING	)		\

#define	FLAG_ING_GOOD	(	FLAG_ING_MAX_HP			| FLAG_ING_MAX_MP		| FLAG_ING_INC_MOV_SPEED	\
						  | FLAG_ING_INC_ATK_SPEED	| FLAG_ING_INC_APOWER	| FLAG_ING_INC_DPOWER		\
						  | FLAG_ING_INC_RES		| FLAG_ING_INC_HIT		| FLAG_ING_INC_CRITICAL		\
						  | FLAG_ING_INC_AVOID		| FLAG_ING_SHIELD_DAMAGE| FLAG_ING_DUMMY_DAMAGE		\
						  | FLAG_ING_DISGUISE		| FLAG_ING_TRANSPARENT	)							\

/// 모든 상태 삭제시 dwSTATUS & FLAG_ING_CLEAR로 삭제해야 됨...
#define	FLAG_ING_CLEAR	(	FLAG_ING_DEC_LIFE_TIME | FLAG_ING_STORE_MODE | FLAG_ING_INTRO_CHAT )


//*************************************************************************************************
/// 보조 지속 상태 유지 플레그 
#define	FLAG_SUB_HIDE				0x00000001		// GM Hide기능 : 이 플레그 설정인 케릭터는 보여주지 않는다.
#define	FLAG_SUB_STORE_MODE			0x00000002		/// 개인 상점 중이다... 명령 불가, 소모 아이템 사용가능
#define	FLAG_SUB_INTRO_CHAT			0x00000004		// 소개글

#define	FLAG_SUB_ARUA_FAIRY			0x40000000		// 아루아 요정 버프...
//#define	FLAG_SUB_PROC_PER_TICK
//#define	FLAG_SUB_CHECK_PER_TICK	  ( 0xfffffff0 )	// 매 틱마다 유지 시간만 체크하면됨


//*************************************************************************************************
/// eING_TYPE의 지속 상태로 상태 플레그 값 뽑는 테이블..
#ifndef	__ADTOOLS
const DWORD c_dwIngFLAG[] = {
	0						,    //	ING_NULL,			
	FLAG_ING_HP				,    //	ING_INC_HP,			
	FLAG_ING_MP				,    //	ING_INC_MP,			
	FLAG_ING_POISONED		,    //	ING_POISONED,		

	FLAG_ING_MAX_HP			,    //	ING_INC_MAX_HP,		
	FLAG_ING_MAX_MP			,    //	ING_INC_MAX_MP,		
	FLAG_ING_INC_MOV_SPEED	,    //	ING_INC_MOV_SPD,	
	FLAG_ING_DEC_MOV_SPEED	,    //	ING_DEC_MOV_SPD,	
	FLAG_ING_INC_ATK_SPEED	,    //	ING_INC_ATK_SPD,	
	FLAG_ING_DEC_ATK_SPEED	,    //	ING_DEC_ATK_SPD,	
	FLAG_ING_INC_APOWER		,    //	ING_INC_APOWER,		
	FLAG_ING_DEC_APOWER		,    //	ING_DEC_APOWER,		
	FLAG_ING_INC_DPOWER		,    //	ING_INC_DPOWER,		
	FLAG_ING_DEC_DPOWER		,    //	ING_DEC_DPWOER,		
	FLAG_ING_INC_RES		,    //	ING_INC_RES,		
	FLAG_ING_DEC_RES		,    //	ING_DEC_RES,		
	FLAG_ING_INC_HIT		,    //	ING_INC_HIT,		
	FLAG_ING_DEC_HIT		,    //	ING_DEC_HIT,		
	FLAG_ING_INC_CRITICAL	,    //	ING_INC_CRITICAL,	
	FLAG_ING_DEC_CRITICAL	,    //	ING_DEC_CRITICAL,	
	FLAG_ING_INC_AVOID		,    //	ING_INC_AVOID,		
	FLAG_ING_DEC_AVOID		,    //	ING_DEC_AVOID,		
	FLAG_ING_DUMB			,    //	ING_DUMB,				// 유지 : 벙어리 : skill stb 5번 컬럼이 7~14인 스킬 사용 못함.
	FLAG_ING_SLEEP			,    //	ING_SLEEP,				// 유지 : 수면 : 명령 불능 상태로.. 공격 받으면 풀림
	FLAG_ING_FAINTING		,    //	ING_FAINTING,			// 유지 : 기절 : 수면과 같으나 공격 받아도 안풀림
	FLAG_ING_DISGUISE		,    //	ING_DISGUISE,			// 유지 : 위장 : 
	FLAG_ING_TRANSPARENT	,    //	ING_TRANSPARENT,		// 유지 : 투명 :
	FLAG_ING_SHIELD_DAMAGE	,    //	ING_SHIELD_DAMAGE,		// 유지 : 방패 데미지
	FLAG_ING_DUMMY_DAMAGE	,    //	ING_DUMMY_DAMAGE,		// 유지 : 추가 데미지

	FLAG_ING_DEC_LIFE_TIME	,	 // ING_DEC_LIFE_TIME		// 추가: 2004. 4. 6 

	0,							// ING_CLEAR_GOOD,			// 즉각 : 유리 상태 해지
	0,							// ING_CLEAR_BAD,			// 즉각 : 불리 상태 해지
	0,							// ING_CLEAR_ALL,			// 즉각 : 모든 상태 해지
	0,							// ING_CLEAR_INVISIBLE,		// 즉각 : 투명상태 해지(ING_DISGUISE,ING_TRANSPARENT) 

	FLAG_ING_TAUNT,				// ING_TAUNT				유지 : 타운트
	FLAG_ING_REVIVE,			// ING_REVIVE
} ;
#endif

//*************************************************************************************************
//*************************************************************************************************
#define	DEAD_HP				  ( -30000 )						/// 죽은 상태의 HP값.
#define	MAX_UNION_COUNT			10								/// 최대 조합 갯수


#define	ITEM_OBJ_FREE_TIME		  (    30 * 1000 )						/// 아이템 소유권이 없는 시간...
#define	ITEM_OBJ_LIVE_TIME		( (    60 * 1000 )+ITEM_OBJ_FREE_TIME )	/// 아이템 처름 생성시 사라지기 까지 시간.
#define	ITEM_OBJ_DROP_TIME		( (30*(60 * 1000))+ITEM_OBJ_FREE_TIME )	/// 인벤토리가 모자라 바닦에 떨어진 아이템

#define	MIN_OBJECTS				1
#define	MAX_SERVER_OBJECTS		0x0ffff
#ifndef	__SERVER
#define	MAX_CLIENT_OBJECTS		4096
#endif

#define	MUL02(V)				( (V) << 1 )
#define	MUL04(V)				( (V) << 2 )
#define	MUL08(V)				( (V) << 3 )
#define	MUL16(V)				( (V) << 4 )

#define	DIV02(V)				( (V) >> 1 )
#define	DIV04(V)				( (V) >> 2 )
#define	DIV08(V)				( (V) >> 3 )
#define	DIV16(V)				( (V) >> 4 )

#define	RANDOM(V)				( rand()% V )
extern const float	g_fMinHeight;
extern const float	g_fMaxDistance;




#define AVT_CLICK_EVENT_RANGE	1000		/// 아바타를 클릭했을때 반응할 최소 거리
#define NPC_CLICK_EVENT_RANGE	250			/// NPC를 클릭했을때 반응할 최소 거리
#define	ITEM_CLICK_EVENT_RANGE	150			/// ITEM 클릭했을때 반응할 최소 거리

#define PAT_DEFAULT_HP			1000		// PAT HP의 기본 값
//-------------------------------------------------------------------------------------------------
#endif
