#ifndef _STRING_MANAGER_
#define _STRING_MANAGER_

#include <string>
#include <vector>

#include "../Util/JSingleton.h"
#include <functional>
#include "../Util/JResourceContainer.h"


template< typename T >
class LessThen_TableEntity : public std::binary_function< T, T, bool >
{
public:
	bool operator () ( T& lhs, const std::string& rhs )
	{
		return lhs < rhs;
	}
};



//-----------------------------------------------------------------------------------------------
/// 지원 언어 타입
//-----------------------------------------------------------------------------------------------
enum LANGUAGE_TYPE
{
	LANGUAGE_KOR = 0,
	LANGUAGE_USA = 1,
	LANGUAGE_JPN = 2,
	LANGUAGE_CHA_TRADITIONAL = 3,
	LANGUAGE_CHA_SIMPLE = 4,
};


//-----------------------------------------------------------------------------------------------
/// 테이블 타입
//-----------------------------------------------------------------------------------------------
enum STRING_TABLE_TYPE
{
	ITEM_TYPE_ST = 0,
	NORMAL_TYPE_ST = 1,
	QUEST_TYPE_ST = 2
};

//-----------------------------------------------------------------------------------------------
/// 아이템 타입 스티링 테이블들
//-----------------------------------------------------------------------------------------------
enum ITEM_TYPE_TABLES
{
	LIST_ARMS_S_TB = 0,
	LIST_BACK_S_TB,
	LIST_BODY_S_TB,

	LIST_CAP_S_TB,
	LIST_FACEITEM_S_TB,
	LIST_FOOT_S_TB,

	LIST_JEMITEM_S_TB,
	LIST_JEWEL_S_TB,
	LIST_NATURAL_S_TB,

	LIST_PAT_S_TB,
	LIST_QUESTITEM_S_TB,
	LIST_SKILL_S_TB,

	LIST_SUBWPN_S_TB,
	LIST_USEITEM_S_TB,
	LIST_WEAPON_S_TB,	
	
	LIST_ZONE_S_TB,	
	HELP_S_TB,

	MAX_ITEM_TYPE_TB,
};


//-----------------------------------------------------------------------------------------------
/// 노말 타입 스티링 테이블들
//-----------------------------------------------------------------------------------------------
enum NORMAL_TYPE_TABLES
{
	STR_ITEMPREFIX = 0,
	LIST_NPC_S_TB,
	LIST_STRING_S_TB,	
	STR_ABILITY,
	STR_JOBNAME,
	STR_ITEMTYPE,
	STR_SKILLTARGET,
	STR_SKILLTYPE,	
	LIST_SELL_S,
	LIST_CLASS_S,
	LIST_UNION_S,
	STR_CLAN,
	STR_PLANET,
	STR_NOTIFY_MESSAGE,
	MAX_NORMAL_TYPE_TB
};

//-----------------------------------------------------------------------------------------------
/// 퀘스트 타입 스티링 테이블들
//-----------------------------------------------------------------------------------------------
enum QUEST_TYPE_TABLES
{
	LIST_QUEST_S_TB = 0,
	LIST_STATUS_S,
	MAX_QUEST_TYPE_TB
};


struct stTypeTable
{
	std::string strKey;
	int			iIndex;
};

//-----------------------------------------------------------------------------------------------
/// 아이템 타입 스티링 테이블
//-----------------------------------------------------------------------------------------------
struct stItemTypeTable : public stTypeTable
{
	std::string strName;
	std::string strDesc;	
};

//-----------------------------------------------------------------------------------------------
/// 일반 스트링 테이블
//-----------------------------------------------------------------------------------------------
struct stNormalTypeTable : public stTypeTable
{
	std::string strString;

	bool operator < ( std::string& rhs )
	{
		return this->strKey < rhs;
	}
};

//-----------------------------------------------------------------------------------------------
/// 퀘스트 스트링 테이블
//-----------------------------------------------------------------------------------------------
struct stQuestTypeTable : public stTypeTable
{
	std::string strName;
	std::string strStartMsg;
	std::string strEndMsg;
	std::string strDesc;

	bool operator < ( std::string& rhs )
	{
		return this->strKey < rhs;
	}
};


typedef JED::CResourceContainer< stItemTypeTable >				ItemTypeMap;
typedef JED::CResourceContainer< stNormalTypeTable >			NormalTypeMap;
typedef JED::CResourceContainer< stQuestTypeTable >				QuestTypeMap;

//typedef CResourceContainer< stItemTypeTable >::iterator		ItemTypeMapItor;
//typedef CResourceContainer< stNormalTypeTable >::iterator	NormalTypeMapItor;
//typedef CResourceContainer< stQuestTypeTable >::iterator		QuestTypeMapItor;


