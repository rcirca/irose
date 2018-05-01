/*
	$Header: /Client/sound/IO_Sound.h 19    04-12-14 11:52a Navy $
*/
#ifndef	__IO_SOUND_H
#define	__IO_SOUND_H
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


struct tagSndFILE {
	t_sounddata *m_pSoundData;
	short		 m_nMixCNT;
	short		 m_nRefCNT;
	tagSndFILE ()	{	m_pSoundData=NULL,m_nMixCNT=1,m_nRefCNT=0;	}
} ;

#define USE_DEFAULT_3D_SOUND // 3d 사운드를 디폴트로 설정하려면

#define SOUND_DEFAULT_MAX_DISTANCE (5000.0f)
#define SOUND_DEFAULT_MIN_DISTANCE (1000.0f)

enum{
	SID_GETTING_ITEM		= 531,
	SID_DROP_ARMOR			= 576,
	SID_REGIST_QUICKSLOT	= 584
};
/// sound list
class CSoundLIST : public CFileLIST< tagSndFILE* >
{
public:
#ifdef USE_DEFAULT_3D_SOUND
	CD3DSOUND m_SOUND;
#else
	CDSOUND m_SOUND;
#endif
	int m_iSoundPan;
	int m_iSoundVol;
	float m_fMinDistance;
	float m_fMaxDistance;

//	classDYNAMICDATA <t_sounddata*> m_DDSND[ MAX_SOUND_TYPE ];

	bool	Load_FILE (tagFileDATA<tagSndFILE*> *pDATA);
	void	Free_FILE (tagFileDATA<tagSndFILE*> *pDATA);

	t_sounddata *Get_DATA();

public :
	CSoundLIST (HWND hWND);
	~CSoundLIST ();

	bool Load (char *szSTBFile, short nFileNameColNO=0);

	int  GetVolume ()	{	return m_iSoundVol;	}

	void SetVolume ( int iSoundVolume ) { m_iSoundVol = iSoundVolume;	}

	int	 GetPan ()		{	return m_iSoundPan;	}

	// false경우 파일을 직접 로드 해야 한다.
	bool KEY_PlaySound (t_HASHKEY HashKEY);
	bool KEY_PlaySound (t_HASHKEY HashKEY, int iVolume, int iPan=SOUND_PAN_CENTER);
	bool KEY_PlaySound3D (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld = 0, float fMinDistance = SOUND_DEFAULT_MIN_DISTANCE, float fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE);
	bool KEY_PlaySound3D (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, int iVolume, int iPan=SOUND_PAN_CENTER, const D3DXVECTOR3 * velWorld = 0,  float fMinDistance = SOUND_DEFAULT_MIN_DISTANCE, float fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE);	

	bool IDX_PlaySound (short nIndex);
	bool IDX_PlaySound3D (short nIndex, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld = 0, float fMinDistance = SOUND_DEFAULT_MIN_DISTANCE, float fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE);

	bool PlaySoundFile (char *szFileName, short nMaxMixCNT=1);
	bool PlaySoundFile3D (char *szFileName, const D3DXVECTOR3& posWorld, short nMaxMixCNT=1, const D3DXVECTOR3 * velWorld = 0, float fMinDistance = SOUND_DEFAULT_MIN_DISTANCE, float fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE);

	t_HASHKEY AddSoundFile (char *szFileName, short nMixCNT, short nIndex=-1);
	void SubSoundFile (t_HASHKEY uiKEY);

	///
	/// Play sound for loop
	///
	bool KEY_PlaySound3DLoop (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, int iVolume );	
	bool KEY_StopSound3DLoop (t_HASHKEY HashKEY );	

	bool IDX_PlaySound3DLoop (short nIndex, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld );
	bool IDX_StopSound3DLoop (short nIndex );	
};

//-------------------------------------------------------------------------------------------------
#endif
