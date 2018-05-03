#ifndef _COMMANDSTATE_
#define _COMMANDSTATE_

#include <list>
#include "Util/JSingleton.h"



enum COMMAND_TYPE
{
	COMMAND_NORMAL = 1,
	COMMAND_SKILL,
	COMMAND_INVENTORY,
};

enum MOUSE_STATE
{
	MOUSE_STATE_NONE = 0,
	MOUSE_STATE_TARGET_SET,
};


enum USER_INPUT_STYLE
{
	DEFAULT_USER_INPUT_STYLE = 0,
	SEVENHEARTS_USER_INPUT_STYLE = 1,
	MAX_USER_INPUT_STYLE,
};


class CObjCHAR;



//----------------------------------------------------------------------------------------------------
///
/// Interface for user input state
///
//----------------------------------------------------------------------------------------------------
class CUserInputState
{
protected:
	//----------------------------------------------------------------------------------------------------
	/// if it is two step command, use follow memberes..
	//----------------------------------------------------------------------------------------------------
	int			m_iCommandType;
	int			m_iCommand;

	//----------------------------------------------------------------------------------------------------
	/// current mouse state.
	//----------------------------------------------------------------------------------------------------
	int			m_iMouseState;
	int			m_iCurrentTargetType;
	int			m_iCurrentTarget;

	//----------------------------------------------------------------------------------------------------
	/// Current active skill
	//----------------------------------------------------------------------------------------------------
	int			m_iCurrentActiveSkillSlot;

public:
	CUserInputState();
	~CUserInputState();

	virtual bool ClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam ) = 0{ *(int*)0 = 10; };
	virtual bool RButtonDown( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam ) = 0{ *(int*)0 = 10; };
	virtual bool DBClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam ) = 0{ *(int*)0 = 10; };

	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam ) = 0;


	//----------------------------------------------------------------------------------------------------
	/// Command state method
	//----------------------------------------------------------------------------------------------------
	void		SetCommandType( int iCommandType ){ m_iCommandType = iCommandType; }
	void		SetCommand( int iCommand ){ m_iCommand = iCommand; }
	int			GetCommandType(){ return m_iCommandType; }
	int			GetCommand(){ return m_iCommand; }

	//----------------------------------------------------------------------------------------------------
	/// Mouse state method
	//----------------------------------------------------------------------------------------------------
	void		SetMouseState( int iMouseState ){ m_iMouseState = iMouseState; }
	void		SetCurrentTarget( int iTarget ){ m_iCurrentTarget = iTarget; }
	void		SetCurrentTargetType( int iTargetType ){ m_iCurrentTargetType = iTargetType; }
	int			GetMouseState(){ return m_iMouseState; }
	int			GetCurrentTarget(){ return m_iCurrentTarget; }
	int			GetCurrentTargetType(){ return m_iCurrentTargetType; }


	//----------------------------------------------------------------------------------------------------
	/// Current active skill
	//----------------------------------------------------------------------------------------------------
	void		SetCurrentActiveSkillSlot( int iSkillSlotNO );
	int			GetCurrentActiveSkillSlot(){ return m_iCurrentActiveSkillSlot; }


	//----------------------------------------------------------------------------------------------------
	/// @brief 현재 키보드 입력 상태에 따른 타겟 세팅
	//----------------------------------------------------------------------------------------------------
	virtual void		ClearMouseState() = 0{ *(int*)0 = 10; };
	virtual void		SetTargetObject( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, DWORD wVKeyState, bool bDBClick = false ) = 0{ *(int*)0 = 10; };
	virtual void		SetTargetObject_Normal( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false ) = 0{ *(int*)0 = 10; };
	virtual void		SetTargetObject_CtrlClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false ) = 0{ *(int*)0 = 10; };
	virtual void		SetTargetObject_ShiftClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false ) = 0{ *(int*)0 = 10; };



	//----------------------------------------------------------------------------------------------------
	/// PVP 관련 설정 함수( non virtual )
	//----------------------------------------------------------------------------------------------------
	void				PVPTarget_Click( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
	static bool			IsEnemy( CObjCHAR* pTargetAVT );
};




//----------------------------------------------------------------------------------------------------
/// 
/// Default command style.. ( Lineage 2 style )
///
//----------------------------------------------------------------------------------------------------
class CDefaultUserInput : public CUserInputState
{

public:
	CDefaultUserInput();
	~CDefaultUserInput();

	/*override*/bool		ClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	/*override*/bool		RButtonDown( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	/*override*/bool		DBClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );

	/*override*/bool		OnKeyDown( WPARAM wParam, LPARAM lParam );

	//----------------------------------------------------------------------------------------------------
	/// @brief 현재 키보드 입력 상태에 따른 타겟 세팅
	//----------------------------------------------------------------------------------------------------
	/*override*/void		ClearMouseState();
	/*override*/void		SetTargetObject( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, DWORD wVKeyState, bool bDBClick = false );
	/*override*/void		SetTargetObject_Normal( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
	/*override*/void		SetTargetObject_CtrlClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
	/*override*/void		SetTargetObject_ShiftClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
};



//----------------------------------------------------------------------------------------------------
/// 
/// Sevenhearts style.. (  )
///
//----------------------------------------------------------------------------------------------------
class CSevenHeartUserInput : public CUserInputState
{

public:
	CSevenHeartUserInput();
	~CSevenHeartUserInput();

