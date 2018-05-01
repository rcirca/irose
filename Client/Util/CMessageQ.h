/*
	$Header: /Client/Util/CMessageQ.h 4     03-09-26 1:26p Icarus $
*/
#ifndef	__CMESSAGEQ_H
#define	__CMESSAGEQ_H
#include "classQUEUE.h"
//-------------------------------------------------------------------------------------------------

struct tagMsgITEM {
	CStrVAR	m_Msg;
	DWORD	m_dwColor;
} ;

class CSystemMESSAGE : public classQUEUE< tagMsgITEM* >
{
private:
public :
	void Add_Message (DWORD dwColor,  char *fmt, ...);
} ;

extern CSystemMESSAGE	g_SysMSG;

//-------------------------------------------------------------------------------------------------
#endif