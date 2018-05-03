/**
 * \ingroup SHO_GS
 * \file	ZoneFILE.h
 * \brief	클라이언트와 같이 사용하는 존 파일을 읽어 존을 구성
 */
#ifndef	__ZONEFILE_H
#define	__ZONEFILE_H
#include "classHASH.h"
#include "CBitARRAY.h"
//-------------------------------------------------------------------------------------------------

// #define	__NPC_TEST

#ifndef	__INC_WORLD
	#undef	__NPC_TEST
#endif

struct tagEVENTPOS {
	short	m_nZoneNO;
	tPOINTF	m_Position;
	CStrVAR	m_Name;
} ;


#define	MAP_COUNT_PER_ZONE_AXIS		64
#define	PATCH_COUNT_PER_MAP_AXIS	16

#define	GRID_COUNT_PER_PATCH_AXIS	this->GridCountPerPatchAXIS()
#define	nPATCH_SIZE					this->nPatch_SIZE()
#define	nGRID_SIZE					this->nGrid_SIZE()

#define	MIN_SECTOR_SIZE				5000
#define	MAX_SECTOR_SIZE				12000
#define	BUF_SECTOR_SIZE(s)			( s * 1/5 )

//#define nSECTOR_SIZE                (4000)		// 60 m

// #define nSECTOR_SIZE                ( 6400 )		// 64 m
// #define	nSECTOR_BUFFER_SIZE			( 3000 )		// 30 m
//#define	nSECTOR_BUFFER_SIZE			( nSECTOR_SIZE * 2/5)


//#define	nSECTOR_HALF_SIZE			( nSECTOR_SIZE / 2	)
//#define	SECTOR_MAP_LIMIT			( nSECTOR_HALF_SIZE+nSECTOR_BUFFER_SIZE )


// 최대 64개 맵 * 각맵은 16개 패치로 구성 * 5m 단위의 속성
#define	MAP_MOVE_ATTR_GRID_CNT		( MAP_COUNT_PER_ZONE_AXIS*PATCH_COUNT_PER_MAP_AXIS*2 )

class CRegenPOINT;

/**
 * \ingroup SHO_GS_LIB
 * \class	CZoneFILE
 * \author	wookSang.Jo
 * \brief	존(zone) 파일을 읽어 존 데이타를 생성하는 클래스
 *			기본 쓰래드 클래스 classTHREAD를 상속받음
 */
class CZoneFILE : public classTHREAD 
{
private:
	classHASH< tagEVENTPOS* >  *m_pPosLIST;
	t_HASHKEY					m_HashKeyRevivePOS;

    void    ReadObjINFO (FILE *fp, long lOffset, int iLumpType, short nMapXIDX, short nMapYIDX);
	char*	GetMapFILE(short nMapX, short nMapY);
    void    LoadMAP (char *szFileName, short nMapXIDX, short nMapYIDX);
	void	LoadMOV (char *szFileName, short nMapXIDX, short nMapYIDX);
    
	void	ReadZoneINFO	( FILE *fp, long lOffset );
    void    ReadEventObjINFO( FILE *fp, long lOffset );
	bool	ReadECONOMY		( FILE *fp, long lOffset );

	short	GridCountPerPatchAXIS()	{	return m_nGridPerPATCH;		}
	short	nPatch_SIZE()			{	return m_nPatchSIZE;		}
	short	nGrid_SIZE()			{	return m_nGridSIZE;			}

	t_HASHKEY	m_HashJoinTRIGGER;
	t_HASHKEY	m_HashKillTRIGGER;
	t_HASHKEY	m_HashDeadTRIGGER;

	CStrVAR		m_ZoneDIR;
	CStrVAR		m_MapFileName;

    short		m_nMinMapX, m_nMinMapY;
    short		m_nMaxMapX, m_nMaxMapY;

	short		m_nZoneNO;
	tPOINTF		m_PosSTART;
	tPOINTF		m_PosREVIVE;

	tPOINTF	   *m_pAgitPOS;
	int			m_iAgitCNT;

	short		m_nDayTimeFROM;
	short		m_nDayTimeTO;

    int         m_iSectorXCnt;
	int			m_iSectorYCnt;

protected:
/*
	tPOINTF    *m_pEventPOS;
	int			m_iEventPosCNT;
*/
	CStrVAR	    m_StrVAR;

    CZoneSECTOR **m_ppSECTOR;
	C1BITARRAY  *m_ppMoveATTR[ MAP_MOVE_ATTR_GRID_CNT ];

	short		m_nSectorSIZE;
	int			m_iSectorLIMIT;

	virtual CZoneTHREAD *GetZonePTR ()=0;
	virtual	bool Add_RegenPOINT (FILE *fp, float fPosX, float fPosY)=0;
	virtual bool Load_Economy (FILE *fp)=0;

public   :
	short		m_nGridPerPATCH;
	short		m_nPatchSIZE;
	short		m_nGridSIZE;

	int			m_iIsDungeon;

	CZoneFILE (bool CreateSuspended);
	virtual __fastcall ~CZoneFILE ();
		
    bool    LoadZONE (char *szBaseDIR, short nZoneNO);
	void	FreeZONE (void);

	short	Get_SectorSIZE()					{	return  m_nSectorSIZE;			}

	tPOINTF	Get_StartPOS ()						{	return	m_PosSTART;				}
	tPOINTF	Get_StartRevivePOS ()				{	return	m_PosREVIVE;			}
	tPOINTF Get_RevivePOS( tPOINTF PosCUR );
	tPOINTF Get_AgitPOS( DWORD dwClanID )		{	return  this->m_pAgitPOS[ dwClanID % this->m_iAgitCNT ];	}

	#define	nATTR_GRID_SIZE			500
	bool	IsMovablePOS(int iXPos, int iYPos)
	{
		int iY = iYPos/nATTR_GRID_SIZE;
		if ( iY >= 0 && iY < MAP_MOVE_ATTR_GRID_CNT )
			return ( 0 == this->m_ppMoveATTR[ iY ]->GetBit( iXPos/nATTR_GRID_SIZE ) );
		return false;
	}

	BYTE	Is_DAY ();
	char*	Get_NAME ()				{	return	ZONE_NAME( m_nZoneNO );	}
	short	Get_ZoneNO ()			{	return	m_nZoneNO;				}
	int		Get_SectorXCNT ()		{	return  m_iSectorXCnt;			}
	int		Get_SectorYCNT ()		{	return  m_iSectorYCnt;			}

	t_HASHKEY	Get_HashJoinTRIGGER()	{	return m_HashJoinTRIGGER;	}
	t_HASHKEY	Get_HashKillTRIGGER()	{	return m_HashKillTRIGGER;	}
	t_HASHKEY	Get_HashDeadTRIGGER()	{	return m_HashDeadTRIGGER;	}

	CStrVAR*GetStrVAR ()			{	return &m_StrVAR;				}
} ;

//-------------------------------------------------------------------------------------------------
#endif
