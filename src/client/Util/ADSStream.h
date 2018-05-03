#ifndef	__DLL_DSSTREAM_H
#define	__DLL_DSSTREAM_H
#include <MMSystem.h>
#include <DSound.h>
#include "vorbis/vorbisfile.h"
#include "CDSound.h"
//-------------------------------------------------------------------------------------------------

class ADirectSoundSTREAM {
protected :
	HANDLE					m_hNotifyThread;
	unsigned				m_dwNotifyThreadID;
	bool					m_bTerminated;

	WORD					m_wBitsPerSample;
    LPDIRECTSOUNDBUFFER		m_pDSBuffer;

	DWORD					m_dwTotalBuffSize;
	DWORD					m_dwDataBlockSize;
	WORD					m_wDataBlockCount;
	WORD					m_wCurrentBlockIDX;

	DWORD					m_dwNextWriteOffset;
	DWORD					m_dwProgress;
	DWORD					m_dwLastPos;
	bool					m_bFoundEnd;
	bool					m_bLooped;

	void	CloseStreamThread ( void );
	HRESULT CreateStreamBuffer (LPDIRECTSOUND pDirectSound, WAVEFORMATEX *pWFX, int iBufferingSec, short nNumPlayNotifications);
	HRESULT RestoreBuffers	( void );
	HRESULT FillBuffer		( void );
	HRESULT WriteToBuffer	( void* pbBuffer, DWORD dwBufferLength );
	HRESULT UpdateProgress	( void );
	WORD    GetBlockIndex( DWORD dwBuffPos );
	bool    IsCurrentBlockPassed();

	virtual DWORD GetReadData()=0;
	virtual void  Reset ()=0;
	virtual bool  ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)=0;

public :
	HANDLE					m_hCheckTimeEvent;
	DWORD					m_dwPlayTimePerBlock;
	struct tagDataBlock {
		DWORD	m_dwFROM;
		DWORD	m_dwTO;
	} *m_pDataBlock;

	ADirectSoundSTREAM ();
	~ADirectSoundSTREAM ();

	void	_Init (void);
	void	_Free (void);

	void	SetVolume (long lVolume);
	HRESULT PlayStreamBuffer	( bool bLooped, long lVolume=DSBVOLUME_MIN );
	void	StopStreamBuffer	( void );
	HRESULT HandleNotification	( void );

	friend unsigned int __stdcall ThreadDSStream( void *lpParameter );
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

	bool OpenSoundStreamFile (char *szOggFile, LPDIRECTSOUND pDirectSound, int iBufferingSec=30, short nNumPlayNotifications=5);
} ;


//-------------------------------------------------------------------------------------------------
#endif


