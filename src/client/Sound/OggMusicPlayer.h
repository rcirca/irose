#ifndef _OGG_MUSIC_PLAYER_
#define _OGG_MUSIC_PLAYER_

#include "MusicPlayer.h"
#include "../Util/ADSStream.h"

//---------------------------------------------------------------------------------------------
///
/// class COggMusicPlayer
/// @brief ogg file player
///
//---------------------------------------------------------------------------------------------

class COggMusicPlayer : public CMusicPlayer
{
private:
	AOggSTREAM*		m_pOggStream;

public:
	COggMusicPlayer(void);
	~COggMusicPlayer(void);

	void	HandleEvent();

	bool	Init();
	bool	Play( const char* fName );
	void	Stop();

	void	Run();
	void    Pause();

	void	SetVolume( long lVolume );
};

#endif //_OGG_MUSIC_PLAYER_
