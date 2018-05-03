#ifndef _NAMEBOX_
#define _NAMEBOX_

#include <string>


class CObjCHAR;


/// display namebox class
#define NAMEBOX_WIDTH	( 256 )
#define NAMEBOX_HEIGHT	48


/**
* 게임내 캐릭터들( 나, 다른 유저, 몹, NPC등)의 이름, HP게이지, 클랜마크등을 그리는 Class
*
* @Author		이동현, 최종진
* @Date			2005/9/5
*/
class CNameBox
{
private:
	std::string			m_strGuildName;
	std::string			m_strName;


public:
	CNameBox();
	~CNameBox();

	void				SetGuildName( const char* pStr ){ SetGuildName( std::string( pStr ) ); };
	void				SetCharName( const char* pStr ){ SetCharName( std::string( pStr ) ); };
	void				SetGuildName( std::string str ){ m_strGuildName= str; };
	void				SetCharName( std::string str ){ m_strName= str; };

	void				Draw( float x, float y, float z );
	void				Draw( CObjCHAR* pCharOBJ, float x, float y, float z );
	DWORD				GetTargetMobNameColor( int iAvatarLv, int iMobLv );

	void				DrawNpcName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted );
	void				DrawMobName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted );
	void				DrawAvatarName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted );
	void				DrawMyName( float x, float y, float z, CObjCHAR* pCharOBJ, bool bTargeted );
	void				DrawTargetMark( CObjCHAR* pChar, RECT& rcDrawName , float z );

private:
	D3DVECTOR			GetClanMarkDrawPos( CObjCHAR* pChar, float x, float y, float z );
};



#endif