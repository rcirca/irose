
#include "stdAFX.h"
#include <process.h>
#include "ADSStream.h"

#ifdef	_DEBUG
	#pragma comment ( lib, "ogg_static_d.lib")
	#pragma comment ( lib, "vorbisfile_static_d.lib")
#else
	#pragma comment ( lib, "ogg_static.lib")
	#pragma comment ( lib, "vorbisfile_static.lib")
#endif


//-------------------------------------------------------------------------------------------------

#define SAFE_DELETE(p)			{ if(p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL; } }


void ODS (char *fmt, ...)
{
	//static char m_StrBUFF[ 2048 ];
 //   try {
 //       va_list argptr;
 //       va_start(argptr, fmt);
 //       vsprintf(m_StrBUFF,fmt,argptr);
 //       va_end(argptr);
 //   }
 //   catch( ... ) {
 //       return;
 //   }

	//::OutputDebugString( m_StrBUFF );
}




//-----------------------------------------------------------------------------
// Name: NotificationProc()
// Desc: Handles dsound notifcation events
//-----------------------------------------------------------------------------
unsigned int __stdcall ThreadDSStream( void *lpParameter )
{
	ADirectSoundSTREAM *pStream = (ADirectSoundSTREAM*)lpParameter;

	::SetThreadPriority( pStream->m_hNotifyThread, THREAD_PRIORITY_ABOVE_NORMAL );

    DWORD   dwResult;
    BOOL    bDone = FALSE;
	
	pStream->m_bTerminated = false;
    while( !bDone ) 
    { 
		dwResult = WaitForSingleObject( pStream->m_hCheckTimeEvent, pStream->m_dwPlayTimePerBlock );
        switch( dwResult )
        {
			case WAIT_TIMEOUT :
				while( pStream->IsCurrentBlockPassed() ) {
					if( FAILED( pStream->HandleNotification () ) ) {
						bDone = TRUE;
						break;
					}
				}
				break;
            case WAIT_OBJECT_0 + 0:
				bDone = TRUE;
				break;
        }
    }

	pStream->StopStreamBuffer ();
	pStream->m_bTerminated = true;

    return 0;
}


//-------------------------------------------------------------------------------------------------
ADirectSoundSTREAM::ADirectSoundSTREAM()
{
	m_hCheckTimeEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	m_dwPlayTimePerBlock = 1000;
	m_pDataBlock = NULL;

	_Init ();

    m_hNotifyThread = (HANDLE)_beginthreadex( NULL, 0, ThreadDSStream, this, CREATE_SUSPENDED, &m_dwNotifyThreadID );
}

void ADirectSoundSTREAM::CloseStreamThread ()
{
	if ( m_hNotifyThread ) {
#ifdef	__USE_DS_NOTIFY
		::PostThreadMessage( m_dwNotifyThreadID, WM_QUIT, 0, 0 );
#else
		::SetEvent( m_hCheckTimeEvent );
#endif
		::WaitForSingleObject( m_hNotifyThread, INFINITE );
		::CloseHandle( m_hNotifyThread );
		m_hNotifyThread = NULL;
		m_dwNotifyThreadID = 0;
	}
}

ADirectSoundSTREAM::~ADirectSoundSTREAM()
{
	this->CloseStreamThread ();

	_Free();

#ifdef	__USE_DS_NOTIFY
	if ( m_hNotificationEvents[ 0 ] != NULL ) {
		::CloseHandle( m_hNotificationEvents[ 0 ] );
		m_hNotificationEvents[ 0 ] = NULL;
	}

	if ( m_hNotificationEvents[ 1 ] != NULL ) {
		::CloseHandle( m_hNotificationEvents[ 1 ] );
		m_hNotificationEvents[ 1 ] = NULL;
	}
#else
	if ( m_hCheckTimeEvent != NULL ) {
		::CloseHandle( m_hCheckTimeEvent );
		m_hCheckTimeEvent = NULL;
	}
	SAFE_DELETE_ARRAY( m_pDataBlock );
#endif
}

//-------------------------------------------------------------------------------------------------
void ADirectSoundSTREAM::_Init (void)
{
	m_pDSBuffer			= NULL;

    m_dwTotalBuffSize   = 0;
    m_dwDataBlockSize   = 0;
	m_wCurrentBlockIDX	= 0;

    m_dwNextWriteOffset = 0;
    m_dwProgress        = 0;
    m_dwLastPos         = 0;
    m_bFoundEnd         = false;

	m_bTerminated		= true;
}


//-------------------------------------------------------------------------------------------------
void ADirectSoundSTREAM::_Free (void)
{
	SAFE_RELEASE( m_pDSBuffer );
}


