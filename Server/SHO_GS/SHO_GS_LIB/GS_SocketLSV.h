/**
 * \ingroup SHO_GS
 * \file	GS_SocketLSV.h
 * \brief	월스 서버와 통신 하는 소켓 클래스
 */
#ifndef	__GS_SOCKETLSV_H
#define	__GS_SOCKETLSV_H
#include "classSYNCOBJ.h"
#include "classTIME.h"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	GSLSV_Socket
 * \author	wookSang.Jo
 * \brief	로즈 월드서버 sho_ws와 통신및 처리하는 클래스
 */
class GSLSV_Socket : public CClientSOCKET {
public :
	bool WndPROC ( WPARAM wParam, LPARAM lParam );
} ;

class GS_lsvSOCKET	// : private CCriticalSection
{
public :
	GSLSV_Socket	 m_SockLSV;
	
private:
	CTimer			*m_pReconnectTimer;
	bool			 m_bTryCONN;

	t_PACKET	*m_pRecvPket;
//	t_PACKET	*m_pSendPket;

	HWND		m_hMainWND;
	UINT		m_uiSocketMSG;
	CStrVAR		m_LoginServerIP;
	int			m_LoginServerPORT;
	CStrVAR		m_TmpSTR;

public :
	GS_lsvSOCKET ();
	~GS_lsvSOCKET ();

	void Init (HWND hWND, char *szLoginServerIP, int iLoginServerPort, UINT uiSocketMSG);

	bool Connect ();
	void Disconnect ();

	void Send_zws_CONFIRM_ACCOUNT_REQ (DWORD dwSocketID, t_PACKET *pPacket);
	void Recv_wls_CONFIRM_ACCOUNT_REPLY ();

	void Send_zws_SERVER_INFO ();

	void Send_zws_SUB_ACCOUNT (DWORD dwLSID, char *szAccount);

	void Recv_lsv_CHECK_ALIVE ();
	void Recv_lsv_ANNOUNCE_CHAT ();

	void Recv_wls_KICK_ACCOUNT ();

	void Send_srv_SET_WORLD_VAR (short nVarIDX, short nValue);
	void Recv_srv_SET_WORLD_VAR ();

	bool Send_gsv_CHANGE_CHAR( classUSER *pUSER );

	void Send_gsv_CHEAT_REQ( classUSER *pUSER, DWORD dwReqWSID, DWORD dwReplyWSID, char *szCheatCode);		// 월드 서버에 치트코드 요청
	void Recv_wsv_CHEAT_REQ ();

	void Send_srv_ACTIVE_MODE (bool bActive);
	void Send_srv_USER_LIMIT (DWORD dwLimit);

	void Send_wls_CHANNEL_LIST ();

	bool Send_zws_CREATE_CLAN( DWORD dwWSID, t_HASHKEY HashCHAR );
	bool Send_gsv_ADJ_CLAN_VAR( t_PACKET *pPacket );
/*
	void Send_gsv_ADD_ZONE_LIST ();
	bool Send_gsv_WARP_USER( classUSER *pUSER );

	void Send_gsv_LEVEL_UP( BYTE btOP, WORD wWSID, short nLevel, int iExp);
	void Recv_wsv_PARTY_CMD ();
*/
	bool Proc_SocketMSG (WPARAM wParam, LPARAM lParam);
} ;
extern GS_lsvSOCKET	*g_pSockLSV;

//-------------------------------------------------------------------------------------------------
#endif
