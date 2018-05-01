#ifndef	__CSQLTHREAD_H
#define	__CSQLTHREAD_H
#include "classTHREAD.h"
#include "classSYNCOBJ.h"
//#include "classMYSQL.h"
#include "classODBC.h"
#include "classSTR.h"
// #include "DLLIST.h"
#include "CDLList.h"
#include "PacketHEADER.h"
//-------------------------------------------------------------------------------------------------

struct tagQueryDATA {
	int			m_iTAG;
	CStrVAR		m_Name;
	int			m_iDataLEN;
	union {
		BYTE			*m_pDATA;
		t_PACKETHEADER	*m_pPacket;
	} ;
} ;


#define	USE_MY_SQL		0x000
#define	USE_ODBC		0x001

class CSqlTHREAD : public classTHREAD {
protected :
	classSQL		*m_pSQL;

	classEVENT	    *m_pEVENT;
    CCriticalSection m_CS;

	CDLList	< tagQueryDATA >	m_RunPACKET;
	CDLList	< tagQueryDATA >	m_AddPACKET;

	CDLList < char* >			m_RunQUERY;
	CDLList < char* >			m_AddQUERY;

	CDLList< tagQueryDATA >::tagNODE *Del_SqlPACKET (CDLList< tagQueryDATA >::tagNODE *pDelNODE);
	virtual bool Run_SqlPACKET( tagQueryDATA *pQueryDATA )=0;

	virtual bool Proc_QuerySTRING ();

public :
	CSqlTHREAD (bool bCreateSuspended);
	virtual ~CSqlTHREAD ();

//	virtual void Execute() = 0;

	bool Connect (BYTE btSqlTYPE, char *szServerIP, char *szUser, char *szPassword, char *szDBName, short nBindParamCNT=64, WORD wQueryBufferLEN=8192);
	void Free ();

	bool Add_SqlPACKET (int iTAG, char *szName, BYTE *pDATA, int iDataSize);
	bool Add_QueryString (char *szQuery );
} ;

//-------------------------------------------------------------------------------------------------
#endif
