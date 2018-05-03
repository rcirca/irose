#ifndef _CENDURANCEPROPERTY_
#define _CENDURANCEPROPERTY_

#include <list>
#include <map>
#include <algorithm>
#include "../Common/IO_STB.h"

class CObjCHAR;
class CEffect;

enum ENDURANCE_ENTITY_TYPE
{
	ENDURANCE_TYPE_SKILL = 0,
	ENDURANCE_TYPE_USEITEM,
};



#define MAX_ENDURAMCE_ENTITY	5

class CEnduranceProperty;


typedef std::vector< CEnduranceProperty* >					ENDURANCE_LIST;
typedef std::vector< CEnduranceProperty* >::iterator		ENDURANCE_LIST_ITOR;
///
/// Endurance entity pack.
/// 지속적으로 수치가 변하는것들은 이 클래스의 proc 에서 처리하고
/// 기타 화면 출력용이나 표시용은 내부 엔티티 리스트를 이용한다.

class CEndurancePack
{
private:
	/// 현재 설정된 상태 플래그..
	DWORD									m_dwStateFlag;
	/// 현재 변경 추가되어야할 값들을 보관..( key 는 상태값 다. )
	int										m_CurrentStateValue[ ING_MAX_STATUS ];

	DWORD									m_dwUpdateCheckTime;
	DWORD									m_dwElapsedUpdateTime;


	/// 상태변경 타입만큼 유지한다.
	CEnduranceProperty*						m_EntityList[ ING_MAX_STATUS ];	
	CObjCHAR*								m_pObjCHAR;



public:
	/// 상태 플래그값으로 상태타입을 찾아내는 테이블
	static std::map< int , int >			m_StateFlagTable;	
	static std::map< int , int >			m_StateSTBIDXTable;	

	void									MakeStateFlagTable();
	void									MakeStateSTBIDXTable();


private:
	CEndurancePack();


public:	
	CEndurancePack( CObjCHAR* pObjCHAR );
	~CEndurancePack();


	void					ResetTimer();



	void					SetObjCHAR( CObjCHAR* pObjCHAR ){ m_pObjCHAR = pObjCHAR; }

	int						GetEntityCount(){ return ING_MAX_STATUS; }
	CEnduranceProperty*		GetEntity( int Index );
	CEnduranceProperty*		GetEntityByStateType( int iStateType );


	/// add new endurance entity.
	bool					AddEntity( int iEntityIDX, int iStateNO, int iEnduranceTime, int iEntityType );
    void					ClearEntityPack();

	void					Update();
	bool					Proc( CEnduranceProperty* pEntity );
	void					DeleteExpiredEntity();
	void					Draw();

	/// 해당 상태에 대한 엔티티를 제거한다.
	void					DeleteEntityByStateType( int iStateType );

	/// 상대값 관련함수
	void					SetStateFlag( DWORD dwState ){ m_dwStateFlag = dwState; }
	void					AddStateFlag( DWORD dwState ){ m_dwStateFlag |= dwState; }
	DWORD					GetStateFlag(){ return m_dwStateFlag; }
	void					SetStateValue( int iType, int iValue );//{ m_CurrentStateValueMap[ iType ] = iValue; }
	int						GetStateValue( int iType )
	{
		/*if( std::find( m_CurrentStateValueMap.begin(), m_CurrentStateValueMap.end(), iType ) == m_CurrentStateValueMap.end() )
			return 0;*/

		return m_CurrentStateValue[ iType ]; 
	}
	void					ClearState(){ m_dwStateFlag = 0; memset( m_CurrentStateValue, 0, sizeof( m_CurrentStateValue ) ); }

	/// 상태 해제
	void					DeleteEnduranceState( DWORD dwNewState );

	/// 유리상태 해지, 불리상태 해지 등..
	int						ProcFlushStateSkill( int iStateTableIndex );
	void					ProcFlushStateSkillByIngState( int iStateType );

	
	//--------------------------------------------------------------------------------------
	/// 특정 상황에서 자동으로 풀려야 하는것들
	//--------------------------------------------------------------------------------------

