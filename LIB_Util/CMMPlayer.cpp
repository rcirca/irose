/*
	$Header: /7HeartsOnline/LIB_Util/CMMPlayer.cpp 1     04-03-25 11:20a Icarus $
*/
#include "malloc.h"
#include "CMMPlayer.h"

#pragma comment( lib , "strmiids.lib")

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------------------------------------------
classMMPLAYER::classMMPLAYER()
{
	m_pGraphBuilder  = NULL;
	m_pMediaEvent    = NULL;
	m_pMediaControl  = NULL;
	m_pMediaSeeking  = NULL;
	m_pSourceCurrent = NULL;
	m_pSourceNext    = NULL;
}

classMMPLAYER::~classMMPLAYER()
{
	this->Free();
}

//-------------------------------------------------------------------------------------------------
HRESULT classMMPLAYER::Run ()
{
	if ( m_pMediaControl )
		return m_pMediaControl->Run ();

	return S_OK;
}

HRESULT classMMPLAYER::Pause ()
{	
	if ( m_pMediaControl )
		return m_pMediaControl->Pause();

	return S_OK;
}
HRESULT	classMMPLAYER::Stop ()
{	
	if ( m_pMediaControl )
		return m_pMediaControl->Stop();

	return S_OK;
}

//-------------------------------------------------------------------------------------------------
void classMMPLAYER::Free (void)
{
    if ( m_pMediaControl )
        m_pMediaControl->Stop();

    SAFE_RELEASE( m_pSourceNext );
    SAFE_RELEASE( m_pSourceCurrent );
    SAFE_RELEASE( m_pMediaSeeking );
    SAFE_RELEASE( m_pMediaControl );
	SAFE_RELEASE( m_pMediaEvent );
    SAFE_RELEASE( m_pGraphBuilder );
}

//-------------------------------------------------------------------------------------------------
HRESULT classMMPLAYER::Init( HWND argHwnd, UINT uiEventMsg )
{
    HRESULT hr;
    // Initialize COM
    if (FAILED (hr = CoInitialize(NULL)) )
        return hr;

    // Create DirectShow Graph
    if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,
                                      CLSCTX_INPROC, IID_IGraphBuilder,
                                      reinterpret_cast<void **>(&m_pGraphBuilder))) )
        return hr;

    // Get the IMediaControl Interface
    if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaControl,
                                 reinterpret_cast<void **>(&m_pMediaControl))))
        return hr;

    // Get the IMediaControl Interface
    if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
                                 reinterpret_cast<void **>(&m_pMediaSeeking))))
        return hr;

	if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaEvent, reinterpret_cast<void **>(&m_pMediaEvent))))
		return hr;
	if( FAILED(m_pMediaEvent->SetNotifyWindow((OAHWND)argHwnd, uiEventMsg, 0)))
		return hr;

	return hr;
}

//-------------------------------------------------------------------------------------------------
HRESULT classMMPLAYER::HandleEvent()
{
	LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    // Make sure that we don't access the media event interface
    // after it has already been released.
    if ( !m_pMediaEvent )
	    return S_OK;

    // Process all queued events
    while ( SUCCEEDED(m_pMediaEvent->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)) ) {
        // Free memory associated with callback, since we're not using it
        hr = m_pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);

        // If this is the end of the clip, reset to beginning
        if ( EC_COMPLETE == evCode ) {
            LONGLONG pos=0;

            // Reset to first frame of movie
            hr = m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
                                   NULL, AM_SEEKING_NoPositioning);
            if ( FAILED(hr) ) {
                // Some custom filters (like the Windows CE MIDI filter)
                // may not implement seeking interfaces (IMediaSeeking)
                // to allow seeking to the start.  In that case, just stop
                // and restart for the same effect.  This should not be
                // necessary in most cases.
                if ( FAILED(hr = m_pMediaControl->Stop ()) )
                {
                  //  Msg(TEXT("Failed(0x%08lx) to stop media clip!\r\n"), hr);
                    break;
                }

                if ( FAILED(hr = m_pMediaControl->Run ()) )
                {
                   // Msg(TEXT("Failed(0x%08lx) to reset media clip!\r\n"), hr);
                    break;
                }
            }

			this->ON_Completed ();
        } else
		if ( EC_USERABORT == evCode ) {
			if ( FAILED(hr = m_pMediaControl->Stop()) ) {
				// Msg(TEXT("Failed(0x%08lx) to stop media clip!\r\n"), hr);
                break;
            }

			this->ON_Abort ();
		}
    }

    return hr;
}


