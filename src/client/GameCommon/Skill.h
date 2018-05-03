/*
	$Header: /Client/GameCommon/Skill.h 21    05-07-29 5:06p Navy $
*/
#ifndef	__SKILL_H
#define	__SKILL_H


#include <list>
#include "Bullet.h"
#include "Common\\IO_Skill.h"
#include "tgamectrl/tobject.h"

///
///	Skill Apply type
///
enum enumSKILL_APPLY_TYPE
{
	SKILL_APPLY_ALL = 0,
	SKILL_APPLY_PASSIVE,
	SKILL_APPLY_ACTIVE,
	SKILL_APPLY_DULATION,
};



///
/// Skill Target type
///

enum enumSKILL_TARGET_TYPE
{
	SKILL_TARGET_NONE = 0,					///< 타겟 비지정
	SKILL_TARGET_OBJECT,					///< 오브젝트 타겟 설정
	SKILL_TARGET_GROUND,					///< 그라운드 타겟( 마우스 바닥클릭 )
	SKILL_TARGET_PB,						///< 자기자신 사용 or.. 날씨면화?
	SKILL_TARGET_SCREEN,					///< 스크린전체
	SKILL_TARGET_ZONE,						///< 존전체.
	SKILL_TARGET_SERVER,					///< 서버전체
	SKILL_TARGET_MAX,
};


///
/// Skill Action type
///

enum enumSKILL_ACTION_TYPE
{
	SKILL_BASE_ACTION = 1,					///< 기본 명령( 앉기, 서기, 줍기... ), 감정표현
	SKILL_CREATE_WINDOW = 2,				///< 창생성( 제조, 파티, 개인상점... )	
	SKILL_ACTION_IMMEDIATE = 3,				///< 근접 즉시 발동.( 즉시 모션 교체 )
	SKILL_ACTION_ENFORCE_WEAPON = 4,		///< 무기상태 변경( 강화, 효과 연출( 정령탄? ) )
	SKILL_ACTION_ENFORCE_BULLET = 5,		///< 강화총알 변경 발사. ( 아이스 애로우.. 실제 화살이 변하는.. )
	SKILL_ACTION_FIRE_BULLET = 6,			///< 발사.( 파이어볼 )
	SKILL_ACTION_AREA_TARGET = 7,			///< 지역 공격마법( 당근 범위.. )
	SKILL_ACTION_SELF_BOUND_DURATION = 8,	///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 능력치 
	SKILL_ACTION_TARGET_BOUND_DURATION = 9,	///< 타겟에게 발동 지속 마법.( 캐스팅 유 ) 능력치
	SKILL_ACTION_SELF_BOUND = 10,			///< 자신에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
	SKILL_ACTION_TARGET_BOUND = 11,			///< 타겟에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
	SKILL_ACTION_SELF_STATE_DURATION = 12,		///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
	SKILL_ACTION_TARGET_STATE_DURATION = 13,	///< 상대에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
	SKILL_ACTION_SUMMON_PET = 14,				///< 팻 소환 스킬
	SKILL_ACTION_PASSIVE = 15,				///< 패시브..
	SKILL_EMOTION_ACTION = 16,				///< 감정 표현
	SKILL_ACTION_SELF_DAMAGE = 17,			///< 자신주위로 데미지를 주는 스킬..( 이건 셀프로 안되나? ) 물론 범위..
	SKILL_ACTION_WARP = 18,					///< 워프 스킬..( 서버에서 사용 )
	SKILL_ACTION_SELF_AND_TARGET = 19,		///< 타겟에게 영향을 주는동시에 나에게 영향을 줌
	SKILL_ACTION_RESURRECTION	= 20,		///< 부활..
};




class CObjCHAR;
class CCommand;

enum SKILL_SLOT_TYPE
{
	SKILL_SLOT_MAIN = 0,
	SKILL_SLOT_CLAN,
};

//////////////////////////////////////////////////////////////////////////////////////
/// CSkill abstraction class for Skill system
//////////////////////////////////////////////////////////////////////////////////////
class CSkill : public CTObject
{
protected:
	bool							m_bEnable;						///활성화 / 비활성화
	short							m_nClanSkillIndex;				///클랜스킬일경우 SkillIndex, 메인 스킬일경우에는 CSkillSlot에서 가져온다.
	int								m_iSkillLevel;
	int								m_iSkillDelayTimer;			///< 지연시간..

	int								m_iSkillSlotType;
	int								m_iSkillSlot;
	DWORD							m_dwExpiredTime;			

