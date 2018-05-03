#ifndef _COMMANDPT_
#define _COMMANDPT_


enum 
{
	COMMAND_COMMAND,			/// Normal command
	EMOTION_COMMAND,
	SKILL_COMMAND,				/// Skill command
	MAX_QUICK_COMMAND,
};

class CCommand;


extern CCommand* g_QuickCommandPool[ MAX_QUICK_COMMAND ];


//----------------------------------------------------------------------------------------------------
/// Class CCommand
/// @brief Abstraction class for command
//----------------------------------------------------------------------------------------------------

class CCommand
{
protected:
	
	int				m_iSlotNO;

public:
	CCommand() : m_iSlotNO(0){};
	virtual ~CCommand(){};

	/// 클래스 상태 초기화.
	void			SetMember( int iSlotNO )
	{		
		m_iSlotNO = iSlotNO;
	}


	
	virtual bool	Execute() = 0{ *(int*)0 = 10; };
};


//----------------------------------------------------------------------------------------------------
/// class CBasicCommand
/// @brief Basic command class
//----------------------------------------------------------------------------------------------------

class CBasicCommand : public CCommand
{
public:
	CBasicCommand();
	virtual ~CBasicCommand();

	virtual bool	Execute();
};


//----------------------------------------------------------------------------------------------------
/// class CEmotionCommand
/// @brief Emotion expression command
//----------------------------------------------------------------------------------------------------

class CEmotionCommand : public CCommand
{
public:
	CEmotionCommand();
	virtual ~CEmotionCommand();

	virtual bool	Execute();
};


//----------------------------------------------------------------------------------------------------
/// class CInventoryCommand
/// @brief Inventory slot command class
//----------------------------------------------------------------------------------------------------

class CInventoryCommand : public CCommand
{
public:
	CInventoryCommand();
	virtual ~CInventoryCommand();

	virtual bool	Execute();
};


//----------------------------------------------------------------------------------------------------
/// class CNormalCommand
/// @brief Normal command class
//----------------------------------------------------------------------------------------------------

class CNormalCommand : public CCommand
{
public:
	CNormalCommand();
	virtual ~CNormalCommand();

	virtual bool	Execute();
};


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Skill command class
//----------------------------------------------------------------------------------------------------

class CSkillCommand : public CCommand
{
public:
	CSkillCommand();
	virtual ~CSkillCommand();

	virtual bool	Execute();
};


#endif //_COMMANDPT_