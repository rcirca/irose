#ifndef _PREVENT_DUPLICATED_COMMAND_
#define _PREVENT_DUPLICATED_COMMAND_

#include "Util/JSingleton.h"
#include "ObjectCommand/ObjCommand.h"
#include < list >


struct SendedCommand
{
	CObjCommand* pCommand;
	int			 iElapsedTime;

	SendedCommand(){ pCommand = NULL; iElapsedTime = 0; }
};

//------------------------------------------------------------------------------------
///
/// class CPreventDuplicatedCommand
/// 서버로 중복된 명력( 특히 스킬명령 ) 전송을 막습니다.
///
//------------------------------------------------------------------------------------

class CPreventDuplicatedCommand : public CJSingleton< CPreventDuplicatedCommand >
{
private:
	std::list< CObjCommand* >		m_CommandPool[ OBJECT_COMMAND_MAX ];
	std::list< SendedCommand* >		m_CommandStructPool;
	std::list< SendedCommand* >		m_SendedCommandQueue;
	

public:
	CPreventDuplicatedCommand(void);
	~CPreventDuplicatedCommand(void);


	void				Init();
	void				ClearCommandQueue();
	void				ClearAll();


	void				Proc();

	CObjCommand*		GetObjCommand( int iType );
	SendedCommand*		GetSendedCommand();

	//----------------------------------------------------------------------------------------------------
	/// 일단 스킬들은 특정 시간이전에 연속 사용이 불가능하다..
	//----------------------------------------------------------------------------------------------------
	bool				PushSelfSkillCommand( int iSkillSlot );	
	bool				PushTargetSkillCommand( int iServerTarget, int iSkillSlot );	
	bool				PushPositionSkillCommand( D3DVECTOR &PosTO, int iSkillSlot );

	bool				CanSendSelfSkillCommand( int iSkillSlot );	
	bool				CanSendTargetSkillCommand( int iServerTarget, int iSkillSlot );	
	bool				CanSendPositionSkillCommand( D3DVECTOR &PosTO, int iSkillSlot );


};


#endif //_PREVENT_DUPLICATED_COMMAND_