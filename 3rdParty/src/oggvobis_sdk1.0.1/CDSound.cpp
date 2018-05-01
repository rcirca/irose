
#include "stdAFX.h"

#include <DSound.h>
#include <DXErr9.h>

#include "CDSound.h"
#include "CWaveFILE.h"
/*
#include "CCamera.h"
#include "OBJECT.h"
#include "CObjCHAR.h"
*/

#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxerr9.lib")
#pragma comment (lib, "winmm.lib")

/*
LPDIRECTSOUND3DLISTENER	CD3DSOUND::m_pDSListener = NULL;
DS3DLISTENER CD3DSOUND::m_dsListenerParams;
*/

//-------------------------------------------------------------------------------------------------

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------------------------------------------
struct tagWAVEDATA {
    WAVEFORMATEX m_sWaveFormat;
    BYTE *m_pWaveData;
    DWORD m_dwDataSize;
} ;


static t_wavedata  *WaveData_Load (LPSTR szFileName, long lFilePtr);
static void WaveData_Free (t_wavedata *pWave);

//-------------------------------------------------------------------------------------------------
//
//	WAVE File I/O
//
static t_wavedata *WaveData_Load (LPSTR szFileName, long lFilePtr)
{
    FILE	   *fpWAV;
    ULONG       lFPos;
    DWORD       dwLength;
    t_wavedata *pWave;
	union {
	    DWORD	dwFOURCC;
		char	cFOURCC[ 4 ];
	} ;

	if ( lFilePtr >= 0 )
		lFPos = lFilePtr + sizeof(long);
	else
		lFPos = 0;

    if ( (fpWAV = fopen (szFileName, "rb")) == NULL )
        return NULL;
    fseek (fpWAV, lFPos, SEEK_SET);

    fread (&dwFOURCC,	sizeof (DWORD), 1, fpWAV);  // rID
    if ( dwFOURCC != mmioFOURCC('R', 'I', 'F', 'F') )
        goto _ExitLoadWaveFile;						// not even RIFF

    fread (&dwLength,   sizeof (DWORD), 1, fpWAV);  // rLen

    fread (&dwFOURCC,	sizeof (DWORD), 1, fpWAV);  // wID
    if ( dwFOURCC != mmioFOURCC('W', 'A', 'V', 'E') )
        goto _ExitLoadWaveFile;                     // not a WAV

    fread (&dwFOURCC,   sizeof (DWORD), 1, fpWAV);  // fID
    if ( dwFOURCC != mmioFOURCC('f', 'm', 't', ' ') )
        goto _ExitLoadWaveFile;                     // not a WAV

    fread (&dwLength,   sizeof (DWORD), 1, fpWAV);  // fLen

    pWave = (t_wavedata *) new t_wavedata;
    if ( pWave == NULL ) return NULL;
	pWave->m_pWaveData = NULL;

    fread (&pWave->m_sWaveFormat, sizeof (WAVEFORMATEX), 1, fpWAV);
    fseek (fpWAV, dwLength-18, SEEK_CUR);

	dwLength = 0;
	do {
		if ( dwLength )	fseek (fpWAV, dwLength, SEEK_CUR);

		fread (&dwFOURCC,	sizeof (DWORD), 1, fpWAV);				// fID
		if ( fread (&dwLength,   sizeof (DWORD), 1, fpWAV) < 1 )	// block size
			goto _ExitLoadWaveFile;
	} while ( dwFOURCC != mmioFOURCC('d', 'a', 't', 'a') );

	pWave->m_dwDataSize = dwLength;
    pWave->m_pWaveData = (BYTE *) new BYTE [ pWave->m_dwDataSize ];
    if ( pWave->m_pWaveData == NULL ) {
        delete pWave;
        return NULL;
    }
    fread (pWave->m_pWaveData, sizeof (BYTE), pWave->m_dwDataSize, fpWAV);

_ExitLoadWaveFile:
    fclose (fpWAV);

    return pWave;
}


//-------------------------------------------------------------------------------------------------
static void WaveData_Free (t_wavedata *pWave)
{
    if ( pWave->m_pWaveData != NULL )
		delete[] pWave->m_pWaveData;

    if ( pWave != NULL )
        delete pWave;
}


//-------------------------------------------------------------------------------------------------
CDSOUND::CDSOUND()
{
    m_pDS = NULL;
}


//-------------------------------------------------------------------------------------------------
CDSOUND::~CDSOUND()
{
	this->_Free ();
}


