#ifndef _CHANGEATTACKMODE_
#define _CHANGEATTACKMODE_



enum AVATAR_ACTION_MODE
{
	AVATAR_NORMAL_MODE = 0,
	AVATAR_ATTACK_MODE = 1,
};


class CObjCHAR;

//----------------------------------------------------------------------------------------------------
///	Change avatar motion mode..
//----------------------------------------------------------------------------------------------------

class CChangeActionMode
{
private:
	int					m_iAvatarActionMode;
	int					m_iElapsedAttackEnd;			// the elapsed time from attack end.

	bool				m_bUpdateMode;					// 전투중인가?


	CObjCHAR*			m_pObjCHAR;

	CChangeActionMode(void);
public:
	CChangeActionMode( CObjCHAR* pObjCHAR );
	~CChangeActionMode(void);

	void				ChangeActionMode( int iActionMode );
	int					GetActionMode(){ return m_iAvatarActionMode; }


	//----------------------------------------------------------------------------------------------------
	/// Get proper motion by action mode
	//----------------------------------------------------------------------------------------------------
	int					GetAdjustedActionIndex( int iActionIdx );


	//----------------------------------------------------------------------------------------------------
	/// Attack start. change attack mode.
	//----------------------------------------------------------------------------------------------------
	void				AttackStart();
	//----------------------------------------------------------------------------------------------------
	/// If avt finished attack command, then change action mode to normal mode after 10 sec from attack end time.
	//----------------------------------------------------------------------------------------------------
	void				AttackEnd();

	//----------------------------------------------------------------------------------------------------
	/// Change action mode by checking time..
	//----------------------------------------------------------------------------------------------------
	void				Proc();
};

#endif //_CHANGEATTACKMODE_