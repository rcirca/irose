/*
	$Header: /Client/Util/CMessageQ.cpp 4     03-09-26 1:26p Icarus $
*/
#include "stdAFX.h"
#include "CMessageQ.h"
#include "classSTR.h"

//-------------------------------------------------------------------------------------------------
void CSystemMESSAGE::Add_Message (DWORD dwColor,  char *fmt, ...)
{
	char *pStr;

	pStr = CStr::GetString ();

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(pStr,fmt,argptr);
    va_end(argptr);

	tagMsgITEM *pData = new tagMsgITEM;

	pData->m_Msg.Set( pStr );
	pData->m_dwColor = dwColor;

	this->AddItem (pData, true);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

