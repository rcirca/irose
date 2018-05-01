/*
	$Header: /Client/CObjUSER.h 151   05-09-01 6:32p Raning $
*/
#ifndef	__COBJUSER_H
#define	__COBJUSER_H
#include "CObjCHAR.h"
#include "Common\CInventory.h"
#include "Common\CUserDATA.h"
#include "Interface\\TypeResource.h"
#include "Interface/SlotContainer/CSkillSlot.h"
#include "Interface/SlotContainer/CItemSlot.h"
#include "Interface/SlotContainer/HotIconSlot.h"
#include "CObjCART.h"
#include <list>
#include <algorithm>


struct SummonMobInfo
{
	int		iIndex;
	int		iCapacity;

	int		iSkillIDX;

	SummonMobInfo(){ iIndex =0; iCapacity = 0; iSkillIDX = 0; }
};



///
///	User class( for main user? )
///

class CObjUSER : public CObjAVT, public CUserDATA 
{
private:
	CObjUSER ();
	virtual ~CObjUSER ();

	static CObjUSER	*m_pInstance;


public :
	static CObjUSER* Instance ();
	void Destroy ();









public:

	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameOBJ virtual functions
	/// <

	/*override*/virtual int			Get_TYPE()		{	return OBJ_USER;		}	

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CAI_OBJ virtual functions
	/// <

	/*override*/int					Get_HP ()				{	return	GetCur_HP ();			}	// 생명력

	/*override*/int					Get_LEVEL ()			{	return	GetCur_LEVEL ();		}
	/*override*/int					Get_ATK ()				{	return	GetCur_ATK ();			}	// 공격력
	/*override*/int					Get_DEF ()				{	return	GetCur_DEF ();			}	// 방어력
	/*override*/int					Get_RES ()				{	return	GetCur_RES ();			}	// 항마력
	/*override*/int					Get_HIT ()				{	return	GetCur_HIT ();			}
	/*override*/int					Get_CHARM ()			{	return	GetCur_CHARM ();		}	// 매력
	/*override*/int					Get_AVOID ()			{	return	GetCur_AVOID ();		}	// 회피력
	/*override*/int					Get_SENSE ()			{	return	GetCur_SENSE ();		}	// 감각
	/*override*/int					Get_CRITICAL()			{	return	GetCur_CRITICAL ();		}

	/*override*/int					Get_TeamNO()			{	return	CObjAVT::Get_TeamNO();		}

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/int					Get_MP ()				{	return	GetCur_MP ();			}
	/*override*/void				Set_HP (int iHP);
	/*override*/void				Set_MP (int iMP);

	/*override*/int					Get_MaxHP ()			{	return	GetCur_MaxHP ();			}
	/*override*/int					Get_MaxMP ()			{	return	GetCur_MaxMP ();			}
	
	/*override*/int					Get_CON ()				{	return	GetCur_CON ();			}	// 집중

	/*override*/int					GetAdd_RecoverHP()		{	return this->m_btRecoverHP;		}
	/*override*/int					GetAdd_RecoverMP()		{	return this->m_btRecoverMP;		}

	virtual short					GetPsv_ATKSPEED()		{	return this->GetPassiveSkillAttackSpeed();	}
	virtual short					GetPsv_ATKSPEED (float fCurSpeed, short nRightWeaponItemNo) ;

	virtual int						GetOri_MaxHP();
	virtual int						GetOri_MaxMP();
	/*override*/void				RideCartToggle( bool bRide );

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjCHAR virtual functions
	/// <

	/*override*/int					Get_INT ()				{	return	GetCur_INT ();			}	// 지력

	/*override*/int					Add_HP (int iValue)		{	return  AddCur_HP(iValue);		}
	/*override*/int					Add_MP (int iValue)		{	return  AddCur_MP(iValue);		}

	/*override*/int					Sub_HP (int iValue)		{	return  SubCur_HP(iValue);		}
	/*override*/int					Sub_MP (int iValue)		{	return  SubCur_MP(iValue);		}


	/*override*/void				Add_EXP (short nExp);

	/*override*/int					Get_BulletNO ();


	/// 지속형의 변경수치 적용을 위해서 현재 적용되어있는 능력수치( 패시브 스킬 포함 )
	/*override*/int					Get_DefaultAbilityValue( int iType ){ return GetCur_AbilityValue( iType ); }


	/*override*/int					Proc (void);

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAVT virtual functions
	/// <

	/*override*/void				Set_SEX ( BYTE btRace )		{	CObjAVT::Set_SEX( btRace );		}
	/*override*/void				Set_CON (int iValue)		{	SetDef_CON (iValue);			}

