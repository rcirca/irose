#include "stdafx.h"
#include ".\objectactionprocessor.h"
#include "CGameObj.h"
#include "Object.h"
#include "Network/CNetWork.h"
#include "ObjFixedEvent.h"
#include "Event/Quest_FUNC.h"


//------------------------------------------------------------------------------------------------
/// Managing for CActionProcessChain
//------------------------------------------------------------------------------------------------

CObjectActionProcessor __objectActionProcess;

CObjectActionProcessor::CObjectActionProcessor(void)
{
	m_pRootActionChain = NULL;
}

CObjectActionProcessor::~CObjectActionProcessor(void)
{
	if( m_pRootActionChain != NULL )
	{
		delete m_pRootActionChain;
		m_pRootActionChain = NULL;
	}
}

void CObjectActionProcessor::MakeChain()
{
	CWarpObjectActionProcessor* pWarpObjActionProc = new CWarpObjectActionProcessor();
	SetNextChain( pWarpObjActionProc );

	CEventObjectActionProcessor* pEventObjActionProc = new CEventObjectActionProcessor();
	SetNextChain( pEventObjActionProc );
}

void CObjectActionProcessor::ClearChain()
{
	if( m_pRootActionChain != NULL )
	{
		m_pRootActionChain->DeleteNextChian();
		
		delete m_pRootActionChain;
		m_pRootActionChain = NULL;
	}
}

void CObjectActionProcessor::SetNextChain( CActionProcessChain* pChain )
{
	if( m_pRootActionChain == NULL )
	{
		m_pRootActionChain = pChain;
		return;
	}

	m_pRootActionChain->AddNextChain( pChain );
}

void CObjectActionProcessor::ProcessChain( HNODE hNode )
{
	if( m_pRootActionChain != NULL )
		m_pRootActionChain->ProcessChain( hNode );
}


//------------------------------------------------------------------------------------------------
/// Interface for "getUserdata function"
//------------------------------------------------------------------------------------------------

CActionProcessChain::CActionProcessChain(void)
{
	m_pNextActionChain = NULL;
}

CActionProcessChain::~CActionProcessChain(void)
{
	if( m_pNextActionChain != NULL )
	{
		delete m_pNextActionChain;
		m_pNextActionChain = NULL;
	}
}

void CActionProcessChain::DeleteNextChian()
{
	if( m_pNextActionChain != NULL )
	{		
		m_pNextActionChain->DeleteNextChian();

		delete m_pNextActionChain;
		m_pNextActionChain = NULL;

		return;
	}
}

void CActionProcessChain::AddNextChain( CActionProcessChain* pNext )
{
	if( m_pNextActionChain == NULL )
	{
		m_pNextActionChain = pNext;
		return;
	}

	m_pNextActionChain->AddNextChain( pNext );
}


//------------------------------------------------------------------------------------------------
/// Warp object process
//------------------------------------------------------------------------------------------------

CWarpObjectActionProcessor::CWarpObjectActionProcessor(void)
{
}

CWarpObjectActionProcessor::~CWarpObjectActionProcessor(void)
{
}


void CWarpObjectActionProcessor::ProcessChain( HNODE hNode )
{
	HNODE hUserData = ::getUserData( hNode );
	if ( hUserData ) 
	{
		CGameOBJ* pObj = reinterpret_cast< CGameOBJ* >( hUserData );
        
		/// warp object 인지 검사..
		if( pObj->IsA( OBJ_GROUND ) )
		{
			int iSpecialData = pObj->GetSpecialUserNumericData();
			
			if( iSpecialData >= WARP_OBJECT_INDEX_OFFSET ) 
			{
				g_pNet->Send_cli_TELEPORT_REQ ( g_pAVATAR, iSpecialData - WARP_OBJECT_INDEX_OFFSET );
				assert( 0 && " This is warp object" );
				setUserData( hNode, (HNODE)0 );
			}
		}
	}
	
	if( m_pNextActionChain )
		m_pNextActionChain->ProcessChain( hNode );
}


//------------------------------------------------------------------------------------------------
/// Event object process
//------------------------------------------------------------------------------------------------

CEventObjectActionProcessor::CEventObjectActionProcessor(void)
{
}

CEventObjectActionProcessor::~CEventObjectActionProcessor(void)
{
}


void CEventObjectActionProcessor::ProcessChain( HNODE hNode )
{	
	if( g_pAVATAR == NULL )
		return;

	if( ::intersectNode( g_pAVATAR->GetZMODEL(), hNode ) == 0 )
		return;

	HNODE hUserData = ::getUserData( hNode );
	if ( hUserData ) 
	{
		CGameOBJ* pObj = reinterpret_cast< CGameOBJ* >( hUserData );        

		/// Event object 인지 검사..
		if( pObj->IsA( OBJ_EVENTOBJECT ) )
		{		
			CObjFixedEvent* pEventObj = (CObjFixedEvent*)pObj;

			//----------------------------------------------------------------------------------------
			/// 한번 충돌후 10초동안( 30 FPS 기준 ) 다시 충돌하지 않음
			//----------------------------------------------------------------------------------------
			if( pEventObj->GetCollisionCheckFrame() < ( g_GameDATA.m_dwElapsedGameFrame - 1 ) )
			///if( pEventObj->CanCheckCollision() )
			{			

				/// 트리거가 설정된 오브젝트라면..
				if( pEventObj->GetTriggerName() != NULL )
				{
					::QF_doQuestTrigger( pEventObj->GetTriggerName() );				
					///::setUserData( hNode, (HNODE)0 );
				}

				if( pEventObj->GetScriptName() != NULL )
				{
					pEventObj->ExecEventScript( pEventObj->GetCurrentState() + 1, false, true );
				}

				pEventObj->SetCollisionCheckFrame( g_GameDATA.m_dwElapsedGameFrame + 300 );
			}
			///pEventObj->SetCheckCollisionFlag( false );
			
		}
	}
	
	if( m_pNextActionChain )
		m_pNextActionChain->ProcessChain( hNode );
}
