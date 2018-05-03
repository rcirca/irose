/**
 * \ingroup SHO_GS
 * \file	GS_ThreadZONE.h
 * \brief	CZoneFILE클래스를 상속받으며, 존과 존에 등록된 객체 관리/처리
 */
#ifndef ThreadZONEH
#define ThreadZONEH
//---------------------------------------------------------------------------
// #include <Classes.hpp>
// #include <SyncObjs.hpp>
#include "classTHREAD.h"
#include "classSYNCOBJ.h"
#include "cdxHPC.h"
#include "OBJECT.h"
#include "ZoneSECTOR.h"
#include "ZoneFILE.h"
#include "CEconomy.h"
//---------------------------------------------------------------------------

struct tagZoneTRIGGER {
	t_HASHKEY	m_TriggerHASH;
	short		m_nTeamNO;
} ;

struct tagCalledMOB {
	DWORD	m_dwActionTIME;
	float	m_fCenterX;
	float	m_fCenterY;
	int		m_iRange;
	int		m_iCharNO;
	int		m_iCount;
	int		m_iTeamNO;
} ;


/**
 * \ingroup SHO_GS_LIB
 * \class	CZoneTHREAD
 * \author	wookSang.Jo
 * \brief	존(zone) 처리 하는 쓰래드 클래스
 *			쓰래드 클래스및 존파일 데이타를 포함하고 있는 CZoneFILE클래스를 상속받음
 */
class CZoneTHREAD : public CZoneFILE
{
private  :
    classEVENT *m_pEvent;
    CDXHPC		m_Timer;
	DWORD		m_dwCurrentTIME;
	DWORD		m_dwTimeGetTIME;
	DWORD		m_dwCurAbsSEC;

	bool		m_bEnableREGEN;
	classDLLIST< CRegenPOINT* >	m_RegenLIST;

	CCriticalSection			m_csZoneMobCALLED;
	classSLLIST< tagCalledMOB >	m_MobCALLED;

	CCriticalSection			m_csZoneObjWAIT;
	classDLLIST< CGameOBJ* >	m_ObjWAIT;			// 존에 들어가기위해 대기하는 객체...

    classDLLIST< CGameOBJ* >    m_ObjLIST;			// 존에 들어가 있는 객체...

	CCriticalSection			m_csPacketLIST;
	classSLLIST< classPACKET* >	m_ToSendPacketLIST;
	classSLLIST< classPACKET* >	m_SendingPacketLIST;

	long	m_lMaxUSER;
	long	m_lUserCNT;
    bool	m_bEndExecuteFunc;
	DWORD	m_dwGlobalFLAGS;

	// Find Object var
	int		m_iFindSecIDX;
	int		m_iFindDistance;
	tPOINTF	m_FindPOS;
    POINTS  m_FindSECTOR;
    classDLLNODE< CGameOBJ* > *m_pFindObjNODE;
	CGameOBJ	*m_pObjFINDER;

	void	DeleteZoneOBJ ();

	CCriticalSection				m_csTriggerLIST;
	classSLLIST< tagZoneTRIGGER >	m_TriggerLIST;
	void	Proc_ZoneTRIGGER ();

protected:
	CZoneTHREAD *GetZonePTR ()						{	return	this;	}

	bool Add_RegenPOINT (FILE *fp, float fPosX, float fPosY);
	bool Load_Economy (FILE *fp);

    void Execute();
	
public:
	CEconomy	m_Economy;
	void	   *m_pListITEM;

    CZoneTHREAD(bool CreateSuspended);
    ~CZoneTHREAD();