	/*override*/void				Update_SPEED ();
	/*override*/void				Update (bool bUpdateBONE=false);
	/*override*/short				GetStamina();
	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////	
	


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CUserDATA virtual functions
	/// <
	virtual BYTE  GetCur_MOVE_MODE ();



private:
	/*override*/ void				SetCur_PartITEM	(short nPartIdx, tagITEM &sITEM)	{	this->SetPartITEM(nPartIdx, sITEM);		}
	/*override*/ short				GetCur_PartITEM(short nPartIdx)						{	return GetPartITEM(nPartIdx);			}

public:

	/// 지속 스킬에 의해 보정된 값을 리턴할 함수...
	/*override*/ short				GetCur_RACE(){ return this->m_btRace; };
	/*override*/ short				GetCur_ATK_SPD ()		{ return Get_nAttackSPEED();											}
	/*override*/ float				GetCur_MOVE_SPEED ()	{ return Get_DefaultSPEED();	}
	/*override*/ int				GetCur_ATK();
	/*override*/ int				GetCur_DEF();
	/*override*/ int				GetCur_RES()			{ return GetDef_RES() + m_EndurancePack.GetStateValue( ING_INC_RES ) - m_EndurancePack.GetStateValue( ING_DEC_RES );		}
	/*override*/ int				GetCur_HIT()			{ return GetDef_HIT() + m_EndurancePack.GetStateValue( ING_INC_HIT ) - m_EndurancePack.GetStateValue( ING_DEC_HIT );		}
	/*override*/ int				GetCur_AVOID()			{ return GetDef_AVOID() + m_EndurancePack.GetStateValue( ING_INC_AVOID ) - m_EndurancePack.GetStateValue( ING_DEC_AVOID );		}
	/*override*/ int				GetCur_CRITICAL();

	/*ov...   */ void				UpdateCur_Ability()		{	this->UpdateAbility();		}

	/*override*/ int				GetCur_MaxHP ();
	/*override*/ int				GetCur_MaxMP ();
	/*override*/ int				GetCur_MaxSTAMINA () { return 5000; }

	/*override*/ DWORD				GetCur_IngStatusFLAG()				{	return this->GetIngDurationStateFLAG();	}

	/*override*/int					GetCur_R_WEAPON ()					{	return this->Get_R_WEAPON();				}
	/*override*/int					GetCur_L_WEAPON ()					{	return this->Get_L_WEAPON();				}	
		
	/*override*/bool				Quest_IsPartyLEADER();
	/*override*/int					Quest_GetPartyLEVEL();
	/*override*/int					Quest_GetZoneNO();
	/*override*/int					Quest_DistanceFrom(int iX, int iY, int iZ);
	/*override*/int					Quest_PartyMemberCNT();

	/*override*/void				SetCur_TeamNO(short nTeamNO ){ CObjAVT::SetTeamInfo( nTeamNO ); }

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	short		GetCur_STAMINA();
	void		SetCur_STAMINA( short nStamina );
	//-------------------------------------------------------------------------------------
private:
	std::list< SummonMobInfo >		m_SummonedMobList;
	int								m_iSummonMobCapacity;

public:
	int								GetCur_SummonMaxCapacity();
	int								GetCur_SummonUsedCapacity(){ return m_iSummonMobCapacity; }
	int								GetCur_SummonCNT(){ return m_SummonedMobList.size(); };			/// 현재 존에 소환되어 있는 NPC갯수(죽을때, 존 워프시 0으로 리셋함)
	void							ClearSummonedMob(){ m_SummonedMobList.clear(); m_iSummonMobCapacity = 0; }
	void							AddSummonedMob( int iIndex, int iSkillIDX );
	void							SubSummonedMob( int iIndex );
	bool							GetSummonedMobInfo( int iIndex, SummonMobInfo& mobInfo );
	//-------------------------------------------------------------------------------------


	
	//-------------------------------------------------------------------------------------------------
	/// Get/Set property for user property
	//-------------------------------------------------------------------------------------------------	
	char							Get_SEX ()					{	return (this->m_bIsFemale) ? 1:0;	}

//	short							Get_WEIGHT ()				{	return	GetCur_WEIGHT ();			}

	void							Set_FAME (int iValue)		{	SetCur_FAME (iValue);				}
	void							Set_UNION (int iValue)		{	SetCur_UNION (iValue);				}
	void							Set_RANK (int iValue)		{	SetCur_RANK (iValue);				}
	void							Set_JOB (int iValue);

	void							Set_LEVEL (int iValue)		{	SetCur_LEVEL (iValue);				}
	void							Set_EXP (__int64 lValue)		{	SetCur_EXP (lValue);				}
	void							Set_BonusPOINT(short nValue)	{	SetCur_BonusPOINT (nValue);		}
	void							Set_SkillPOINT(short nValue)	{	SetCur_SkillPOINT (nValue);		}


