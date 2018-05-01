#include "stdafx.h"
#include "stringmanager.h"
#include "../Util/VFSManager.h"
#include "../Util/Localizing.h"
#include "../Common/IO_STB.h"
#include "../Common/IO_skill.h"
#include "../Common/IO_Quest.h"
#include "../Interface/CHelpMgr.h"
#include "../interface/it_MGR.h"
#include "item.h"

#include <algorithm>

/// for singleton
CStringManager _stringMGR;


const char* _ItemTypeTable[ MAX_ITEM_TYPE_TB ] = 
{ 
	"3DData\\STB\\LIST_ARMS_s.stl",
	"3DData\\STB\\LIST_BACK_s.stl",
	"3DData\\STB\\LIST_BODY_s.stl",

	"3DData\\STB\\LIST_CAP_s.stl",
	"3DData\\STB\\LIST_FACEITEM_s.stl",
	"3DData\\STB\\LIST_FOOT_s.stl",

	"3DData\\STB\\LIST_JEMITEM_s.stl",
	"3DData\\STB\\LIST_JEWEL_s.stl",
	"3DData\\STB\\LIST_NATURAL_s.stl",

	"3DData\\STB\\LIST_PAT_s.stl",
	"3DData\\STB\\LIST_QUESTITEM_s.stl",
	"3DData\\STB\\LIST_SKILL_s.stl",

	"3DData\\STB\\LIST_SUBWPN_s.stl",
	"3DData\\STB\\LIST_USEITEM_s.stl",
	"3DData\\STB\\LIST_WEAPON_s.stl",	
	
	"3DData\\STB\\LIST_ZONE_s.stl",
	"3DData\\STB\\HELP_s.stl"
};

const char* _NormalTypeTable[ MAX_NORMAL_TYPE_TB ] = 
{ 
	"3DData\\STB\\STR_ITEMPREFIX.stl",
	"3DData\\STB\\LIST_NPC_s.stl",
	"3DData\\STB\\LIST_STRING.stl",		
	"3DData\\STB\\STR_ABILITY.stl",
	"3DData\\STB\\STR_JOB.stl",
	"3DData\\STB\\STR_ITEMTYPE.stl",
	"3DData\\STB\\STR_SKILLTARGET.stl",	
	"3DData\\STB\\STR_SKILLTYPE.stl",
	"3DData\\STB\\LIST_SELL_S.stl",
	"3DData\\STB\\LIST_CLASS_S.stl",
	"3DData\\STB\\LIST_UNION_S.stl",
	"3DData\\STB\\STR_CLAN.stl",
	"3DData\\STB\\STR_PLANET.stl",
	"3DData\\STB\\LIST_EventString.stl"

};

const char* _QuestTypeTable[ MAX_QUEST_TYPE_TB ] = 
{ 
	"3DData\\STB\\LIST_QUEST_s.stl",	
	"3DData\\STB\\LIST_STATUS_s.stl",	
};


const int _ItemTypeToItemTable[ ITEM_TYPE_RIDE_PART + 1 ] = 
{
	0,										
	LIST_FACEITEM_S_TB,		//ITEM_TYPE_FACE_ITEM = 1,		// 1	LIST_FACEITEM.stb	얼굴 장식	
	LIST_CAP_S_TB,			//ITEM_TYPE_HELMET,				// 2	LIST_CAP.stb
	LIST_BODY_S_TB,			//ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
	LIST_ARMS_S_TB,			//ITEM_TYPE_GAUNTLET,			// 4	LIST_ARMS.stb
	LIST_FOOT_S_TB,			//ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
	LIST_BACK_S_TB,			//ITEM_TYPE_KNAPSACK,			// 6	LIST_BACK.stb
	LIST_JEWEL_S_TB,		//ITEM_TYPE_JEWEL,				// 7	LIST_JEWEL.stb		장신구 : 목걸이 반지
	LIST_WEAPON_S_TB,		//ITEM_TYPE_WEAPON,				// 8	LIST_WEAPON.stb		무기
	LIST_SUBWPN_S_TB,		//ITEM_TYPE_SUBWPN,				// 9	LIST_SUBWPN.stb
	LIST_USEITEM_S_TB,		//ITEM_TYPE_USE,					// 10	LIST_USEITEM.stb	소모	
	LIST_JEMITEM_S_TB,		//ITEM_TYPE_GEM = ITEM_TYPE_ETC,	// 11	LIST_JEMITEM.stb	기타 : 보석
	LIST_NATURAL_S_TB,		//ITEM_TYPE_NATURAL,				// 12	LIST_NATURAL.stb
	LIST_QUESTITEM_S_TB,	//ITEM_TYPE_QUEST,				// 13	LIST_QUESTITEM.stb
	LIST_PAT_S_TB			//ITEM_TYPE_RIDE_PART=14,
};