	int		Get_UserCNT ()		{	return m_lUserCNT;	}
	void	Inc_UserCNT ()		
	{	
		::InterlockedIncrement( &m_lUserCNT );
		if ( m_lUserCNT > m_lMaxUSER ) {
			m_lMaxUSER = m_lUserCNT;
			CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 2, m_lUserCNT );
		}
		CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 1, m_lUserCNT );
	}
	void	Dec_UserCNT ()		
	{	
		::InterlockedDecrement( &m_lUserCNT );
		CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 1, m_lUserCNT );
	}

    bool	Add_OBJECT( CGameOBJ *pObj );
	bool	Add_DIRECT( CGameOBJ *pObj );
    void	Sub_DIRECT( CGameOBJ *pObj, bool bSubFromREGEN=true );

	CZoneSECTOR *GetSECTOR (int iSectorX, int iSectorY);
	bool	UpdateSECTOR( CObjCHAR *pCHAR );

	CObjMOB*RegenMOB( float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO );
	bool	RegenCharacter( CObjMOB *pNewMOB, float fCenterX, float fCenterY, int iRange, int iIndex, int iTeamNO);
	CObjMOB*RegenCharacter(float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO, bool bImmediate );
    void	RegenCharacter( CRegenPOINT *pRegenPOINT, int iIndex, int iCount );
    void	SendPacketToSectors (CGameOBJ *pGameObj, classPACKET *pCPacket);
	void	SendTeamPacketToSectors (CGameOBJ *pGameObj, classPACKET *pCPacket, int iTeamNO);

    bool	AddObjectToSector   (CGameOBJ *pObj, BYTE btUpdateFLAG);
    void	SubObjectFromSector (CGameOBJ *pObj, char cUpdateFLAG);

    bool	Init (char *szBaseDIR, short nZoneNO);
    bool	Free (void);

	inline int Get_ItemBuyPRICE  (short nItemTYPE, short nItemNO, short nBuySkillVALUE)
	{	
		return m_Economy.Get_ItemBuyPRICE( nItemTYPE, nItemNO, nBuySkillVALUE );	
	}
	inline int Get_ItemSellPRICE (tagITEM &sITEM, short nSellSkillVALUE)
	{	
		return m_Economy.Get_ItemSellPRICE( sITEM, nSellSkillVALUE );
	}
	inline int Get_EconomyVAR	 (short nVarIDX)
	{
		return m_Economy.Get_EconomyVAR( nVarIDX );
	}

	DWORD	GetPassTIME ()				{	return	m_Timer.GetPassTIME();			}
	DWORD	GetCurrentTIME ()			{	return	m_dwCurrentTIME;				}
	DWORD	GetTimeGetTIME ()			{	return	m_dwTimeGetTIME;				}
	DWORD	GetCurAbsSEC ()				{	return	m_dwCurAbsSEC;					}
	DWORD	GetEconomyTIME ()			{	return	this->m_Economy.m_dwUpdateTIME;	}
	DWORD	GetGlobalFLAG()				{	return	this->m_dwGlobalFLAGS;			}
	int		GetGameObjCNT()				{	return	m_ObjLIST.GetNodeCount();		}

	void	BuyITEMs( tagITEM &sITEM )					{	m_Economy.BuyITEM( sITEM );					}
	void	SellITEMs( tagITEM *pITEM, int iQuantity )	{	m_Economy.SellITEM( *pITEM, iQuantity );	}

	bool	SendShout (CGameOBJ *pGameOBJ, classPACKET *pCPacket, int iTeamNo=0);
	bool	SendPacketToZONE (t_PACKET *pSendPacket);

	void	Send_EconomyDATA ();
	void	Send_GLOBAL_FLAG ();
	DWORD	Toggle_PK_FLAG ();
	void	Set_PK_FLAG( BYTE btOnOff );

	CObjCHAR* FindFirstCHAR (tPOINTF &PosCENTER, int iDistance, CGameOBJ *pObjFINDER);
	CObjCHAR* FindFirstCHAR (CObjCHAR *pCenterCHAR, int iDistance, CGameOBJ *pObjFINDER);
	CObjCHAR* FindNextCHAR  ();

	void	Kill_AllMOB (classUSER *pUSER);
	bool	Toggle_REGEN ()						{	m_bEnableREGEN = !m_bEnableREGEN;	return m_bEnableREGEN;	}
	void	Set_REGEN ( bool bEnableRegen )		{	m_bEnableREGEN = bEnableRegen;	}

	void	Reset_REGEN ();
	short	GetRegenPointCNT ();
	short   GetRegenPointMOB( short nIndex, classUSER *pUSER );
	CRegenPOINT *GetRegenPOINT(short nIndex);
	short	CallRegenPointMOB( short nIndex );
	short	GetNearstRegenPOINT (tPOINTF &PosCENTER);

	void	Do_QuestTrigger( short nTeamNO, t_HASHKEY HashKEY );
} ;

#include "CRegenAREA.h"

//---------------------------------------------------------------------------
#endif
