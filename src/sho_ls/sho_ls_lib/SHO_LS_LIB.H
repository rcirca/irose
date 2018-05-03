#ifndef	__SHO_LS_LIB_H
#define	__SHO_LS_LIB_H
#include "CLS_API.h"
#include "classTIME.h"
//-------------------------------------------------------------------------------------------------


class SHO_LS
{
private :
	int		m_iClientListenPortNO;
	int		m_iServerListenPortNO;
	bool	m_bShowOnlyWS;

	SHO_LS (EXE_LS_API *pExeAPI);
	~SHO_LS ();
	void SystemINIT( HINSTANCE hInstance );

	static SHO_LS	*m_pInstance;
	EXE_LS_API *m_pExeAPI;

	CTimer *m_pTIMER;

public :
    union {
        BYTE    m_btMD5[ 33 ];
	    DWORD	m_dwMD5[ 8 ];
    } ;

	bool StartClientSOCKET (int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32]);
	void CloseClientSOCKET ();
	bool StartServerSOCKET (HWND hMainWND, char *szDBServerIP, int iServerListenPort, DWORD dwLoginRight, char *szGumsIP, int iGumsPORT, bool bShowOnlyWS);
	void Shutdown ();

	void Send_ANNOUNCE( void *pServer, char *szAnnounceMsg );

	void SetLoginRIGHT( DWORD dwLoginRight );
	void SetLimitUserCNT( int iLimitUserCNT );

	//---------------------------------------------------------------------------------------------
	static bool    IsShowOnlyWS()				{	return m_pInstance->m_bShowOnlyWS;	}
	static EXE_LS_API *ExeAPI()					{	return m_pInstance->m_pExeAPI;		}
	static SHO_LS *GetInstance ()				{	return m_pInstance;					}
	static SHO_LS *InitInstance( HINSTANCE hInstance, EXE_LS_API *pExeAPI )
	{
		if ( NULL == m_pInstance ) {
			// m_pInstance = (class SHO_WS *)1;
			m_pInstance = new SHO_LS( pExeAPI );
			_ASSERT( m_pInstance );
			m_pInstance->SystemINIT( hInstance );
		}
		return m_pInstance;
	}
	void Destroy ()							{	SAFE_DELETE( m_pInstance );	}
} ;


extern DWORD GetServerBuildNO ();
extern DWORD GetServerStartTIME ();

//-------------------------------------------------------------------------------------------------
#endif