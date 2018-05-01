#ifndef	__DLL_DSSTREAM_H
#define	__DLL_DSSTREAM_H
#include <MMSystem.h>
#include <DSound.h>
#include "vorbis/vorbisfile.h"
//-------------------------------------------------------------------------------------------------

class ADirectSoundSTREAM {
protected :
	HANDLE					m_hNotifyThread;
	DWORD					m_dwNotifyThreadID;

	WORD					m_wBitsPerSample;
	short					m_nNumPlayNotifications;		// 전체 버퍼를 몇번에 나눠서 읽어 들일것인가?

    LPDIRECTSOUNDBUFFER		m_pDSBuffer;
	LPDIRECTSOUNDNOTIFY		m_pDSNotify;

	DSBPOSITIONNOTIFY	   *m_pDSBPosNotify;

	DWORD					m_dwBufferSize;
	DWORD					m_dwNotifySize;
	DWORD					m_dwNextWriteOffset;
	DWORD					m_dwProgress;
	DWORD					m_dwLastPos;
	bool					m_bFoundEnd;
	bool					m_bLooped;

	void	CloseStreamThread ( void );
	HRESULT CreateStreamBuffer (LPDIRECTSOUND pDirectSound, WAVEFORMATEX *pWFX, int iBufferingSec=8, short nNumPlayNotifications=32);
	HRESULT RestoreBuffers	( void );
	HRESULT FillBuffer		( void );
	HRESULT WriteToBuffer	( void* pbBuffer, DWORD dwBufferLength );
	HRESULT UpdateProgress	( void );

	virtual DWORD GetReadData()=0;
	virtual void  Reset ()=0;
	virtual bool  ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)=0;

public :
	HANDLE					m_hNotificationEvents[ 2 ];

	ADirectSoundSTREAM ();
	~ADirectSoundSTREAM ();

	void	_Init (void);
	void	_Free (void);

	void	SetVolume (long lVolume);
	HRESULT PlayStreamBuffer	( bool bLooped );
	void	StopStreamBuffer	( void );
	HRESULT HandleNotification	( void );

	friend DWORD WINAPI ThreadDSStream ( LPVOID lpParameter );
} ;


//-------------------------------------------------------------------------------------------------
class AOggSTREAM : public ADirectSoundSTREAM
{
private :
	FILE			*m_fp;
	OggVorbis_File	*m_pOVF;

	DWORD m_dwReadData;

	bool  OpenFile (char *szOggFile, int iBufferingSec, int iBufferBlock);
	void  CloseFile ();

public :
	AOggSTREAM ();
	~AOggSTREAM ();

	DWORD GetReadData()		{	return m_dwReadData;	}
	void  Reset ();
	bool  ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten);

	bool OpenSoundStreamFile (char *szOggFile, LPDIRECTSOUND pDirectSound, int iBufferingSec, short nNumPlayNotifications, bool bAlreadyCoInitialized);
} ;


//-------------------------------------------------------------------------------------------------
#endif