//-----------------------------------------------------------------------------------------------
/// 게임내 사용될 스트링들을 외부 스트링과 연결 관리..
//-----------------------------------------------------------------------------------------------
class CStringManager  : public CJSingleton< CStringManager >
{
private:
	LANGUAGE_TYPE	m_iLanguageType;

	ItemTypeMap		m_ItemTypeTables[ MAX_ITEM_TYPE_TB ];
	NormalTypeMap	m_NormalTypeTables[ MAX_NORMAL_TYPE_TB ];
	QuestTypeMap	m_QuestTypeTables[ MAX_QUEST_TYPE_TB ];

	char			m_TempBuf[1024];

	std::string	m_strNull;


public:
	CStringManager(void);
	~CStringManager(void);

	//-----------------------------------------------------------------------------------------------
	/// 캐릭터 셋으로 게임내 언어 아이디를 구한다.
	//-----------------------------------------------------------------------------------------------
	LANGUAGE_TYPE	GetLanguageIDInGame( int iCharSet );

	//-----------------------------------------------------------------------------------------------
	/// 캐릭터 셋으로 미리 선정된 폰트를 리턴.
	//-----------------------------------------------------------------------------------------------
	char*			GetFontNameByCharSet( int iCharSet );


	//-----------------------------------------------------------------------------------------------
	/// 스트링테이블 초기화( 메모리 풀에 관한 작업 )
	//-----------------------------------------------------------------------------------------------
	void			InitStringTables();
	void			ClearStringTables(); 

	//-----------------------------------------------------------------------------------------------
	/// 모든 테이블 로드 및 클리어( 실제 로드한 스트링에 대한 작업 )
	//-----------------------------------------------------------------------------------------------
	bool			LoadTables( int iCharSet );
	void			ClearTables();
	void			ClearTypeTable();


	//-----------------------------------------------------------------------------------------------
	/// 특정 타입 테이블 로드
	//-----------------------------------------------------------------------------------------------
	bool			LoadTypeTable( LANGUAGE_TYPE languageType, const char* pszFileName , int iStringType );
	

	//-----------------------------------------------------------------------------------------------
	/// String access methodes
	//-----------------------------------------------------------------------------------------------
	//const char*	GetItemString( int iType, int iItemNO );

	stItemTypeTable* GetItemStringData( int iType, int iItemNo );
	const char* GetItemName( int iType, int iItemNo );
	const char* GetItemDesc( int iType, int iItemNo );

	stItemTypeTable* GetSkillStringData( int iSkillNo );
	const char* GetSkillName( int iSkillNo );
	const char* GetSkillDesc( int iSkillNo );
	
	stQuestTypeTable* GetQuestStringData( int iQuestNo );
	const char* GetQuestName( int iQuestNo );
	const char* GetQuestStartMsg( int iQuestNo );
	const char* GetQuestEndMsg( int iQuestNo );
	const char* GetQuestDesc( int iQuestNo );

	stQuestTypeTable* GetStatusStringData( int iStatusNo );
	const char* GetStatusName( int iStatusNo );
	const char* GetStatusStartMsg( int iStatusNo );
	const char* GetStatusEndMsg( int iStatusNo );
	//-----------------------------------------------------------------------------------------------
	/// NormalType Table String access methodes
	//-----------------------------------------------------------------------------------------------
	char* GetString( int iIndex );
	char* GetJobName( int iIndex );
	char* GetSkillType( int iIndex );
	char* GetSkillTarget( int iIndex );
	char* GetItemType( int iIndex );
	char* GetAbility( int iIndex );
	const char* GetStoreTabName( int iIndex );
	const char* GetClassName( int iIndex );
	const char* GetUnionName( int iIndex );
	const char* GetClanClass( int iClassNo );
	const char* GetPlanetName( int iPlanetNo );
	const std::string&  GetItemPrefix( int iIndex );
	//-----------------------------------------------------------------------------------------------
	/// Npc String
	//-----------------------------------------------------------------------------------------------
	const char*	GetNpcName( int iIndex );
	const char*	GetNpcDesc( int iIndex );

	//-----------------------------------------------------------------------------------------------
	/// Zone String
	//-----------------------------------------------------------------------------------------------
	stItemTypeTable* GetZoneStringData( int iZoneNO );
	const char* GetZoneName( int iZoneNO );
	const char* GetZoneDesc( int iZoneNO );

	//-----------------------------------------------------------------------------------------------
	/// Tip String
	//-----------------------------------------------------------------------------------------------
	const char* GetTipHeader( int iIndex );
	const char* GetTipContent( int iIndex );


	//-----------------------------------------------------------------------------------------------
	/// Notify Message String
	//-----------------------------------------------------------------------------------------------
	const char* GetNotifyMessageString( int iIndex );


private:
	char* GetNormalTypeTableString( int iType, std::string& strKey );
	
};


#endif //_STRING_MANAGER_