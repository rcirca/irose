#ifndef _BGM_MANAGER_
#define _BGM_MANAGER_
#include "../util/JSingleton.h"


//---------------------------------------------------------------------------------------
///
/// class CBGManager
/// 배경음악 플레이어 관리, 음악 전이..
///
//---------------------------------------------------------------------------------------
class CBGMManager : public CJSingleton< CBGMManager >
{
private:
	int				m_iCurrentVolumeIdx;
	int				m_iStartVolumeIdx;
	bool			m_bToFadeOut;

	DWORD			m_dwTransitionPrevTime;
	DWORD			m_dwElapsedTime;


	bool			m_bEnableTransition;
	bool			m_bToNight;	
	
	int				m_iZoneNO;


public:
	CBGMManager(void);
	~CBGMManager(void);

	/// 밤낮 변경에 따른 배경음악 전이..
	void			ChangeBGMMusicWithDayAndNight( int iZoneNO, bool bToNight );

	void			EndTransition();
	void			Proc();
};

#endif //_BGM_MANAGER_