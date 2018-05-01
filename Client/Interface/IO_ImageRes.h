#ifndef _IOIMAGERES_
#define _IOIMAGERES_


#include "../Util/JSingleton.h"

#include <vector>
#include <map>


/// 컨트롤의 리소스관리와 드로우 
struct stTexture
{
	char					m_szName[MAX_PATH]; //이름 
	DWORD					m_dwColorKey;
	HNODE					m_Texture;
	short					m_nTXID;
};

struct stSprite 
{
	short	m_nTextureID;
	RECT	m_Rect;
	DWORD	m_dwColor;

	char	m_szID[32];	
	
} ;



typedef std::vector< stTexture >					TextureVEC;
typedef std::vector< stTexture >::iterator			TextureVEC_itor;

typedef std::vector< stSprite >						SpriteVEC;
typedef std::vector< stSprite >::iterator			SpriteVEC_itor;


class CFileSystem;
////////////////////////////////////////////////////////////////////////////////////////
/// Image resource for UI
////////////////////////////////////////////////////////////////////////////////////////
class CImageRes
{
private:
	short						m_nSpriteCNT;

	TextureVEC						m_TextureVec;	
	SpriteVEC						m_SpriteVec;


public:
	CImageRes();
	~CImageRes();

	/// Load TSI file
	bool		LoadRES( CFileSystem* pFileSystem, const char* strResPath = "3DData\\Control" );
	void		ReleaseRES();

	stTexture*	GetTexture( int iSprIndex );
	stSprite*	GetSprite( int iSprIndex );
	short		GetSpriteCount(){ return m_nSpriteCNT; }


};





/// 게임내부에서 사용되는 이미지 리소스들..
enum
{	
	IMAGE_RES_UI				= 0,
	IMAGE_RES_ITEM				= 1,
	IMAGE_RES_NPCFACE			= 2,///없어짐 .EXUI의 XML에서의 MODULE INDEX때문에 그대로 둠.
	IMAGE_RES_EXUI				= 3,
	IMAGE_RES_SKILL_ICON		= 4,
	IMAGE_RES_STATE_ICON		= 5,
	IMAGE_RES_SOCKETJAM_ICON	= 6,
	IMAGE_RES_CLANBACK			= 7,
	IMAGE_RES_CLANCENTER		= 8,
	IMAGE_RES_TARGETMARK		= 9,
	IMAGE_RES_WARNING			= 10,
	IMAGE_RES_NEWUI				= 11,
	IMAGE_RES_MAX,
};

////////////////////////////////////////////////////////////////////////////////////////
/// Image res manager for UI
////////////////////////////////////////////////////////////////////////////////////////
typedef std::map< int, std::string >	LIST_INDEX_STRING;
typedef LIST_INDEX_STRING::iterator		LIST_INDEX_STRING_ITER;

class CImageResManager : public CJSingleton< CImageResManager >
{
private:
	CImageRes			m_ImageRes[ IMAGE_RES_MAX ];

	void				ReleaseResource( short i );
	///list_npcface의 정보를 저장
	LIST_INDEX_STRING	m_NpcFaceFiles;
public:
	CImageResManager();
	~CImageResManager();

	bool				LoadImageResources();
	void				ReleaseResources();

	CImageRes*			GetImageRes( int iType );

	/// NPC Face Image 관련
	bool		Add_NpcFaceFileInfo( int index, const char* filename );
	HNODE		Load_NpcFace( int index );
	HNODE		Load_NpcFace( const char * szName_ );
};




#endif //_IOIMAGERES_