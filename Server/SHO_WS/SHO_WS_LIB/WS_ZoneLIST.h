#ifndef	__WS_ZONELIST_H
#define	__WS_ZONELIST_H
#include "CWorldVAR.h"
//---------------------------------------------------------------------------------------

#define	MAP_COUNT_PER_ZONE_AXIS		64
#define	PATCH_COUNT_PER_MAP_AXIS	16

#define	GRID_COUNT_PER_PATCH_AXIS	this->GridCountPerPatchAXIS()
#define	nPATCH_SIZE					this->nPatch_SIZE()
#define	nGRID_SIZE					this->nGrid_SIZE()

struct tagEVENTPOS {
	short	m_nZoneNO;
	tPOINTF	m_Position;
	CStrVAR	m_Name;
} ;

class CWS_Server;
class WS_Zone
{
private:
public :
	CWS_Server	*m_pZoneSERVER;

	short	m_nZoneNO;
	char   *m_szZoneNAME;

	CStrVAR	m_ZoneIP;
	int		m_iZonePORT;

	int		m_iMaxUserCNT;
	int		m_iCurUserCNT;

	bool	m_bActive;

	classHASH< tagEVENTPOS* >  *m_pPosLIST;

	tagEVENTPOS	*m_pStartPOS;
	tPOINTF		 m_PosREVIVE;
	t_HASHKEY	 m_HashKeyRevivePOS;

	void		*m_pListITEM;

	WS_Zone ()			
	{
		m_pZoneSERVER = NULL;

		m_pPosLIST = new classHASH< tagEVENTPOS* >( 64 );
		m_HashKeyRevivePOS = 0;

		m_bActive=false;
		m_nZoneNO=0;

		m_pStartPOS = NULL;
		m_szZoneNAME=NULL;

		m_PosREVIVE.x = 0;
		m_PosREVIVE.y = 0;

		m_pListITEM = NULL;
	}
	~WS_Zone ()
	{
		t_HASHKEY	 HashKEY;
		tagEVENTPOS* pEventPOS;

		while( m_pPosLIST->GetFirst( &HashKEY, &pEventPOS ) ) {
			m_pPosLIST->Delete( HashKEY );
		}
		SAFE_DELETE( m_pPosLIST );

		_ASSERT( NULL == m_pZoneSERVER );
	}

	tPOINTF Get_RevivePOS( tPOINTF PosCUR );

	char *Get_NAME ()		{	return m_szZoneNAME;	}
	short Get_ZoneNO()		{	return m_nZoneNO;		}
} ;

//---------------------------------------------------------------------------------------

class WS_ZoneLIST : public CWorldVAR
{
private:
	classHASH< tagEVENTPOS* > m_PosLIST;

	short	m_nGridPerPATCH;
	short	m_nPatchSIZE;
	short	m_nGridSIZE;

	short	GridCountPerPatchAXIS()	{	return m_nGridPerPATCH;		}
	short	nPatch_SIZE()			{	return m_nPatchSIZE;		}
	short	nGrid_SIZE()			{	return m_nGridSIZE;			}

	bool	LoadZONE (char *szBaseDIR, short nZoneNO);
	void	ReadZoneINFO (FILE *fp, long lOffset);
	void	ReadEventObjINFO (FILE *fp, long lOffset, short nZoneNO);

	short	m_nZoneCNT;

	WS_Zone	*m_pZONE;

public :
	// Interited from CWorldVAR
	void Set_WorldVAR (short nVarIDX, short nValue);
	bool Save_WorldVAR ();

	WS_ZoneLIST ()		{	m_pZONE=NULL;					}
	~WS_ZoneLIST ();

	bool Init (char *szBaseDIR);

	short		GetZoneCNT()						{	return m_nZoneCNT;		}
	WS_Zone*	GetZONE( short nZoneNO )			{	return IsActive(nZoneNO) ? &this->m_pZONE[ nZoneNO ] : NULL;			}

	tagEVENTPOS *Add_EventPOS( t_HASHKEY HashKEY, short nZoneNO, char *szPosName, float fXPos, float fYPos);
	tagEVENTPOS *Get_EventPOS( short nZoneNO, char *szPosName );

	tPOINTF	Get_StartPOS (short nZoneNO)			{	return m_pZONE[nZoneNO].m_pStartPOS->m_Position;	}
	tPOINTF	Get_StartRevivePOS (short nZoneNO)		{	return m_pZONE[nZoneNO].m_PosREVIVE;				}
	
	bool IsActive( short nZoneNO )					
	{	
		if ( nZoneNO>0 && nZoneNO<m_nZoneCNT ) {
			return m_pZONE[nZoneNO].m_bActive;
		}
		
		g_LOG.CS_ODS(0xffff, "  @@@@ Invalied ZoneNO %d in IsActive() ...\n", nZoneNO );

		return false;
	}
} ;
extern WS_ZoneLIST g_ZoneLIST;

//---------------------------------------------------------------------------------------
#endif
