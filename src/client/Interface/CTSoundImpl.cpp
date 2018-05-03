#include "stdafx.h"
#include "ctsoundimpl.h"
#include "../Sound/IO_Sound.h"

CTSoundImpl	g_SoundImpl;

CTSoundImpl::CTSoundImpl(void)
{
}

CTSoundImpl::~CTSoundImpl(void)
{

}
void CTSoundImpl::Playsound( int i )
{
	g_pSoundLIST->IDX_PlaySound( (short) i );
}