//-------------------------------------------------------------------------------------------------
bool CDSOUND::_Init( HWND  hWnd, 
                         DWORD dwCoopLevel, 
                         DWORD dwPrimaryChannels, 
                         DWORD dwPrimaryFreq, 
                         DWORD dwPrimaryBitRate )
{
    // Create IDirectSound using the primary sound device
	m_hR = DirectSoundCreate8( NULL, &m_pDS, NULL );
    if ( FAILED( m_hR ) ) {
		OutputDebugString ( "ERROR : DirectSoundCreate8\n");
        return false;
	}

    // Set DirectSound coop level 
	m_hR = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel );
    if( FAILED( m_hR ) ) {
        OutputDebugString ("ERROR : SetCooperativeLevel\n");
		return false;
	}
    
    // Set primary buffer format
    return SetPrimaryBufferFormat( dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate );
}


//-------------------------------------------------------------------------------------------------
void CDSOUND::_Free ()
{
	if ( m_pDS ) {
		m_pDS->Release();
		m_pDS = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
bool CDSOUND::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
											 DWORD dwPrimaryFreq, 
                                             DWORD dwPrimaryBitRate )
{
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return false;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
	m_hR = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL );
    if( FAILED( m_hR ) ) {
        OutputDebugString ("ERROR : CreateSoundBuffer\n");
		return false;
	}

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	m_hR = pDSBPrimary->SetFormat(&wfx);
	if( FAILED( m_hR ) ) {
		pDSBPrimary->Release();
        OutputDebugString ("ERROR : DSoundBuffer SetFormat\n");
		return false;
	}

	SAFE_RELEASE( pDSBPrimary )

    return true;
}


//-------------------------------------------------------------------------------------------------
t_sounddata *CDSOUND::CreateSoundData (t_wavedata *pWave, DWORD dwCreationFlags, GUID guid3DAlgorithm)
{
    if( m_pDS == NULL )
        return NULL;

    DSBUFFERDESC	dsbd;
    LPDIRECTSOUNDBUFFER lpDSB;
    LPVOID			pMem1, pMem2;
    DWORD			dwSize1, dwSize2;

    ::ZeroMemory (&dsbd, sizeof (dsbd));

    dsbd.dwSize = sizeof (DSBUFFERDESC);
    // DSBCAPS_CTRLPAN | DSBCAPS_GETCURRENTPOSITION2 
    // DSBCAPS_CTRLFREQUENCY
	// DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN
	// DSBCAPS_CTRL3D;
    dsbd.dwFlags	     =  dwCreationFlags;
    dsbd.lpwfxFormat     = &pWave->m_sWaveFormat;
    dsbd.dwBufferBytes   =  pWave->m_dwDataSize;
	dsbd.guid3DAlgorithm =  guid3DAlgorithm;

    m_hR = m_pDS->CreateSoundBuffer (&dsbd, &lpDSB, NULL);
    if ( m_hR != DS_OK ) 
		return NULL;

    m_hR = lpDSB->Lock (0, dsbd.dwBufferBytes, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);

    if ( m_hR == DS_OK ) {
        ::CopyMemory (pMem1, pWave->m_pWaveData, dwSize1);

        if ( dwSize2 != 0 )
            ::CopyMemory (pMem2, (pWave->m_pWaveData+dwSize1), dwSize2);

        lpDSB->Unlock (pMem1, dwSize1, pMem2, dwSize2);

        if ( dsbd.dwFlags & DSBCAPS_CTRLVOLUME )
            lpDSB->SetVolume (DSBVOLUME_MAX);

        if ( dsbd.dwFlags & DSBCAPS_CTRLFREQUENCY )
            lpDSB->SetFrequency (DSBFREQUENCY_ORIGINAL);

        if ( dsbd.dwFlags & DSBCAPS_CTRLPAN )
            lpDSB->SetPan (DSBPAN_CENTER);

		// return lpDSB;
		t_sounddata *pSound;
		pSound = new t_sounddata;
		if ( pSound ) {
			pSound->m_pDSB = lpDSB;
			return pSound;
		}
	} 
	
    lpDSB->Release ();
    lpDSB = NULL;

    return NULL;
}