	/*override*/bool		ClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	/*override*/bool		RButtonDown( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	/*override*/bool		DBClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );

	/*override*/bool		OnKeyDown( WPARAM wParam, LPARAM lParam );

	//----------------------------------------------------------------------------------------------------
	/// @brief 현재 키보드 입력 상태에 따른 타겟 세팅
	//----------------------------------------------------------------------------------------------------
	/*override*/void		ClearMouseState();
	/*override*/void		SetTargetObject( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, DWORD wVKeyState, bool bDBClick = false );
	/*override*/void		SetTargetObject_Normal( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
	/*override*/void		SetTargetObject_CtrlClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );
	/*override*/void		SetTargetObject_ShiftClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, bool bDBClick = false );	
};



class CObjectTargetEffect;

//----------------------------------------------------------------------------------------------------
///
///	user input system
///
//----------------------------------------------------------------------------------------------------
class CUserInputSystem : public CJSingleton< CUserInputSystem >
{
private:
	CUserInputState*				m_pUserInput;
	int								m_iCurrentUserInputStyle;

	CObjectTargetEffect*			m_pObjectTargetEffect;
	
public:
	CUserInputSystem();
	~CUserInputSystem();

	void		Init();
	void		Clear();

	bool		ClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	bool		RButtonDown( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );
	bool		DBClickObject( int iTarget, D3DXVECTOR3& PickPos, WPARAM wParam );

	bool		OnKeyDown( WPARAM wParam, LPARAM lParam );

	//----------------------------------------------------------------------------------------------------
	/// Change user input style
	//----------------------------------------------------------------------------------------------------
	void		ChangeUserInputStyle( int iStyle );
	int			GetUserInputStyle(){ return m_iCurrentUserInputStyle; }

	
	//----------------------------------------------------------------------------------------------------
	/// Command state method
	//----------------------------------------------------------------------------------------------------
	void		SetCommandType( int iCommandType )	{ m_pUserInput->SetCommandType( iCommandType ); }
	void		SetCommand( int iCommand )			{ m_pUserInput->SetCommand( iCommand ); }
	int			GetCommandType()					{ return m_pUserInput->GetCommandType(); }
	int			GetCommand()						{ return m_pUserInput->GetCommand(); }

	//----------------------------------------------------------------------------------------------------
	/// Mouse state method
	//----------------------------------------------------------------------------------------------------
	void		SetMouseState( int iMouseState )	{ m_pUserInput->SetMouseState( iMouseState ); }
	void		SetCurrentTarget( int iTarget )		{ m_pUserInput->SetCurrentTarget( iTarget ); }
	void		SetCurrentTargetType( int iTargetType ){ m_pUserInput->SetCurrentTargetType( iTargetType ); }
	int			GetMouseState()						{ return m_pUserInput->GetMouseState(); }
	int			GetCurrentTarget()					{ return m_pUserInput->GetCurrentTarget(); }
	int			GetCurrentTargetType()				{ return m_pUserInput->GetCurrentTargetType(); }


	
	//----------------------------------------------------------------------------------------------------
	/// Current active skill
	//----------------------------------------------------------------------------------------------------
	void		SetCurrentActiveSkillSlot( int iSkillSlotNO )	{ m_pUserInput->SetCurrentActiveSkillSlot( iSkillSlotNO ); };
	int			GetCurrentActiveSkillSlot()						{ return m_pUserInput->GetCurrentActiveSkillSlot(); };


	//----------------------------------------------------------------------------------------------------
	/// @brief 현재 키보드 입력 상태에 따른 타겟 세팅
	//----------------------------------------------------------------------------------------------------
	void		ClearMouseState()					{ m_pUserInput->ClearMouseState(); }
	void		SetTargetObject( int iTargetType, int iTargetObj, D3DVECTOR& pickPos, DWORD wVKeyState )
													{ m_pUserInput->SetTargetObject( iTargetType, iTargetObj, pickPos, wVKeyState ); }
	void		SetTargetObject_Normal( int iTargetType, int iTargetObj, D3DVECTOR& pickPos )
													{ m_pUserInput->SetTargetObject_Normal( iTargetType, iTargetObj, pickPos ); }
	void		SetTargetObject_CtrlClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos )
													{ m_pUserInput->SetTargetObject_CtrlClick( iTargetType, iTargetObj, pickPos ); }
	void		SetTargetObject_ShiftClick( int iTargetType, int iTargetObj, D3DVECTOR& pickPos )
													{ m_pUserInput->SetTargetObject_ShiftClick( iTargetType, iTargetObj, pickPos ); }

	/// 자신을 타겟으로 한다.
	void		SetTargetSelf();
};

#define g_UserInputSystem (CUserInputSystem::GetSingleton())

#endif //_COMMANDSTATE_