//-------------------------------------------------------------------------------------------------
void ADirectSoundSTREAM::SetVolume (long lVolume)
{
	HRESULT hR;

	if ( m_pDSBuffer ) {
		hR = m_pDSBuffer->SetVolume ( lVolume );
		switch ( hR ) {
			case DSERR_CONTROLUNAVAIL	:
				LogString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_CONTROLUNAVAIL\n");
				break;
			case DSERR_GENERIC			:
				LogString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_GENERIC\n");
				break;
			case DSERR_INVALIDPARAM		:
				LogString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_INVALIDPARAM\n");
				break;
			case DSERR_PRIOLEVELNEEDED  :
				LogString (LOG_NORMAL, "DSStream::SetVolume return  DSERR_PRIOLEVELNEEDED\n");
				break;
			default :
				LogString (LOG_NORMAL, "DSStream::SetVolume return  %d\n", hR);
		}
	} else
		LogString (LOG_NORMAL, "DSStream::SetVolume Buffer == NULL \n");
}


//-------------------------------------------------------------------------------------------------
HRESULT ADirectSoundSTREAM::CreateStreamBuffer (LPDIRECTSOUND pDirectSound, WAVEFORMATEX *pWFX, int iBufferingSec, short nNumPlayNotifications)
{
	if ( !pDirectSound ) 
		return E_FAIL;

	m_wDataBlockCount = nNumPlayNotifications;

	m_wBitsPerSample	= pWFX->wBitsPerSample;

    m_dwTotalBuffSize   = 0;
    m_dwDataBlockSize	= 0;
    m_dwNextWriteOffset = 0;
    m_dwProgress        = 0;
    m_dwLastPos         = 0;
    m_bFoundEnd         = false;

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

    // The m_dwDataBlockSize should be an integer multiple of nBlockAlign
    m_dwDataBlockSize	= iSamplesPerSec * iBufferingSec * dwBlockAlign / m_wDataBlockCount;
    m_dwDataBlockSize -= m_dwDataBlockSize % dwBlockAlign;   

    // The buffersize should approximately 3 seconds of wav data
    m_dwTotalBuffSize  = m_dwDataBlockSize * m_wDataBlockCount;
    m_bFoundEnd     = false;
    m_dwProgress    = 0;
    m_dwLastPos     = 0;

    DSBUFFERDESC        dsbd;
    ZeroMemory (&dsbd, sizeof (dsbd));

    dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = m_dwTotalBuffSize;
    dsbd.lpwfxFormat   = pWFX;	

    hResult = pDirectSound->CreateSoundBuffer (&dsbd, &m_pDSBuffer, NULL);
    if ( hResult != DS_OK ) 
		return hResult;
	m_pDSBuffer->SetVolume( DSBVOLUME_MIN );	// 초기 묵음으로..

	m_dwPlayTimePerBlock = 1000 * iBufferingSec / (m_wDataBlockCount*1);
	m_pDataBlock = new ADirectSoundSTREAM::tagDataBlock[ m_wDataBlockCount ];
	for (WORD wI=0; wI<m_wDataBlockCount; wI++) {
		m_pDataBlock[ wI ].m_dwFROM = wI * m_dwDataBlockSize;
		m_pDataBlock[ wI ].m_dwTO   = m_pDataBlock[ wI ].m_dwFROM + m_dwDataBlockSize - 1;
	}

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: Play the DirectSound buffer
//-------------------------------------------------------------------------------------------------
HRESULT ADirectSoundSTREAM::PlayStreamBuffer ( bool bLooped, long lVolume )
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

	this->SetVolume( lVolume );
	::ResumeThread( this->m_hNotifyThread );

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
void ADirectSoundSTREAM::StopStreamBuffer (void) 
{
    if ( NULL == m_pDSBuffer )
		return;

    HRESULT hr;
    void*   pbBuffer = NULL;
    DWORD   dwBufferLength;

    m_pDSBuffer->Stop();
    m_pDSBuffer->SetCurrentPosition( 0 );

    // Lock the buffer down, 
    if( FAILED( hr = m_pDSBuffer->Lock( 0, m_dwTotalBuffSize, &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return;

	FillMemory( pbBuffer, m_dwTotalBuffSize, (BYTE)( m_wBitsPerSample == 8 ? 128 : 0 ) );

    // Now unlock the buffer
    m_pDSBuffer->Unlock( pbBuffer, m_dwTotalBuffSize, NULL, 0 );
}


//-------------------------------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-------------------------------------------------------------------------------------------------
HRESULT ADirectSoundSTREAM::RestoreBuffers (void)
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
HRESULT ADirectSoundSTREAM::FillBuffer (void)
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
	this->Reset();

    m_pDSBuffer->SetCurrentPosition( 0 );

    // Lock the buffer down, 
    if( FAILED( hr = m_pDSBuffer->Lock( 0, m_dwTotalBuffSize, &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return hr;

    // Fill the buffer with wav data 
    if( FAILED( hr = WriteToBuffer (pbBuffer, dwBufferLength ) ) )
        return hr;

    // Now unlock the buffer
    m_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );

    m_dwNextWriteOffset  = dwBufferLength; 
    m_dwNextWriteOffset %= m_dwTotalBuffSize; // Circular buffer

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name: WriteToBuffer()
// Desc: Writes wave data to the streaming DirectSound buffer 
//-------------------------------------------------------------------------------------------------
HRESULT ADirectSoundSTREAM::WriteToBuffer (void* pbBuffer, DWORD dwBufferLength)
{
    HRESULT hr;
    UINT	nActualBytesWritten;

    if ( !m_bFoundEnd ) {
        // Fill the DirectSound buffer with WAV data
		if( FAILED( hr = this->ReadData( dwBufferLength, (BYTE*) pbBuffer, &nActualBytesWritten ) ) )
			return hr;
    } else {
        // Fill the DirectSound buffer with silence
        FillMemory( pbBuffer, dwBufferLength, (BYTE)( m_wBitsPerSample == 8 ? 128 : 0 ) );
		return S_OK;
    }

	ODS( "Read:: BuffLen:%d, Buff:%d, Actual: %d \n", dwBufferLength, pbBuffer, nActualBytesWritten );

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

			while( nWritten < dwBufferLength )
			{  
				// This will keep reading in until the buffer is full. For very short files.
				this->Reset();
				
				if( FAILED( hr = this->ReadData( (UINT)dwBufferLength - nWritten,
															(BYTE*)pbBuffer + nWritten, &nActualBytesWritten ) ) )
					return hr;

				// m_dwReadData = m_pClassMP3->m_dwReadData;
				nWritten += nActualBytesWritten;
			} 
        } 
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------------------
bool ADirectSoundSTREAM::IsCurrentBlockPassed ()
{
    DWORD dwPlayPos, dwWritePos;
    if ( FAILED( m_pDSBuffer->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return false;

	return this->m_wCurrentBlockIDX != this->GetBlockIndex( dwPlayPos );
}

WORD ADirectSoundSTREAM::GetBlockIndex( DWORD dwBuffPos )
{
	for (WORD wI=0; wI<m_wDataBlockCount; wI++)
		if ( dwBuffPos >= m_pDataBlock[wI].m_dwFROM &&
			 dwBuffPos <= m_pDataBlock[wI].m_dwTO )
			return wI;

	return 0xffff;
}

//-------------------------------------------------------------------------------------------------
// Name: UpdateProgress()
// Desc: Update the progress variable to know when the entire buffer was played
//-------------------------------------------------------------------------------------------------
HRESULT ADirectSoundSTREAM::UpdateProgress(void)
{
    HRESULT hr;
    DWORD   dwPlayPos, dwWritePos, dwPlayed;

    if ( FAILED( hr = m_pDSBuffer->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return hr;

	ODS( "		CurBLOCK: %d / PlayingBLOCK: %d / %d\n", m_wCurrentBlockIDX, this->GetBlockIndex(dwPlayPos), this->GetBlockIndex(dwWritePos) );
	++m_wCurrentBlockIDX %= m_wDataBlockCount;

    if( dwPlayPos < m_dwLastPos )
        dwPlayed = m_dwTotalBuffSize - m_dwLastPos + dwPlayPos;
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
HRESULT ADirectSoundSTREAM::HandleNotification (void) 
{
    HRESULT			hr;
    VOID* pbBuffer  = NULL;
    DWORD dwBufferLength;

    UpdateProgress();

    // Lock the buffer down
    if( FAILED( hr = m_pDSBuffer->Lock( m_dwNextWriteOffset, m_dwDataBlockSize, 
                                        &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return hr;

	ODS( "Next:%d, Size:%d, BUF: %d \n", m_dwNextWriteOffset, m_dwDataBlockSize, pbBuffer );

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
        if ( this->GetReadData() > m_dwDataBlockSize ) {
            if ( m_dwProgress >= this->GetReadData() - m_dwDataBlockSize )
                m_pDSBuffer->Stop ();
        } else {	// For short files.
            if ( m_dwProgress >= this->GetReadData() ) 
                m_pDSBuffer->Stop ();
        }
    }

    m_dwNextWriteOffset += dwBufferLength; 
    m_dwNextWriteOffset %= m_dwTotalBuffSize; // Circular buffer

    return S_OK;
}



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
AOggSTREAM::AOggSTREAM ()
{
	m_fp = NULL;
	m_pOVF = NULL;

	m_dwReadData = 0;
}
AOggSTREAM::~AOggSTREAM ()
{
	// 주의 !! 쓰레드가 종료된 후에 ogg가 종료되야 함...
	this->CloseStreamThread ();
	while( !this->m_bTerminated )
		::Sleep( 100 );
	this->CloseFile ();
}

void AOggSTREAM::CloseFile ()
{
	if ( m_pOVF ) {
		::ov_clear( m_pOVF );
		SAFE_DELETE( m_pOVF );
	}

	if ( m_fp ) {
		fclose( m_fp );
		m_fp = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
bool AOggSTREAM::OpenFile (char *szOGGFile, int iBufferingSec, int iBufferBlock)
{
	this->CloseFile ();

	m_fp = fopen( szOGGFile, "rb" );
	if ( !m_fp )
		return false;

	m_pOVF = new OggVorbis_File;

	// int ov_open(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
	if ( ::ov_open( m_fp, m_pOVF, NULL, 0 ) ) {
		this->CloseFile ();
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void AOggSTREAM ::Reset ()
{
	m_dwReadData = 0;
	::ov_pcm_seek (this->m_pOVF, 0 );
}


//-------------------------------------------------------------------------------------------------
bool AOggSTREAM ::ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)
{
	/*
	long ov_read(OggVorbis_File *vf, char *buffer, int length, int bigendianp, int word, int sgned, int *bitstream);
	Parameters ::
		vf			A pointer to the OggVorbis_File structure--this is used for ALL the externally visible libvorbisfile functions. 
		buffer		A pointer to an output buffer. The decoded output is inserted into this buffer. 
		length		Number of bytes to be read into the buffer. Should be the same size as the buffer. A typical value is 4096. 
		bigendianp	Specifies big or little endian byte packing. 0 for little endian, 1 for b ig endian. Typical value is 0. 
		word		Specifies word size. Possible arguments are 1 for 8-bit samples, or 2 or 16-bit samples. Typical value is 2. 
		sgned		Signed or unsigned data. 0 for unsigned, 1 for signed. Typically 1. 
		bitstream	A pointer to the number of the current logical bitstream. 
	Return Values ::
		OV_HOLE		indicates there was an interruption in the data. 
					(one of: garbage between pages, loss of sync followed by recapture, or a corrupt page) 
		OV_EBADLINK indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt. 
		0			indicates EOF 
		n			indicates actual number of bytes read. ov_read() will decode at most one vorbis packet per invocation, so the value returned will generally be less than length. 
	*/
	long lReadBytes;
	int	iBitStream;
	
	*puiActualBytesWritten = 0;

	do {
		lReadBytes = ::ov_read( m_pOVF, (char*)pBuffer + *puiActualBytesWritten, uiSizeToRead, 0, 2, 1, &iBitStream);
		if ( lReadBytes == 0 ) {
			// EOF
			LogString( LOG_NORMAL, "EOF OGGG..." );
			break;
		} else
		if ( lReadBytes < 0 ) {
			// Error in Stream ...
			return false;
		}

		uiSizeToRead -= lReadBytes;
		*puiActualBytesWritten += lReadBytes;
	} while( uiSizeToRead > 0 );
	
	return true;
}


//-------------------------------------------------------------------------------------------------
bool AOggSTREAM ::OpenSoundStreamFile (char *szOggFile, LPDIRECTSOUND pDirectSound, int iBufferingSec, short nNumPlayNotifications)
{
	if ( !this->OpenFile( szOggFile, iBufferingSec, nNumPlayNotifications ) )
		return false;
	
	vorbis_info	*pInfo = ::ov_info (m_pOVF, -1);
	if ( NULL == pInfo || 0 == pInfo->channels )
		return false;

	WAVEFORMATEX sWFX;
    sWFX.cbSize			= sizeof( WAVEFORMATEX );			// If wFormatTag is WAVE_FORMAT_PCM, this value is ignored.
	sWFX.wFormatTag		= WAVE_FORMAT_PCM;
    sWFX.nChannels		= pInfo->channels;       
    sWFX.nSamplesPerSec	= pInfo->rate;  
    sWFX.wBitsPerSample = 16;  
    sWFX.nBlockAlign	= (WORD)( sWFX.wBitsPerSample / 8 * sWFX.nChannels );
    sWFX.nAvgBytesPerSec= sWFX.nSamplesPerSec * sWFX.nBlockAlign; 

	if ( S_OK != this->CreateStreamBuffer (  pDirectSound, &sWFX, iBufferingSec, nNumPlayNotifications ) )
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