//-------------------------------------------------------------------------------------------------
HRESULT classMMPLAYER::Play (LPSTR argName)
{
	WCHAR wFileName[MAX_PATH];
	HRESULT hr;
	IPin	*pPin = NULL;

	DWORD dwAttr = GetFileAttributes(argName);
	if (dwAttr == (DWORD) -1)
		return ERROR_FILE_NOT_FOUND;

	#ifndef UNICODE
		MultiByteToWideChar(CP_ACP, 0, argName, -1, wFileName, MAX_PATH);
	#else
		lstrcpy(wFileName, argName);
	#endif

	hr = m_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &m_pSourceNext);
	if ( SUCCEEDED(hr) ) 
	{
		hr = m_pSourceNext->FindPin(L"Output", &pPin);
	}

	if ( SUCCEEDED(hr) )
	{
		hr = m_pMediaControl->Stop();
	}
	
	if (SUCCEEDED(hr)) {
		IEnumFilters *pFilterEnum = NULL;
		IBaseFilter  *pFilterTemp = NULL;
		if(SUCCEEDED(hr = m_pGraphBuilder->EnumFilters(&pFilterEnum))){
			int iFiltCount = 0;
			int iPos = 0;
			while( S_OK == pFilterEnum->Skip(1)){
				iFiltCount++;
			}
			IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>(_alloca(sizeof(IBaseFilter *) *iFiltCount));
			pFilterEnum->Reset();
			while(S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));
			SAFE_RELEASE(pFilterEnum);
			for(iPos = 0;iPos < iFiltCount; iPos++){
				m_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
				if(ppFilters[iPos] != m_pSourceCurrent)
				{
					m_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
				}
				SAFE_RELEASE(ppFilters[iPos]);
			}
		}
	}
	
	if ( SUCCEEDED(hr) ) {
		hr = m_pGraphBuilder->Render(pPin);
		m_pSourceCurrent = m_pSourceNext;
		m_pSourceNext    = NULL;
	}
	SAFE_RELEASE(pPin);

	if ( SUCCEEDED(hr) ) {
		LONGLONG IIPos = 0;
		hr = m_pMediaSeeking->SetPositions(&IIPos, AM_SEEKING_AbsolutePositioning,
			&IIPos, AM_SEEKING_NoPositioning);
	}

	if ( SUCCEEDED(hr) ) {
		hr = m_pMediaControl->Run();
	}

	SAFE_RELEASE(m_pSourceCurrent);
	return S_OK;
}


//-------------------------------------------------------------------------------------------------
/*
#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L

//-------------------------------------------------------------------------------------------------
HRESULT classMMPLAYER::MuteAudio (void)
{
    HRESULT hr=S_OK;
    IBasicAudio *pBA=NULL;
    long lVolume;

    if ( !m_pGraphBuilder )
        return S_OK;

    hr = m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&pBA);
    if (FAILED(hr))
        return S_OK;

    // Read current volume
    hr = pBA->get_Volume(&lVolume);
    if (hr == E_NOTIMPL)
    {
        // Fail quietly if this is a video-only media file
        pBA->Release();
        return hr;
    }
    else if (FAILED(hr))
    {
        pBA->Release();
        return hr;
    }

    lVolume = VOLUME_SILENCE;

    // Set new volume
    hr = pBA->put_Volume(lVolume);
    if (FAILED(hr))
    {
        RetailOutput(TEXT("Failed in pBA->put_Volume!  hr=0x%x\r\n"), hr);
    }

    pBA->Release();
    return hr;
}
*/