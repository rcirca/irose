
#ifndef _CTSOUNDIMPL_
#define _CTSOUNDIMPL_

#include "tgamectrl/itsound.h"
/**
* 인터페이스 라이브러리(TGameCtrl)에게 알려줄 ITSound의 Implemented Class
*  : TGameCtrl내부적으로 사운드를 플레이할때 이 인터페이스를 통해서 호출한다. 
*
* @Author		최종진
*
* @Date			2005/9/5
*/

class CTSoundImpl :	public ITSound
{
public:
	CTSoundImpl(void);
	~CTSoundImpl(void);

	void Playsound( int i );
};

extern CTSoundImpl		g_SoundImpl;

#endif