	/// Skill Creation type
	BYTE							m_btSkillTargetType;
	BYTE							m_btSkillActionType;
	BYTE							m_btSkillFilterType;

	

	//----------------------------------------------------------------------------------------------------	
	/// @brief Skill Command object
	//----------------------------------------------------------------------------------------------------
	CCommand*						m_pCommand;



	//----------------------------------------------------------------------------------------------------
	/// For skill Time
	//----------------------------------------------------------------------------------------------------
	int								m_iElapsedTime;

public:
	CSkill();
	~CSkill();


	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief Skill Action interface
	//----------------------------------------------------------------------------------------------------
	virtual void	Action( CObjCHAR* pAttacker, CObjCHAR* pTarget ) = 0{ *(int*)0 = 10; };
	virtual void	DrawIcon( int iX, int iY );
	virtual int		Process();
	virtual bool	Execute();
	virtual int		SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) = 0{ *(int*)0 = 10; };	

	

	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief get/set Skill slot
	//----------------------------------------------------------------------------------------------------
	void			SetSkillSlot( int iSlot );	
	int				GetSkillSlot( ){ return m_iSkillSlot; }


	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief Skill index method
	//----------------------------------------------------------------------------------------------------
	short			GetSkillIndex();

	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief Skill Level.. 현재는 스킬이 완전히 바뀌는관계로 사용하지 않는다.
	//----------------------------------------------------------------------------------------------------
	short			GetSkillLevel();

	
	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief Skill timer method
	//----------------------------------------------------------------------------------------------------
	void			ResetSkillTimer(){ m_iSkillDelayTimer = 0;}
	void			SetSkillDelayTime( int iTime );	
	int				GetSkillDelayTime(){ return m_iSkillDelayTimer; }


	//----------------------------------------------------------------------------------------------------
	/// @param
	/// @brief 스킬타입은 ( target type, action type, filter type 의 조합으로 생성 )
	//----------------------------------------------------------------------------------------------------
	bool			Create( int iSkillType, short nSkillIdx, short nSkillType, short nSkillLevel = 0 );


	//----------------------------------------------------------------------------------------------------
	/// @brief 클랜 스킬관련해서 제한걸린 스킬을 위한 Methods
	//----------------------------------------------------------------------------------------------------
	bool			HasExpiredTime();
	DWORD			GetExpiredTime();
	void			SetExpiredTime( DWORD dwAbsSec );
	//	bool			HasLimitUseCount();
//	bool			HasLimitClass();


//	int				GetLimitUseCount();
//	int				GetLimitClass();

	void			SetEnable();
	void			SetDisable();
	bool			IsEnable();
};







//////////////////////////////////////////////////////////////////////////////////////
/// Base Action Skill
//////////////////////////////////////////////////////////////////////////////////////
class CBaseActionSkill : public CSkill
{
private:
	

public:
	CBaseActionSkill();
	~CBaseActionSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};






//////////////////////////////////////////////////////////////////////////////////////
/// Action create window Skill
//////////////////////////////////////////////////////////////////////////////////////
class CCreateWindowActionSkill : public CSkill
{
private:

public:
	CCreateWindowActionSkill();
	~CCreateWindowActionSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};







//////////////////////////////////////////////////////////////////////////////////////
/// Action immediate Skill
//////////////////////////////////////////////////////////////////////////////////////
class CImmediateActionSkill : public CSkill
{
private:
	
public:
	CImmediateActionSkill();
	~CImmediateActionSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};






//////////////////////////////////////////////////////////////////////////////////////
/// Fire Skill
//////////////////////////////////////////////////////////////////////////////////////
class CFireSkill : public CSkill
{
private:
	/// Bullet list
	/// 어짜피 Bullet List 에 등록 될꺼기에 별로 필요 없을것 같군..
	std::list< CBullet* >			m_BulletList;

public:
	CFireSkill();
	~CFireSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};





//////////////////////////////////////////////////////////////////////////////////////
/// Fire to position Skill
//////////////////////////////////////////////////////////////////////////////////////
class CFireToPositionSkill : public CSkill
{
private:
	/// Bullet list
	/// 어짜피 Bullet List 에 등록 될꺼기에 별로 필요 없을것 같군..
	std::list< CBullet* >			m_BulletList;

public:
	CFireToPositionSkill();
	~CFireToPositionSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};







