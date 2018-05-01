/*
	$Header: /Client/Sound/CMusic.h 4     03-12-30 6:43p Jeddli $
*/
#ifndef	__CMUSIC_H
#define	__CMUSIC_H
#include "..\util\CMMPlayer.h"
//-------------------------------------------------------------------------------------------------

#define MAX_MUSIC_VOLUME		0

#define	WM_MUSIC_EVENT	( WM_APP + 9238 )
class CMusic : public classMMPLAYER
{
private:
	CMusic ();
	~CMusic ();

	static CMusic* m_pInstance;

	IBasicAudio   *m_pBA;

public :
	static CMusic* Instance ();
	void Destroy ();


	void ON_Completed ();
//	void ON_Abort ();

	HRESULT SetVolume( long lVolume );
} ;
extern CMusic *g_pMusic;

//-------------------------------------------------------------------------------------------------
#endif
