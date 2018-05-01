/**
 * \ingroup SHO_GS
 * \file	CCharData.h
 * \brief	mob/npc 케릭터 파일을 읽어 보관
 */
#ifndef	__CCharDATA_H
#define	__CCharDATA_H
#include "classSTR.h"
#include "IO_Motion.h"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	CCharDATA
 * \author	wookSang.Jo
 * \brief	npc/mob의 기본 데이타를 포함하고있는 클래스
 */
class CCharDATA
{
private:
#ifdef	_DEBUG
	CStrVAR		 m_Name;
#endif
	short		 m_nAniCNT;
	tagMOTION  **m_ppAniFILE;	// [ MAX_MOB_ANI ];
public :
	CCharDATA ();
	~CCharDATA ();

	bool Load_MOBorNPC (FILE *fp, t_HASHKEY *pAniKEY, short nAniCNT);
	bool IsValid ()				{	return NULL != m_ppAniFILE;		}
	tagMOTION *GetMOTION (BYTE btIndex)			
	{	
		if( btIndex >= m_nAniCNT ) {
			g_LOG.CS_ODS( 0xffff, "OutofRange Ani index : %d\n", btIndex);
			return NULL;
		}

		return m_ppAniFILE[ btIndex ];
	}
} ;


/**
 * \ingroup SHO_GS_LIB
 * \class	CCharDatLIST
 * \author	wookSang.Jo
 * \brief	list_npc.stb의 전체 npc/mob의 데이타를 관리하는 클래스
 */
class CCharDatLIST 
{
private:
	short		m_nModelCNT;
	CCharDATA  *m_pMODELS;

public :
	CCharDatLIST ();
	~CCharDatLIST ();

	bool Load_MOBorNPC (char *szBaseDIR, char *szFileName);
	void Free ();

	CCharDATA *GetMODEL( short nIndex ) 
	{
		if ( nIndex < 1 || nIndex >= m_nModelCNT )
			return NULL;
		return &m_pMODELS[ nIndex ];
	}

	DECLARE_INSTANCE( CCharDatLIST )
} ;
extern CCharDatLIST	*g_pCharDATA;

//-------------------------------------------------------------------------------------------------
#endif