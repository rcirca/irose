#ifndef _SFX_FONT_
#define _SFX_FONT_

#include "ISFX.h"


//--------------------------------------------------------------------------------------------------
///
/// class CSFXFont
///	Cinematic font effect..
///
//--------------------------------------------------------------------------------------------------
class CEffectString;
class CSFXFont : public ISFX
{
	std::list<CEffectString*> m_children;
public:
	CSFXFont(void);
	virtual ~CSFXFont(void);

	virtual bool	Init();
	virtual void	Release();

	virtual	void	Update();
	virtual void	Draw();

	void	AddEffectString( CEffectString* child );
	void    RemoveEffectStringsByType( int type );

	enum{
		TYPE_NONE,
		TYPE_ZONE_NAME,
		TYPE_ZONE_DESC,
		TYPE_QUEST_NAME,
		TYPE_QUEST_STARTMSG,
	};
};
#endif //_SFX_FONT_



