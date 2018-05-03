#ifndef	__IO_AI_H
#define	__IO_AI_H
#include "..\AI_LIB\CAI_LIB.H"
//-------------------------------------------------------------------------------------------------

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