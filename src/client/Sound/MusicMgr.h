#ifndef _MUSIC_MENAGER_
#define _MUSIC_MENAGER_

#include "../Util/JSingleton.h"

class CMusicPlayer;


enum PLAYER_TYPE
{
	DIRECT_MUSIC_TYPE = 0,
	OGG_MUSIC_TYPE,
	MSS_MUSIC_TYPE,
};

//---------------------------------------------------------------------------------------------
///
/// class CMusicMgr
/// @brief Music play manager..
///
//---------------------------------------------------------------------------------------------

class CMusicMgr : public CJSingleton< CMusicMgr >
{
private:
	/// 현재 뮤직 플레이어.
	CMusicPlayer*		m_pPlayer;
	bool				m_bReadyDevice;

public:
	CMusicMgr(void);
	~CMusicMgr(void);	

	bool	Init( PLAYER_TYPE playerType );
	bool	bIsReady(){ return m_bReadyDevice; }
	void	Clear();

	bool	Play( const char* fName );
	void	Stop();

	void	Run();
	void    Pause();
	
	void	SetVolume( long lVolume );

	void	HandleEvent();
};

#endif //_MUSIC_MENAGER_