#ifndef	__CRANDOM_H
#define __CRANDOM_H
#include "LIB_Util.h"
//-------------------------------------------------------------------------------------------------

#define	MAX_RANDOM_FUNC		4

#define	CRandom			CR001

#define	AcRANDOM		R_AC
#define	BcRANDOM		R_BC
#define	VcRANDOM		R_VC
#define	MyRANDOM		R_MY

class CR001 {
private:
	DWORD	m_dwVcCallCnt;
	DWORD	m_dwBcCallCnt;
	DWORD	m_dwAcCallCnt;
	DWORD	m_dwMyCallCnt;

	BYTE	m_btType;

	int		m_iVcSeed;
	int		m_iBcSeed;
	int		m_iAcSeed;
	int		m_iMySeed;

//	DECLARE_INSTANCE( CR001 )

public :
	void Init (DWORD dwSeed);
	void SetType (BYTE btRandTYPE);
	int	 Get ();

	int	 R_AC ();
	int  R_BC ();
	int  R_VC ();
	int	 R_MY ();
} ;

//-------------------------------------------------------------------------------------------------
#endif