#ifndef	__IO_MOTION_H
#define	__IO_MOTION_H
#include "..\Util\CFileLIST.h"
typedef unsigned int HNODE;
//-------------------------------------------------------------------------------------------------
struct tagMOTION {
	HNODE	m_hMotion;

	WORD	m_wFPS;
	WORD	m_wTotalFrame;	
	short  *m_pFrameEvent;
	short	m_nActionIdx;

	short	m_nActionPointCNT;
	WORD	m_wTatalAttackFrame;
	
#ifdef	__SERVER
	short  *m_pActionPoint;
#else
	int	m_iInterpolationInterval;
#endif

	tagMOTION ();
	~tagMOTION ()		{	SAFE_DELETE( m_pFrameEvent );	}

	bool	LoadZMO (char *szFileName);

	// dwPassTIME동안 진행될 프레임수... dwPassTIME == 1000이면 1초 !!
	WORD	Get_TotalFRAME ()								{	return	m_wTotalFrame;									}

	WORD	Get_ReaminFRAME(WORD wCurFrame)					{	return (m_wTotalFrame-wCurFrame);						}

	WORD	Get_PassFRAME(DWORD dwPassTIME, float fRatio)	{	return (WORD) ( (fRatio*m_wFPS*dwPassTIME) / 1000.f );	}

	// wFrame동안 소용될 시간...
	DWORD	Get_NeedTIME (WORD wFrame)						{	return (DWORD)( (1000 * wFrame) / m_wFPS );				}
	DWORD	Get_NeedTIME (WORD wFrame, float fRatio)		{	return (DWORD)( (1000 * wFrame) / (fRatio*m_wFPS) );	}
} ;

/// motion list
class CMotionLIST : public CFileLIST<tagMOTION*> {
private :
	tagMOTION *m_pTmpMotion;

	short	m_nFemaleIndex;

	bool	Load_FILE (tagFileDATA<tagMOTION*> *pData);
	void	Free_FILE (tagFileDATA<tagMOTION*> *pData);

public  :
#ifdef	__SERVER
	CMotionLIST () : CFileLIST<tagMOTION*>("ANI ", 2048)	{	;	}
#else
	CMotionLIST () : CFileLIST<tagMOTION*>("ANI ")			{	;	}
#endif
	~CMotionLIST ();

	bool Load (char *szSTBFile, short nFileNameColNO=0, char *szBaseDIR=NULL);
	void Free ();

	tagMOTION*KEY_GetMOTION(unsigned int uiKEY)		{	return this->Get_DATAUseKEY( uiKEY );		}
	tagMOTION*IDX_GetMOTION(short nIndex, bool bIsFemale);
	HNODE	  KEY_GetZMOTION(unsigned int uiKEY);
} ;
extern CMotionLIST g_MotionFILE;

//-------------------------------------------------------------------------------------------------
#endif