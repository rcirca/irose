/**
 * \ingroup SHO_GS
 * \file	CRegenArea.h
 * \brief	몹이 출몰하는 리젠 포인트 관리
 */
#ifndef	__CRegenAREA_H
#define	__CRegenAREA_H
//-------------------------------------------------------------------------------------------------

#ifndef	__SERVER
	class CMAP;
	class CObjCHAR;
#endif


struct tagREGENMOB {
	short	m_iMobIDX;
	short	m_iMobCNT;
} ;


/**
 * \ingroup SHO_GS_LIB
 * \class	CRegenPOINT
 * \author	wookSang.Jo
 * \brief	존파일에서 설정된 몹이 출현 처리 클래스
 */
class CRegenPOINT {
// private:
public :
	#ifndef	__SERVER
	CMAP	   *m_pZONE;
	#else
    CZoneTHREAD*m_pZONE;
	#endif

	DWORD		m_dwCheckTIME;

	int			m_iBasicCNT;
	int			m_iTacticsCNT;

	tagREGENMOB*m_pBasicMOB;
	tagREGENMOB*m_pTacticsMOB;

	int			m_iInterval;
	int			m_iLimitCNT;
	int			m_iTacticsPOINT;	// 전술P 주기

	int			m_iLiveCNT;
	int			m_iCurTactics;		// 현재 전술P
//#ifdef	_DEBUG
//	int			m_iLastCurTactics;
//#endif

#ifdef	__VIRTUAL_SERVER
	classDLLIST< CObjCHAR* >	m_CharLIST;
#endif

public :
	float		m_fXPos, m_fYPos;
	int			m_iRange;

	#ifndef	__SERVER
	CRegenPOINT (CMAP *m_pZONE, float fXPos, float fYPos);
	#else
	CRegenPOINT (CZoneTHREAD *pZONE, float fXPos, float fYPos);
	#endif
	~CRegenPOINT ();

	#ifndef	__SERVER
	void RegenCharacter (int iIndex, int iCount);
	bool Load ( CFileSystem* pFileSystem );	
	#else
	inline void RegenCharacter (int iIndex, int iCount)		{	m_pZONE->RegenCharacter( this, iIndex, iCount );	}
	bool Load ( FILE *fp );
	#endif
	void ClearCharacter (CObjCHAR *pCharOBJ);

	void Proc (DWORD dwCurTIME);

    void AddLiveCNT ()      {   m_iLiveCNT++;       }
	void SubLiveCNT ()		
	{	
		// 강제 초기화할경우 - 값이 될수 있다.
		if ( m_iLiveCNT > 0 )
			m_iLiveCNT--;	    
	}
    int  GetLiveCNT ()      {   return m_iLiveCNT;  }
	void Reset ()
	{
		m_iLiveCNT = 0;
		m_iCurTactics = 1;
	}
} ;


#ifndef	__SERVER
class CRegenAREA {
private:
public :
	classDLLIST< CRegenPOINT* >	m_LIST;

	CRegenAREA ();
	~CRegenAREA ();

	void Proc ();
} ;
#endif

//-------------------------------------------------------------------------------------------------
#endif