	/// 카트탐승시 풀려야 될 스킬
	void					ClearStateByDriveCart();

	/// 장비교체시 해당 장비에 걸려있던 상태 제거..
	void					ClearRWeaponSkillEffect();
	void					ClearLWeaponSkillEffect();
	
	/// 맞았을때 자동으로 풀려야하는 것들
	void					ClearStateByHitted();

	/// 공격시 자동으로 풀려야하는 스킬들
	void					ClearStateByAttack();

	/// 이동시 자동으로 풀려야하는 스킬들
	void					ClearStateByMove();



	/// Check visibility
	/// visibility 관련 스킬이 걸렸을경우.. 업데이트 해야될시기에는( 무기를 교체한다던지 ) 비지빌리티 업데이트
	void					UpdateVisibility();


public:
	/// 출력 아이콘 개수..
	static int								m_iDrawIconCnt;

};





///
/// Base class of endurance property entity
///

class CEnduranceProperty
{
protected:
	CObjCHAR*		m_pObjCHAR;
	CEffect*		m_pEffect;

	int				m_iEndityIdx;
	int				m_iEnduranceTime;
	int				m_iEnduranceEndityType;
	bool			m_bExpired;

	DWORD			m_iStartTime;

	/// 상태유형..
	int				m_iStatusSTBNO;
	
public:
	CEnduranceProperty();
	virtual ~CEnduranceProperty();

	virtual bool	CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iEntityIdx, int iStatusSTBNO, int iEnduranceTime ) = 0;

	virtual void	SetExpire()			{ m_iEnduranceTime = 0; m_bExpired = true; }
	virtual bool	IsExpired()			{ return m_bExpired; }
		
	virtual int		GetEntityIDX()		{ return m_iEndityIdx; };
	virtual int		GetEnduranceTime()	{ return m_iEnduranceTime; }
	virtual int		GeteEntityType()	{ return m_iEnduranceEndityType; };
	virtual int		GetStartTime()		{ return m_iStartTime; }
	virtual int		GetStateSTBNO()		{ return m_iStatusSTBNO; }

	virtual bool	CreateEffect( CObjCHAR* pObjCHAR, int iEffectIDX ) = 0{ *(int*)0 = 10; };
	virtual void	DeleteEffect() = 0{ *(int*)0 = 10; };

	virtual int		GetIconNO() = 0{ *(int*)0 = 10; };

	/// if return value is false, this endurance entity is expired.
	virtual bool	Proc( CObjCHAR* pObjCHAR ) = 0{ *(int*)0 = 10; };
};


///
/// Endurance skill class
///

class CEnduranceSkill : public CEnduranceProperty
{
private:
		
public:	
	CEnduranceSkill();
	//CEnduranceSkill( CObjCHAR* pObjCHAR, int iSkillIdx, int iEnduranceTime );
	~CEnduranceSkill();	
	
	virtual bool	CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iSkillIdx, int iStateNO, int iEnduranceTime );
	virtual bool	CreateEffect( CObjCHAR* pObjCHAR, int iEffectIDX );
	virtual void	DeleteEffect();

	virtual int		GetIconNO();

	bool			Proc( CObjCHAR* pObjCHAR );
};


///
/// Endurance item class
///

class CEnduranceItem : public CEnduranceProperty
{
private:	
	/// 현재까지 적용된 수치..
	int				m_iTotalAppliedProperty[ STATE_APPLY_ABILITY_CNT ];
	
public:
	CEnduranceItem();
	//CEnduranceItem( CObjCHAR* pObjCHAT, int iUseItemIdx, int iEnduranceTime );
	~CEnduranceItem();

	virtual bool	CreateEnduranceEntity( CObjCHAR* pObjCHAR, int iUseItemIdx, int iStateNO, int iEnduranceTime );
	virtual bool	CreateEffect( CObjCHAR* pObjCHAR, int iEffectIDX );
	virtual void	DeleteEffect();

	virtual int		GetIconNO();

	bool			Proc( CObjCHAR* pObjCHAR );
};



#endif // _CENDURANCEPROPERTY_	