//////////////////////////////////////////////////////////////////////////////////////
/// Self Bound Skill
/// SKILL_ACTION_SELF_BOUND_DURATION = 8,	///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 능력치 
/// SKILL_ACTION_SELF_BOUND = 10,			///< 자신에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
/// SKILL_ACTION_SELF_STATE_DURATION = 12,		///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
//////////////////////////////////////////////////////////////////////////////////////
class CSelfBoundSkill : public CSkill
{
private:

public:
	CSelfBoundSkill();
	~CSelfBoundSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};



//////////////////////////////////////////////////////////////////////////////////////
/// Self Bound Skill
/// SKILL_ACTION_TARGET_BOUND_DURATION = 9,	///< 타겟에게 발동 지속 마법.( 캐스팅 유 ) 능력치
/// SKILL_ACTION_TARGET_BOUND = 11,			///< 타겟에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
/// SKILL_ACTION_TARGET_STATE_DURATION = 13,	///< 상대에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
//////////////////////////////////////////////////////////////////////////////////////
class CTargetBoundSkill : public CSkill
{
private:

public:
	CTargetBoundSkill();
	~CTargetBoundSkill();

	/*
	*	Skill Action interface
	*/
	virtual void Action( CObjCHAR* pAttacker, CObjCHAR* pTarget );
	virtual void DrawIcon( int iX, int iY );
	virtual int Process();
	virtual int SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
};







//////////////////////////////////////////////////////////////////////////////////////
///
/// Skill manager
///
//////////////////////////////////////////////////////////////////////////////////////
class CSkillManager
{
private:
	std::list< CSkill* >			m_SkillList;


public:
	CSkillManager();
	~CSkillManager();


	//---------------------------------------------------------------------------------
	/// static function

	static int	GetSkillTargetType( int iSkillIndex );
	static int	GetSkillActionType( int iSkillIndex ){ return SKILL_TYPE( iSkillIndex ); }
	static int	GetSkillFilterType( int iSkillIndex ){ return SKILL_CLASS_FILTER( iSkillIndex ); }

	
	/// 스킬타입은 ( target type, action type, filter type 의 조합으로 생성 )
	CSkill*		CreateNewSkill( int iSkillSlotType, short nSkillIdx, char cSkillLevel = 0 );


	/// 일단 가자.. XP 여 영원하라.. !!!
	static bool		ActionSkill( short nSkillIdx, CObjCHAR* pSrc, CObjCHAR* pTarget, D3DXVECTOR3 PosTo = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), bool bTargetPos = false );

	
	/// 스킬 발동 조건 체크..
	static bool		CheckConditionForFireSkill( int iSkillIDX, int iCurrentTarget );
	/// 스킬 발동..
	static bool		FireSkill( int iSkillSlotIDX, int iTargetObj, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	/// 현재 활성화된 스킬 발동..
	static bool		ActionActiveSkill( int iTargetObj, D3DXVECTOR3 &PosTO = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );



	/// 소모된 속성
	static void UpdateUseProperty( CObjAI* pObjAI, int iSkillIdx );	


	///
	///	스킬 사용가능 여부 체크..
	///
	
	static bool	CheckCastingCondition( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX );

	/// 타이머 체크( 연속 사용여부관련 )..
	static bool CheckSkillTimer( int iSkillIDX );
	/// 타겟의 유효성 검사..
	static bool CheckCastingTarget( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX );

	/// 타겟 필터링 검사
	static bool CheckCastingTargetFilter( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX );

	/// 소환몹 capacity 검사
	static bool CheckSummonCapacity( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX );

	/// 필요 능력치 체크..
	static bool CheckNeedProperty( int iSkillIDX, CObjCHAR* pCaster );
	/// 필요장비 체크..
	static bool CheckNeedWeapon( int iSkillIDX, CObjCHAR* pCaster );
	/// 중복적용 가능 체크...
	static bool CheckDuplicatable( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX );
	
	
	///
	///	스킬 습득가능 여부 체크..
	///

	/// 스킬을 배우기에 유효한 직업인가?
	static bool CheckJobForStudy( int iSkillIDX );
	/// 스킬을 배우기 위한 선행 스킬이 만족하는가?
	static bool CheckProSkillForStudy( int iSkillIDX );
	/// 스킬을 배우기 위한 기본 능력치를 만족하는가?
	static bool CheckPropertyForStudy( int iSkillIDX );


	///
	///	스킬 사용을 위한 타겟 구하기
	/// 일반 스킬일경우 hp체크를 하지만 리졀렉션같은 스킬의 경우 hp 체크를 하지 않는다.
	///
	static CObjCHAR* GetSkillTarget( int iServerObjIDX, int iSkillIDX );

};

extern CSkillManager	g_SkillManager;

#endif
