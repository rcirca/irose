#ifndef _COBJCART_
#define _COBJCART_

#include "cobjchar.h"



enum enumCART_ANI
{
	CART_ANI_STOP1 = 0,	
	CART_ANI_MOVE,	
	CART_ANI_ATTACK01,
	CART_ANI_ATTACK02,
	CART_ANI_ATTACK03,
	CART_ANI_DIE,
	CART_ANI_SPECIAL01,
	CART_ANI_SPECIAL02,
};

enum enumPETMODE_AVATAR_ANI
{
	PETMODE_AVATAR_ANI_STOP1 = 0,	
	PETMODE_AVATAR_ANI_MOVE,	
	PETMODE_AVATAR_ANI_ATTACK01,
	PETMODE_AVATAR_ANI_ATTACK02,
	PETMODE_AVATAR_ANI_ATTACK03,
	PETMODE_AVATAR_ANI_DIE,		
	PETMODE_AVATAR_ANI_SPECIAL01,
	PETMODE_AVATAR_ANI_SPECIAL02,
};


class CObjCHAR;

///
/// class for cart object
///

class CObjCART : public CObjCHAR
{
protected:
	CObjCHAR*				m_pObjParent;

	int						m_iCartType;
	CStrVAR					m_Name;


	CCharMODEL				m_CharMODEL;

//------------------------------------------------------------------------------------
//박지호:: 파츠 수정 
#if defined(_GBC)
	union 
	{
		short				m_nPartItemIDX[ MAX_RIDING_PART ];

		struct 
		{
			// 순서는 t_CharPART에 따라서...			
			short			m_nBodyIDX;
			short			m_nEngineIDX;
			short			m_nLegIDX;
			short			m_nAbilIDX;
			short			m_nWeaponIDX;			
		} ;
	} ;


#else 
	union 
	{
		short				m_nPartItemIDX[ MAX_RIDING_PART ];

		struct 
		{
			// 순서는 t_CharPART에 따라서...			
			short			m_nBodyIDX;
			short			m_nEngineIDX;
			short			m_nLegIDX;
			short			m_nAbilIDX;
		} ;
	} ;
#endif 
//------------------------------------------------------------------------------------

 

public:
	CObjCART(void);
	virtual ~CObjCART(void);

	int							m_iCurrentCartState;
	int							m_iOldCartState;

	enum CART_STATE
	{
		CART_STATE_STOP = 0,
		CART_STATE_MOVE = 1,
		CART_STATE_ATTACK = 2,
	};

	void						UnLinkChild(int iStart=0);

	CObjCHAR*					GetParent(){ return m_pObjParent; }

	void						SetPetParts( unsigned int iPartIDX, unsigned int iItemIDX );	
	int							GetCartType(){ return m_iCartType; }
	int							GetPetParts( int iPartIDX ){ return m_nPartItemIDX[ iPartIDX ] ;}

	///
	/// 카트에 이펙트 설정
	///
	void						SetEffect();
	void						SetPartEffect( int iPart );
	void						SetEffectByMoveState( bool bShow = true );

	///
	/// 카트에 사운드 설정
	///
	void						PlaySound( int iCurrentState );
	void						PlayPartSound( int iPart, int iCurrentState  );
	void						StopSound( int iCurrentState );
	void						StopPartSound( int iPart, int iCurrentState  );

	
	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameOBJ virtual functions
	/// <

	/*override*/virtual int		Get_TYPE()				{	return OBJ_CART;					}
	char*						Get_NAME ()				{	return m_Name.Get ();				}

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CAI_OBJ virtual functions
	/// <
	
