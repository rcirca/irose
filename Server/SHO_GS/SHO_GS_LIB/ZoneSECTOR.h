/**
 * \ingroup SHO_GS
 * \file	ZoneSECTOR.h
 * \brief	존을 구성하는 섹터 처리
 */
#ifndef __ZONESECTOR_H
#define __ZONESECTOR_H
#include "classPACKET.h"
#include "Object.h"
//-------------------------------------------------------------------------------------------------

// #define	__USE_CS_SECTOR

/**
 * \ingroup SHO_GS_LIB
 * \class	CZoneSECTOR
 * \author	wookSang.Jo
 * \brief	존의 영역을 구성하는 기본 섹터 클래스
 */
class CZoneSECTOR {
protected:
	static	int				m_iSectorCNT;

    classDLLIST< CGameOBJ* >  m_ObjLIST;
	classDLLNODE< CGameOBJ* >*m_pObjNODE;

#ifdef	__USE_CS_SECTOR
	CCriticalSection		m_csSECTOR;

    void LockSector ()        { ::EnterCriticalSection( &m_csSECTOR );	}
    void UnlockSector ()      {	::LeaveCriticalSection( &m_csSECTOR );  }
#else
    void LockSector ()        {	;	}
    void UnlockSector ()      {	;	}
#endif

public :
	static int	GetSectorCount ()	{	return m_iSectorCNT;	}

	int	m_iCenterX;
	int m_iCenterY;

    CZoneSECTOR ();
    ~CZoneSECTOR ();

	void SetCenterPOS(int iCenterX, int iCenterY);
    void InsToSEC  ( CGameOBJ *pObj );
    void DelFromSEC( CGameOBJ *pObj );

	virtual classDLLNODE< CGameOBJ* > *FindFirstCHAR (CGameOBJ *pObjFINDER) = 0;
	virtual classDLLNODE< CGameOBJ* > *FindNextCHAR (classDLLNODE< CGameOBJ* > *pCurNODE, CGameOBJ *pObjFINDER) = 0;

    virtual void SendPacketToSECTOR( classPACKET *pCPacket, CGameOBJ *pObjSENDER ) = 0;
	virtual void SendTeamPacketToSECTOR( classPACKET *pCPacket, int iTeamNo, CGameOBJ *pObjSENDER ) = 0;

    virtual void SendAddUserPacket( classUSER *pUSER, classPACKET *pCPacket ) = 0;
    virtual void SendSubUserPacket( classUSER *pUSER, classPACKET *pCPacket ) = 0;
} ;


//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CAgitSECTOR
 * \author	wookSang.Jo
 * \brief	일반 존의 영역을 구성하는 섹터 클래스
 *			기본섹터 클래스 CZoneSECTOR를 상속받음
 */
class CNormalSECTOR : public CZoneSECTOR
{
public :
	classDLLNODE< CGameOBJ* > *FindFirstCHAR (CGameOBJ *pObjFINDER);
	classDLLNODE< CGameOBJ* > *FindNextCHAR (classDLLNODE< CGameOBJ* > *pCurNODE, CGameOBJ *pObjFINDER);

    void SendPacketToSECTOR( classPACKET *pCPacket, CGameOBJ *pObjSENDER  );
	void SendTeamPacketToSECTOR( classPACKET *pCPacket, int iTeamNo, CGameOBJ *pObjSENDER );

    void SendAddUserPacket( classUSER *pUSER, classPACKET *pCPacket );
    void SendSubUserPacket( classUSER *pUSER, classPACKET *pCPacket );
} ;

//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CAgitSECTOR
 * \author	wookSang.Jo
 * \brief	아지트 존의 영역을 구성하는 섹터 클래스
  *			기본섹터 클래스 CZoneSECTOR를 상속받음
*/
class CAgitSECTOR : public CZoneSECTOR
{
public :
	classDLLNODE< CGameOBJ* > *FindFirstCHAR (CGameOBJ *pObjFINDER);
	classDLLNODE< CGameOBJ* > *FindNextCHAR (classDLLNODE< CGameOBJ* > *pCurNODE, CGameOBJ *pObjFINDER);

    void SendPacketToSECTOR( classPACKET *pCPacket, CGameOBJ *pObjSENDER  );
	void SendTeamPacketToSECTOR( classPACKET *pCPacket, int iTeamNo, CGameOBJ *pObjSENDER );

    void SendAddUserPacket( classUSER *pUSER, classPACKET *pCPacket );
    void SendSubUserPacket( classUSER *pUSER, classPACKET *pCPacket );
} ;

//-------------------------------------------------------------------------------------------------
#endif
 