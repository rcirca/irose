/*
	$Header: /Client/Sound/SoundSYSTEM.h 1     03-05-29 9:50a Kauri $
*/
#ifndef	__SOUNDSYSTEM_H
#define	__SOUNDSYSTEM_H
#include "..\util\CFileLIST.h"
#include "..\util\CDSound.h"
//-------------------------------------------------------------------------------------------------

#define	SOUND_VOLUME_MAX	DSBVOLUME_MAX			// 0
#define SOUND_VOLUME_90	   (DSBVOLUME_MIN*1 / 10)
#define SOUND_VOLUME_70	   (DSBVOLUME_MIN*3 / 10)
#define SOUND_VOLUME_60	   (DSBVOLUME_MIN*4 / 10)
#define SOUND_VOLUME_HALF  (DSBVOLUME_MIN*5 / 10)
#define SOUND_VOLUME_MIN	DSBVOLUME_MIN			// -10000

#define	SOUND_PAN_LEFT		DSBPAN_LEFT				// -10000
#define SOUND_PAN_CENTER	DSBPAN_CENTER			// 0
#define SOUND_PAN_RIGHT		DSBPAN_RIGHT			// 10000


/*
enum {
	SNDTYPE_MESSAGE = 0,
	SNDTYPE_CNST_SEL,
	SNDTYPE_CHAR_SEL,
	SNDTYPE_CHAR_CMD,
	SNDTYPE_CHAR_YAP,
	SNDTYPE_CHAR_DIE,
	SNDTYPE_ETC,
	SNDTYPE_BULLET,

	MAX_SOUND_TYPE
} ;
*/
class CSoundSYSTEM : public CFileLIST< t_sounddata* >
{
private:
	CDSOUND	m_SOUND;
	int		m_iSoundPan;
	int		m_iSoundVol;
//	classDYNAMICDATA <t_sounddata*> m_DDSND[ MAX_SOUND_TYPE ];

	bool	Load_FILE (tagFileDATA<t_sounddata*> *pDATA);
	void	Free_FILE (tagFileDATA<t_sounddata*> *pDATA);

	t_sounddata *Get_DATA();

public :
	CSoundSYSTEM (HWND hWND);
	~CSoundSYSTEM ();

	bool KEY_PlaySound (unsigned int uiKEY);	// false경우 파일을 직접 로드 해야 한다.
	bool IDX_PlaySound (short nIndex);
	bool PlaySoundFile (char *szFileName);
};

//-------------------------------------------------------------------------------------------------
#endif
