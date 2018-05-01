#ifndef	__BS_TWAIN_H
#define	__BS_TWAIN_H
//-------------------------------------------------------------------------------------------------
#include "classTHREAD.h"
#include "classSYNCOBJ.h"
#include "../../TwainSDK/SoftStarLoginClient_Interface.h"

//#define	__TW_REAL_LOGIN
#define	TB_CONNECTION_BROKEN	5		// Login Server연결 중단 (Connection State Result)
#define	TB_CONNECTION_FAILED	6		// Login Server연결 실패 (Connection State Result)
#define	TB_CONNECTED			31		// Game Server등록 성공 (Connection State result)

class CBS_Taiwan
#ifndef	__SHO_LS_LIB
				: public classTHREAD, public CCriticalSection
#endif
{
protected:
	HINSTANCE	m_hDLL;
	timeval		m_TimeVal;
	int			m_iServerID;
	bool		m_bTestLS;

	ISoftstar_Login_Client *m_pLoginClient;

	ISoftstar_Login_Client*  (*CreateSoftstarLoginClient)(char *strIPAddr, int iPort, S_CALLBACK *sCallback);
	void   (*DestroySoftstarLoginClient)(ISoftstar_Login_Client* client);

	CBS_Taiwan (char *szServerIP, int iPortNo, int iServerID);
	~CBS_Taiwan ();

#ifndef	__SHO_LS_LIB
	enum e_BSReqType {
		BST_PRELOGIN	= 0,
		BST_LOGIN,
		BST_LOGOUT,
		BST_RELOGIN
	} ;
	struct tagBSItem {
		e_BSReqType		m_eType;
		CStrVAR			m_PlayerID;
		CStrVAR			m_Password;
		unsigned int	m_uiIP;
		unsigned int	m_uiUserTag;
	} ;
	CSLList< tagBSItem >	m_RecvQ;
	CSLList< tagBSItem >	m_WorkQ;
    classEVENT *m_pEvent;
	void Execute(void);
#endif

	static CBS_Taiwan *m_pInstance;
public :
	int		m_iLastConnectResult;
	int		m_iConnectResult;		// -1:openConnection호출후 응답대기 상태, -2:openConnection호출 해야 한다.

	static CBS_Taiwan *InitInstance (char *szServerIP, int iPortNo, int iServerID)
	{
		if ( NULL == m_pInstance ) {
			m_pInstance = new CBS_Taiwan(szServerIP,iPortNo,iServerID);
			if ( iServerID >= 0 && NULL == m_pInstance->m_hDLL ) {
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}
		return m_pInstance;
	}
	static CBS_Taiwan *Instance()		{	return m_pInstance;		}
	static void Destroy () 
	{
		if ( m_pInstance ) {
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static bool IsTestLS ()
	{
		if ( m_pInstance ) {
			return m_pInstance->m_bTestLS;
		}
		return false;
	}
	static void ConnectionState(int iResult);
	static void PreloginResult(char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uPreloginResult, int iLoginState,int right);
	static void LoginResult   (char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uLoginResult, int iLoginState);
	static void ReloginResult(char *strPlayerID,unsigned short u16ChargeType, double dPoint, unsigned int uReloginResult, int iLoginState);
	static void LogoutResult(char *strPlayerID, unsigned int uIdentity, unsigned int uLogoutResult);
	static void BillingMessage(char *strPlayerID, char *strBillingMessage, unsigned int uIdentity);
	static void EnforceLogout(int iPlayerNo, char *strPlayerID);

	bool CBS_Taiwan::PreloginToServer (char *szPlayerID, char *szPassword, char *szIP, unsigned int uiUserTAG)
	{
#ifdef	__SHO_LS_LIB
		if ( m_pInstance ) {
			unsigned int uiIPNum = ::inet_addr( szIP );
			return m_pLoginClient->preloginToServer( szPlayerID, szPassword, uiIPNum, uiUserTAG );
		}
#endif
		return true;
	}
	bool CBS_Taiwan::LoginToServer (char *szPlayerID, char *szPassword, char *szIP, unsigned int uiUserTAG)
	{
#ifndef	__SHO_LS_LIB
		CSLList< tagBSItem >::tagNODE *pNewNode;

		pNewNode = new CSLList< tagBSItem >::tagNODE;
		pNewNode->m_VALUE.m_eType = BST_LOGIN;
		pNewNode->m_VALUE.m_PlayerID.Set( szPlayerID );
		pNewNode->m_VALUE.m_Password.Set( szPassword );
		pNewNode->m_VALUE.m_uiIP = ::inet_addr( szIP );
		pNewNode->m_VALUE.m_uiUserTag = uiUserTAG;

		this->Lock ();
			this->m_RecvQ.AppendNode( pNewNode );
		this->Unlock ();
#endif
		//if ( m_pInstance ) {
		//	unsigned int uiIPNum = ::inet_addr( szIP );
		//	return m_pLoginClient->loginToServer( szPlayerID, szPassword, uiIPNum, uiUserTAG );
		//}
		return true;
	}
	bool CBS_Taiwan::ReloginToServer (char *szPlayerID, char *szIP)
	{
#ifndef	__SHO_LS_LIB
		CSLList< tagBSItem >::tagNODE *pNewNode;

		pNewNode = new CSLList< tagBSItem >::tagNODE;
		pNewNode->m_VALUE.m_eType = BST_RELOGIN;
		pNewNode->m_VALUE.m_PlayerID.Set( szPlayerID );
		pNewNode->m_VALUE.m_uiIP = ::inet_addr( szIP );

		this->Lock ();
			this->m_RecvQ.AppendNode( pNewNode );
		this->Unlock ();
#endif
		//if ( m_pInstance ) {
		//	unsigned int uiIPNum = ::inet_addr( szIP );
		//	return m_pLoginClient->reloginToServer( szPlayerID, uiIPNum );
		//}
		return true;
	}
	bool CBS_Taiwan::LogoutFromServer (char *szPlayerID, unsigned int uiUserTAG)
	{
#ifndef	__SHO_LS_LIB
		CSLList< tagBSItem >::tagNODE *pNewNode;

		pNewNode = new CSLList< tagBSItem >::tagNODE;
		pNewNode->m_VALUE.m_eType = BST_LOGOUT;
		pNewNode->m_VALUE.m_PlayerID.Set( szPlayerID );
		pNewNode->m_VALUE.m_uiUserTag = uiUserTAG;

		this->Lock ();
			this->m_RecvQ.AppendNode( pNewNode );
		this->Unlock ();
#endif
		//if ( m_pInstance ) {
		//	return m_pLoginClient->logoutToServer( szPlayerID, uiUserTAG );
		//}
		return true;
	} 
	bool CBS_Taiwan::FlushNetwork ()
	{
		if ( m_pInstance ) {
			if ( -2 == m_iConnectResult ) {
				m_pLoginClient->openConnection( m_iServerID );
				m_iConnectResult = -1;
			}

			return m_pLoginClient->flushNetwork( &m_TimeVal );
		}
		return true;
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif