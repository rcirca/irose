#include "stdafx.h"
#include ".\preventduplicatedcommand.h"
#include "../Game.h"


/// for singleton
CPreventDuplicatedCommand _PreventDuplicatedCommand;

CPreventDuplicatedCommand::CPreventDuplicatedCommand(void)
{
}

CPreventDuplicatedCommand::~CPreventDuplicatedCommand(void)
{
}

//----------------------------------------------------------------------------------------------------
/// 각 풀들 초기화
//----------------------------------------------------------------------------------------------------
void CPreventDuplicatedCommand::Init()
{
	int i = 0;
	int j = 0;

	CObjCommand* pCommand = NULL;

	for( i = 0; i < OBJECT_COMMAND_MAX ; i++ )
	{
		for( j = 0; j < 10 ; j++ )
		{
			switch( i )
			{				
				//case OBJECT_COMMAND_STOP:
				//	pCommand = new CObjStopCommand();
				//case OBJECT_COMMAND_ATTACK:
				//	pCommand = new CObjAttackCommand();
				//case OBJECT_COMMAND_SIT:
				//	pCommand = new CObjSitCommand();
				//case OBJECT_COMMAND_STAND:
				//	pCommand = new CObjStandCommand();
				//case OBJECT_COMMAND_MOVE:
				//	pCommand = new CObjMoveCommand();
				//case OBJECT_COMMAND_DIE:
				//	pCommand = new CObjDieCommand();
				//case OBJECT_COMMAND_TOGGLE:
				//	pCommand = new CObjToggleCommand();
				case OBJECT_COMMAND_Skill2SELF:
					pCommand = new CObjSkill2SelfCommand();
					break;
				case OBJECT_COMMAND_Skill2OBJ:
					pCommand = new CObjSkill2ObjCommand();
					break;
				case OBJECT_COMMAND_Skill2POS:
					pCommand = new CObjSkill2PosCommand();
					break;
			}

			if( pCommand )
				m_CommandPool[ i ].push_back( pCommand ); 
		}
	}

	for( i = 0; i < 10 ; i++ )
	{
		SendedCommand* pSendedCommand = new SendedCommand();
		pSendedCommand->iElapsedTime = 0;
		m_CommandStructPool.push_back( pSendedCommand );
	}	
}

//----------------------------------------------------------------------------------------------------
/// 명령 체크 큐 초기화
//----------------------------------------------------------------------------------------------------
void CPreventDuplicatedCommand::ClearCommandQueue()
{
	std::list< SendedCommand* >::iterator begin = m_SendedCommandQueue.begin();
	std::list< SendedCommand* >::iterator end = m_SendedCommandQueue.end();

	for( ; begin != end ; ++begin )
	{
		SendedCommand* pCommand = *begin;
		delete pCommand;		
	}

	m_SendedCommandQueue.clear();
}

//----------------------------------------------------------------------------------------------------
/// 모든 리소스클리어
//----------------------------------------------------------------------------------------------------
void CPreventDuplicatedCommand::ClearAll()
{
	ClearCommandQueue();

	int i = 0;

	/// clear command pool
	for( i = 0; i < OBJECT_COMMAND_MAX ; i++ )
	{
		std::list< CObjCommand* >::iterator begin = m_CommandPool[ i ].begin();
		std::list< CObjCommand* >::iterator end = m_CommandPool[ i ].end();

		for( ; begin != end ; ++begin )
		{
			CObjCommand* pCommand = *begin;
			delete pCommand;		
		}
	}
	m_CommandPool[ i ].clear();

	/// clear sendedcommand pool
	for( i = 0; i < OBJECT_COMMAND_MAX ; i++ )
	{
		std::list< SendedCommand* >::iterator begin = m_CommandStructPool.begin();
		std::list< SendedCommand* >::iterator end = m_CommandStructPool.end();

		for( ; begin != end ; ++begin )
		{
			SendedCommand* pCommand = *begin;
			delete pCommand;		
		}
	}
	m_CommandStructPool.clear();	
}


