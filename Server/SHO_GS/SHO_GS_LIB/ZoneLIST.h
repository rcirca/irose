/**
 * \ingroup SHO_GS
 * \file	ZoneLIST.h
 * \brief	전체 존 관리 클래스
 */
#ifndef	__ZONELIST_H
#define	__ZONELIST_H
#include "GS_ThreadZONE.h"
#include "CWorldVAR.h"
//-------------------------------------------------------------------------------------------------


#define	AGIT_ZONE_TYPE						11		// ZONE_PVP_STATE값이 11이면 아지트 존이다...


/**
 * \ingroup SHO_GS_LIB
 * \class	CZoneLIST
 * \author	wookSang.Jo
 * \brief	전체 존(zone)관리하는 클래스
			월드 변수 클래스 CWorldVAR를 상속받음
 */
class CZoneLIST : public CWorldVAR {
private:
	classHASH< tagEVENTPOS* > m_PosLIST;
	classHASH< CObjEVENT*	> m_EventOBJECT;

	short	m_nZoneCNT;
	CZoneTHREAD	**m_ppThreadZONE;
	bool   *m_pValidZONE;
	short  *m_pSectorSIZE;

	CZoneLIST ();
	~CZoneLIST ();
	
	CObjNPC **m_ppNpcLIST;

public :
	// Inherited from CWorldVAR
	void Set_WorldVAR (short nVarIDX, short nValue);

	void Add_LocalNPC ( CObjNPC *pObjNPC );
	CObjNPC *Get_LocalNPC( int iNpcNO );
	CObjVAR *Get_NpcVAR( int iNpcNO )
	{
		return this->Get_LocalNPC( iNpcNO );
	}

	void Add_EventOBJ( CObjEVENT *pObjEVENT );
	CObjEVENT *Get_EventOBJ( short nZoneNO, t_HASHKEY HashKEY );

	void Init_NpcObjVAR (short nNpcIDX, BYTE *pVAR);
	void Init_EventObjVAR (t_HASHKEY HashID, BYTE *pVAR);

	bool InitZoneLIST (char *szBaseDIR);
	void FreeZoneLIST ();

	bool		 IsAgitZONE(short nZoneNO);
	bool		 IsValidZONE(short nZoneNO)		{	return ( nZoneNO>0 && nZoneNO<m_nZoneCNT ) ? m_pValidZONE[nZoneNO] : false;	}
	short	     GetZoneCNT()					{	return m_nZoneCNT;				}
	short		 GetSectorSIZE(short nZoneNO)	{	return m_pSectorSIZE[nZoneNO];	}

	CZoneTHREAD *GetZONE( short nZoneNO );

	tagEVENTPOS *Add_EventPOS(t_HASHKEY HashKEY, short nZoneNO, char *szPosName, float fXPos, float fYPos);
	tagEVENTPOS *Get_EventPOS( short nZoneNO, char *szPosName );
//	tagEVENTPOS *Get_NearstEventPOS( short nZoneNO, char *szPosName, tPOINTF PosCUR );

	void Send_gsv_ANNOUNCE_CHAT( CObjVAR *pObjVAR, char *szMessage );
	void Send_gsv_ANNOUNCE_CHAT( short nZoneNO, char *szMessage, char *szAccount  );
	void Send_gsv_ANNOUNCE_CHAT( char *szMessage, char *szAccount  );
	void Send_gsv_ANNOUNCE_CHAT( t_PACKET *pSendPacket );

	void Set_PK_FLAG (short nZoneNO, BYTE btOnOff);
	void Set_PK_FLAG (CAI_OBJ *pChar, BYTE btOnOff);
	bool Do_QuestTrigger( short nZoneNO, short nTeamNO, t_HASHKEY HashKEY );

	void Toggle_RegenSYSTEM ( CGameOBJ *pObj );
	void Set_RegenSYSTEM( CGameOBJ *pObj, BYTE btOnOff );

	void Toggle_RegenSYSTEM ( CAI_OBJ *pChar );
	void Set_RegenSYSTEM( CAI_OBJ *pChar, BYTE btOnOff );

	void Toggle_RegenSYSTEM ( short nZoneNo );
	void Set_RegenSYSTEM( short nZoneNo, BYTE btOnOff );

	CObjMOB*RegenCharacter (short nZoneNO, float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO);

	DECLARE_INSTANCE( CZoneLIST )
} ;
extern CZoneLIST	*g_pZoneLIST;


//-------------------------------------------------------------------------------------------------
inline int	 Get_WorldVAR(short nVarIDX)
{
	return g_pZoneLIST->Get_WorldVAR( nVarIDX );
}

inline DWORD Get_WorldPassTIME ()
{
	return g_pZoneLIST->m_dwAccTIME;
}

inline short Get_WorldTIME ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_TIME );
}


inline short Get_WorldDROP_M ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_DROP_M );
}
inline short Get_WorldDROP ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_DROP_I );
}
inline short Get_WorldEXP ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_EXP );
}
inline short Get_WorldPROD ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_PRODUCT );
}
inline short Get_WorldRATE ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_PRICES );
}
inline short Get_WorldREWARD ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_QST_REWARD );
}
inline short Get_WorldSTAMINA ()
{
	return g_pZoneLIST->Get_WorldVAR( WORLD_VAR_STAMINA );
}

//-------------------------------------------------------------------------------------------------
inline void Set_WorldDROP_M (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_DROP_M, nValue );
}
inline void Set_WorldDROP (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_DROP_I, nValue );
}
inline void Set_WorldEXP (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_EXP, nValue );
}
inline void Set_WorldPROD (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_PRODUCT, nValue );
}
inline void Set_WorldRATE (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_PRICES, nValue );
}
inline void Set_WorldREWARD (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_QST_REWARD, nValue );
}
inline void Set_WorldSTAMINA (short nValue)
{
	g_pZoneLIST->Set_WorldVAR( WORLD_VAR_STAMINA, nValue );
}


//-------------------------------------------------------------------------------------------------
#endif
