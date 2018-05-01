#ifndef _ITSOUND_
#define _ITSOUND_

#include "TGameCtrl.h"
#include <crtdbg.h>

//------------------------------------------------------------------------------------------------------------------------
///	Client가 상속받아 구현해야할 Play Sound Interface - TGameCtrl은 이 인터페이스를 통해서 Sound를 Play한다.
///
/// @Author			최종진
/// @Date			2005/08/23
//------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API ITSound
{
public:
	virtual void Playsound( int iIndex ) = 0;/* { _ASSERT( 0 && "ITSound::PlaySound" );};*/
};
#endif