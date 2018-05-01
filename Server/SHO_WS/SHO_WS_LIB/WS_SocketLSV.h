#ifndef	__WS_SOCKETLSV_H
#define	__WS_SOCKETLSV_H
#include "classSYNCOBJ.h"
#include "classTIME.h"
//-------------------------------------------------------------------------------------------------

class WSLSV_Socket : public CClientSOCKET {
public :
	bool WndPROC ( WPARAM wParam, LPARAM lParam );
} ;

class WS_lsvSOCKET	// : private CCriticalSection
{
public :
	WSLSV_Socket	 m_SockLSV;

private:
	CTimer			*m_pReconnectTimer;
	bool			 m_bTryCONN;

	t_PACKET	*m_pRecvPket;

	HWND		m_hMainWND;
	UINT		m_uiSocketMSG;
	CStrVAR		m_LoginServerIP;
	int			m_LoginServerPORT;

	bool		m_bActiveMODE;

public :
	WS_lsvSOCKET ();
	~WS_lsvSOCKET ();

	void Init (HWND hWND, char *szLoginServerIP, int iLoginServerPort, UINT uiSocketMSG, bool bAutoActive);

	bool Connect ();
	void Disconnect ();

	void Send_zws_SERVER_INFO ();

	void Send_srv_ACTIVE_MODE (bool bActive);

	void Send_zws_CONFIRM_ACCOUNT_REQ (DWORD dwSocketID, t_PACKET *pRecvPacket);
	void Recv_wls_CONFIRM_ACCOUNT_REPLY ();

	void Send_zws_SUB_ACCOUNT (DWORD dwLSID, char *szAccount);

	void Recv_lsv_CHECK_ALIVE ();
	void Recv_lsv_ANNOUNCE_CHAT ();

	void Recv_wls_KICK_ACCOUNT ();

	void Send_wls_CHANNEL_LIST ();
	void Send_wls_ACCOUNT_LIST ();

	bool Proc_SocketMSG (WPARAM wParam, LPARAM lParam);
} ;
extern WS_lsvSOCKET	*g_pSockLSV;

//-------------------------------------------------------------------------------------------------
#endif
