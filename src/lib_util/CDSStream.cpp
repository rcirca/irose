

#include <Windows.h>
#include <DSound.h>

// #include "iCARUS_DLL.h"
#include "CDSSTREAM.h"
#include "classLOG.h"

//-------------------------------------------------------------------------------------------------

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


//-------------------------------------------------------------------------------------------------
classDSOUNDSTREAM::classDSOUNDSTREAM()
{
    m_hNotificationEvents[ 0 ] = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_hNotificationEvents[ 1 ] = CreateEvent( NULL, FALSE, FALSE, NULL );

	_Init();
}
classDSOUNDSTREAM::~classDSOUNDSTREAM()
{
	_Free();

	if ( m_hNotificationEvents[ 0 ] != NULL ) {
		CloseHandle( m_hNotificationEvents[ 0 ] );
		m_hNotificationEvents[ 0 ] = NULL;
	}

	if ( m_hNotificationEvents[ 1 ] != NULL ) {
	    CloseHandle( m_hNotificationEvents[ 1 ] );
		m_hNotificationEvents[ 1 ] = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
void classDSOUNDSTREAM::_Init (void)
{
	m_bMP3Stream		= false;

	m_pClassMP3			= NULL;
	m_pClassWAV			= NULL;

	m_pDSBuffer			= NULL;
	m_pDSNotify			= NULL;

//    ZeroMemory( &m_sDSBPosNotify, sizeof(DSBPOSITIONNOTIFY) * (NUM_PLAY_NOTIFICATIONS + 1) );
	m_pDSBPosNotify		= NULL;
    m_dwBufferSize      = 0;
    m_dwNotifySize      = 0;
    m_dwNextWriteOffset = 0;
    m_dwProgress        = 0;
    m_dwLastPos         = 0;
    m_bFoundEnd         = false;
}


//-------------------------------------------------------------------------------------------------
void classDSOUNDSTREAM::_Free (void)
{
	SAFE_RELEASE( m_pDSNotify );
	SAFE_RELEASE( m_pDSBuffer );

	SAFE_DELETE ( m_pClassMP3 );
	SAFE_DELETE ( m_pClassWAV );

	SAFE_DELETE ( m_pDSBPosNotify );
}


//-------------------------------------------------------------------------------------------------
bool classDSOUNDSTREAM::OpenMP3StreamFile (char *szMP3File, LPDIRECTSOUND pDirectSound, int iBufferingSec, short nNumPlayNotifications, bool bAlreadyCoInitialized)
{
	SAFE_DELETE ( m_pClassMP3 );

	m_pClassMP3 = new classMP3();
	m_pClassMP3->_Init ( bAlreadyCoInitialized );
//	if ( !m_pClassMP3->OpenFile (szMP3File, iBufferingSec, NUM_PLAY_NOTIFICATIONS) )
	if ( !m_pClassMP3->OpenFile (szMP3File, iBufferingSec, nNumPlayNotifications) )
		return false;

	if ( S_OK != CreateStreamBuffer (pDirectSound, &m_pClassMP3->m_sWFX, iBufferingSec) )
		return false;

	CLOG::OutputString (LOG_NORMAL, "classDSOUNDSTREAM::OpenMP3StreamFile\n");

	m_bMP3Stream = true;
	return true;
}


//-------------------------------------------------------------------------------------------------
bool classDSOUNDSTREAM::OpenWAVStreamFile (char *szWAVFile, LPDIRECTSOUND pDirectSound, int iBufferingSec, short nNumPlayNotifications, long lFilePtr)
{
	SAFE_DELETE ( m_pClassWAV );

	m_pClassWAV = new classWAV();
	m_pClassWAV->_Init ();
	if ( !m_pClassWAV->OpenFile (szWAVFile, lFilePtr) )
		return false;

	if ( S_OK != CreateStreamBuffer (pDirectSound, &m_pClassWAV->m_sWFX, iBufferingSec) ) {
		CLOG::OutputString (LOG_NORMAL, "classDSOUNDSTREAM::CreateStreamBuffer Failed\n");
		return false;
	}
	CLOG::OutputString (LOG_NORMAL, "classDSOUNDSTREAM::OpenWAVStreamFile\n");

	m_bMP3Stream = false;
	return true;
}

//-------------------------------------------------------------------------------------------------
void classDSOUNDSTREAM::SetVolume (long lVolume)
{
	HRESULT hR;

	if ( m_pDSBuffer ) {
		hR = m_pDSBuffer->SetVolume ( lVolume );
		switch ( hR ) {
			case DSERR_CONTROLUNAVAIL	:
				CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_CONTROLUNAVAIL\n");
				break;
			case DSERR_GENERIC			:
				CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_GENERIC\n");
				break;
			case DSERR_INVALIDPARAM		:
				CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_INVALIDPARAM\n");
				break;
			case DSERR_PRIOLEVELNEEDED  :
				CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_PRIOLEVELNEEDED\n");
				break;
			default :
				CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume return  %d\n", hR);
				break;
		}
	} else {
		CLOG::OutputString (LOG_NORMAL, "DSStream::SetVolume Buffer == NULL \n");
	}
}


//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::CreateStreamBuffer (LPDIRECTSOUND pDirectSound, WAVEFORMATEX *pWFX, int iBufferingSec, short nNumPlayNotifications)
{
	CLOG::OutputString (LOG_NORMAL, "classDSOUNDSTREAM::CreateStreamBuffer\n");

	if ( !pDirectSound ) 
		return E_FAIL;

	m_nNumPlayNotifications = nNumPlayNotifications;

//    ZeroMemory( &m_sDSBPosNotify, sizeof(DSBPOSITIONNOTIFY) * (NUM_PLAY_NOTIFICATIONS + 1) );
	m_pDSBPosNotify = new DSBPOSITIONNOTIFY[ m_nNumPlayNotifications+1 ];

	m_wBitsPerSample	= pWFX->wBitsPerSample;

    m_dwBufferSize      = 0;
    m_dwNotifySize      = 0;
    m_dwNextWriteOffset = 0;
    m_dwProgress        = 0;
    m_dwLastPos         = 0;
    m_bFoundEnd         = false;

	SAFE_RELEASE( m_pDSNotify );
	SAFE_RELEASE( m_pDSBuffer );

	HRESULT	hResult;

    // This samples works by dividing a 3 second streaming buffer into 
    // NUM_PLAY_NOTIFICATIONS (or 16) pieces.  it creates a notification for each
    // piece and when a notification arrives then it fills the circular streaming 
    // buffer with new wav data over the sound data which was just played

    // The size of wave data is in pWaveFileSound->m_ckIn
    DWORD	dwBlockAlign;
    int		iSamplesPerSec;

    dwBlockAlign	= (DWORD)pWFX->nBlockAlign;
    iSamplesPerSec	=		 pWFX->nSamplesPerSec;

    // The m_dwNotifySize should be an integer multiple of nBlockAlign
//  m_dwNotifySize	= iSamplesPerSec * iBufferingSec * dwBlockAlign / NUM_PLAY_NOTIFICATIONS;
    m_dwNotifySize	= iSamplesPerSec * iBufferingSec * dwBlockAlign / m_nNumPlayNotifications;
    m_dwNotifySize -= m_dwNotifySize % dwBlockAlign;   

    // The buffersize should approximately 3 seconds of wav data
//  m_dwBufferSize  = m_dwNotifySize * NUM_PLAY_NOTIFICATIONS;
    m_dwBufferSize  = m_dwNotifySize * m_nNumPlayNotifications;
    m_bFoundEnd     = false;
    m_dwProgress    = 0;
    m_dwLastPos     = 0;

    DSBUFFERDESC        dsbd;
    ZeroMemory (&dsbd, sizeof (dsbd));

    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = m_dwBufferSize;
    dsbd.lpwfxFormat   = pWFX;

    hResult = pDirectSound->CreateSoundBuffer (&dsbd, &m_pDSBuffer, NULL);
    if ( hResult != DS_OK ) 
		return hResult;

    // Create a notification event, for when the sound stops playing
    hResult = m_pDSBuffer->QueryInterface( IID_IDirectSoundNotify, (VOID**)&m_pDSNotify );
	if ( FAILED( hResult ) ) {
		SAFE_RELEASE( m_pDSBuffer );
		return hResult;
	}

	short nI;
/*
	for( nI=0; nI<NUM_PLAY_NOTIFICATIONS; nI++ ) {
		m_sDSBPosNotify[ nI ].dwOffset	   = (m_dwNotifySize * nI) + m_dwNotifySize - 1;
		m_sDSBPosNotify[ nI ].hEventNotify =  m_hNotificationEvents[ 0 ];
	}
*/
    for( nI=0; nI<m_nNumPlayNotifications; nI++ )
    {
        m_pDSBPosNotify[ nI ].dwOffset	   = (m_dwNotifySize * nI) + m_dwNotifySize - 1;
        m_pDSBPosNotify[ nI ].hEventNotify =  m_hNotificationEvents[ 0 ];
    }
    
//    m_sDSBPosNotify[ NUM_PLAY_NOTIFICATIONS ].dwOffset     = DSBPN_OFFSETSTOP;
//    m_sDSBPosNotify[ NUM_PLAY_NOTIFICATIONS ].hEventNotify = m_hNotificationEvents[ 1 ];
    m_pDSBPosNotify[ m_nNumPlayNotifications ].dwOffset     = DSBPN_OFFSETSTOP;
    m_pDSBPosNotify[ m_nNumPlayNotifications ].hEventNotify = m_hNotificationEvents[ 1 ];


    // Tell DirectSound when to notify us. the notification will come in the from 
    // of signaled events that are handled in WinMain()
//    hResult = m_pDSNotify->SetNotificationPositions( NUM_PLAY_NOTIFICATIONS + 1, m_sDSBPosNotify );
    hResult = m_pDSNotify->SetNotificationPositions( m_nNumPlayNotifications + 1, m_pDSBPosNotify );
	if ( FAILED( hResult ) ) {
		// Release DirectSound interfaces
		SAFE_RELEASE( m_pDSNotify );
		SAFE_RELEASE( m_pDSBuffer );

        return hResult;
	}

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: Play the DirectSound buffer
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::PlayStreamBuffer ( bool bLooped )
{
    HRESULT hResult;
    VOID*   pbBuffer = NULL;

    if ( NULL == m_pDSBuffer )
        return E_FAIL;

	m_bLooped = bLooped;

    // Restore the buffers if they are lost
    if ( FAILED( hResult = RestoreBuffers() ) )
        return hResult;

    // Fill the entire buffer with wave data
    if ( FAILED( hResult = FillBuffer () ) )
        return hResult;

    // Always play with the LOOPING flag since the streaming buffer
    // wraps around before the entire WAV is played
    if ( FAILED( hResult = m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING ) ) )
        return hResult;

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: StopBuffer()
// Desc: Stop the DirectSound buffer
//-------------------------------------------------------------------------------------------------
void classDSOUNDSTREAM::StopStreamBuffer (void) 
{
    if ( NULL == m_pDSBuffer )
		return;

    HRESULT hr;
    void*   pbBuffer = NULL;
    DWORD   dwBufferLength;

    m_pDSBuffer->Stop();
    m_pDSBuffer->SetCurrentPosition( 0 );

    // Lock the buffer down, 
    if( FAILED( hr = m_pDSBuffer->Lock( 0, m_dwBufferSize, &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return;

	FillMemory( pbBuffer, m_dwBufferSize, (BYTE)( m_wBitsPerSample == 8 ? 128 : 0 ) );

    // Now unlock the buffer
    m_pDSBuffer->Unlock( pbBuffer, m_dwBufferSize, NULL, 0 );
}


//-------------------------------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::RestoreBuffers (void)
{
    HRESULT hResult;

    if ( NULL == m_pDSBuffer )
        return S_OK;

    DWORD dwStatus;
    if( FAILED( hResult = m_pDSBuffer->GetStatus( &dwStatus ) ) )
        return hResult;

    if ( dwStatus & DSBSTATUS_BUFFERLOST ) {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do  {
            hResult = m_pDSBuffer->Restore();
            if( hResult == DSERR_BUFFERLOST )
                Sleep( 10 );
        } while( hResult = m_pDSBuffer->Restore() );

        if( FAILED( hResult = FillBuffer () ) )
            return hResult;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: FillBuffer()
// Desc: Fills the DirectSound buffer with wave data
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::FillBuffer (void)
{
    HRESULT hr;
    void*   pbBuffer = NULL;
    DWORD   dwBufferLength;

    if ( NULL == m_pDSBuffer )
        return E_FAIL;

    m_bFoundEnd			= false;
    m_dwNextWriteOffset = 0; 
    m_dwProgress		= 0;
    m_dwLastPos			= 0;

    // Reset the wav file to the beginning
	if ( m_bMP3Stream )
		m_pClassMP3->Reset();
	else
		m_pClassWAV->Reset();

    m_pDSBuffer->SetCurrentPosition( 0 );

    // Lock the buffer down, 
    if( FAILED( hr = m_pDSBuffer->Lock( 0, m_dwBufferSize, &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return hr;

    // Fill the buffer with wav data 
    if( FAILED( hr = WriteToBuffer (pbBuffer, dwBufferLength ) ) )
        return hr;

    // Now unlock the buffer
    m_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );

    m_dwNextWriteOffset  = dwBufferLength; 
    m_dwNextWriteOffset %= m_dwBufferSize; // Circular buffer

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: WriteToBuffer()
// Desc: Writes wave data to the streaming DirectSound buffer 
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::WriteToBuffer (void* pbBuffer, DWORD dwBufferLength)
{
    HRESULT hr;
    UINT	nActualBytesWritten;

    if ( !m_bFoundEnd ) {
        // Fill the DirectSound buffer with WAV data
		if ( m_bMP3Stream ) {
			// MP3
			if( FAILED( hr = m_pClassMP3->ReadData( dwBufferLength, (BYTE*) pbBuffer, &nActualBytesWritten ) ) )
				return hr;

			m_dwReadData = m_pClassMP3->m_dwReadData;
		} else {
			// WAV
			if( FAILED( hr = m_pClassWAV->ReadData( dwBufferLength, (BYTE*) pbBuffer, &nActualBytesWritten ) ) )
				return hr;

			m_dwReadData = m_pClassWAV->m_dwReadData;
		}
    } else {
        // Fill the DirectSound buffer with silence
        FillMemory( pbBuffer, dwBufferLength, (BYTE)( m_wBitsPerSample == 8 ? 128 : 0 ) );
		return S_OK;
    }

    // If the number of bytes written is less than the 
    // amount we requested, we have a short file.
    if ( nActualBytesWritten < dwBufferLength ) {
        if ( !m_bLooped ) {
            m_bFoundEnd = true;

            // Fill in silence for the rest of the buffer.
            FillMemory( (BYTE*) pbBuffer + nActualBytesWritten, 
                        dwBufferLength - nActualBytesWritten, (BYTE)( m_wBitsPerSample == 8 ? 128 : 0 ) );
        } else {
            // We are looping.
            UINT nWritten = nActualBytesWritten;    // From previous call above.

			if ( m_bMP3Stream ) {
				// MP3
				while( nWritten < dwBufferLength )
				{  
					// This will keep reading in until the buffer is full. For very short files.
					m_pClassMP3->Reset();
					
					if( FAILED( hr = m_pClassMP3->ReadData( (UINT)dwBufferLength - nWritten,
															  (BYTE*)pbBuffer + nWritten, &nActualBytesWritten ) ) )
						return hr;

					m_dwReadData = m_pClassMP3->m_dwReadData;
					nWritten += nActualBytesWritten;
				} 
			} else {
				// WAV
				while( nWritten < dwBufferLength )
				{  
					// This will keep reading in until the buffer is full. For very short files.
					m_pClassWAV->Reset();
					
					if( FAILED( hr = m_pClassWAV->ReadData( (UINT)dwBufferLength - nWritten,
															  (BYTE*)pbBuffer + nWritten, &nActualBytesWritten ) ) )
						return hr;

					m_dwReadData = m_pClassWAV->m_dwReadData;
					nWritten += nActualBytesWritten;
				} 
			}
        } 
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: UpdateProgress()
// Desc: Update the progress variable to know when the entire buffer was played
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::UpdateProgress(void)
{
    HRESULT hr;
    DWORD   dwPlayPos;
    DWORD   dwWritePos;
    DWORD   dwPlayed;

    if ( FAILED( hr = m_pDSBuffer->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return hr;

    if( dwPlayPos < m_dwLastPos )
        dwPlayed = m_dwBufferSize - m_dwLastPos + dwPlayPos;
    else
        dwPlayed = dwPlayPos - m_dwLastPos;

    m_dwProgress += dwPlayed;
    m_dwLastPos   = dwPlayPos;

    return S_OK;
}



//-------------------------------------------------------------------------------------------------
// Name: HandleNotification()
// Desc: Handle the notification that tell us to put more wav data in the 
//       circular buffer
//-------------------------------------------------------------------------------------------------
HRESULT classDSOUNDSTREAM::HandleNotification (void) 
{
    HRESULT			hr;
    VOID* pbBuffer  = NULL;
    DWORD dwBufferLength;

    UpdateProgress();

    // Lock the buffer down
    if( FAILED( hr = m_pDSBuffer->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
                                        &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return hr;

    // Fill the buffer with wav data 
    if( FAILED( hr = WriteToBuffer (pbBuffer, dwBufferLength ) ) ) {
	    m_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );
        return hr;
	}

    // Now unlock the buffer
    m_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );
    pbBuffer = NULL;

    // If the end was found, detrimine if there's more data to play 
    // and if not, stop playing
    if ( m_bFoundEnd ) {
        // We don't want to cut off the sound before it's done playing.
        // if doneplaying is set, the next notification event will post a stop message.
        if ( m_dwReadData > m_dwNotifySize ) {
            if ( m_dwProgress >= m_dwReadData - m_dwNotifySize )
                m_pDSBuffer->Stop ();
        } else {	// For short files.
            if ( m_dwProgress >= m_dwReadData ) 
                m_pDSBuffer->Stop ();
        }
    }

    m_dwNextWriteOffset += dwBufferLength; 
    m_dwNextWriteOffset %= m_dwBufferSize; // Circular buffer

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


