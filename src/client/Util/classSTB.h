/*
	$Header: /7HeartsOnline/Client/Util/classSTB.h 4     04-09-13 12:32a Icarus $
*/
#ifndef	__CLASSSTB_H
#define	__CLASSSTB_H
#include <stdio.h>
#include "classSTR.h"
//-------------------------------------------------------------------------------------------------
#ifndef	MAX_PATH
#define	MAX_PATH	260
#endif

// #define	__MAPPING_FPOS2MEM

class classSTB 
{
private :
	FILE   *m_fp;
	long	m_lFP;
	
	#ifdef	__MAPPING_FPOS2MEM
		struct tagSTB {
			long	m_lFilePOS;
			short	m_nStrLen;
		} *m_pDataOFF;
	#else
		long   *m_pOffset;
		int		m_iRowIndex;
		int		m_iColIndex;
	#endif
	
	CStrVAR*m_RowNAME;
	char	m_szValue[ 4096 ];
	

public  :
	int   m_iRowCount;
	int	  m_iColCount;

	classSTB();
	~classSTB();				

	bool  Open  (char *szFileName, long lFilePtr=-1);
	void  Close ();

	char		*GetNAME (int iRow)	{	return m_RowNAME[ iRow ].Get();		}
	unsigned int GetHASH (int iRow)	{	return m_RowNAME[ iRow ].GetHASH();	}
		
	int   GetRowCount()			{	return m_iRowCount;				}
	int   GetColumnCount()		{	return m_iColCount;				}

#ifndef	__MAPPING_FPOS2MEM
	bool  SetIndexPosition (int iCol, int iRow);
	int   GetInteger();
	char *GetString();
#endif

	int	  GetInteger(int iCol, int iRow);
	char *GetString(int iCol, int iRow);
} ;

//-------------------------------------------------------------------------------------------------
#endif