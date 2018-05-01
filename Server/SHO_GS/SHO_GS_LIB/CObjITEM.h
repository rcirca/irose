/**
 * \ingroup SHO_GS
 * \file	CObjITEM.h
 * \brief	필드에 등록된 아이템 객체
 */
#ifndef	__COBJITEM_H
#define	__COBJITEM_H
#include "CGameOBJ.h"
#include "CItem.h"
//-------------------------------------------------------------------------------------------------
class classUSER;
class CParty;

/**
 * \ingroup SHO_GS_LIB
 * \class	CObjITEM
 * \author	wookSang.Jo
 * \brief	바닦에 떨어진 아이템 객체 관리및 처리하는 클래스
 *			CGameOBJ클래스를 상속받음
 */
class CObjITEM : public CGameOBJ
{
private:
	CParty	   *m_pOwnerPARTY;
	DWORD		m_dwOwnerDBID;

public :
    tagITEM     m_ITEM;

	int			m_iOwnerObjIDX;			/// 아이템을 소유할수 있는 유저
	int			m_iRemainTIME;			/// 귀속되어 있는 남은 시간

	bool		m_bDropperIsUSER;		/// 사용자가 떨군 아이템이냐?
	CStrVAR		m_DropperACCOUNT;		/// 떨군 사용자의 계정
	CStrVAR		m_DropperNAME;			/// 떨군 케릭터 이름
	CStrVAR		m_DropperIP;			/// 떨군 사용자 IP
	DWORD		m_dwDropperDBID;		/// db id
	DWORD		m_dwDropperClanID;

/*
	CObjITEM ();
*/
	virtual ~CObjITEM ();

    t_ObjTAG Get_TYPE ()						{	return OBJ_ITEM;	}

	void InitItemOBJ (classUSER *pDropper, tPOINTF &PosCUR, POINTS &PosSEC, tagITEM &sITEM, classUSER *pOwner=NULL, bool bActive=true, CParty *pOwnerPARTY=NULL);
	void SetACTIVE ();

	void Update_OwnerObjIDX ( classUSER *pUSER );
    bool Make_gsv_ADD_OBJECT( classPACKET *pCPacket );

	bool Is_FREE ()								{	return ( 0 == m_iOwnerObjIDX || m_iRemainTIME < ITEM_OBJ_FREE_TIME );	}

	bool Check_PrivateRIGHT(int iTryObjIDX)		{	return iTryObjIDX == m_iOwnerObjIDX;					}
	bool Check_PartyRIGHT( CParty *pTryPARTY )	{	return ( pTryPARTY && pTryPARTY == m_pOwnerPARTY );		}

	int	 Proc (void);
} ;

//-------------------------------------------------------------------------------------------------
#endif