int CALLBACK EnumFontFamExProc( ENUMLOGFONTEX *lpelfe,
                                NEWTEXTMETRICEX *lpntme,
                                int FontType,
                                LPARAM lParam)
{
	std::list< std::string >* pFontList = (std::list< std::string >*) lParam;
	pFontList->push_back( std::string( (char*)lpelfe->elfFullName ) ); 

	return 1; // I want to get all fonts
}





//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
CStringManager::CStringManager(void)
{
	m_iLanguageType	= LANGUAGE_KOR;
	m_strNull = "";

}

//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
CStringManager::~CStringManager(void)
{
	this->ClearStringTables();
}


LANGUAGE_TYPE CStringManager::GetLanguageIDInGame( int iCharSet )
{
	switch( iCharSet )
	{
		case SHIFTJIS_CHARSET:
			return LANGUAGE_JPN;
		case HANGUL_CHARSET:
			return LANGUAGE_KOR;
		case GB2312_CHARSET:
			return LANGUAGE_CHA_SIMPLE;
			
		case CHINESEBIG5_CHARSET:
			return LANGUAGE_CHA_TRADITIONAL;
			

		case GREEK_CHARSET:			
		case TURKISH_CHARSET:			
		case HEBREW_CHARSET:			
		case ARABIC_CHARSET:			
		case BALTIC_CHARSET:			
		case THAI_CHARSET:			
		case EASTEUROPE_CHARSET:
		default:			
			return LANGUAGE_USA;
	}
}


char* CStringManager::GetFontNameByCharSet( int iCharSet )
{
	m_iLanguageType = GetLanguageIDInGame( iCharSet );

	HRESULT hr = S_OK;
	
	switch( m_iLanguageType )
	{
		case LANGUAGE_KOR:
			{
				return "굴림체";
			}
			break;
		case LANGUAGE_USA:
			return "Verdana";///"Arial"
			
		case LANGUAGE_JPN:
			{				
				LOGFONT lf;
				//to enumerate all styles of Arial font that cover the ANSI charset
				strcpy( (LPSTR)&lf.lfFaceName, "굃굍 굊긕긘긞긏" );
				/*if (FAILED(hr))
				{
					assert( 0 && "StringCchCopy error" );
				}*/
				lf.lfCharSet = iCharSet;

				std::list< std::string > fontList;
				HDC hdc = ::GetDC( g_pCApp->GetHWND() );
				::EnumFontFamiliesEx(	hdc, 
										&lf, 
										(FONTENUMPROC) EnumFontFamExProc, 
										(LPARAM) &fontList, 0);

				/// "MS Gothic" 가 없다.
				if( fontList.empty() )
				{
					return "MS PGothic";
				}

				return "굃굍 굊긕긘긞긏";
			}
			break;
		case LANGUAGE_CHA_SIMPLE:			
			return "SimSun";

		case LANGUAGE_CHA_TRADITIONAL:
			return "MingLiU";
	}
	return "Arial";
}

//-----------------------------------------------------------------------------------------------
/// 스트링테이블 초기화
//-----------------------------------------------------------------------------------------------
void CStringManager::InitStringTables()
{
	/*m_ItemTypeTables[ MAX_ITEM_TYPE_TB ];
	m_NormalTypeTables[ MAX_NORMAL_TYPE_TB ];
	m_QuestTypeTables[ MAX_QUEST_TYPE_TB ];*/
}

void CStringManager::ClearStringTables()
{
	int i = 0;

	for( i = 0; i < MAX_ITEM_TYPE_TB; i++ )
		m_ItemTypeTables[ i ].ClearPool();

	for( i = 0; i < MAX_NORMAL_TYPE_TB; i++ )
		m_NormalTypeTables[ i ].ClearPool();

	for( i = 0; i < MAX_QUEST_TYPE_TB; i++ )
		m_QuestTypeTables[ i ].ClearPool();	
}


