#include "stdafx.h"
#include ".\objfixedevent.h"
#include "System/SystemProcScript.h"


CObjFixedEvent::CObjFixedEvent(void)
{
	m_iCurrentState		= EVENT_OBJ_INIT_STATE;

	m_iUniqueEventID	= 0;	

	m_PositionDummy		= 0;
	m_AnimatibleDummy	= 0;

	m_dwCollisionCheckFrame = 0;

	m_bCanCheckCollision = false;
}

CObjFixedEvent::~CObjFixedEvent(void)
{
}

//-------------------------------------------------------------------------------------------------
/// 이벤트 오브젝트는 추가 더미때문에 따로 처리
//-------------------------------------------------------------------------------------------------
void  CObjFixedEvent::UnlinkVisibleWorld (void)
{
	assert( 0 && "CObjFixedEvent::UnlinkVisibleWorld" );
}

//-------------------------------------------------------------------------------------------------
/// 이벤트 오브젝트는 추가 더미때문에 따로 처리
//-------------------------------------------------------------------------------------------------
void CObjFixedEvent::LinkToModel( HNODE hModel )
{
	assert( 0 && "CObjFixedEvent::LinkToModel" );
}

//-------------------------------------------------------------------------------------------------
/// 이벤트 오브젝트는 추가 더미때문에 따로 처리
//-------------------------------------------------------------------------------------------------
void CObjFixedEvent::InsertToScene (void)
{
	if ( m_bIsVisible || !m_pMODEL || !m_PositionDummy )
		return;

	m_bIsVisible = true;
	
	::insertToScene( m_PositionDummy );		

	ExecEventScript( this->m_iCurrentState, 1, false, true );
}

//-------------------------------------------------------------------------------------------------
/// 이벤트 오브젝트는 추가 더미때문에 따로 처리
//-------------------------------------------------------------------------------------------------
void CObjFixedEvent::RemoveFromScene (bool bIncludeEFFECT)
{
	if ( !m_bIsVisible || !m_PositionDummy )
		return;

	m_bIsVisible = false;
		
	::removeFromScene( m_PositionDummy );		
}

//------------------------------------------------------------------------------------------
/// 추가적인 노드들때문에 약간의 조절이 필요
//------------------------------------------------------------------------------------------
void CObjFixedEvent::SetPOSITION (D3DVECTOR &Position)
{	
	if( m_PositionDummy == NULL )
		return;


	::setPosition  ( m_PositionDummy, Position.x, Position.y, Position.z );
	m_PosCUR  = Position;
}

//------------------------------------------------------------------------------------------
/// 이벤트 오브젝트는 모션을 붙이거나 등을 위해서 추가적인 노드가 필요하다.
//------------------------------------------------------------------------------------------
bool CObjFixedEvent::Create (CMODEL<CFixedPART> *pMODEL, D3DVECTOR &Position, bool bCreateEffect )
{
	/// 더미 포지션이 있기때문에 오브젝트 자체는 원점에서 생성하고
	/// 실제 좌표이동은 더미포지션 오브젝트에 적용한다. ( SetPOSITION 참고 현재 위치( m_PosCUR ) 을 그쪽에서 등록 )
	D3DVECTOR vPos = { 0.0f, 0.0f, 0.0f};
	if( !CObjFIXED::Create( pMODEL, vPos, bCreateEffect ) )
		return false;

	if( m_PositionDummy )
		::unloadVisible( m_PositionDummy );

	std::string strName( "FixedEvent_" );
	char *pName = Make_ZNAME( this->m_dwCreateOrder, 100 );
	std::string strTempName( strName + std::string( pName ) );
	/// for position
	std::string strPosName = strTempName + "PosDummy";
	m_PositionDummy = ::loadVisible ( strPosName.c_str(), NULL, NULL, NULL ); 
	if( !m_PositionDummy )
	{
		assert( 0 && "CObjFixedEvent::Create[ loadVisible failed ]" );
		return false;
	}
	::setCollisionLevel( m_PositionDummy, 0 );

	if( m_AnimatibleDummy )
		::unloadVisible( m_AnimatibleDummy );

	std::string strRotName = strTempName + "AniDummy";
	/// for animation
	m_AnimatibleDummy = ::loadAnimatable ( strRotName.c_str(), 0, 0, 0, 0 ); 
	if( !m_AnimatibleDummy )
	{
		assert( 0 && "CObjFixedEvent::Create[ loadAnimatable failed ]" );
		return false;
	}

	::linkNode( m_AnimatibleDummy, this->GetRootZNODE() );
	::linkNode( m_PositionDummy, m_AnimatibleDummy );

	SetPOSITION( Position );

	return true;
}

void CObjFixedEvent::Delete ()
{
	CObjFIXED::Delete();

	if( m_PositionDummy )
		::unloadVisible( m_PositionDummy );

	if( m_AnimatibleDummy )
		::unloadVisible( m_AnimatibleDummy );
}

//-------------------------------------------------------------------------------------------------
/// 상태에 따른 이벤트 발동
//------------------------------------------------------------------------------------------------
void CObjFixedEvent::ExecEventScript( int iCurrentState, bool bJustEnd, bool bCollisionEvent, bool bInsertToScene )
{
	/// 충돌에 의한 스크립트 실행이라면
	if( bCollisionEvent )
	{
		/// 상태가 변경되지 않았다면 그냥 리턴..
		if( m_iCurrentState == iCurrentState )
			return;
	}

	m_iCurrentState = iCurrentState;

	if( m_strScriptName.c_str() != NULL && m_strScriptName != "EMPTY" )
	{
		CSystemProcScript::GetSingleton().CallLuaFunction( m_strScriptName.c_str(), 
															ZZ_PARAM_INT, m_iUniqueEventID, 
															ZZ_PARAM_INT, m_iCurrentState, 
															ZZ_PARAM_INT, (int)bJustEnd,
															ZZ_PARAM_INT, (int)bInsertToScene,
															ZZ_PARAM_END );
	}
}

//-------------------------------------------------------------------------------------------------
/// 트리거 발동
//------------------------------------------------------------------------------------------------
void CObjFixedEvent::ExecTrigger()
{

}