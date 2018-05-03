/*
	$Header: /Client/Sound/SoundSYSTEM.cpp 1     03-05-29 9:50a Kauri $
*/
#include "stdAFX.h"
#include "SoundSYSTEM.h"

//-------------------------------------------------------------------------------------------------
CSoundSYSTEM::CSoundSYSTEM (HWND hWND) : CFileLIST<t_sounddata*>("SND ")
{
	m_SOUND._Init ( hWND, DSSCL_PRIORITY, 2, 22050, 16);

	m_iSoundPan = SOUND_PAN_CENTER;
	m_iSoundVol = SOUND_VOLUME_MAX;
}
CSoundSYSTEM::~CSoundSYSTEM ()
{
	CFileLIST< t_sounddata* >::Free ();
	m_SOUND._Free ();
}

//-------------------------------------------------------------------------------------------------
bool CSoundSYSTEM::Load_FILE(tagFileDATA<t_sounddata*> *pDATA)
{
	short nMixCNT = 1;
	pDATA->m_DATA = m_SOUND.LoadSoundData (pDATA->m_FileName.Get(), nMixCNT);
	return ( pDATA->m_DATA != NULL );
}
void CSoundSYSTEM::Free_FILE (tagFileDATA<t_sounddata*> *pDATA)
{
	m_SOUND.FreeSoundData (pDATA->m_DATA);
}

//-------------------------------------------------------------------------------------------------
bool CSoundSYSTEM::KEY_PlaySound (unsigned int uiKEY)
{
	t_sounddata *pSD = Get_DATAUseKEY( uiKEY );

	if ( NULL == pSD ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

	m_SOUND.PlaySound (pSD, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CSoundSYSTEM::IDX_PlaySound (short nIndex)
{
	t_sounddata *pSD = Get_DATAUseIDX( nIndex );

	if ( NULL == pSD ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

	m_SOUND.PlaySound (pSD, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CSoundSYSTEM::PlaySoundFile (char *szFileName)
{
	t_HASHKEY uiKEY = this->Add_FILE (szFileName);
	return KEY_PlaySound( uiKEY );
}

//-------------------------------------------------------------------------------------------------