//------------------------------------------------------------------------------------------
// Noraml Type Table중 Item Type Table에 사용되는 Data가 있다.. 먼저 Loading할것 - 2005/1/11 - nAvy
//------------------------------------------------------------------------------------------
bool CStringManager::LoadTables( int iCharSet )
{

	InitStringTables();

	m_iLanguageType = GetLanguageIDInGame( iCharSet );

	//--------------------------------------------------------
	// Normal Type Table
	//--------------------------------------------------------
	for( int i = 0; i < MAX_NORMAL_TYPE_TB; i++ )
	{
		if( !LoadTypeTable( m_iLanguageType, _NormalTypeTable[ i ] ,i ) )
		{
			sprintf( g_itMGR.m_MessageBuf, "%s open error", _NormalTypeTable[ i ] );
			g_itMGR.OpenMsgBox( g_itMGR.m_MessageBuf );
		}
	}

	//--------------------------------------------------------
	// Item Type Table
	//--------------------------------------------------------
	for( int i = 0; i < MAX_ITEM_TYPE_TB; i++ )
	{
		if(	!LoadTypeTable( m_iLanguageType, _ItemTypeTable[ i ] ,i ) )
		{			
			sprintf( g_itMGR.m_MessageBuf, "%s open error", _ItemTypeTable[ i ] );
			g_itMGR.OpenMsgBox( g_itMGR.m_MessageBuf );
		}
	}


	//--------------------------------------------------------
	// Quest Type Table
	//--------------------------------------------------------
	for( int i = 0; i < MAX_QUEST_TYPE_TB; i++ )
	{
		if( !LoadTypeTable( m_iLanguageType, _QuestTypeTable[ i ] ,i ) )
		{
			sprintf( g_itMGR.m_MessageBuf, "%s open error", _QuestTypeTable[ i ] );
			g_itMGR.OpenMsgBox( g_itMGR.m_MessageBuf );
		}
	}

	return true;
}


//---------------------------------------------------------------------------------------------
/// 모든 로드된 테이블 프리..
/// 현재 모든 스트링 테이블 구조테는 리소스 컨테이너의 메모리 풀로 부터 나온다.
/// 할당및 해제에대한 모든 책임은 리소스 컨테이너에게 있다.
//---------------------------------------------------------------------------------------------
void CStringManager::ClearTables()
{
	////--------------------------------------------------------
	//// Item Type Table
	////--------------------------------------------------------
	//for( int i = 0; i < MAX_ITEM_TYPE_TB; i++ )
	//{
	//	ClearTypeTable( _ItemTypeTable[ i ] );		
	//}

	////--------------------------------------------------------
	//// Normal Type Table
	////--------------------------------------------------------
	//for( int i = 0; i < MAX_NORMAL_TYPE_TB; i++ )
	//{
	//	ClearTypeTable( _NormalTypeTable[ i ] );		
	//}

	////--------------------------------------------------------
	//// Quest Type Table
	////--------------------------------------------------------
	//for( int i = 0; i < MAX_QUEST_TYPE_TB; i++ )
	//{
	//	ClearTypeTable( _QuestTypeTable[ i ] );	
	//}
}

//---------------------------------------------------------------------------------------------
// 한 타입 테이블 프리
//---------------------------------------------------------------------------------------------
void CStringManager::ClearTypeTable()
{
}






////------------------------------------------------------------------------------------------
/////
////------------------------------------------------------------------------------------------
//void CStringManager::InitTempKeyVector( int iSize )
//{
//	///m_pIndexToKey = new std::vector< std::string >;
//	m_IndexToKey.reserve( iSize );	
//	
//	m_iIndexToKeyTableSize = iSize;
//}
//
////------------------------------------------------------------------------------------------
/////
////------------------------------------------------------------------------------------------
//void CStringManager::ClearTempKeyVector()
//{
//	m_IndexToKey.clear();	
//	std::vector< std::string >().swap( m_IndexToKey );
//}


