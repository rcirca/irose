#include "stdafx.h"
#include "IO_ImageRes.h"
#include "Game.h"
#include "Util/VFSManager.h"
#include "../Util/CFileSystem.h"
#include "ResourceMgr.h"
#include <algorithm>

using namespace std;



////////////////////////////////////////////////////////////////////////////////////////
/// Image resource for UI
////////////////////////////////////////////////////////////////////////////////////////

CImageRes::CImageRes()
{
}

CImageRes::~CImageRes()
{
}

/// Load TSI file
bool CImageRes::LoadRES( CFileSystem* pFileSystem, const char* strResPath )
{
	if( pFileSystem == NULL )
		return false;

	setDelayedLoad(0);
	char					szTXFileName[MAX_PATH];

	//RES폴더에 스프라이트 파일과 텍스쳐파일이 같이 있다..		
	stTexture				tempTexture;
	
	short nTextureCNT = 0;
	short nSpriteCNT  = 0;
	short nLen;

	pFileSystem->ReadInt16( &nTextureCNT );

	int iSize = m_TextureVec.size();

	m_TextureVec.reserve( nTextureCNT );

	short nTX_ID = 0;
	for( short nC = 0 ; nC < nTextureCNT ; nC++ ) 
	{
		//같은 이름이 있으면 로드하지 않음	
		bool			bContinue = false;

		ZeroMemory( tempTexture.m_szName, sizeof( tempTexture.m_szName ) );
		
		pFileSystem->ReadInt16( &nLen );
		pFileSystem->Read( tempTexture.m_szName, sizeof(char) * nLen );
		pFileSystem->Read( &tempTexture.m_dwColorKey, sizeof( DWORD ) );

		//지금까지 등록된 이름을 비교한다 
		for(short nB=0 ; nB < nC ; nB++ ) 
		{
			if( stricmp( m_TextureVec[ nB ].m_szName, tempTexture.m_szName ) == 0 ) 
			{
				tempTexture.m_nTXID = nB;
				tempTexture.m_Texture  = NULL;
				m_TextureVec.push_back( tempTexture );				
				bContinue = true;
				break;
				
			}
		}
		if(bContinue) continue;

		sprintf( szTXFileName, "%s\\Res\\%s", strResPath, tempTexture.m_szName );
		
		/// Texture load
		tempTexture.m_Texture = ::loadTexture( szTXFileName, szTXFileName, 1, 0 );
		if( tempTexture.m_Texture == 0 )
		{
			return false;
		}
		else 
		{
			// 리스트에 등록 
			tempTexture.m_nTXID = nC;
			m_TextureVec.push_back( tempTexture );			
		}		
	}

	pFileSystem->ReadInt16( &m_nSpriteCNT );
	m_SpriteVec.reserve( m_nSpriteCNT );
	
	stSprite	tempSprite;

	short nG;
	short nL;
	short nI;
	/// 각각의 텍스쳐에 할당된 개수만큼..
	for( nI = 0, nL = 0 ; nI < nTextureCNT ; nI++ ) 
	{
		pFileSystem->ReadInt16( &nLen );

		for (nG=0; nG<nLen; nG++, nL++) 
		{
			pFileSystem->ReadInt16	( &tempSprite.m_nTextureID);
			pFileSystem->Read		( &tempSprite.m_Rect,	 sizeof(RECT) );
			pFileSystem->Read		( &tempSprite.m_dwColor, sizeof(DWORD) );
			
			pFileSystem->Read		( &tempSprite.m_szID, sizeof( tempSprite.m_szID ));			

			//이전에 같은게 있으면 그번호로 
			short nID = tempSprite.m_nTextureID;
			tempSprite.m_nTextureID = m_TextureVec[ nID ].m_nTXID;
			
			m_SpriteVec.push_back( tempSprite );
		}		
	}

	return true;
}

void ReleaseTexture( stTexture texture )
{
	if( texture.m_Texture != NULL )
	{
		::unloadTexture( texture.m_Texture );
		texture.m_Texture = NULL;
	}
}

void CImageRes::ReleaseRES()
{
	for_each( m_TextureVec.begin(), m_TextureVec.end(), ReleaseTexture );

	m_TextureVec.clear();
	m_SpriteVec.clear();
}


stTexture* CImageRes::GetTexture(int iSprIndex)
{
	if( (unsigned int)iSprIndex >= m_SpriteVec.size() || iSprIndex < 0 )
	{
		assert( 0 && "Argument is Invalid @CImageRes::GetSprite()" );
		return NULL;
	}

	short nTextureID = m_SpriteVec[ iSprIndex ].m_nTextureID;

	return &( m_TextureVec[ nTextureID ] );
}

stSprite* CImageRes::GetSprite( int iSprIndex )
{
	if( (unsigned int)iSprIndex >= m_SpriteVec.size() || iSprIndex < 0)
	{
		assert( 0 && "Argument is Invalid @CImageRes::GetSprite()" );
		return NULL;
	}

	return &( m_SpriteVec[ iSprIndex ] );
}





////////////////////////////////////////////////////////////////////////////////////////
/// Image res manager for UI
////////////////////////////////////////////////////////////////////////////////////////
/// for singleton
CImageResManager	__ImgResMgr;


CImageResManager::CImageResManager()
{
}

CImageResManager::~CImageResManager()
{
}

