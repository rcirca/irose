/**
 * \ingroup SHO_GS
 *
 * \file	IO_AI.h
 *
 * \brief	전체 인공지능 데이타 보관및 처리
 *
 * \todo 
 *
 * \bug 
 *
 * \warning
 *
 */
#ifndef	__IO_AI_H
#define	__IO_AI_H
#include "..\AI_LIB\CAI_LIB.H"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	CAI_LIST
 * \author	wookSang.Jo
 * \brief	전체 인공지능 데이타 보관및 처리 클래스
 */
class CAI_LIST 
{
private:
	CAI_FILE  **m_ppAI;
	short		m_nAICNT;

public :
	CAI_LIST ();
	~CAI_LIST ();

	bool Load (char *szBaseDIR, char *szSTBFile, char *szLangSTB, int iLangCol);

	void Free (void);

	void AI_Created		(int iAI_IDX, CObjCHAR *pSourCHAR);
	void AI_Stop		(int iAI_IDX, CObjCHAR *pSourCHAR);
	void AI_AttackMove	(int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR);
	void AI_Damaged		(int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage);
	void AI_Kill		(int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage);
	void AI_Dead		(int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage);
};

extern CAI_LIST		 g_AI_LIST;

//-------------------------------------------------------------------------------------------------
#endif