#ifndef _MUSIC_PLAYER_
#define _MUSIC_PLAYER_


//---------------------------------------------------------------------------------------------
///
/// class CMusicPlayer
/// @brief abstraction class for music play in game
///        can use dsound, mms
///
//---------------------------------------------------------------------------------------------

class CMusicPlayer
{
public:
	CMusicPlayer(void);
	virtual ~CMusicPlayer(void);

	virtual void	HandleEvent() = 0;

	virtual bool	Init() = 0;
	virtual bool	Play( const char* fName ) = 0;
	virtual void	Stop() = 0;

	virtual void	Run() = 0;
	virtual void    Pause() = 0;
	
	virtual void	SetVolume( long lVolume ) = 0;
};

#endif //_MUSIC_PLAYER_