bool CImageResManager::LoadImageResources()
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();


	pFileSystem->OpenFile( "3DData\\Control\\Res\\TARGETMARK.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_TARGETMARK ].LoadRES( pFileSystem ) == false )
		return false;


	pFileSystem->OpenFile( "3DData\\Control\\Res\\CLANBACK.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_CLANBACK ].LoadRES( pFileSystem ) == false )
		return false;

	pFileSystem->OpenFile( "3DData\\Control\\Res\\CLANCENTER.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_CLANCENTER ].LoadRES( pFileSystem ) == false )
		return false;



	//EX UI
	pFileSystem->OpenFile( "3DData\\Control\\Res\\ExUi.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_EXUI ].LoadRES( pFileSystem ) == false )
		return false;

	CResourceMgr::GetInstance()->AddResource("3DData\\Control\\XML\\EXUI_strID.ID",IMAGE_RES_EXUI);

#ifdef _WARNING
	//WARNING
	pFileSystem->OpenFile( "3DData\\Control\\Res\\WARNING.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_WARNING ].LoadRES( pFileSystem ) == false )
		return false;

	CResourceMgr::GetInstance()->AddResource("3DData\\Control\\XML\\WARNING_strID.ID",IMAGE_RES_WARNING);
#endif


#ifdef _NEWUI
	pFileSystem->OpenFile( "3DData\\Control\\Res\\UI2.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_NEWUI ].LoadRES( pFileSystem ) == false )
		return false;

	CResourceMgr::GetInstance()->AddResource("3DData\\Control\\Res\\UI2_strID.ID",IMAGE_RES_NEWUI);
#endif


	pFileSystem->OpenFile( "3DData\\Control\\Res\\Ui.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_UI ].LoadRES( pFileSystem ) == false )
		return false;

	CResourceMgr::GetInstance()->AddResource("3DData\\Control\\XML\\UI_strID.ID",IMAGE_RES_UI);

	pFileSystem->OpenFile( "3DData\\Control\\Res\\Item1.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_ITEM ].LoadRES( pFileSystem ) == false )
	{		
		for( int i = 0; i < IMAGE_RES_ITEM ; i++ )
		{
			m_ImageRes[ i ].ReleaseRES();
		}
		return false;
	}

	//pFileSystem->OpenFile( "3DData\\Control\\Res\\FACE.TSI", OPEN_READ_BIN );
	//if( m_ImageRes[ IMAGE_RES_FACE ].LoadRES( pFileSystem ) == false )
	//{		
	//	for( int i = 0; i < IMAGE_RES_ITEM ; i++ )
	//	{
	//		m_ImageRes[ i ].ReleaseRES();
	//	}
	//	return false;
	//}

	pFileSystem->OpenFile( "3DData\\Control\\Res\\SKILLICON.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_SKILL_ICON ].LoadRES( pFileSystem ) == false )
	{		
		for( int i = 0; i < IMAGE_RES_ITEM ; i++ )
		{
			m_ImageRes[ i ].ReleaseRES();
		}
		return false;
	}

	pFileSystem->OpenFile( "3DData\\Control\\Res\\STATEICON.TSI", OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_STATE_ICON ].LoadRES( pFileSystem ) == false )
	{		
		for( int i = 0; i < IMAGE_RES_ITEM ; i++ )
		{
			m_ImageRes[ i ].ReleaseRES();
		}
		return false;
	}

	pFileSystem->OpenFile( "3DData\\Control\\Res\\SOKETJAM.TSI",OPEN_READ_BIN );
	if( m_ImageRes[ IMAGE_RES_SOCKETJAM_ICON ].LoadRES( pFileSystem ) == false )
		return false;

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return true;
}

void CImageResManager::ReleaseResources()
{
	for( int i = 0; i < IMAGE_RES_MAX ; i++ )
	{
		m_ImageRes[ i ].ReleaseRES();
	}
}

void CImageResManager::ReleaseResource( short i )
{
	if( i < 0 || i >= IMAGE_RES_MAX )
		return;
	m_ImageRes[i].ReleaseRES();
}

CImageRes* CImageResManager::GetImageRes( int iType )
{
	if( iType >= IMAGE_RES_MAX || iType < 0 )
		return NULL;

	return &m_ImageRes[ iType ]; 

}


/// NPC Face Image 관련
bool CImageResManager::Add_NpcFaceFileInfo( int index, const char* filename )
{
	assert( filename );
	if( NULL == filename )
		return false;

	LIST_INDEX_STRING_ITER iter = m_NpcFaceFiles.find( index );
	if( iter != m_NpcFaceFiles.end() )
	{
		_RPT1( _CRT_ASSERT,"Duplicated Npc Face File(%s)", filename );
		return false;
	}
	m_NpcFaceFiles.insert( make_pair( index, filename ) );
	return true;
}

HNODE CImageResManager::Load_NpcFace( int index )
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem == NULL )
		return false;

	
	LIST_INDEX_STRING_ITER iter = m_NpcFaceFiles.find( index );
	if( iter == m_NpcFaceFiles.end() )
	{
		_RPT1( _CRT_ASSERT,"Not Found Npc Face File Infomation(%d)", index );
		return false;
	}

	if( false == pFileSystem->OpenFile( iter->second.c_str(), OPEN_READ_BIN ) )
	{
		_RPT1( _CRT_ASSERT,"File Open Error	(%d)", index );
		return false;
	}


	setDelayedLoad(0);

	HNODE hNode = loadTexture( iter->second.c_str(), iter->second.c_str(), 1, 0 );

	setDelayedLoad(1);


	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
	return hNode;
}

HNODE CImageResManager::Load_NpcFace( const char * szName_ )
{
	HNODE hNode = false;

// 홍근 : 히어로 퀘스트 추가 구현.
#ifdef _WORK

	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem == NULL )
		return false;
	

	if( false == pFileSystem->OpenFile( szName_, OPEN_READ_BIN ) )
	{
		_RPT1( _CRT_ASSERT,"File Open Error	(%s)", szName_ );
		return false;
	}


	setDelayedLoad(0);

	hNode = loadTexture( szName_, szName_, 1, 0 );

	setDelayedLoad(1);


	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );	

#endif

	return hNode;

}