	/*override*/virtual int		 Get_LEVEL ()			{ return 0; };
	/*override*/virtual int		 Get_ATK ()				{ return 0; };
	/*override*/virtual int		 Get_DEF ()				{ return 0; };
	/*override*/virtual int		 Get_RES ()				{ return 0; };
	/*override*/virtual int		 Get_HIT ()				{ return 0; };
	/*override*/virtual int		 Get_CHARM ()			{ return 0; };
	/*override*/virtual int		 Get_AVOID ()			{ return 0; };
	/*override*/virtual int		 Get_SENSE ()			{ return 0; };
	/*override*/virtual int		 Get_GiveEXP ()			{ return 0; };
	/*override*/virtual int		 Get_CRITICAL()			{ return 0; };

	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/virtual int		GetANI_Stop();
	/*override*/virtual int		GetANI_Move();
	/*override*/virtual int		GetANI_Attack();
	/*override*/virtual int		GetANI_Die ();
	/*override*/virtual int		GetANI_Hit();
	/*override*/virtual int		GetANI_Casting();
	/*override*/virtual int		GetANI_CastingRepeat();
	/*override*/virtual int		GetANI_Skill();

	/*override*/int				Get_MP ()							{	return 32767;			}
	/*override*/int				Get_R_WEAPON ()						{	return 0;				}
	/*override*/int				Get_L_WEAPON ()						{	return 0;				}
	

	/// 최대 생명력
	/*override*/int				Get_MaxHP()							{	return m_pObjParent->Get_MaxHP(); }
	/// 최대 마나
	/*override*/int				Get_MaxMP()							{	return m_pObjParent->Get_MaxMP(); }

	/*override*/virtual int		Def_AttackRange()					{	return 0; }


	/*override*/ virtual bool	SetCMD_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE );	
	/*override*/ virtual void	SetCMD_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget);
	/*override*/ virtual bool	SetCMD_STOP( void );
	/*override*/ virtual void	SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO);	

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjCHAR virtual functions
	/// <

public:
	/*override*/virtual bool				IsFemale ()				{ return true; }
	/*override*/virtual short				IsMagicDAMAGE()			{ return false; }


	/*override*/virtual tagMOTION*			Get_MOTION (short nActionIdx);


	/*override*/virtual void				Add_EXP (short nExp){}
	/*override*/virtual short				GetOri_WalkSPEED () { return ( ( m_pObjParent )? m_pObjParent->GetOri_WalkSPEED(): 10 ); }
	/*override*/virtual short				GetOri_ATKSPEED()	{ return ( ( m_pObjParent )? m_pObjParent->GetOri_ATKSPEED(): 10 ); }
	/*override*/virtual short				GetOri_RunSPEED()	{ return ( ( m_pObjParent )? m_pObjParent->GetOri_RunSPEED(): 10 ); }


	
	//----------------------------------------------------------------------------------------------------	
	/// @brief 카트계열은 LIST_PAT 에서 공격관련 데이터는 얻어온다.
	///        Move Speed 등은 펫을 탓다는 가정하에 계산이 부모에게 계산이 된다. 결국 부모의 스피드를 이용하면된다.
	///			이는 부모의 SpeedUpdate 를 할때 펫 탑승여부가 고려되어서 계산되기 때문이다.
	///			그러나 Attack Speed 따위는 틀리다. 이는 테이블에서 얻어와야한다.( 아바타는 무기에 의존하고 기타등등..
	//----------------------------------------------------------------------------------------------------

	/*override*/ virtual float				Get_MoveSPEED ();
				virtual float				Get_DefaultSPEED ();

	/*override*/ virtual short				Get_nAttackSPEED ();
	/*override*/ virtual float				Get_fAttackSPEED ();
	/*override*/ virtual int				Get_AttackRange ();

	

private:

	/*override*/HNODE			Get_SKELETON ()			{	return g_DATA.Get_PetSKELETON( m_iCartType );	}

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////

	
public:

	int		GetRideAniPos();
	
	virtual bool	Create( CObjCHAR* pParent, int iCartType, D3DVECTOR &Position );
	/// 충돌에 필요한 위치정보를 복사한다.
	void	CopyCollisionInformation( bool bRiding = true );
	
//------------------------------------------------------------------------------
//박지호::...
	CObjCHAR*				m_pRideUser;				//2인승 탑승자 

	bool Create(CObjCHAR* pTarget);
	void SetCartPartVisible(float fv);	
//------------------------------------------------------------------------------

};



#endif //_COBJCART_
