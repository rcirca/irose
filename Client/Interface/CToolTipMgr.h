#ifndef _CToolTipMgr_
#define _CToolTipMgr_
#include "CInfo.h"


///<- 이하 define은 사용되지 않는것 같다 2005/9/5 최종진
#define INFO_STATUS_DEF			0x00000001			
#define INFO_STATUS_ATK			0x00000002
#define INFO_STATUS_ACC			0x00000004
#define INFO_STATUS_GEM			0x00000010
#define INFO_STATUS_USE			0x00000020
#define INFO_STATUS_ETC			0x00000040
//->

/**
* 툴팁 시스템에서의 관리자 Class
*	- 현재 툴팁은 매프레임당 CInfo및 string을 새로 등록하고 Draw하는 비효율적 구조이다. 
*
* @Author		최종진
* @Date			2005/9/5	
*/

class CToolTipMgr
{
	CToolTipMgr(void);
public:
	~CToolTipMgr(void);
	static CToolTipMgr& GetInstance();

	/// 툴팁 등록 Methods
	void  RegUIInfo( const int x, const int y, const DWORD dwDlgType, const int iToolTipID );
	void  RegistInfo( const CInfo& Info );
	void  RegToolTip( int x, int y, const char* pszMsg );

	/// 등록된 툴팁 Draw
	void  Draw();

private:
	bool			m_bHasDrawInfo;
	CInfo			m_Info;

};
#endif