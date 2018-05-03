#ifndef _JUSTMODELAVT_
#define _JUSTMODELAVT_

#include "CModelCHAR.h"
#include <string>
#include "tgamectrl/tcommandq.h"

class CAvatarState;
///
/// This is a class for viewing char model
///
/// Init->SetRace->UpdateModel->LoadModelVisible ... ->InsertToScene
///
class CJustModelAVT
{
private:
	std::string					m_strName;

	BYTE						m_btRace;
	BYTE						m_bIsFemale;

	int							m_iJob;
	int							m_iLevel;
	DWORD						m_dwDeleteRemainSEC;

	/// Model nodes..
	HNODE						m_hNodeMODEL;
	union 
	{
		HNODE					*m_phPartVIS[ MAX_BODY_PART ];
		struct 
		{
			// 순서는 t_CharPART에 따라서...
			HNODE				*m_phFaceVIS;
			HNODE				*m_phHairVIS;
			HNODE				*m_phHelmetVIS;

			HNODE				*m_phArmorVIS;
			HNODE				*m_phGauntletVIS;
			HNODE				*m_phBootsVIS;

			HNODE				*m_phGoggleVIS;
			HNODE				*m_phKnapsackVIS;

			HNODE				*m_phRWeaponVIS;
			HNODE				*m_phLWeaponVIS;
		} ;
	} ;

	/// Model Index
	union 
	{
		int						m_nPartItemIDX[ MAX_BODY_PART ];
		struct 
		{
			// 순서는 t_CharPART에 따라서...
			int					m_nFaceIDX;
			int					m_nHairIDX;
			int					m_nHelmetIDX;

			int					m_nArmorIDX;
			int					m_nGauntletIDX;
			int					m_nBootsIDX;

			int					m_nGoggleIDX;
			int					m_nKnapsackIDX;

			int					m_nRWeaponIDX;
			int					m_nLWeaponIDX;
		} ;
	} ;


	/// 실제 캐릭터 파트 정보( ModelMerger 로부터 생성된 데이터 )
	CCharMODEL					m_CharMODEL;
	bool						m_bIsVisible;




public:
	CJustModelAVT(void);
	~CJustModelAVT(void);


	void SwitchState( int iID );
	
	enum{
		STATE_NORMAL,
		STATE_SITDOWN,
		STATE_SIT,
		STATE_STANDUP,
		STATE_MAX
	};


	/// Load Model Node
	bool						LoadModelNODE( const char *szName );
	void						UnloadModelNODE ();


	/// Init

	void						Init(	const char* strName = NULL,
										int iBODY_PART_FACE = 0,	// 0 link to bone
										int iBODY_PART_HAIR = 0,
										int iBODY_PART_HELMET = 0,
										int iBODY_PART_ARMOR = 0,	// 3 skinning
										int iBODY_PART_GAUNTLET = 0,
										int iBODY_PART_BOOTS = 0,
										int iBODY_PART_FACE_ITEM = 0,// 6 link to dummy
										int iBODY_PART_KNAPSACK = 0,
										// 오른손, 왼손 순서 주의 !!!
										int iBODY_PART_WEAPON_R = 0,		// 8 link to dummy	
										int iBODY_PART_WEAPON_L = 0	);

	void						Init( int iPartsArray[ MAX_BODY_PART ] );

	void						Update();
	void						PushBackCommand( CTCommand* pCmd );
	void						PushFrontCommand( CTCommand* pCmd );

	/// set race and sex
	void						SetRace( BYTE btRace );
	void						SetSex( int iSex );

	/// set body part
	bool						SetPart( int iPartType, int iPartIDX );

	/// Load visible
	bool						LoadModelVisible();

	/// Delete model visible
	void						UnloadModelVisible();

	/// update model
	bool						UpdateModel();


	/// 
	/// scene...
	///
	void						SetVisible( bool bShow ){ m_bIsVisible = bShow; }

	void						InsertToScene();
	void						RemoveFromScene();

	void						SetPosition( float x, float y, float z );
	bool						SetMotion( int iMotionIDX ,int iRepeatCount = 0 );
	bool						SetMotion( const char* strMotion ,int iRepeatCount = 0 );						
	const std::string&			GetName(){ return m_strName; }
	HNODE						GetModelNode(){ return m_hNodeMODEL; } 


	void SetJob( int iJob ){ m_iJob = iJob; }
	void SetLevel( int iLevel ){ m_iLevel = iLevel; }
	void SetDeleteRemainSEC( DWORD dwRemainSEC ){ m_dwDeleteRemainSEC = dwRemainSEC; }
	bool IsPlatinum();
	void SetPlatinum(bool platinum);
	int  GetJob(){ return m_iJob; }
	int  GetLevel(){ return m_iLevel; }
	DWORD GetDeleteRemainSEC(){ return m_dwDeleteRemainSEC; }

	bool	IsFemale(){ return m_bIsFemale?true:false; }
	void	SetIndex( int index );
	int		GetIndex();

private:
	bool			m_bPlatinum;
	CTCommandQ		m_CommandQ;
	CAvatarState*	m_pState[STATE_MAX];
	CAvatarState*	m_pCurrState;
	int				m_iIndex;

};


#endif // _JUSTMODELAVT_
