#ifndef _OBJFIXEDEVENT_
#define _OBJFIXEDEVENT_

#include "cobjGROUND.h"


enum EVENT_OBJ_STATE
{
	EVENT_OBJ_INIT_STATE = 0,
	EVENT_OBJ_CLOSE_STATE = 1,
	EVENT_OBJ_OPEN_STATE = 1,
};


//----------------------------------------------------------------------------------------------------
/// class CObjFixedEvent
/// 이벤트를 가진 정적 오브젝트
//----------------------------------------------------------------------------------------------------

class CObjFixedEvent :	public CObjFIXED
{
private:
	/// ExecEventScript 호출과 동시에 세팅됨.
	int				m_iCurrentState;


	int				m_iUniqueEventID;
	std::string		m_strTriggerName;
	std::string		m_strScriptName;

	/// 모션을 세팅하기위한 더미 노드들..
	HNODE			m_PositionDummy;
	HNODE			m_AnimatibleDummy;


	/// 한번 충돌해서 이벤트를 발생시킨 오브젝트는 다시 충돌하지 않음 -> 충돌함이 되기전에는 이벤트를 다시 발생시키지 않는다.
	DWORD			m_dwCollisionCheckFrame;

	bool			m_bCanCheckCollision;


public:
	CObjFixedEvent(void);
	~CObjFixedEvent(void);

	int				GetCurrentState(){ return m_iCurrentState; }




	//////////////////////////////////////////////////////////////////////////////////////////
	/// < Inherited from CGameObj 

	/*override*/ int					Get_TYPE()		{	return OBJ_EVENTOBJECT;	}	
	
	/// < 
	//////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////
	/// < Inherited from CObjFIXED 

	/*override*/char*				Make_ZNAME(int iCreateOrder, short nPartIdx)	{	return CStr::Printf (NAME_TREEOBJ, iCreateOrder, nPartIdx);	}	
	
	/// < End 
	//////////////////////////////////////////////////////////////////////////////////////////	


	void			SetUniqueEventID( int iID ){ m_iUniqueEventID = iID; }
	void			SetTriggerName( char* pFileName ){ m_strTriggerName = pFileName; }
	void			SetScriptName( char* pFileName ){ m_strScriptName = pFileName; }

	int				GetUniqueEventID(){ return m_iUniqueEventID; }
	const char*		GetTriggerName(){ return m_strTriggerName.c_str(); }			/// 트리거 이름이 들어감.
	const char*		GetScriptName(){ return m_strScriptName.c_str(); }					/// 실제로 스크립터 이름이 들어간다.


	//-------------------------------------------------------------------------------------------------
	/// 이벤트 오브젝트는 모션을 붙이거나 등을 위해서 추가적인 노드가 필요하다.	
	//-------------------------------------------------------------------------------------------------
	HNODE			GetPositionNode(){ return m_PositionDummy; }
	HNODE			GetAnimatibleNode(){ return m_AnimatibleDummy; }

	/*override*/	void UnlinkVisibleWorld (void);	
	/*override*/	void LinkToModel( HNODE hModel );	
	/*override*/	void InsertToScene (void);
	/*override*/	void RemoveFromScene (bool bIncludeEFFECT);

	/*override*/	void SetPOSITION (D3DVECTOR &Position);
	/*override*/	bool Create (CMODEL<CFixedPART> *pMODEL, D3DVECTOR &Position, bool bCreateEffect=false);
	/*override*/	void Delete ();


	//-------------------------------------------------------------------------------------------------
	/// 상태에 따른 이벤트 발동
	//------------------------------------------------------------------------------------------------
	void			ExecEventScript( int iCurrentState, bool bJustEnd = false, bool bCollisionEvent = false, bool bInsertToScene = false );

	void			ExecTrigger();


	/// 현재 충돌을 체크할수 있는 상태인가?
	/// 한번 충돌후 특정한 상황이( 일정거리가 떨어진다던지 아니면 기타등 ) 일어나기 전에는 disable 된다.
	bool			CanCheckCollision(){ return m_bCanCheckCollision; }
	void			SetCheckCollisionFlag( bool bCanCheck ) { m_bCanCheckCollision = bCanCheck; }
	void			SetCollisionCheckFrame( DWORD dwCheckFrame ){ m_dwCollisionCheckFrame = dwCheckFrame; }
	DWORD			GetCollisionCheckFrame(){ return m_dwCollisionCheckFrame; }
};


#endif //_OBJFIXEDEVENT_