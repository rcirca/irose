#include "stdafx.h"
#include ".\objcommand.h"
#include "../CObjCHAR.h"




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  Manager class for object command
//----------------------------------------------------------------------------------------------------

CObjCommandManager::CObjCommandManager()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjCommandManager::~CObjCommandManager()
{
	ClearCommand();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjCommand* CObjCommandManager::GetObjCommand( int iCommandType )
{
	switch( iCommandType )
	{
		case OBJECT_COMMAND_STOP:
			return new CObjStopCommand();			
		case OBJECT_COMMAND_ATTACK:
			return new CObjAttackCommand();			
		case OBJECT_COMMAND_SIT:
			return new CObjSitCommand();
		case OBJECT_COMMAND_STAND:
			return new CObjStandCommand();
		case OBJECT_COMMAND_MOVE:
			return new CObjMoveCommand();
		case OBJECT_COMMAND_DIE:
			return new CObjDieCommand();
		case OBJECT_COMMAND_TOGGLE:
			return new CObjToggleCommand();
		case OBJECT_COMMAND_Skill2SELF:
			return new CObjSkill2SelfCommand();
		case OBJECT_COMMAND_Skill2OBJ:
			return new CObjSkill2ObjCommand();
		case OBJECT_COMMAND_Skill2POS:
			return new CObjSkill2PosCommand();
	}

	return NULL;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjCommandManager::IsEmpty()
{
	return m_ObjCommandList.empty();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

int CObjCommandManager::GetCommandCount()
{
	return m_ObjCommandList.size();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjCommandManager::PushCommand( CObjCommand* pObjCommand )
{
	//----------------------------------------------------------------------------------------------------
	/// Skill_of_skill 을 못받은 명령들은 모두 제거
	//----------------------------------------------------------------------------------------------------
	std::list< CObjCommand* >::iterator begin = m_ObjCommandList.begin();

	for( ; begin != m_ObjCommandList.end() ; )
	{
		if( (*begin)->m_iType == OBJECT_COMMAND_Skill2SELF	||
			(*begin)->m_iType == OBJECT_COMMAND_Skill2OBJ	||
			(*begin)->m_iType == OBJECT_COMMAND_Skill2POS )
		{
			/// 아직 결과를 받지 못한..
			if( (*begin)->m_bGetResultOfSkill == false )
			{
				begin = m_ObjCommandList.erase( begin );
				continue;
			}
		}else
		{
			begin = m_ObjCommandList.erase( begin );
			continue;
		}

		++begin;        
	}

	m_ObjCommandList.push_back( pObjCommand );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 가장 최근에 들어온 스킬 명령을 찾는다. 왜냐.. Skill start 를 받았을 경우에는 가장 최신에 받은 
///		   스킬명령의 것이다.
//----------------------------------------------------------------------------------------------------

CObjCommand* CObjCommandManager::GetLastInvalidSkillCommand()
{
	CObjCommand* pObjCommand	= NULL;
	bool		bClearDummyCmd	= false;	/// 중간에 취소되야할 명령들( start_of_skill 이 왔을때 이전에 명령만있는것들은 모두 제거 )

	std::list< CObjCommand* >::reverse_iterator rbegin = m_ObjCommandList.rbegin();

	for( ; rbegin != m_ObjCommandList.rend() ; ++rbegin )
	{
		if( (*rbegin)->m_iType == OBJECT_COMMAND_Skill2SELF	||
			(*rbegin)->m_iType == OBJECT_COMMAND_Skill2OBJ	||
			(*rbegin)->m_iType == OBJECT_COMMAND_Skill2POS )
		{
			/// 아직 유효하지 않은 마지막 스킬..			
			if( (*rbegin)->m_bValid == false )
			{
				pObjCommand = (*rbegin);
				bClearDummyCmd = true;
				break;
			}
		}		
	}

	if( pObjCommand != NULL )
	{
		/// 마지막 스킬에 플래그세팅 완료되었다.. 앞쪽의 유효하지 않은건 모두 지워라..
		if( bClearDummyCmd )
		{
			std::list< CObjCommand* >::iterator begin = m_ObjCommandList.begin();

			for( ; begin != m_ObjCommandList.end() ; )
			{
				if( (*begin) == pObjCommand )
					break;

				if( (*begin)->m_bValid == false )
				{
					begin = m_ObjCommandList.erase( begin );
					continue;
				}

				 ++begin;
			}
		}
	}

	return pObjCommand;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 Skill_Start  패킷에서만 사용된다. 결국 Skill_Start 가 오면 큐에 있는 명령중
///			가장 최신에 받은 스킬컴맨드의 플래그를 세팅..
//----------------------------------------------------------------------------------------------------

void CObjCommandManager::SetValidFlag()
{
	if( m_ObjCommandList.empty() )
		return;

	CObjCommand* pObjCommand = GetLastInvalidSkillCommand();
	if( pObjCommand )
	{
		pObjCommand->m_bValid = true;
	}else
		assert( 0 && "Revc skill start, but can't recv skill command" );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 Result_of_Skill  패킷에서만 사용된다. 
///			큐에 들어간 스킬명령은 결과를 받은후 그 플래그가 하나밖에 없어 이전 스킬이 이걸 리셋할수있다 그래서 저장..
//----------------------------------------------------------------------------------------------------

void CObjCommandManager::SetValidResultOfSkill()
{
	CObjCommand* pObjCommand = NULL;

	std::list< CObjCommand* >::iterator begin = m_ObjCommandList.begin();

	for( ; begin != m_ObjCommandList.end() ; ++begin )
	{
		///  처음 유효한명령( 당연 스킬명령 )이명 그 명령에 결과 저장..
		if( (*begin)->m_bValid == true )
		{
			if( (*begin)->m_iType == OBJECT_COMMAND_Skill2SELF ||
				(*begin)->m_iType == OBJECT_COMMAND_Skill2OBJ ||
				(*begin)->m_iType == OBJECT_COMMAND_Skill2POS )
			{
				/// start of skill 은 받았는데 result 를 못받은 첫놈을..
				if( (*begin)->m_bGetResultOfSkill == false )
				{
					(*begin)->m_bGetResultOfSkill = true;
					Log_String( LOG_NORMAL, "이것은 큐에 있는 스킬에 결과 정보를 저장\n" ); 
					return;
				}
			}else
				assert( 0 && "Command flag is valid, but this is not skill commnad" );
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 유효한 첫번째 명령을 꺼내온다. 현재는 스킬명령의 경우 START_OF_SKILL 이 들어온것.
//----------------------------------------------------------------------------------------------------

CObjCommand* CObjCommandManager::PopCommand( bool& bSkillCommand )
{
	if( IsEmpty() )
		return NULL;

	/// 마지막 명령을 저장해둔후에..
	/// 왜????
	CObjCommand* pObjCommand = NULL;/// = m_ObjCommandList.back();
	///m_ObjCommandList.pop_back();

	/// 리스트 중간에 유효한 스킬명령이 있는 지를 확인하고..
	std::list< CObjCommand* >::iterator begin = m_ObjCommandList.begin();

	for( ; begin != m_ObjCommandList.end() ; )
	{
		if( (*begin)->m_bValid == false )
		{
			delete *begin;
			begin = m_ObjCommandList.erase( begin );
		}
		else
		{			
			/// 기억된 마지막 명령을 다시 리스트의 마지막에 넣음
			/// 왜?
			///m_ObjCommandList.push_back( pObjCommand );

			/// 그리고 현재 선택된 유효한 명령을 리턴( 대략 Result_of_Skill 을 받은 스킬명령 )
			pObjCommand = *begin;
			m_ObjCommandList.erase( begin );

			bSkillCommand = true;

			return pObjCommand;
		}
	}	

	bSkillCommand = false;
	return pObjCommand;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 처리해야할 마지막 컴맨드.. 중간에 활성화된 스킬 명령이 있다면 스킬명령 수행..
//----------------------------------------------------------------------------------------------------

CObjCommand* CObjCommandManager::PopLastCommand( bool& bSkillCommand )
{
	if( IsEmpty() )
		return NULL;	

	/// 리스트 내부에 유효한 스킬 컴맨드가 있다면.. 그 명령을 리턴..	
	CObjCommand* pValidSkillCommand  = PopCommand( bSkillCommand );
	if( pValidSkillCommand )
	{			
		return pValidSkillCommand;
	}

	///assert( 0 && "Invalid command queue" );
	return NULL;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjCommandManager::ClearCommand()
{
	std::list< CObjCommand* >::iterator begin = m_ObjCommandList.begin();

	for( ; begin != m_ObjCommandList.end() ; )
	{
		begin = m_ObjCommandList.erase( begin );
	}
}






//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

///
/// abstract class for object command
///

CObjCommand::CObjCommand(void)
{
	m_bValid = false;
	m_bGetResultOfSkill = false;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjCommand::~CObjCommand(void)
{
}




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

///
/// command class for object attack command
///

CObjStopCommand::CObjStopCommand()
{
	m_iType = OBJECT_COMMAND_STOP;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjStopCommand::~CObjStopCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjStopCommand::SetCMD_STOP( )
{	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjStopCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_STOP( );

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object attack command
//----------------------------------------------------------------------------------------------------

CObjAttackCommand::CObjAttackCommand()
{
	m_iServerIndex = 0;

	m_iType = OBJECT_COMMAND_ATTACK;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

CObjAttackCommand::~CObjAttackCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CObjAttackCommand::SetCMD_ATTACK( int iServerIndex, WORD wSrvDIST, const D3DVECTOR& PosGOTO )
{
	m_iServerIndex = iServerIndex;

	m_wSrcDIST = wSrvDIST;
	m_PosGOTO = PosGOTO;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

bool CObjAttackCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_ATTACK( m_iServerIndex, m_wSrcDIST, m_PosGOTO );

	return true;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

CObjSitCommand::CObjSitCommand()
{
	m_iType = OBJECT_COMMAND_SIT;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

CObjSitCommand::~CObjSitCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CObjSitCommand::SetCMD_SIT()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

bool CObjSitCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_SIT();

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object stand command
//----------------------------------------------------------------------------------------------------

CObjStandCommand::CObjStandCommand()
{
	m_iType = OBJECT_COMMAND_STAND;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjStandCommand::~CObjStandCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjStandCommand::SetCMD_STAND()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjStandCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_STAND();

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object move command
//----------------------------------------------------------------------------------------------------

CObjMoveCommand::CObjMoveCommand()
{
	m_bMoveToObjectMode = false;

	m_btRunMODE			= 0;

	m_wSrvDIST = 0;

	m_PosTO.x = 0;
	m_PosTO.y = 0;	

	m_iServerTarget = 0;

	m_iType = OBJECT_COMMAND_MOVE;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjMoveCommand::~CObjMoveCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjMoveCommand::SetCMD_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE )
{
	m_bMoveToObjectMode = false;

	m_PosTO = PosTO;

	m_btRunMODE = btRunMODE;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjMoveCommand::SetCMD_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget )
{
	m_bMoveToObjectMode = true;

	m_wSrvDIST	= wSrvDIST;
	m_PosTO		= PosTO;

	m_iServerTarget = iServerTarget;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjMoveCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	if( m_bMoveToObjectMode == false )
	{
		pObjCHAR->SetCMD_MOVE( m_PosTO, m_btRunMODE );
	}else
	{
		pObjCHAR->SetCMD_MOVE( m_wSrvDIST, m_PosTO, m_iServerTarget );
	}

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object die command
//----------------------------------------------------------------------------------------------------

CObjDieCommand::CObjDieCommand()
{
	m_iType = OBJECT_COMMAND_DIE;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjDieCommand::~CObjDieCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjDieCommand::SetCMD_DIE()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjDieCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_DIE();

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object toggle command
//----------------------------------------------------------------------------------------------------

CObjToggleCommand::CObjToggleCommand()
{
	m_btTYPE = 0;
	m_iType = OBJECT_COMMAND_TOGGLE;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjToggleCommand::~CObjToggleCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjToggleCommand::SetCMD_TOGGLE( BYTE btTYPE )
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjToggleCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_TOGGLE( m_btTYPE );

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object Skill2Self command
//----------------------------------------------------------------------------------------------------

CObjSkill2SelfCommand::CObjSkill2SelfCommand()
{
	m_iSkillIDX = 0;

	m_iType = OBJECT_COMMAND_Skill2SELF;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjSkill2SelfCommand::~CObjSkill2SelfCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjSkill2SelfCommand::SetCMD_Skill2SELF (short nSkillIDX)
{
	m_iSkillIDX = nSkillIDX;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjSkill2SelfCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_Skill2SELF( m_iSkillIDX );

	return true;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief command class for object Skill2Obj command
//----------------------------------------------------------------------------------------------------

CObjSkill2ObjCommand::CObjSkill2ObjCommand()
{
	m_wSrvDIST		= 0;
	m_PosTO.x		= 0;
	m_PosTO.y		= 0;

	m_iSkillIDX			= 0;
	m_iServerTarget		= 0;

	m_iType = OBJECT_COMMAND_Skill2OBJ;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

CObjSkill2ObjCommand::~CObjSkill2ObjCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CObjSkill2ObjCommand::SetCMD_Skill2OBJ ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX)
{
	m_wSrvDIST	= wSrvDIST;
	m_PosTO		= PosTO;

	m_iSkillIDX			= nSkillIDX;
	m_iServerTarget		= iServerTarget;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CObjSkill2ObjCommand::SetCMD_Skill2OBJ ( int iServerTarget, short nSkillIDX )
{
	m_iSkillIDX			= nSkillIDX;
	m_iServerTarget		= iServerTarget;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

bool CObjSkill2ObjCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	return pObjCHAR->SetCMD_Skill2OBJ( m_wSrvDIST, m_PosTO, m_iServerTarget, m_iSkillIDX );
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  command class for object Skill2Pos command
//----------------------------------------------------------------------------------------------------

CObjSkill2PosCommand::CObjSkill2PosCommand()
{
	m_PosGOTO.x = 0;
	m_PosGOTO.y = 0;
	
	m_iSkillIDX = 0;

	m_iType = OBJECT_COMMAND_Skill2POS;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjSkill2PosCommand::~CObjSkill2PosCommand()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CObjSkill2PosCommand::SetCMD_Skill2POS (const D3DVECTOR& PosGOTO, short nSkillIDX)
{
	m_PosGOTO	= PosGOTO;
	m_iSkillIDX = nSkillIDX;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjSkill2PosCommand::Execute( CObjCHAR* pObjCHAR )
{
	if( pObjCHAR == NULL )
	{
		assert( 0 && "Command::Execute parameter is NULL:" );
		return false;
	}

	pObjCHAR->SetCMD_Skill2POS( m_PosGOTO, m_iSkillIDX );

	return true;
}

