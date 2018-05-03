#ifndef	__CLASSSYS_H
#define	__CLASSSYS_H
#include <pdh.h>
#include <pdhmsg.h>
//-------------------------------------------------------------------------------------------------

class CPDH {
private:
	HQUERY					m_hQuery;
	HCOUNTER				m_hCounter;
 	PDH_STATUS				m_pdhStatus;
	PDH_FMT_COUNTERVALUE	m_fmtValue;
	
public :
	CPDH ();
	~CPDH ();
	bool Init( int iCpuOrder );
	long GetValue ();
} ;

//-------------------------------------------------------------------------------------------------

typedef	bool (__stdcall *CALLBACK_FINDPROCESS) ( DWORD dwPID, char *szFileName );

class CSystem {
private:
	// SYSTEM_INFO	 m_SystemInfo;
	//CSystem ();
	//~CSystem ();
	//static CSystem *m_pInstance;

public :
//	static	CSystem *Instance ();
	static DWORD	ShowLastError( LPSTR lpszFunction );

	static DWORD	GetOSPlatfrom ();
	static bool		IsNTPlatfrom ();

	static DWORD	GetCPUCount ();

	static DWORD	GetProcessMemory (DWORD dwPID);
} ;

//-------------------------------------------------------------------------------------------------
#endif