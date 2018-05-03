#ifndef _CLAN_MARK_MANAGER_
#define _CLAN_MARK_MANAGER_

#include "CClanMarkUserDefined.h"
#include "../Util/JSingleton.h"

#include <list>
#include <string>


//----------------------------------------------------------------------------------
/// class for texture pool.
///
///
//----------------------------------------------------------------------------------

class CClanMarkManager : public CJSingleton< CClanMarkManager >
{
public:
	CClanMarkManager(void);
	~CClanMarkManager(void);


	CClanMarkUserDefined*	GetClanMark( const char* pstrName, WORD crc16 , int iClanID );

	//------------------------------------------------------------------
	/// 노드들의 상태 업데이트( 가베지 컬렉팅, 텍스쳐 로드 )
	//------------------------------------------------------------------
	void					UpdatePool();
	//------------------------------------------------------------------
	/// Clear m_TextureNodePool
	//------------------------------------------------------------------
	void					Free();
	void					ReloadTexture( const char* FileName, WORD wCRC16 );




private:
	//------------------------------------------------------------------
	/// Search texture form pool.
	//------------------------------------------------------------------
	CClanMarkUserDefined*	SearchTexture( const char* pstrName );

	//------------------------------------------------------------------
	/// 새로운 UserDefinedClanMark 객체를 생성하고 리턴..
	//------------------------------------------------------------------
	CClanMarkUserDefined*	GetUserdefinedClanMark( const char* pstrName, WORD crc16, int iClanID );

	//------------------------------------------------------------------
	/// Load new texture form HDD.
	//------------------------------------------------------------------
	HNODE					LoadNewTexture( const char* pstrName, WORD crc16);

	
	//------------------------------------------------------------------
	/// 텍스쳐 로드 플래그가 켜진 노드들에 대한 텍스쳐 로딩..
	//------------------------------------------------------------------
	void					LoadRealTexture( CClanMarkUserDefined* );


private:

	std::list< CClanMarkUserDefined* >		m_TextureNodePool;


};

#endif //_CLAN_MARK_MANAGER_