//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
bool CStringManager::LoadTypeTable( LANGUAGE_TYPE languageType, const char* pszFileName , int iStringType )
{
	int		i = 0;	
	
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( pszFileName ) == false )	
	{			
		assert( 0 && "File Not Found" );
		return false;
	}

	//--------------------------------------------------------
	// Get Table Type
	//--------------------------------------------------------
	int	 iTableType = 0;
	char strStlType[32];
	pFileSystem->ReadPascalString( strStlType, sizeof( strStlType ) );
	
	if( strcmpi( strStlType, "ITST01" ) == 0 )
	{
		iTableType = ITEM_TYPE_ST;
	}else if( strcmpi( strStlType, "NRST01" ) == 0 )
	{
		iTableType = NORMAL_TYPE_ST;
	}else if( strcmpi( strStlType, "QEST01" ) == 0 )
	{
		iTableType = QUEST_TYPE_ST;
	}else
	{
		pFileSystem->CloseFile();
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
		return false;
	}




	//--------------------------------------------------------
	// Read key data
	//--------------------------------------------------------
	int iKeyCount = 0;
	int iIndex = 0;
	pFileSystem->ReadInt32( &iKeyCount );


	/// 뒤에서 키 신퀀스 인덱스에 대응하는 아이템타입객체
	 stTypeTable** pTempTypeList = NULL;
	 try
	 {
		pTempTypeList = new stTypeTable*[ iKeyCount ];
	 }catch( ... )
	 {
		 MessageBox( NULL, "Not enough memory", "ERROR", MB_OK );

		 pFileSystem->CloseFile();
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

		return false;
	 }
	 
	 LogString( LOG_NORMAL, "KeyCount [ %d ]\n", iKeyCount );

	/*switch( iTableType )
	{
		case ITEM_TYPE_ST: 
			m_ItemTypeTables[ iStringType ].reserve( iKeyCount );
			break;
		case NORMAL_TYPE_ST: 
			m_NormalTypeTables[ iStringType ].reserve( iKeyCount );
			break;
	}*/

	


	for( i = 0; i < iKeyCount ; i++ )
	{
		int iStrLen = pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
		(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );
		pFileSystem->ReadInt32( &iIndex );		

		switch( iTableType )
		{
			case ITEM_TYPE_ST:
				{
					stItemTypeTable* itTable = m_ItemTypeTables[ iStringType ].GetObjectInstanceFromPool();
					//stItemTypeTable* itTable = new stItemTypeTable;
					itTable->strKey = m_TempBuf;
					itTable->iIndex = iIndex;
					m_ItemTypeTables[ iStringType ].AddObject( m_TempBuf, itTable );					

					pTempTypeList[ i ] = (stTypeTable*)itTable;
				}
				break;

			case NORMAL_TYPE_ST:
				{
					stNormalTypeTable* itTable = m_NormalTypeTables[ iStringType ].GetObjectInstanceFromPool();
					//stNormalTypeTable* itTable = new stNormalTypeTable;
					itTable->strKey = m_TempBuf;
					itTable->iIndex = iIndex;
					m_NormalTypeTables[ iStringType ].AddObject( m_TempBuf, itTable );					

					pTempTypeList[ i ] = (stTypeTable*)itTable;
				}
				break;
			case QUEST_TYPE_ST:
				{					
					stQuestTypeTable* itTable = m_QuestTypeTables[ iStringType ].GetObjectInstanceFromPool();
					//stQuestTypeTable* itTable = new stQuestTypeTable;
					itTable->strKey = m_TempBuf;
					itTable->iIndex = iIndex;
					m_QuestTypeTables[ iStringType ].AddObject( m_TempBuf, itTable );	

					pTempTypeList[ i ] = (stTypeTable*)itTable;
				}
				break;
		}
	}



	//--------------------------------------------------------
	// Language set offset
	//--------------------------------------------------------
	int iLanguageSetCnt = 0;
	pFileSystem->ReadInt32( &iLanguageSetCnt );	
	pFileSystem->Seek( languageType * sizeof( int ), FILE_POS_CUR );

	int iTargetLangPos = 0;
	pFileSystem->ReadInt32( &iTargetLangPos ); 

	/// 해당 언더 블락으로 이동..
	pFileSystem->Seek( iTargetLangPos, FILE_POS_SET );	


	//--------------------------------------------------------
	// Language set data
	//--------------------------------------------------------
	/// 스트링을 블락으로 읽었을때 참조할 옵셋.
	for( i = 0; i < iKeyCount ; i++ )
	{
		int iDataOffset = 0;
		pFileSystem->ReadInt32( &iDataOffset );		
	}



	/// 실제 스트링들..
	for( i = 0; i < iKeyCount ; i++ )
	{		
		switch( iTableType )
		{
			case ITEM_TYPE_ST:
				{
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );
					
					///stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ iStringType ].GetObjectByName( m_IndexToKey[ i ].c_str() );
					stItemTypeTable* pItemTypeTable = (stItemTypeTable*)pTempTypeList[ i ];

					if( pItemTypeTable )
						pItemTypeTable->strName = m_TempBuf;

					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );					

					if( pItemTypeTable )
						pItemTypeTable->strDesc = m_TempBuf;					
				}
				break;

			case NORMAL_TYPE_ST:
				{
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );
					///stNormalTypeTable* pNormalTypeTable = m_NormalTypeTables[ iStringType ].GetObjectByName( m_IndexToKey[ i ].c_str() );
					stNormalTypeTable* pNormalTypeTable = (stNormalTypeTable*)pTempTypeList[ i ];

					if( pNormalTypeTable )
						pNormalTypeTable->strString = m_TempBuf;					
				}
				break;
			case QUEST_TYPE_ST:
				{
					/// Name
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );
					///stQuestTypeTable* pQuestTypeTable = m_QuestTypeTables[ iStringType ].GetObjectByName( m_IndexToKey[ i ].c_str() );
					stQuestTypeTable* pQuestTypeTable = (stQuestTypeTable*)pTempTypeList[ i ];

					if( pQuestTypeTable )
						pQuestTypeTable->strName = m_TempBuf;					

					/// Desc
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );

					if( pQuestTypeTable )
						pQuestTypeTable->strDesc = m_TempBuf;
					
					/// Start msg
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );

					if( pQuestTypeTable )
						pQuestTypeTable->strStartMsg = m_TempBuf;
					
					/// End msg
					pFileSystem->ReadPascalString( m_TempBuf, sizeof( m_TempBuf ) );
					(CLocalizing::GetSingleton()).UTF8ToMBCS( m_TempBuf, sizeof( m_TempBuf ) );

					if( pQuestTypeTable )
						pQuestTypeTable->strEndMsg = m_TempBuf;					
				}
				break;
		}		
	}
	
	delete[] pTempTypeList;
	
	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
    
	return true;
}


