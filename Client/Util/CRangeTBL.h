/*
	$Header: /Client/Util/CRangeTBL.h 5     03-11-27 3:05p Jeddli $
*/
#ifndef	__CRANGETBL_H
#define	__CRANGETBL_H
//-------------------------------------------------------------------------------------------------

class CRangeTBL
{
private :
	CRangeTBL ();
	~CRangeTBL ();

	static CRangeTBL *m_pInstance;

public  :
	static CRangeTBL* Instance ();
	void Destroy ();

	short	m_nMaxArray;
	short	m_nMaxRange;

	short  *m_pStartIdx;
	short  *m_pIndexCnt;

	char   *m_pTABLE;
	POINTS *m_pPOINTS;

	bool Load_TABLE ( char *szFileName );
	void Free_TABLE (void);

	short GetMaxArray ()				{	return m_nMaxArray;						}
	short GetStartIndex(short nR)		{	return m_pStartIdx[ nR ];				}
	short GetIndexCount(short nR)		{	return m_pIndexCnt[ nR ];				}
	short GetMaxRange ()				{	return m_nMaxRange;						}
	char  GetTABLE (short nX, short nY)	{	return m_pTABLE[ m_nMaxRange*nY+nX ];	}
} ;


//-------------------------------------------------------------------------------------------------
#endif
