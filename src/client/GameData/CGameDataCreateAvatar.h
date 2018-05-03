#ifndef _CGAMEDATACREATEAVATAR_
#define _CGAMEDATACREATEAVATAR_
#include <map>
#include <vector>
#include "tgamectrl/tcontaineritem.h"

class CJustModelAVT;

/**
* 캐릭터 선택하는 부분에서 사용하는 Data Class + 약간의 정보 출력 추가
*	- CJustModelAVT list와 Interface용 Data들을 보관한다.
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameDataCreateAvatar
{
	CGameDataCreateAvatar(void);

public:
	static CGameDataCreateAvatar& GetInstance();
	~CGameDataCreateAvatar(void);
	void Clear();

	///모델관련
	void	AddCreateAvatar( CJustModelAVT* pAvatar );
	void	RemoveCreateAvatar( const char* pszName );

	HNODE	GetModelNode( const char* pszName );

	CJustModelAVT*			FindAvatar( const char* pszName );
	const char*				Pick_AVATAR( long x, long y );
	
	const char*				GetSelectedAvatarName();
	int						GetSelectedAvatarIndex();
	CJustModelAVT*			GetSelectedAvatar();
	void					ClearSelectedAvatar();
	void					SelectAvatar( const char* pszName );
	void					SelectAvatar( int iIndex );
	void					Update();
	int						GetAvatarCount();
private:
	std::string m_strSelectedAvatarName;///선택되어져 있는 아바타의 이름

	std::multimap< int , CJustModelAVT* > m_avatars_premium;
	std::multimap< int , CJustModelAVT* > m_avatars_platinum;

};
#endif