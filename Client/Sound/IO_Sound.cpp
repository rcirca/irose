/*
	$Header: /Client/Sound/IO_Sound.cpp 23    04-10-08 8:02p Jeddli $
*/
#include "stdAFX.h"
#include "IO_Sound.h"

//-------------------------------------------------------------------------------------------------
CSoundLIST::CSoundLIST (HWND hWND) : CFileLIST<tagSndFILE*>("SND ")
{
	m_SOUND._Init ( hWND, DSSCL_PRIORITY, 2, 44100, 16);

	m_iSoundPan = SOUND_PAN_CENTER;
	m_iSoundVol = SOUND_VOLUME_MAX;
	m_fMinDistance = SOUND_DEFAULT_MIN_DISTANCE;
	m_fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE;
}
CSoundLIST::~CSoundLIST ()
{
	CFileLIST< tagSndFILE* >::Free ();
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::Load (char *szSTBFile, short nFileNameColNO)
{
	CGameSTB fSTB;

	if ( fSTB.Open (szSTBFile) ) {
		char *szFileName;

		Alloc_FILES( fSTB.GetRowCount() );

		for (short nI=0; nI<m_nDataCNT; nI++) {
			szFileName = fSTB.GetString(nFileNameColNO, nI);
			if ( szFileName ) {
				this->AddSoundFile (szFileName, fSTB.GetInteger( nFileNameColNO+1, nI), nI);
			}
		}
		fSTB.Close ();
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::Load_FILE(tagFileDATA<tagSndFILE*> *pDATA)
{
	LogString (LOG_DEBUG, "Load_FILE::[ %s ] \n", pDATA->m_FileName.Get () );

	if( pDATA == NULL )
		return false;

	if( pDATA->m_DATA )
	{
		pDATA->m_DATA->m_pSoundData = m_SOUND.LoadSoundData( pDATA->m_FileName.Get(), pDATA->m_DATA->m_nMixCNT);
		return ( pDATA->m_DATA->m_pSoundData != NULL );
	}

	return false;
}
void CSoundLIST::Free_FILE (tagFileDATA<tagSndFILE*> *pDATA)
{
	if ( pDATA->m_DATA->m_pSoundData ) {
		m_SOUND.FreeSoundData( pDATA->m_DATA->m_pSoundData );
		pDATA->m_DATA->m_pSoundData = NULL;
		SAFE_DELETE( pDATA->m_DATA );
	}
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::KEY_PlaySound (t_HASHKEY HashKEY)
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	if (!m_SOUND.Set3D (pSoundData->m_pSoundData, true)) return false; // 안-3D 모드로 설정
#endif

	m_SOUND.PlaySound (pSoundData->m_pSoundData, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::KEY_PlaySound3D (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld,  float fMinDistance, float fMaxDistance)
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	//if (!m_SOUND.Set3D (pSoundData->m_pSoundData, false)) return false; // 3D 모드로 설정
	if (!m_SOUND.SetPosition (pSoundData->m_pSoundData, posWorld)) return false;
#endif

	if (velWorld)
		m_SOUND.SetVelocity(pSoundData->m_pSoundData, *velWorld);

	m_fMinDistance = fMinDistance;
	m_SOUND.SetMinDistance(pSoundData->m_pSoundData, fMinDistance);
	m_fMaxDistance = fMaxDistance;
	m_SOUND.SetMaxDistance(pSoundData->m_pSoundData, fMaxDistance);

	m_SOUND.PlaySound (pSoundData->m_pSoundData, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

bool CSoundLIST::KEY_PlaySound (t_HASHKEY HashKEY, int iVolume, int iPan)
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	if (!m_SOUND.Set3D (pSoundData->m_pSoundData, true)) return false; // 안-3D 모드로 설정
#endif

	m_SOUND.PlaySound (pSoundData->m_pSoundData, iVolume, iPan, 0);
	return true;
}

bool CSoundLIST::KEY_PlaySound3D (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, int iVolume, int iPan, const D3DXVECTOR3 * velWorld, float fMinDistance, float fMaxDistance)
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	//if (!m_SOUND.Set3D (pSoundData->m_pSoundData, false)) return false; // 3D 모드로 설정
	if (!m_SOUND.SetPosition (pSoundData->m_pSoundData, posWorld)) return false;
#endif

	if (velWorld)
		m_SOUND.SetVelocity(pSoundData->m_pSoundData, *velWorld);
	
	m_fMinDistance = fMinDistance;
	m_SOUND.SetMinDistance(pSoundData->m_pSoundData, fMinDistance);
	m_fMaxDistance = fMaxDistance;
	m_SOUND.SetMaxDistance(pSoundData->m_pSoundData, fMaxDistance);

	m_SOUND.PlaySound (pSoundData->m_pSoundData, iVolume, iPan, 0);
	return true;
}



///
/// Play sound for loop
///
bool CSoundLIST::KEY_PlaySound3DLoop (t_HASHKEY HashKEY, const D3DXVECTOR3& posWorld, int iVolume )
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	//if (!m_SOUND.Set3D (pSoundData->m_pSoundData, false)) return false; // 3D 모드로 설정
	if (!m_SOUND.SetPosition (pSoundData->m_pSoundData, posWorld)) return false;
#endif

	
	m_fMinDistance = SOUND_DEFAULT_MIN_DISTANCE;
	m_SOUND.SetMinDistance(pSoundData->m_pSoundData, m_fMinDistance);
	m_fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE;
	m_SOUND.SetMaxDistance(pSoundData->m_pSoundData, m_fMaxDistance);

	m_SOUND.PlaySound (pSoundData->m_pSoundData, iVolume, SOUND_PAN_CENTER, DSBPLAY_LOOPING );
	
	return true;
}

bool CSoundLIST::KEY_StopSound3DLoop (t_HASHKEY HashKEY )
{
	tagSndFILE *pSoundData = Get_DATAUseKEY( HashKEY );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

	m_SOUND.StopSound( pSoundData->m_pSoundData );

	return true;
}

bool CSoundLIST::IDX_PlaySound3DLoop (short nIndex, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld )
{
	tagSndFILE *pSoundData = Get_DATAUseIDX( nIndex );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	//if (!m_SOUND.Set3D (pSoundData->m_pSoundData, false)) return false; // 3D 모드로 설정
	if (!m_SOUND.SetPosition (pSoundData->m_pSoundData, posWorld)) return false;
#endif

	if (velWorld)
		m_SOUND.SetVelocity(pSoundData->m_pSoundData, *velWorld);
	
	m_fMinDistance = SOUND_DEFAULT_MIN_DISTANCE;
	m_SOUND.SetMinDistance(pSoundData->m_pSoundData, m_fMinDistance);
	m_fMaxDistance = SOUND_DEFAULT_MAX_DISTANCE;
	m_SOUND.SetMaxDistance(pSoundData->m_pSoundData, m_fMaxDistance);

	m_SOUND.PlaySound ( pSoundData->m_pSoundData, m_iSoundVol, SOUND_PAN_CENTER, DSBPLAY_LOOPING );
	return true;
}

bool CSoundLIST::IDX_StopSound3DLoop ( short nIndex )
{
	tagSndFILE *pSoundData = Get_DATAUseIDX( nIndex );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

	m_SOUND.StopSound( pSoundData->m_pSoundData );

	return true;
}











//-------------------------------------------------------------------------------------------------
bool CSoundLIST::IDX_PlaySound (short nIndex)
{
	tagSndFILE *pSoundData = Get_DATAUseIDX( nIndex );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	if (!m_SOUND.Set3D (pSoundData->m_pSoundData, true)) return false; // 안-3D 모드로 설정
#endif
	m_SOUND.PlaySound (pSoundData->m_pSoundData, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

bool CSoundLIST::IDX_PlaySound3D (short nIndex, const D3DXVECTOR3& posWorld, const D3DXVECTOR3 * velWorld, float fMinDistance, float fMaxDistance)
{
	tagSndFILE *pSoundData = Get_DATAUseIDX( nIndex );

	if ( NULL == pSoundData ) {
		// 사운드파일리스트 에 등록 되지 않는 사운드 파일이다.
		return false;
	}

#ifdef USE_DEFAULT_3D_SOUND
	//if (!m_SOUND.Set3D (pSoundData->m_pSoundData, false)) return false; // 3D 모드로 설정
	if (!m_SOUND.SetPosition (pSoundData->m_pSoundData, posWorld)) return false;
#endif

	if (velWorld)
		m_SOUND.SetVelocity(pSoundData->m_pSoundData, *velWorld);
	
	m_fMinDistance = fMinDistance;
	m_SOUND.SetMinDistance(pSoundData->m_pSoundData, fMinDistance);
	m_fMaxDistance = fMaxDistance;
	m_SOUND.SetMaxDistance(pSoundData->m_pSoundData, fMaxDistance);

	m_SOUND.PlaySound (pSoundData->m_pSoundData, m_iSoundVol, m_iSoundPan, 0);
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::PlaySoundFile (char *szFileName, short nMaxMixCNT)
{
	t_HASHKEY uiKEY = this->AddSoundFile (szFileName, nMaxMixCNT);
	return KEY_PlaySound( uiKEY );	
}

//-------------------------------------------------------------------------------------------------
bool CSoundLIST::PlaySoundFile3D (char *szFileName, const D3DXVECTOR3& posWorld, short nMaxMixCNT, const D3DXVECTOR3 * velWorld, float fMinDistance, float fMaxDistance)
{
	t_HASHKEY uiKEY = this->AddSoundFile (szFileName, nMaxMixCNT);

	return KEY_PlaySound3D( uiKEY, posWorld, velWorld, fMinDistance, fMaxDistance);
}

//-------------------------------------------------------------------------------------------------
/// @bug pSndFileDATA 있는데.. pDATA->m_DATA NULL  일 경우가 있냐?
t_HASHKEY CSoundLIST::AddSoundFile (char *szFileName, short nMixCNT, short nIndex)
{
	t_HASHKEY uiKEY = CStr::GetHASH( szFileName );
	
	tagFileDATA< tagSndFILE* > *pSndFileDATA = KEY_Find_DATA( uiKEY );
	if ( pSndFileDATA ) 
	{
		if( pSndFileDATA->m_DATA )
		{
			pSndFileDATA->m_DATA->m_nRefCNT ++;			
		}
		return uiKEY;
	}
	
	tagSndFILE	*pSndFILE;

	pSndFILE = new tagSndFILE;
	pSndFILE->m_nMixCNT = nMixCNT;
	pSndFILE->m_nRefCNT = 1;
	uiKEY = this->Add_FILE( szFileName, nIndex, pSndFILE, false);

	return uiKEY;
}
void CSoundLIST::SubSoundFile (t_HASHKEY uiKEY)
{
	tagFileDATA< tagSndFILE* > *pSndFileDATA = KEY_Find_DATA( uiKEY );
	if ( pSndFileDATA ) 
	{
		if( pSndFileDATA->m_DATA )
		{
			if ( --pSndFileDATA->m_DATA->m_nRefCNT <= 0 ) 
			{
				pSndFileDATA->m_iRefCnt = 0;	// 강제로 1로 설정하여 삭제대기 버퍼에 등록되도록....
				this->Sub_DATA (pSndFileDATA, false);
			}
		}		
	}
}
	

//-------------------------------------------------------------------------------------------------