//----------------------------------------------------------------------------------------------------
/// 타이머 갱신..
//----------------------------------------------------------------------------------------------------
const int SKILL_DUPLICATED_DELAY = 1000;
void CPreventDuplicatedCommand::Proc()
{
	std::list< SendedCommand* >::iterator begin = m_SendedCommandQueue.begin();
	std::list< SendedCommand* >::iterator end = m_SendedCommandQueue.end();

	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	for( ; begin != end ; )
	{
		SendedCommand* pSendedCommand = *begin;
		CObjCommand* pCommand = pSendedCommand->pCommand;
		
		/// 2초가 경과 되었다면 파괴..
		if( ( dwCurrentTime - pSendedCommand->iElapsedTime ) > SKILL_DUPLICATED_DELAY )
		{			
			if( pCommand )
			{
				m_CommandPool[ pCommand->GetType() ].push_back( pCommand );
				pSendedCommand->pCommand = NULL;
				pSendedCommand->iElapsedTime = 0;
				m_CommandStructPool.push_back( pSendedCommand );				
			}			

			begin = m_SendedCommandQueue.erase( begin );
		}else
			++begin;
	}	
}


//----------------------------------------------------------------------------------------------------
/// 풀에서 컴맨드 객체를 구한다.
//----------------------------------------------------------------------------------------------------

CObjCommand* CPreventDuplicatedCommand::GetObjCommand( int iType )
{
	if( iType < 0 || iType >= OBJECT_COMMAND_MAX )
		return NULL;

	CObjCommand* pCommand = NULL;

	if( m_CommandPool[ iType ].empty() )
	{		
		switch( iType )
		{
			case OBJECT_COMMAND_Skill2SELF:
				pCommand = new CObjSkill2SelfCommand();
				break;
			case OBJECT_COMMAND_Skill2OBJ:
				pCommand = new CObjSkill2ObjCommand();
				break;
			case OBJECT_COMMAND_Skill2POS:
				pCommand = new CObjSkill2PosCommand();
				break;
		}

		return pCommand;
	}

	pCommand = m_CommandPool[ iType ].back();
	m_CommandPool[ iType ].pop_back();

	return pCommand;
}


//----------------------------------------------------------------------------------------------------
/// 보낸 명령 구조체를 보관한다.
//----------------------------------------------------------------------------------------------------

SendedCommand* CPreventDuplicatedCommand::GetSendedCommand()
{
	if( m_CommandStructPool.empty() )
		return new SendedCommand();

	SendedCommand* pSendedCommand = NULL;
	pSendedCommand = m_CommandStructPool.back();
	m_CommandStructPool.pop_back();

	return pSendedCommand;
}


//----------------------------------------------------------------------------------------------------
///
/// 일단 스킬들은 특정 시간이전에 연속 사용이 불가능하다..
///
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/// 셀프 스킬
//----------------------------------------------------------------------------------------------------
bool CPreventDuplicatedCommand::PushSelfSkillCommand( int iSkillSlot )
{
	CObjSkill2SelfCommand* pCommand = (CObjSkill2SelfCommand*)GetObjCommand( OBJECT_COMMAND_Skill2SELF );

	if( pCommand )
	{
		pCommand->SetCMD_Skill2SELF( iSkillSlot );

		SendedCommand* pSendedCommand = GetSendedCommand();

		pSendedCommand->iElapsedTime = g_GameDATA.GetGameTime();
		pSendedCommand->pCommand = pCommand;

		m_SendedCommandQueue.push_back( pSendedCommand );
	}


	return true;
}