//-----------------------------------------------------------------------------------------------
/// String access methodes
//-----------------------------------------------------------------------------------------------
//const char* CStringManager::GetItemString( int iType, int iItemNO )
//{
//	if( iType < 1 || iType > ITEM_TYPE_RIDE_PART )
//		return NULL;
//
//	std::string strKey( ITEM_DESC( iType, iItemNO ) );
//
//	if( m_ItemTypeTables[  _ItemTypeToItemTable[ iType ] ].find( strKey ) == m_ItemTypeTables[  _ItemTypeToItemTable[ iType ] ].end() )
//	{
//		assert( 0 && "GetItemString is failed" );
//		return NULL;
//	}
//
//	return m_ItemTypeTables[  _ItemTypeToItemTable[ iType ] ][ strKey ].strDesc.c_str();
//	/*std::equal_range( m_ItemTypeTables[ _ItemTypeToItemTable[ iType ] ].begin(),
//						m_ItemTypeTables[ _ItemTypeToItemTable[ iType ] ].end(),
//						strKey,
//						LessThen_TableEntity< stItemTypeTable >() );				*/
//}


//---------------------------------------------------------------------------------------
///
/// For item type string table function
///
//---------------------------------------------------------------------------------------

stItemTypeTable* CStringManager::GetItemStringData( int iType, int iItemNo )
{
	if( iType < 1 || iType > ITEM_TYPE_RIDE_PART )
		return NULL; 

	if( g_pTblSTBs[iType]->m_ppDESC[ iItemNo ] == NULL )
		return NULL;

	std::string strKey( g_pTblSTBs[iType]->m_ppDESC[ iItemNo ] );

	stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ _ItemTypeToItemTable[ iType ] ].GetObjectByName( strKey.c_str() );
	if( pItemTypeTable == NULL )
	{
		return NULL;
	}

	return pItemTypeTable;
}