	void							Set_STR (int iValue)		{	SetDef_STR (iValue);				}
	void							Set_DEX (int iValue)		{	SetDef_DEX (iValue);				}
	void							Set_INT (int iValue)		{	SetDef_INT (iValue);				}	
	void							Set_CHARM (int iValue)		{	SetDef_CHARM (iValue);				}
	void							Set_SENSE (int iValue)		{	SetDef_SENSE (iValue);				}

	int								Get_BIRTH()					{	return  GetCur_BIRTH();				}	// 탄생석
	int								Get_RANK()					{	return	GetCur_RANK();				}	// 계급
	int								Get_UNION ()				{	return	GetCur_UNION ();			}	// 소속
	int								Get_FAME ()					{	return	GetCur_FAME ();				}	// 명성
	int								Get_JOB ()					{	return	GetCur_JOB ();				}	// 직업

	int								Get_EXP ()					{	return	GetCur_EXP ();				}
	int								Get_BonusPOINT ()			{	return	GetCur_BonusPOINT ();		}
	int								Get_SkillPOINT ()			{	return	GetCur_SkillPOINT ();		}

	BYTE							Get_HeadSIZE()				{	return	GetCur_HeadSIZE();			}
	BYTE							Get_BodySIZE()				{	return	GetCur_BodySIZE();			}

	int								Get_STR ()					{	return	GetCur_STR ();				}	// 근력
	int								Get_DEX ()					{	return	GetCur_DEX ();				}	// 민첩
		
	short							Get_MaxWEIGHT ()			{	return	GetCur_MaxWEIGHT ();	}
	int								Get_MAG ()					{	return	GetCur_MAG ();				}	// 마법력

	void							Set_MONEY (__int64 iMoney)	{	SetCur_MONEY(iMoney);				}
	__int64							Get_MONEY ()				{	return	GetCur_MONEY();				}	// 돈
	void							Add_MONEY (int iMoney)		{	Add_CurMONEY(iMoney);				}

	short							Add_ITEM(short nListRealNO, tagITEM &sITEM);
	void							Sub_ITEM(short nListRealNO, tagITEM &sITEM);

	short							Add_CatchITEM (short nListRealNO, tagITEM &sITEM);
	short							Add_ITEM (tagITEM &sITEM);
	void							Set_ITEM(short nListRealNO, tagITEM& sITEM);
	void							ClearITEM(WORD wListRealNO);

	//-------------------------------------------------------------------------------------------------
	/// Update user states
	//-------------------------------------------------------------------------------------------------
	void							UpdateAbility();
	void							UpdateModelData()			{ Update(); };	/// 모델 데이터 갱신..
	void							UpdateInventory();

	void							Init_DefaultValue (void);	

	void							LevelUP ()
	{
		this->UpdateAbility ();
		this->Set_HP ( this->GetCur_MaxHP() );
		this->Set_MP ( this->GetCur_MaxMP() );

		g_pSoundLIST->IDX_PlaySound3D( SOUND_LEVEL_UP, this->Get_CurPOS() );
	}

	CInventory*						GetInventory()						{ return &m_Inventory; }
	short							GetEmptyInventory(short nInvPAGE)	{ return m_Inventory.GetEmptyInventory(nInvPAGE); }



	//-------------------------------------------------------------------------------------------------
	/// 현재 서버에 설정된 부활 존 번호.
	//-------------------------------------------------------------------------------------------------
	short							m_nReviveZoneNO;


	//----------------------------------------------------------------------------------------------------	
	/// @brief 현재 유져의 인풋명령을 받아들일수 있는가?
	//----------------------------------------------------------------------------------------------------	
	bool							bCanUserInput()
	{
		/// 현재 스킬 시전중 아니고, 스킬 액션중이 아닐때만..
		if( !(this->m_bCastingSTART) && ( this->m_nDoingSkillIDX == 0 ) )	
			return true;
		return false;
	}


	//-------------------------------------------------------------------------------------------------
	/// 현재 캐릭터의 상태정보를 엔진에서 얻어온다.
	/// 회전정보
	//-------------------------------------------------------------------------------------------------
	float							GetDirection()						
	{ 
		if( GetPetMode() >= 0 )
			return ::getModelDirection( this->m_pObjCART->GetZMODEL() ); 
		return ::getModelDirection( this->m_hNodeMODEL ); 
	};



	//-------------------------------------------------------------------------------------------------
	/// 슬롯 컨테이너
private:

