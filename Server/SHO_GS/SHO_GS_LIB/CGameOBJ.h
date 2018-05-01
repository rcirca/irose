/**
 * \ingroup SHO_GS
 * \file	CGameOBJ.h
 * \brief	로즈온라인에서 처리되는 객체의 기본이 되는 클래스
 */#ifndef CObjectH
#define CObjectH

#include "classPACKET.h"
#include "CObjVAR.h"
//---------------------------------------------------------------------------

class classUSER;
class CZoneSECTOR;
class CZoneTHREAD;

//---------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CGameOBJ
 * \author	wookSang.Jo
 * \brief	게임내 존제하는 객체클래스 들이 상속받는 기저 클래스
 */
class CGameOBJ
{
private:
    CZoneTHREAD *m_pZONE;
	
protected:

public :
    int         m_iIndex;						// != 0 이면 존에 들어가 있다 !!!!
    tPOINTF     m_PosCUR;						/// 참가해 있는 존의 위치
    POINTS      m_PosSECTOR;					/// 위치에 해당하는 섹터 좌표
	CZoneSECTOR*m_pGroupSECTOR;					/// 참가해 있는 섹터의 포인터

    classDLLNODE< CGameOBJ* > m_ZoneNODE;
    classDLLNODE< CGameOBJ* > m_SectorNODE;

    CGameOBJ ();
    virtual ~CGameOBJ ();
	void	Init_GameOBJ ();

	virtual t_ObjTAG Get_TYPE ()				{	return OBJ_NULL;					}
	virtual char*    Get_NAME ()				{	return NULL;						}
	virtual int		 Proc (void)				{	return 1;							}
	virtual int		 Get_TEAM ()				{	return 0;							}

	int     Get_INDEX ()						{	return	m_iIndex;					}			

    void    ResetSECTOR (short nSectorSize);
	bool    IsA (t_ObjTAG Obj_TYPE)				{	return ( Get_TYPE()==Obj_TYPE );						}

	bool	IsITEM ()							{	return ( Get_TYPE()==OBJ_ITEM );						}
	bool	IsCNST ()							{	return ( Get_TYPE()==OBJ_CNST );						}
	bool	IsNPC  ()							{	return ( Get_TYPE()>=OBJ_NPC && Get_TYPE()<=OBJ_MOB );	}
	bool	IsCHAR ()							{	return ( Get_TYPE()>=OBJ_NPC );							}
    bool    IsUSER ()							{   return ( Get_TYPE()>=OBJ_AVATAR );						}

	bool	IsNEIGHBOR( CGameOBJ *pObj )		// 섹터 패킷이 전송 가능한 영역에 있나?
	{
		if ( this->GetZONE() == pObj->GetZONE() ) {
			if ( abs( this->m_PosSECTOR.x - pObj->m_PosSECTOR.x ) <= 1 &&
				 abs( this->m_PosSECTOR.y - pObj->m_PosSECTOR.y ) <= 1 )
				return true;
		}
		return false;
	}

	CZoneTHREAD *GetZONE()						{	return	m_pZONE;							}
	void	SetZONE(CZoneTHREAD *pZONE)			{	m_pZONE = pZONE;							}

	int		GetCur_ZoneTIME (void);
	int		GetCur_WorldTIME(void);

	virtual void	Set_TargetObjIDX (int /*iTargetIndex*/)			{	/* nop */		}

    virtual bool	SendPacket( classPACKET* /*pCPacket*/)			{   return true;	}
	virtual bool	Send_gsv_JOIN_ZONE (CZoneTHREAD* /*pZONE*/)		{	return true;	}

	virtual void	Update_OwnerObjIDX ( classUSER* /*pUSER*/ )		{	/* nop */		}
    virtual bool    Make_gsv_ADD_OBJECT( classPACKET* /*pCPacket*/)=0;
	virtual bool    Make_gsv_SUB_OBJECT( classPACKET* /*pCPacket*/);
} ;


//---------------------------------------------------------------------------
#endif