//---------------------------------------------------------------------------------------
/// item name
//---------------------------------------------------------------------------------------
const char* CStringManager::GetItemName( int iType, int iItemNo )
{ 
	
	stItemTypeTable* pItemTypeTable = GetItemStringData( iType, iItemNo );
	if( pItemTypeTable == NULL )
		return m_strNull.c_str(); 

	const std::string& prefix = GetItemPrefix( CItem::GetItemRareType( iType, iItemNo ));

	if( prefix.empty() )
		return pItemTypeTable->strName.c_str();

	return CStr::Printf("%s %s",prefix.c_str() , pItemTypeTable->strName.c_str() );
}

//---------------------------------------------------------------------------------------
/// item desc
//---------------------------------------------------------------------------------------
const char* CStringManager::GetItemDesc( int iType, int iItemNo )
{ 	

	stItemTypeTable* pItemTypeTable = GetItemStringData( iType, iItemNo );
	if( pItemTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}

	return pItemTypeTable->strDesc.c_str();
}

//---------------------------------------------------------------------------------------
/// skill...
//---------------------------------------------------------------------------------------
stItemTypeTable* CStringManager::GetSkillStringData( int iSkillNo )
{
	if( g_SkillList.m_SkillDATA.m_ppDESC[ iSkillNo ] == NULL )
		return NULL;

	std::string strKey( g_SkillList.m_SkillDATA.m_ppDESC[ iSkillNo ] );

	stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ LIST_SKILL_S_TB ].GetObjectByName( strKey.c_str() );
	if( pItemTypeTable == NULL )
	{
		return NULL; 
	}

	return pItemTypeTable;
}

const char* CStringManager::GetSkillName( int iSkillNo )
{ 

	stItemTypeTable* pItemTypeTable = GetSkillStringData( iSkillNo );
	if( pItemTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}

	return pItemTypeTable->strName.c_str();
}

const char* CStringManager::GetSkillDesc( int iSkillNo )
{ 

	stItemTypeTable* pItemTypeTable = GetSkillStringData( iSkillNo );
	if( pItemTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}

	return pItemTypeTable->strDesc.c_str();
}



//---------------------------------------------------------------------------------------
///
/// For item type string table function
///
//---------------------------------------------------------------------------------------
stQuestTypeTable* CStringManager::GetStatusStringData( int iStatusNo )
{
	if( iStatusNo < 0 || iStatusNo >= g_TblSTATE.m_nDataCnt )
		return NULL;

	if( STATE_STRING_ID( iStatusNo ) == NULL )
		return NULL;

	std::string strKey( STATE_STRING_ID( iStatusNo ) );
	return m_QuestTypeTables[ LIST_STATUS_S ].GetObjectByName( strKey.c_str() );
}

const char* CStringManager::GetStatusName( int iStatusNo )
{
	stQuestTypeTable* pQuestTypeTable = GetStatusStringData( iStatusNo );
	if( pQuestTypeTable == NULL )
		return m_strNull.c_str(); 

	return pQuestTypeTable->strName.c_str();
}

const char* CStringManager::GetStatusStartMsg( int iStatusNo )
{
	stQuestTypeTable* pQuestTypeTable = GetStatusStringData( iStatusNo );
	if( pQuestTypeTable == NULL )
		return m_strNull.c_str(); 

	///잘못된것 아님.. 2번째 컬럼에 Setting Msg가 들어가 있다.
	return pQuestTypeTable->strDesc.c_str();
}

const char* CStringManager::GetStatusEndMsg( int iStatusNo )
{
	stQuestTypeTable* pQuestTypeTable = GetStatusStringData( iStatusNo );
	if( pQuestTypeTable == NULL )
		return m_strNull.c_str(); 

	///잘못된것 아님.. 3번째 컬럼에 Delete Msg가 들어있다.
	return pQuestTypeTable->strStartMsg.c_str();
}