	CSkillSlot						m_SkillSlot;
	CItemSlot						m_ItemSlot;
	CHotIconSlot					m_HotIconSlot;
	short							m_nCurrWeight;
public:
	CSkillSlot*						GetSkillSlot()						{ return &m_SkillSlot; }
	CItemSlot*						GetItemSlot()						{ return &m_ItemSlot; }
	CHotIconSlot*					GetHotIconSlot()					{ return &m_HotIconSlot; }
	/// 새로운 스킬 추가.
	bool							AddNewSkill( int iSkillIDX, int iSkillSlotNO, int iSkillLevel = 0 ,bool bSubPOINT = false );
	bool							Skill_LevelUp( int iSkillSlotNo, int iSkillIndex );
	//-------------------------------------------------------------------------------------------------

	


	//-------------------------------------------------------------------------------------------------
	///Bank( 계정창고 )관련
	void							SetBankItem( int iIndex, tagITEM& Item );
	void							InitBank();
	///현재 활성화된 총알의 갯수를 1감소시킨다.
	void							SubActiveBulletQuantity();
	//-------------------------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------------------------
	/// 마일리지 아이템에 의해서 캐릭터의 성별, 얼굴모양, 머리카락의 변화..
	void							ChangeModelInfo( short nType, int iValue );
	//-------------------------------------------------------------------------------------------------




	//-------------------------------------------------------------------------------------------------
	/// PET 관련	
	/// 현재 펫이 조합이 다되어 탈수 있는가?
	bool							CanRidePET();
	bool							IsRideItem(int iIndex_);
	//-------------------------------------------------------------------------------------------------

	virtual short GetCur_WEIGHT();

	void SetCurrWeight( short nWeight );

private:
	DWORD	m_dwUniqueTag;	///전체 월드에서 유니크한 ID(파티에서 사용)
	int		m_iServerSavedMemoCount;///서버에 저장되어 있는 쪽지의 개수
public:
	void	SetUniqueTag( DWORD dwUniqueTag );
	DWORD   GetUniqueTag();
	void	SetServerSavedMemoCount( int iCount );
	int		GetServerSavedMemoCount();
	void	DecreaseServerSavedMemoCount();


private:
	bool	m_bWaitUpdateInventory;
	/// 인벤토리의 아이템 상태가 여러개 변하는 중이다... 
	/// 장착/탈착의 경우 무게게이지 계산이 2번되면서 뛰기가->걷기로 변경된다.
	/// default : false

	DWORD	m_dwLastBattleTime;
public:
	void	SetWaitUpdateInventory( bool bWait );

	/// 서버에게 인벤토리 변경관련 패킷을 보내기 전에 변경후 인벤토리에 다들어갈수 있는지 체크한다.
	bool	IsInventoryFull( std::list< tagITEM >& appendItems, std::list< pair<int,tagITEM> >& removeItems );
	bool	IsInventoryFull( std::list< tagITEM >& appendItems );
	void	SetBattleTime( DWORD dwTime );
	DWORD   GetLastBattleTime();


	//-------------------------------------------------------------------------------------------------
	// 05.05.19 icarus::  와우 스타일의 NPC 퀘스트 노출 작업...
public:	
	// 자신의 아바타의 퀘스트가 갱신되었는지 여부를 판다.
	// 기본값 false, 존 갱신시 false로 설정. Object Loop에서 마지막에 true로 설정.
	// 서버에서 퀘스트 갱신 패킷을 받으면 false로 설정.
	bool	m_bQuestUpdate;


//------------------------------------------------------------------------------------
/// 아루아 버프 걸린 상태에 대한 추가적인 능력치 변경 관련
///
//------------------------------------------------------------------------------------
public:
	virtual		void Calc_AruaAddAbility();

protected:
	int		m_AruaAddHp;
	int		m_AruaAddMp;
	int		m_AruaAddCritical;
	int		m_AruaAddAttackPower;
	int		m_AruaAddDefence;

//------------------------------------------------------------------------------------
/// 카트 관련 수정/추가 : 2005/7/26 - nAvy
//------------------------------------------------------------------------------------
protected:
	int		m_PatGuageMax;

public:
	virtual bool	SetCMD_Skill2OBJ ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX);
	virtual void	SetCMD_Skill2SELF (short nSkillIDX);
	virtual void	SetCMD_Skill2POS (const D3DVECTOR& PosGOTO, short nSkillIDX);
	
	int				GetPatGuageMax();
	int				GetPatGuageCurrent();
	void			SetPatGuageMax( int guage );
	void			SetPatGuageCurrent( int guage );
protected:
	//void			SubPatGuagePerPatSkillExcute( short nSkillIDX );
	void			Cal_PatAbility();

//------------------------------------------------------------------------
//박지호::
public:
	BOOL	m_PushskillIcon;
	BOOL	m_PushUseItemIcon;
	
	DWORD   m_sSkillTick,m_sPushTick;

	BOOL SetCoolTimeUseItem(char* uName,short iItem);
	BOOL SetCoolTimeSkill(char* uName,int iSkill);
//------------------------------------------------------------------------

	void Set_Block_CartRide(bool ride);

} ;


#endif

