#ifndef _SFX_MANAGER_
#define _SFX_MANAGER_

#include <list>
#include "../Util/JSingleton.h"

class ISFX;

//-------------------------------------------------------------------------------------------
///
/// class CSFXManager
/// @brief manager of sfx in game.
///
//-------------------------------------------------------------------------------------------

class CSFXManager : public CJSingleton< CSFXManager >
{
private:
	std::list< ISFX* >			m_SFXList;


public:
	CSFXManager(void);
	~CSFXManager(void);

	//-----------------------------------------------------------------------------------------
	/// General purpose..
	//-----------------------------------------------------------------------------------------

	void						Update();
	void						Draw();
	void						Clear();

	ISFX*						AddSFX( ISFX* pSFX );
	bool						DeleteSFX( ISFX* pSFX );

	ISFX*						FindSFXWithType( int iSFXType );

	//-----------------------------------------------------------------------------------------
	/// Specific special effect
	//-----------------------------------------------------------------------------------------

	bool						StartWideScreenEffect();
	void						StopWideScreenEffect();


};



#endif //_SFX_MANAGER_