//-------------------------------------------------------------------------------------------------
t_sounddata *CDSOUND::CreateSoundData (CWaveFile *pCWave, DWORD dwCreationFlags, GUID guid3DAlgorithm)
{
    if( m_pDS == NULL )
        return NULL;

    DSBUFFERDESC	dsbd;
    LPDIRECTSOUNDBUFFER lpDSB;
    LPVOID			pMem1, pMem2;
    DWORD			dwSize1, dwSize2;

    ::ZeroMemory (&dsbd, sizeof (dsbd));

    dsbd.dwSize = sizeof (DSBUFFERDESC);
    // DSBCAPS_CTRLPAN | DSBCAPS_GETCURRENTPOSITION2 
    // DSBCAPS_CTRLFREQUENCY
	// DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN
	// DSBCAPS_CTRL3D;
    dsbd.dwFlags	     = dwCreationFlags;
    dsbd.lpwfxFormat     = pCWave->m_pwfx;
    dsbd.dwBufferBytes   = pCWave->m_dwSize;//m_dwDataSize;
	dsbd.guid3DAlgorithm = guid3DAlgorithm;

    m_hR = m_pDS->CreateSoundBuffer (&dsbd, &lpDSB, NULL);
    if ( m_hR != DS_OK ) 
		return NULL;

    m_hR = lpDSB->Lock (0, dsbd.dwBufferBytes, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);

    pCWave->ResetFile();
	DWORD dwWavDataRead = 0;    // Amount of data read from the wav file 
    if ( m_hR == DS_OK ) {
		// ::CopyMemory (pMem1, pWave->m_pWaveData, dwSize1);
		if ( FAILED( pCWave->Read( (BYTE*)pMem1, dwSize1, &dwWavDataRead ) ) )
			goto _RET_ERROR;
		if( dwWavDataRead == 0 ) {
			// Wav is blank, so just fill with silence
			::FillMemory( (BYTE*) pMem1, dwSize1, (BYTE)(pCWave->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
		} else
        if ( dwSize2 != 0 ) {
            // ::CopyMemory (pMem2, (pWave->m_pWaveData+dwSize1), dwSize2);
			if ( FAILED( pCWave->Read( (BYTE*)pMem2, dwSize2, &dwWavDataRead ) ) )
				goto _RET_ERROR;
			if( dwWavDataRead == 0 )
				// Wav is blank, so just fill with silence
				::FillMemory( (BYTE*) pMem2, dwSize2, (BYTE)(pCWave->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
		}

        lpDSB->Unlock (pMem1, dwSize1, pMem2, dwSize2);

        if ( dsbd.dwFlags & DSBCAPS_CTRLVOLUME )
            lpDSB->SetVolume (DSBVOLUME_MAX);

        if ( dsbd.dwFlags & DSBCAPS_CTRLFREQUENCY )
            lpDSB->SetFrequency (DSBFREQUENCY_ORIGINAL);

        if ( dsbd.dwFlags & DSBCAPS_CTRLPAN )
            lpDSB->SetPan (DSBPAN_CENTER);

		// return lpDSB;
		t_sounddata *pSound;
		pSound = new t_sounddata;
		if ( pSound ) {
			pSound->m_pDSB = lpDSB;
			return pSound;
		}
	} 
	
_RET_ERROR:
    lpDSB->Release ();
    lpDSB = NULL;

    return NULL;
}


//-------------------------------------------------------------------------------------------------
t_sounddata *CDSOUND::DuplicateSoundData (LPDIRECTSOUNDBUFFER lpDSBSour)
{
    LPDIRECTSOUNDBUFFER lpDSBDest;

    m_hR = m_pDS->DuplicateSoundBuffer (lpDSBSour, &lpDSBDest);
	if( FAILED( m_hR ) ) {
        ::OutputDebugString ( "DuplicateSoundBuffer\n" );
        return NULL;
    }
                    
	t_sounddata *pSound;
	pSound = new t_sounddata;
	pSound->m_pDSB = lpDSBDest;

    return pSound;
}


//-------------------------------------------------------------------------------------------------
// #define	__USE_CWAVE_CLASS
t_sounddata *CDSOUND::LoadSoundData (LPSTR szFileName, short nMaxMixCnt, DWORD dwCreationFlags, GUID guid3DAlgorithm, long lFilePtr)
{
    if ( m_pDS == NULL ) return NULL;

    t_sounddata *pSound;

#ifndef	__USE_CWAVE_CLASS
    t_wavedata  *pWave;
    pWave = ::WaveData_Load (szFileName, lFilePtr);
    if ( pWave == NULL ) 
		return NULL;
	
    pSound = this->CreateSoundData (pWave, dwCreationFlags, guid3DAlgorithm);
	::WaveData_Free (pWave);
#else
    CWaveFile *pWaveFile = NULL;

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL ) {
		return NULL;
    }

	pWaveFile->Open( szFileName, NULL, WAVEFILE_READ );
    if ( pWaveFile->GetSize() == 0 ) {
        // Wave is blank, so don't create it.
		return NULL;
    }

    pSound = this->CreateSoundData (pWaveFile, dwCreationFlags, guid3DAlgorithm);
	SAFE_DELETE( pWaveFile );
#endif

	if ( pSound ) {
	    pSound->m_pNext = NULL;
		pSound->m_nMaxMixCnt = nMaxMixCnt;
	}

    return pSound;
}


//-------------------------------------------------------------------------------------------------
void CDSOUND::FreeSoundData (t_sounddata *pData)
{
    t_sounddata *pNext;

    while ( pData != NULL ) {
        pNext = pData->m_pNext;
		SAFE_DELETE( pData );
        pData = pNext;
    }
}


//-------------------------------------------------------------------------------------------------
DWORD CDSOUND::GetStatus (t_sounddata *pData)
{
	DWORD dwStatus;

	pData->m_pDSB->GetStatus (&dwStatus);

	return dwStatus;
}


//-------------------------------------------------------------------------------------------------
void  CDSOUND::SetMaxMixCount (t_sounddata *pData, short nMaxMinCnt)
{
	if ( pData ) {
		pData->m_nMaxMixCnt = nMaxMinCnt;
	}
}

//-------------------------------------------------------------------------------------------------
short CDSOUND::GetMaxMixCount (t_sounddata *pData)
{
	if ( pData ) {
		return pData->m_nMaxMixCnt;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
void CDSOUND::PlaySound (t_sounddata *pData, long lVolume, long lPan, DWORD dwFlags)
{
    static DWORD		dwStatus;
    static t_sounddata *pSound;
    static short		nMixCnt;

    if ( m_pDS == NULL )
        return;

    if ( pData == NULL || 
		 pData->m_pDSB == NULL ) return;

    pSound = pData;

    nMixCnt = 0;
    while ( true ) {
        pSound->m_pDSB->GetStatus( &dwStatus );
        if ( dwStatus == DSBSTATUS_PLAYING ||
             dwStatus == DSBSTATUS_LOOPING ) {

            if ( ++nMixCnt >= pData->m_nMaxMixCnt ) return;

            if ( pSound->m_pNext == NULL ) {
                pSound->m_pNext = this->DuplicateSoundData (pData->m_pDSB);
				if ( NULL == pSound->m_pNext )
					return;
                pSound->m_pNext->m_pNext = NULL;

				/*
                pSound->m_pNext = (t_sounddata*) new t_sounddata;
                pSound = pSound->m_pNext;
                pSound->m_pDSB = this->DuplicateSoundData (pData->m_pDSB);
                pSound->m_pNext = NULL;
                if ( pSound->m_pDSB == NULL )
                    return;
				*/
            } else {
                pSound = pSound->m_pNext;
                continue;
            }
        }

        break;
   }

    pSound->m_pDSB->SetVolume ( lVolume );
	
    pSound->m_pDSB->SetPan ( lPan );
    m_hR = pSound->m_pDSB->Play (0, 0, dwFlags);
}


//-------------------------------------------------------------------------------------------------
void CDSOUND::StopSound (t_sounddata *pData)
{
    t_sounddata *pNext;

    while ( pData != NULL ) {
        pNext = pData->m_pNext;

        if ( pData->m_pDSB != NULL ) 
            pData->m_pDSB->Stop ();

        pData = pNext;
    }
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/*
CD3DSOUND::CD3DSOUND ()
{
	m_pDSListener = NULL;
	//DS3DALG_NO_VIRTUALIZATION;
	//DS3DALG_HRTF_FULL;
	//DS3DALG_HRTF_LIGHT;
	m_guid3DAlgorithm = DS3DALG_DEFAULT;
}
CD3DSOUND::~CD3DSOUND ()
{
	SAFE_RELEASE( m_pDSListener );
	CDSOUND::_Free ();
}

//-------------------------------------------------------------------------------------------------
HRESULT CD3DSOUND::Get3DListenerInterface ( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( ppDSListener == NULL )
        return E_INVALIDARG;
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
    if ( FAILED( m_hR = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), m_hR );

    if ( FAILED( m_hR = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, (VOID**)ppDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR( TEXT("QueryInterface"), m_hR );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}

//-------------------------------------------------------------------------------------------------
bool CD3DSOUND::_Init( HWND  hWnd, 
					  DWORD dwCoopLevel, 
                      DWORD dwPrimaryChannels, 
                      DWORD dwPrimaryFreq, 
                      DWORD dwPrimaryBitRate )
{
	if ( !CDSOUND::_Init( hWnd, dwCoopLevel, dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) )
		return false;

	m_hR = this->Get3DListenerInterface( &m_pDSListener );
	if ( FAILED( m_hR ) ) {
		return false;
	}

    // Get listener parameters
    m_dsListenerParams.dwSize = sizeof(DS3DLISTENER);
    m_pDSListener->GetAllParameters( &m_dsListenerParams );

	// Set distance factor 
	float fDistanceFactor = 0.01f; // engine space(cm) => dsound space(meter)

	if (FAILED(m_pDSListener->SetDistanceFactor(fDistanceFactor, DS3D_DEFERRED))) {
		return false;
	}

	//if (FAILED(m_pDSListener->SetRolloffFactor(fGlobal3DVolume * (DS3D_MAXROLLOFFFACTOR - DS3D_MINROLLOFFFACTOR), DS3D_DEFERRED))) {
	//	return false;
	//}
	//
	return true;
}


//-------------------------------------------------------------------------------------------------
t_sounddata* CD3DSOUND::CreateSoundData (t_wavedata *pWave, DWORD dwCreationFlags, GUID guid3DAlgorithm)
{
	t_sounddata *pSound;

	pSound = CDSOUND::CreateSoundData(pWave, dwCreationFlags, guid3DAlgorithm);
	if ( pSound ) {
		m_hR = pSound->m_pDSB->QueryInterface( IID_IDirectSound3DBuffer, (VOID**)&pSound->m_pDS3DBuffer );
		if ( FAILED( m_hR ) ) {
			this->FreeSoundData( pSound );
	        DXTRACE_ERR( TEXT("3DS->QueryInterface"), m_hR );
			return NULL;
		}

		// Get the 3D buffer parameters
		m_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
		pSound->m_pDS3DBuffer->GetAllParameters( &m_dsBufferParams );

		// Set new 3D buffer parameters
		m_dsBufferParams.flMaxDistance = SOUND_DEFAULT_MAX_DISTANCE; // 여기까지만 볼륨적용
		m_dsBufferParams.flMinDistance = SOUND_DEFAULT_MIN_DISTANCE; //  여기까지는 최대 볼륨 적용
		m_dsBufferParams.dwMode = DS3DMODE_NORMAL;
		pSound->m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_DEFERRED );
	}

	return pSound;
}

t_sounddata	*CD3DSOUND::LoadSoundData (LPSTR szFileName, short nMaxMixCnt, GUID guid3DAlgorithm, long lFilePtr)
{
	DWORD dwCreationFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;

	return CDSOUND::LoadSoundData(szFileName, nMaxMixCnt, dwCreationFlags, guid3DAlgorithm, lFilePtr);
}

//-------------------------------------------------------------------------------------------------
void CD3DSOUND::Set3DBufferParam (t_sounddata *pSound, DS3DBUFFER &dsBufferParam)
{
	pSound->m_pDS3DBuffer->SetAllParameters( &dsBufferParam, DS3D_DEFERRED );
}

//-------------------------------------------------------------------------------------------------
void CD3DSOUND::SetListenerParam (void)
{
//typedef struct _DS3DLISTENER
//{
//    DWORD           dwSize;
//    D3DVECTOR       vPosition;
//    D3DVECTOR       vVelocity;
//    D3DVECTOR       vOrientFront;
//    D3DVECTOR       vOrientTop;
//    D3DVALUE        flDistanceFactor;
//    D3DVALUE        flRolloffFactor;
//    D3DVALUE        flDopplerFactor;
//} DS3DLISTENER, *LPDS3DLISTENER;

	if ( !m_pDSListener )
		return;
	
	// m_dsListenerParams.

	// DS3D_DEFERRED	: 애플리케이션이 IDirectSound3DListener8::CommitDeferredSettings 메서드를 호출할 때까지, 설정은 적용되지 않는다. 복수의 설정을 변경한 후, 1 회의 재계산으로 그것들을 적용할 수 있다.  
	// DS3D_DEFERRED	: 설정을 즉시 적용해, 시스템은 모든 3D 사운드 버퍼의 3D 좌표를 재계산한다.  
    m_pDSListener->SetAllParameters( &m_dsListenerParams, DS3D_DEFERRED );
}

//-------------------------------------------------------------------------------------------------
void CD3DSOUND::ConvertToSoundSpace ( D3DXVECTOR3& posSoundSpace, const D3DXVECTOR3& posWorldSpace )
{
	posSoundSpace.x = posWorldSpace.x;
	posSoundSpace.y = posWorldSpace.z; // swap y-z
	posSoundSpace.z = posWorldSpace.y;
}

//-------------------------------------------------------------------------------------------------
bool CD3DSOUND::SetListenerPosition (const D3DXVECTOR3& PosWorld)
{
	if (!m_pDSListener) return false;

	D3DXVECTOR3 PosSound;
	ConvertToSoundSpace(PosSound, PosWorld);
	if (FAILED(m_pDSListener->SetPosition(PosSound.x, PosSound.y, PosSound.z, DS3D_DEFERRED))) {
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CD3DSOUND::SetListenerOrientation (const D3DXVECTOR3& PosFront, const D3DXVECTOR3& PosTop)
{
	if (!m_pDSListener) return false;
	
	if (FAILED(m_pDSListener->SetOrientation(PosFront.x, PosFront.z, PosFront.y, PosTop.x, PosTop.z, PosTop.y, DS3D_DEFERRED))) {
		return false;
	}
	return true;
}

bool CD3DSOUND::SetListenerRolloffFactor ( float fRolloffFactor )
{
	if (!m_pDSListener) return false;
	
	if (FAILED(m_pDSListener->SetRolloffFactor( fRolloffFactor, DS3D_DEFERRED))) {
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CD3DSOUND::UpdateListener ( const CCamera * pCamera )
{
	if ( !m_pDSListener ) return false;

	D3DXVECTOR3 posEye, dirFront, dirTop;

	HNODE hCamera = pCamera->GetZHANDLE();
	if ( 0 == getCameraEye( hCamera, posEye ) ) return false;
	if ( 0 == getCameraDir( hCamera, dirFront ) ) return false;
	if ( 0 == getCameraUp( hCamera, dirTop ) ) return false;

	// 아바타의 위치로 리스너 위치 세팅
	//if ( FAILED( SetListenerPosition( g_pAVATAR->Get_CurPOS() ) ) ) {
	//	return false;
	//}

	if ( FAILED( SetListenerPosition( posEye ) ) ) {
		return false;
	}
	
	if ( FAILED( SetListenerOrientation( dirFront, dirTop ) ) ) {
		return false;
	}

	if  (FAILED ( m_pDSListener->CommitDeferredSettings() ) ) {
		return false;
	}

	return true;
}

bool CD3DSOUND::Set3D ( t_sounddata * pData, bool b3DMode )
{
	if (!pData || !pData->m_pDS3DBuffer) return false;

	DWORD dwMode = (b3DMode) ? DS3DMODE_NORMAL : DS3DMODE_DISABLE;
	return !FAILED( pData->m_pDS3DBuffer->SetMode( dwMode, DS3D_DEFERRED ) );
}

bool CD3DSOUND::SetPosition ( t_sounddata * pData, const D3DXVECTOR3& posWorld )
{
	if (!pData || !pData->m_pDS3DBuffer) return false;

	D3DXVECTOR3 posSound;
	ConvertToSoundSpace( posSound, posWorld );

	return !FAILED(pData->m_pDS3DBuffer->SetPosition( posSound.x, posSound.y, posSound.z, DS3D_DEFERRED));
}

bool CD3DSOUND::SetVelocity ( t_sounddata * pData, const D3DXVECTOR3& posVelocity )
{
	if (!pData || !pData->m_pDS3DBuffer) return false;

	D3DXVECTOR3 posSound;
	ConvertToSoundSpace( posSound, posVelocity );

	return !FAILED(pData->m_pDS3DBuffer->SetVelocity( posSound.x, posSound.y, posSound.z, DS3D_DEFERRED));
}

bool CD3DSOUND::SetMinDistance ( t_sounddata * pData, float fMinDistance )
{
	if (!pData || !pData->m_pDS3DBuffer) return false;

	return !FAILED(pData->m_pDS3DBuffer->SetMinDistance( fMinDistance, DS3D_DEFERRED));
}

bool CD3DSOUND::SetMaxDistance ( t_sounddata * pData, float fMaxDistance )
{
	if (!pData || !pData->m_pDS3DBuffer) return false;

	return !FAILED(pData->m_pDS3DBuffer->SetMaxDistance( fMaxDistance, DS3D_DEFERRED));
}
*/