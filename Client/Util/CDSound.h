#ifndef	__DLL_DSOUND_H
#define	__DLL_DSOUND_H
#include <MMSystem.h>
#include <DSound.h>
//-------------------------------------------------------------------------------------------------

typedef struct tagSOUNDDATA		t_sounddata;
typedef struct tagWAVEDATA		t_wavedata;



#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
struct tagSOUNDDATA	{
    LPDIRECTSOUNDBUFFER		m_pDSB;
    t_sounddata			   *m_pNext;
    short					m_nMaxMixCnt;
	LPDIRECTSOUND3DBUFFER   m_pDS3DBuffer;

	tagSOUNDDATA ()
	{
		m_pDSB = NULL;
		m_pNext = NULL;
		m_pDS3DBuffer = NULL;
	}

	~tagSOUNDDATA ()
	{
		if ( m_pDSB ) {
			m_pDSB->Stop ();
		}
		SAFE_RELEASE( m_pDS3DBuffer );
		SAFE_RELEASE( m_pDSB );
	}
} ;



//-------------------------------------------------------------------------------------------------
class CWaveFile;
class CDSOUND
{
protected :
	virtual t_sounddata *CreateSoundData (CWaveFile *pCWave, DWORD dwCreationFlags, GUID guid3DAlgorithm);
	virtual t_sounddata *CreateSoundData (t_wavedata *pWave, DWORD dwCreationFlags, GUID guid3DAlgorithm);
	t_sounddata *DuplicateSoundData (LPDIRECTSOUNDBUFFER lpDSBSour);
	bool SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );

public  :
    LPDIRECTSOUND8		m_pDS;
	HRESULT				m_hR;

	CDSOUND();
	virtual ~CDSOUND();

	virtual bool _Init( HWND  hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
	void _Free ();

	t_sounddata	*LoadSoundData (LPSTR szFileName, short nMaxMixCnt, DWORD dwCreationFlags=DSBCAPS_STATIC|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN, GUID guid3DAlgorithm=DS3DALG_DEFAULT, long lFilePtr=-1);
	void		 FreeSoundData (t_sounddata *pData);

	//
	//	lVolume ::	DSBVOLUME_MAX ( 0 ) to DSBVOLUME_MIN ( -10,000 - silence).
	//	lPan    ::  DSBPAN_LEFT( -10,000 ),  DSBPAN_CENTER( 0 ), DSBPAN_RIGHT( 10,000 )
	//	dwFlags ::  IDirectSoundBuffer8::Play 인자의 dwFlags에 전달된다.
	//
	void  PlaySound (t_sounddata *pData, long lVolume, long lPan, DWORD dwFlags);
	void  StopSound (t_sounddata *pData);
	DWORD GetStatus (t_sounddata *pData);		// Return IDirectSoundBuffer8::GetStatus( &DWORD );

	short GetMaxMixCount (t_sounddata *pData);
	void  SetMaxMixCount (t_sounddata *pData, short nMaxMinCnt);
} ;


//-------------------------------------------------------------------------------------------------
class CCamera;
class CD3DSOUND : public CDSOUND 
{
private :
	t_sounddata *CreateSoundData (t_wavedata *pWave, DWORD dwCreationFlags, GUID guid3DAlgorithm);
	HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );
	DS3DBUFFER				m_dsBufferParams;

	void Set3DBufferParam (t_sounddata *pSound, DS3DBUFFER &dsBufferParam);
	void SetListenerParam (void);
	static void ConvertToSoundSpace ( D3DXVECTOR3& posSoundSpace, const D3DXVECTOR3& posWorldSpace );

public  :
	static LPDIRECTSOUND3DLISTENER	m_pDSListener;
	static DS3DLISTENER            m_dsListenerParams;
	GUID					m_guid3DAlgorithm;

	CD3DSOUND ();
	~CD3DSOUND ();

	bool _Init( HWND  hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );

	t_sounddata	*LoadSoundData (LPSTR szFileName, short nMaxMixCnt, GUID guid3DAlgorithm=DS3DALG_NO_VIRTUALIZATION, long lFilePtr=-1);

	// 리스너 관련
	static bool SetListenerPosition ( const D3DXVECTOR3& PosWorld );
	static bool SetListenerOrientation ( const D3DXVECTOR3& PosFront, const D3DXVECTOR3& PosTop );
	static bool SetListenerRolloffFactor ( float fMinRolloffFactor );
	static bool UpdateListener ( const CCamera * pCamera );

	// 3D 사운드 속성 관련
	bool Set3D ( t_sounddata * pData, bool b3DMode ); // 3D 모드인지 아닌지 여부 설정, 디폴트는 3D
	bool SetPosition ( t_sounddata * pData, const D3DXVECTOR3& posWorld );
	bool SetVelocity ( t_sounddata * pData, const D3DXVECTOR3& posVelocity );
	bool SetMinDistance ( t_sounddata * pData, float fMinDistance );
	bool SetMaxDistance ( t_sounddata * pData, float fMaxDistance );
} ;

//-------------------------------------------------------------------------------------------------

#endif