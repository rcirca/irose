#ifndef _TUTORIAL_EVENT_MANAGER_
#define _TUTORIAL_EVENT_MANAGER_

#include "../Util/JSingleton.h"

#include "TutorialEvent.h"
#include "LevelUpEvent.h"
#include <deque>


//---------------------------------------------------------------------------------------
///
/// class CTutorialEventManager
///
/// Manager class for all tutorial event and levelup event..
///
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
/// Tutorial Image Queue Item
//---------------------------------------------------------------------------------------
struct	S_TutorialImage{
	HNODE		m_hNode;
	std::string	m_filename;
	DWORD		m_drawstart_time;	// milisecond
	float		m_fadein_endtime;
	float		m_fadeout_starttime;		// second
	float		m_max_time;			// second
	int			m_displayscreen_x;
	int			m_displayscreen_y;
	int			m_width;
	int			m_height;
};
//---------------------------------------------------------------------------------------
class CTutorialEventManager : public CJSingleton< CTutorialEventManager >
{
private:	
	
	CTutorialEvent			m_TutorialEvent;
	CLevelUpEvent			m_LevelUpEvent;
	std::deque< S_TutorialImage >	m_tutorial_images;
public:
	CTutorialEventManager(void);
	~CTutorialEventManager(void);

	bool			Init();
	void			Release();

	void			Proc();

	void			CheckLevelUpEvent( int iLevel );


	void			RegistImage( const char* filename, int x, int y, float fadein_endtime, float fadeout_starttime, float max_time, int append_or_renewal );


private:
	void			ProcImage();
	bool			LoadImage( S_TutorialImage& newitem );
};

#endif //_TUTORIAL_EVENT_MANAGER_