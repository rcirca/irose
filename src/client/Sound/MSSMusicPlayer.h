#ifndef _MSS_MUSIC_PLAYER_
#define _MSS_MUSIC_PLAYER_

#include "MusicPlayer.h"
#ifdef _USE_MSS
#include "MSS.h"
#endif

//---------------------------------------------------------------------------------------------
///
/// class CMssMusicPlayer
/// @brief miles sound system BGM player
///
//---------------------------------------------------------------------------------------------

class CMSSMusicPlayer : public CMusicPlayer
{
private:
#ifdef _USE_MSS
	HDIGDRIVER		m_hDig;
	HSTREAM			m_hStream;
	
	U32				m_Paused;	
#endif

public:
	CMSSMusicPlayer(void);
	~CMSSMusicPlayer(void);

	void	HandleEvent();

	bool	Init();
	bool	Play( const char* fName );
	void	Stop();

	void	Run();
	void    Pause();
	
	void	SetVolume( long lVolume );
};


#endif //_MSS_MUSIC_PLAYER_