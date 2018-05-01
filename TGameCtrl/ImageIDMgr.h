#ifndef	_IMAGEIDMGR_
#define _IMAGEIDMGR_
#include "TGameCtrl.h"
#include <map>
#include <string>
#include <list>
using namespace std;

typedef		map< string, int >			TABLE_IMAGE_ID;
typedef		TABLE_IMAGE_ID::iterator	TABLE_IMAGE_ID_ITER;

typedef		map< int,TABLE_IMAGE_ID >	TABLE_MODULE;
typedef		TABLE_MODULE::iterator		TABLE_MODULE_ITER;
////////////////////////////////////////////////////////////////////////////////////////////
/// 2003 / 11 /28 / 최종진
/// 문자열일 그래픽ID와 숫자 그래픽ID를 맵핑시켜주는 클래스
////////////////////////////////////////////////////////////////////////////////////////////
class TGAMECTRL_API CImageIDMgr
{
private:
	CImageIDMgr(void);
	virtual ~CImageIDMgr(void);
	static	CImageIDMgr*	s_pInstance;


public:
	static	CImageIDMgr*	GetInstance();
	void	Destroy();

	int GetImageNID( int iModuleID, const char* szSID );///문자열ID와 ModuleID로 숫자ID를 찾는다.
	bool AddResource( char* szFileName, int iModuleID );///Client에서 Table에 등록을 요청할때 사용되는 operation


protected:
	void LoadResourceFile( char* szFileName, int iModuleID );
	

protected:
	TABLE_MODULE		m_ResourceTable;
};
#endif
