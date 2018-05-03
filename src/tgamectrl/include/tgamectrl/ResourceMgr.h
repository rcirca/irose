#ifndef	_RESOURCEMGR_
#define _RESOURCEMGR_

#include "TGameCtrl.h"
#include <map>
#include <string>
#include <list>
using namespace std;

typedef		map< string, int >			TABLE_IMAGE_ID;
typedef		TABLE_IMAGE_ID::iterator	TABLE_IMAGE_ID_ITER;

typedef		map< int,TABLE_IMAGE_ID >	TABLE_MODULE;
typedef		TABLE_MODULE::iterator		TABLE_MODULE_ITER;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// 문자열일 그래픽ID와 숫자 그래픽ID를 맵핑시켜주는 클래스
///		- 스프라이트 에디터에서 입력한 문자열 ID와 생성된 숫자 ID를 저장하고 Client에서는 해당 Sprite사용시 문자열 ID로 이용할수 있도록 도와준다.
///		- Module별로 별도로 맵에 저장한다.
///		- 가상화일시스템(TriggerVFS)를 사용하지 않기 때문에 배포판에서 밖으로 노출되어 있다.
///
/// @Author			최종진
/// @Date			2003/11/28
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CResourceMgr
{
private:
	CResourceMgr(void);
	virtual ~CResourceMgr(void);
	static	CResourceMgr*	s_pInstance;

public:
	static	CResourceMgr*	GetInstance();
	void	Destroy();

	int GetImageNID( int iModuleID, const char* szSID );				/// 문자열ID와 ModuleID로 숫자ID를 찾는다.
	bool AddResource( char* szFileName, int iModuleID );				/// Client에서 스프라이트 정보 추가
	void UnLoadResource( int iModuleID );								/// 해당 Module에 대한 정보를 Unload


protected:
	void LoadResourceFile( char* szFileName, int iModuleID );
	

protected:
	TABLE_MODULE		m_ResourceTable;
};
#endif