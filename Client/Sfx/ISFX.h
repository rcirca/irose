#ifndef _SFX_
#define _SFX_


enum SFX_TYPE
{
	SFX_WIDE_SCREEN = 0,
	SFX_FONT,
	SFX_MAX,
};

//--------------------------------------------------------------------------------------
///
/// class ISFX
/// Interface of SFX classes
///
//--------------------------------------------------------------------------------------

class ISFX
{
protected:
	int				m_iType;

public:
	ISFX(void);
	virtual ~ISFX(void);


	int				GetType(){ return m_iType; }

	virtual	bool	Init() = 0;
	virtual	void	Release() = 0;

	virtual	void	Update() = 0;
	virtual void	Draw() = 0;
};

#endif //_SFX_