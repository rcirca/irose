#ifndef _KEYMOVE_
#define _KEYMOVE_


///
/// Moving control class by KeyBoard
///

class CKeyMove
{
private:
	bool		m_bLeftKeyDown;
	bool		m_bRightKeyDown;
	bool		m_bAltKeyDown;
	bool		m_bUpKeyDown;
	bool		m_bDownKeyDown;
	bool		m_bWKeyDown;
	bool		m_bSKeyDown;
	bool		m_bAKeyDown;
	bool		m_bDKeyDown;


	float		m_fYawValue;
	

public:
	CKeyMove();
	~CKeyMove();

	void		Proc();
};

extern CKeyMove		g_KeyMove;

#endif // _KEYMOVE_	