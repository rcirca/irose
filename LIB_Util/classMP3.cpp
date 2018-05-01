
#include <Windows.h>
#include <MMSystem.h>
#include "classMP3.h"


#pragma comment (lib, "amstrmid.lib")

//-------------------------------------------------------------------------------------------------

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------------------------------------------
classMP3::classMP3 ()
{
	m_bAlreadyCoInitialized = true;
}


//-------------------------------------------------------------------------------------------------
bool classMP3::_Init (bool bAlreadyCoInitialized)
{
	HRESULT hResult;

	if ( !bAlreadyCoInitialized ) {
		// Initialize COM
		hResult = ::CoInitialize (NULL);
		if ( FAILED ( hResult ) )
			return false;
	}
	m_bAlreadyCoInitialized = bAlreadyCoInitialized;

	hResult = ::CoCreateInstance( CLSID_AMMultiMediaStream, NULL,
		CLSCTX_INPROC_SERVER,IID_IAMMultiMediaStream, reinterpret_cast<void **>(&m_pAMStream) );
	if ( FAILED( hResult ) )
		return false;

	hResult = m_pAMStream->Initialize(STREAMTYPE_READ, AMMSF_NOGRAPHTHREAD,NULL );
    if ( FAILED( hResult ) )
		return false;

	hResult = m_pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, 0,NULL );
    if ( FAILED( hResult ) )
		return false;

    m_pMMStream = m_pAMStream;

	m_pMediaStream			= NULL;
	m_pAudioStreamSample	= NULL;
	m_pAudioMediaStream		= NULL;
	m_pAudioData			= NULL;
	m_hEvent				= NULL;
	m_dwReadData			= 0;

	OutputDebugString ("classMP3::_Init\n");

    return true;
}


//-------------------------------------------------------------------------------------------------
void classMP3::_Free (void)
{
	if ( m_hEvent != NULL )
		CloseHandle ( m_hEvent );

	SAFE_RELEASE( m_pAudioStreamSample );
	SAFE_RELEASE( m_pAudioData );
	SAFE_RELEASE( m_pAudioMediaStream );
	SAFE_RELEASE( m_pMediaStream );

	SAFE_RELEASE( m_pMMStream );

	if ( !m_bAlreadyCoInitialized ) {
		// Clean up COM
		CoUninitialize();
	}
}


//-------------------------------------------------------------------------------------------------
bool classMP3::OpenFile (char *szMP3File, int iBufferingSec, int iBufferBlockCnt)
{
	HRESULT hResult;
	WCHAR  *pWszName;
	int     iLength, iRet;
	DWORD	dwBlockAlign;
	int		iSamplesPerSec;
	
	iLength = strlen( szMP3File ) + 1;

	pWszName = new WCHAR[ iLength ];
	MultiByteToWideChar( CP_ACP, 0, szMP3File, -1, pWszName, iLength );
	hResult = m_pAMStream->OpenFile(pWszName, AMMSF_RUN );
	delete[] pWszName;

    if ( FAILED( hResult ) ) return false;

	// Get the media stream
	hResult = m_pMMStream->GetMediaStream( MSPID_PrimaryAudio, &m_pMediaStream );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;

	// Get an IAudioStream interface
	hResult = m_pMediaStream->QueryInterface( IID_IAudioMediaStream, (void **)&m_pAudioMediaStream );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;

	// Get the format
	hResult = m_pAudioMediaStream->GetFormat( &m_sWFX );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;

	// Create an IAudioData interface
    iRet = ::CoCreateInstance( CLSID_AMAudioData, NULL,CLSCTX_INPROC_SERVER,IID_IAudioData, (void **) &m_pAudioData );
    if ( iRet != S_OK ) goto _RETURN_FALSE;


	// Set the format
	hResult = m_pAudioData->SetFormat( &m_sWFX );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;


	dwBlockAlign   = (DWORD)m_sWFX.nBlockAlign;
	iSamplesPerSec = m_sWFX.nSamplesPerSec;

	// The g_dwNotifySize should be an integer multiple of nBlockAlign
	iLength  = iSamplesPerSec * iBufferingSec * dwBlockAlign / iBufferBlockCnt;
	iLength -= iLength % dwBlockAlign;   

	// Aloocate memory for the streamed data
    m_pBuffer = (PBYTE) LocalAlloc( LMEM_FIXED, iLength );
	if ( m_pBuffer == NULL )  goto _RETURN_FALSE;

	// Set the buffer
	hResult = m_pAudioData->SetBuffer( iLength, m_pBuffer, 0 );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;


	// Create a sample
	hResult = m_pAudioMediaStream->CreateSample( m_pAudioData, 0, &m_pAudioStreamSample );
    if ( FAILED( hResult ) ) goto _RETURN_FALSE;

	// Create the event
	m_hEvent = CreateEvent( FALSE, NULL, NULL, FALSE );
	if ( m_hEvent == NULL )	goto _RETURN_FALSE;

	m_dwReadData = 0;

	return true;

_RETURN_FALSE :

	// Clean up
	if ( m_pBuffer ) LocalFree( (HLOCAL) m_pBuffer );

	SAFE_RELEASE( m_pAudioStreamSample );
	SAFE_RELEASE( m_pAudioData );
	SAFE_RELEASE( m_pAudioMediaStream );
	SAFE_RELEASE( m_pMediaStream );

	return false;
}


//-------------------------------------------------------------------------------------------------
HRESULT classMP3::ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)
{
	HRESULT		hResult;
	DWORD		dwLength;

	*puiActualBytesWritten = 0;

	while ( *puiActualBytesWritten < uiSizeToRead ) {
		// get a sample
		hResult = m_pAudioStreamSample->Update( 0, m_hEvent, NULL, 0 );
		if ( hResult == FAILED(hResult) || hResult == MS_S_ENDOFSTREAM )
			return hResult;

		// wait for Update to complete
		WaitForSingleObject( m_hEvent, INFINITE );

		// get sample information
		hResult = m_pAudioData->GetInfo( NULL, NULL, &dwLength );
		if ( FAILED( hResult ) )
			return hResult;

		// store rgew sample
		CopyMemory( ((BYTE*)pBuffer + *puiActualBytesWritten), m_pBuffer, dwLength);

		m_dwReadData += dwLength;
		*puiActualBytesWritten += dwLength;
	}

	return S_OK;
}


//-------------------------------------------------------------------------------------------------