stQuestTypeTable* CStringManager::GetQuestStringData( int iQuestNo )
{
	if( g_QuestList.m_STB.m_ppDESC[ iQuestNo ] == NULL )
		return NULL;

	std::string strKey( g_QuestList.m_STB.m_ppDESC[ iQuestNo ] );
	
	stQuestTypeTable* pQuestTypeTable = m_QuestTypeTables[ LIST_QUEST_S_TB ].GetObjectByName( strKey.c_str() );
	if( pQuestTypeTable == NULL )
	{
		return NULL; 
	}	

	return pQuestTypeTable;
}

/// Quest name
const char* CStringManager::GetQuestName( int iQuestNo )
{ 		
	stQuestTypeTable* pQuestTypeTable = GetQuestStringData( iQuestNo );
	if( pQuestTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}	

	return pQuestTypeTable->strName.c_str();
}

/// Quest start msg
const char* CStringManager::GetQuestStartMsg( int iQuestNo )
{ 	
	stQuestTypeTable* pQuestTypeTable = GetQuestStringData( iQuestNo );
	if( pQuestTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}	

	return pQuestTypeTable->strStartMsg.c_str();
}

/// Quest end msg
const char* CStringManager::GetQuestEndMsg( int iQuestNo )
{ 	
	stQuestTypeTable* pQuestTypeTable = GetQuestStringData( iQuestNo );
	if( pQuestTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}	

	return pQuestTypeTable->strEndMsg.c_str();
}

/// Quest desc
const char* CStringManager::GetQuestDesc( int iQuestNo )
{ 
	stQuestTypeTable* pQuestTypeTable = GetQuestStringData( iQuestNo );
	if( pQuestTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}	

	return pQuestTypeTable->strDesc.c_str();
}



//-----------------------------------------------------------------------------------------------
/// Npc String
//-----------------------------------------------------------------------------------------------
const char*	CStringManager::GetNpcName( int iIndex )
{
	if( NPC_STRING_ID( iIndex ) == NULL )
		return m_strNull.c_str();

	std::string strKey( NPC_STRING_ID( iIndex ) );

	return GetNormalTypeTableString( LIST_NPC_S_TB, strKey );
}

const char*	CStringManager::GetNpcDesc( int iIndex )
{
	return m_strNull.c_str();
}




//-----------------------------------------------------------------------------------------------
/// NormalType Table String access methodes
//-----------------------------------------------------------------------------------------------
char* CStringManager::GetNormalTypeTableString( int iType, std::string& strKey )
{
	assert( iType >= 0 && iType < MAX_NORMAL_TYPE_TB );
	assert( !strKey.empty() );

	stNormalTypeTable* pNormalTypeTable = m_NormalTypeTables[ iType ].GetObjectByName( strKey.c_str() );
	if( pNormalTypeTable == NULL )
	{
		return (char*)m_strNull.c_str(); 
	}		
	
	return (char*)(pNormalTypeTable->strString.c_str());
}

const std::string& CStringManager::GetItemPrefix( int iIndex )
{
	std::string strKey( CStr::Printf("%d",iIndex));

	stNormalTypeTable* pNormalTypeTable = m_NormalTypeTables[ STR_ITEMPREFIX ].GetObjectByName( strKey.c_str() );
	if( pNormalTypeTable == NULL )
		return m_strNull; 
	
	return pNormalTypeTable->strString;
}

char* CStringManager::GetString( int iIndex )
{
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( LIST_STRING_S_TB, strKey );
}

const char* CStringManager::GetClanClass( int iClassNo )
{
	std::string strKey( CStr::Printf("%d",iClassNo));
	return GetNormalTypeTableString( STR_CLAN, strKey );
}

const char* CStringManager::GetPlanetName( int iPlanetNo )
{
	std::string strKey( CStr::Printf("%d",iPlanetNo ));
	return GetNormalTypeTableString( STR_PLANET, strKey );
}

char* CStringManager::GetJobName( int iIndex )
{ 
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_JOBNAME, strKey );
}

char* CStringManager::GetSkillType( int iIndex )
{ 
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_SKILLTYPE, strKey );
}

char* CStringManager::GetSkillTarget( int iIndex )
{ 
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_SKILLTARGET, strKey );
}

char* CStringManager::GetItemType( int iIndex )
{ 
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_ITEMTYPE, strKey );
}

char* CStringManager::GetAbility( int iIndex )
{ 
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_ABILITY, strKey );
}