//----------------------------------------------------------------------------------------------------
/// 타겟 스킬
//----------------------------------------------------------------------------------------------------
bool CPreventDuplicatedCommand::PushTargetSkillCommand( int iServerTarget, int iSkillSlot )
{
	CObjSkill2ObjCommand* pCommand = (CObjSkill2ObjCommand*)GetObjCommand( OBJECT_COMMAND_Skill2OBJ );

	if( pCommand )
	{
		pCommand->SetCMD_Skill2OBJ( iServerTarget, iSkillSlot );

		SendedCommand* pSendedCommand = GetSendedCommand();

		pSendedCommand->iElapsedTime = g_GameDATA.GetGameTime();
		pSendedCommand->pCommand = pCommand;

		m_SendedCommandQueue.push_back( pSendedCommand );
	}

	return true;
}

//----------------------------------------------------------------------------------------------------
/// 지역스킬
//----------------------------------------------------------------------------------------------------
bool CPreventDuplicatedCommand::PushPositionSkillCommand( D3DVECTOR &PosTO, int iSkillSlot )
{
	CObjSkill2PosCommand* pCommand = (CObjSkill2PosCommand*)GetObjCommand( OBJECT_COMMAND_Skill2POS );

	if( pCommand )
	{
		pCommand->SetCMD_Skill2POS( PosTO, iSkillSlot );

		SendedCommand* pSendedCommand = GetSendedCommand();

		pSendedCommand->iElapsedTime = g_GameDATA.GetGameTime();
		pSendedCommand->pCommand = pCommand;

		m_SendedCommandQueue.push_back( pSendedCommand );
	}

	return true;
}

bool CPreventDuplicatedCommand::CanSendSelfSkillCommand( int iSkillSlot )
{
	std::list< SendedCommand* >::iterator begin = m_SendedCommandQueue.begin();
	std::list< SendedCommand* >::iterator end = m_SendedCommandQueue.end();

	for( ; begin != end ; ++begin )
	{
		SendedCommand* pSendedCommand = *begin;
		
		CObjCommand* pCommand = pSendedCommand->pCommand;

		if( pCommand->GetType() == OBJECT_COMMAND_Skill2SELF )
		{
			CObjSkill2SelfCommand* pSelfCommand = (CObjSkill2SelfCommand*)pCommand;
			if( pSelfCommand->GetSkillIndex( ) == iSkillSlot )
				return false;
		}
	}

	return true;
}

bool CPreventDuplicatedCommand::CanSendTargetSkillCommand( int iServerTarget, int iSkillSlot )
{
	std::list< SendedCommand* >::iterator begin = m_SendedCommandQueue.begin();
	std::list< SendedCommand* >::iterator end = m_SendedCommandQueue.end();

	for( ; begin != end ; ++begin )
	{
		SendedCommand* pSendedCommand = *begin;
		
		CObjCommand* pCommand = pSendedCommand->pCommand;

		if( pCommand->GetType() == OBJECT_COMMAND_Skill2OBJ )
		{
			CObjSkill2ObjCommand* pObjCommand = (CObjSkill2ObjCommand*)pCommand;
			if( pObjCommand->GetSkillIndex() == iSkillSlot &&
				pObjCommand->GetServerTarget() == iServerTarget )
				return false;
		}
	}

	return true;
}

bool CPreventDuplicatedCommand::CanSendPositionSkillCommand( D3DVECTOR &PosTO, int iSkillSlot )
{
	std::list< SendedCommand* >::iterator begin = m_SendedCommandQueue.begin();
	std::list< SendedCommand* >::iterator end = m_SendedCommandQueue.end();

	for( ; begin != end ; ++begin )
	{
		SendedCommand* pSendedCommand = *begin;
		
		CObjCommand* pCommand = pSendedCommand->pCommand;

		if( pCommand->GetType() == OBJECT_COMMAND_Skill2POS )
		{
			CObjSkill2PosCommand* pObjCommand = (CObjSkill2PosCommand*)pCommand;

			D3DVECTOR& vec = pObjCommand->GetTargetPos();
			if( pObjCommand->GetSkillIndex() == iSkillSlot )
			{
				if( vec.x == PosTO.x &&
					vec.y == PosTO.y &&
					vec.z == PosTO.z )
				{
					return false;	
				}				
			}
		}
	}

	return true;
}
