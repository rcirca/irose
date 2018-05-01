#ifndef _RELOAD_PROCESS_
#define _RELOAD_PROCESS_


/**
* 재장전에 필요한 시간을 표시하는 클래스
* 
* @Todo		엔진에서 처리하는 방식으로 바뀜에 따라 IT_MGR등의 관리자 클래스나 함수로 빼는게 좋을것 같다.( 최종진 )
* @Author	이동현
* @Date		2005/9/5
*/

class CReloadProcess
{

public:
	CReloadProcess(void);
	virtual ~CReloadProcess(void);

	bool					LoadReloadTexture();

	void					Draw( int iX, int iY, int iModuleID, int iGraphicID, float fProcessRate );
};


#endif //_RELOAD_PROCESS_