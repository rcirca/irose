#ifndef	__CLASSMP3_H
#define __CLASSMP3_H
#include <mmstream.h>		// multimedia stream interfaces
#include <amstream.h>		
#include <ddstream.h>		
//-------------------------------------------------------------------------------------------------

#ifdef	DLL_ICARUS_EXPORTS
	#define DLL_MP3DECODE	__declspec(dllexport)
#else
	#define DLL_MP3DECODE	//__declspec(dllimport)
#endif

//-------------------------------------------------------------------------------------------------

class DLL_MP3DECODE classMP3 {
private :
	IMultiMediaStream		*m_pMMStream;
	IAMMultiMediaStream		*m_pAMStream;


	IMediaStream			*m_pMediaStream;
	IAudioStreamSample		*m_pAudioStreamSample;
	IAudioMediaStream		*m_pAudioMediaStream;
	IAudioData				*m_pAudioData;

	bool					 m_bAlreadyCoInitialized;
public :
	WAVEFORMATEX			 m_sWFX;
	PBYTE					 m_pBuffer;
	HANDLE					 m_hEvent;
	DWORD					 m_dwReadData;

	classMP3();
	~classMP3 ()	{	_Free();	}

	bool	_Init (bool bAlreadyCoInitialized);
	void	_Free (void);

	void	Reset (void)	{	m_dwReadData = 0;	m_pMMStream->Seek( 0 );	}

	bool	OpenFile (char *szMP3File, int iBufferingSec, int iBufferBlock);
	HRESULT	ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten);
} ;


//-------------------------------------------------------------------------------------------------
#endif