const char* CStringManager::GetStoreTabName( int iIndex )
{
	if( iIndex < 0 || iIndex > g_TblStore.m_nDataCnt )
		return m_strNull.c_str();

	if( g_TblStore.m_ppVALUE[ iIndex ][ 1 ].GetSTR() )
	{
		std::string strKey( g_TblStore.m_ppVALUE[ iIndex ][ 1 ].GetSTR() );
		return GetNormalTypeTableString( LIST_SELL_S, strKey );
	}
	return m_strNull.c_str();
}

const char* CStringManager::GetUnionName( int iIndex )
{
	if( iIndex < 0 || iIndex >= g_TblUnion.m_nDataCnt )
		return m_strNull.c_str();

	if( UNION_STRING_ID( iIndex ) )
	{
		std::string strKey( UNION_STRING_ID( iIndex ) );
		return GetNormalTypeTableString( LIST_UNION_S, strKey );
	}
	return m_strNull.c_str();
}

const char* CStringManager::GetClassName( int iIndex )
{
	if( iIndex < 0 || iIndex >= g_TblClass.m_nDataCnt )
		return m_strNull.c_str();

	if( CLASS_STRING_ID( iIndex ) )
	{
		std::string strKey( CLASS_STRING_ID( iIndex ) );
		return GetNormalTypeTableString( LIST_CLASS_S, strKey );
	}
	return m_strNull.c_str();
}

//-----------------------------------------------------------------------------------------------
/// Zone String
//-----------------------------------------------------------------------------------------------
stItemTypeTable* CStringManager::GetZoneStringData( int iZoneNO )
{
	if( ZONE_STRING_ID( iZoneNO ) == NULL )
		return NULL;

	std::string strKey( ZONE_STRING_ID( iZoneNO ) );

	stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ LIST_ZONE_S_TB ].GetObjectByName( strKey.c_str() );
	if( pItemTypeTable == NULL )
	{
		return NULL;
	}

	return pItemTypeTable;
}

const char* CStringManager::GetZoneName( int iZoneNO )
{

	stItemTypeTable* pItemTypeTable = GetZoneStringData( iZoneNO );
	if( pItemTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}

	return pItemTypeTable->strName.c_str();
}

const char* CStringManager::GetZoneDesc( int iZoneNO )
{
	stItemTypeTable* pItemTypeTable = GetZoneStringData( iZoneNO );
	if( pItemTypeTable == NULL )
	{
		return m_strNull.c_str(); 
	}

	return pItemTypeTable->strDesc.c_str();
}




const char* CStringManager::GetTipHeader( int iIndex )
{
	assert( iIndex >= 0 && iIndex < g_TblHELP.m_nDataCnt );
	if( iIndex < 0 || iIndex >= g_TblHELP.m_nDataCnt )
		return m_strNull.c_str(); 

	assert( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() );
	if( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() == NULL )
		return m_strNull.c_str(); 

	std::string strKey( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() );

	stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ HELP_S_TB ].GetObjectByName( strKey.c_str() );
	if( pItemTypeTable == NULL )
		return m_strNull.c_str(); 

	return pItemTypeTable->strName.c_str();
}

const char* CStringManager::GetTipContent( int iIndex )
{
	assert( iIndex >= 0 && iIndex < g_TblHELP.m_nDataCnt );
	if( iIndex < 0 || iIndex >= g_TblHELP.m_nDataCnt )
		return m_strNull.c_str(); 

	assert( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() );
	if( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() == NULL )
		return m_strNull.c_str(); 

	std::string strKey( g_TblHELP.m_ppVALUE[iIndex][5].GetSTR() );

	stItemTypeTable* pItemTypeTable = m_ItemTypeTables[ HELP_S_TB ].GetObjectByName( strKey.c_str() );
	if( pItemTypeTable == NULL )
		return m_strNull.c_str(); 

	return pItemTypeTable->strDesc.c_str();

}


//-----------------------------------------------------------------------------------------------
/// Notify Message String
//-----------------------------------------------------------------------------------------------
const char* CStringManager::GetNotifyMessageString( int iIndex )
{
	std::string strKey( CStr::Printf("%d",iIndex));
	return GetNormalTypeTableString( STR_NOTIFY_MESSAGE, strKey );
}