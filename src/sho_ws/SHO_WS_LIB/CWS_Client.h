#ifndef	__CWS_CLIENT_H
#define	__CWS_CLIENT_H
//-------------------------------------------------------------------------------------------------
#include "CWS_Account.h"

extern CPacketCODEC	*g_pPacketCODEC;

class WS_Zone;
class CWS_Party;
class CWS_Server;
class CMessenger;

#define	RIGHT_MASTER		0x00800			// 마스타..
#define	IS_HACKING(pUSER, fn)		pUSER->IsHacking(fn, __FILE__, __LINE__)

class CWS_Client : public iocpSOCKET
{
private:
	//----------------------- virtual function inherit from iocpSOCKET
    int     m_iRecvSeqNO;

    bool HandlePACKET( t_PACKETHEADER *pPacket );
	WORD  E_SendP (t_PACKETHEADER *pPacket)		{	return g_pPacketCODEC->Encode_SendServerPACKET( pPacket );					}
	WORD  D_RecvH (t_PACKETHEADER *pPacket)		{	return g_pPacketCODEC->Decode_RecvClientHEADER( pPacket, m_iRecvSeqNO );	}
	short D_RecvB (t_PACKETHEADER *pPacket)		{	return g_pPacketCODEC->Decode_RecvClientBODY( pPacket );					}
	WORD  P_Length(t_PACKETHEADER *pPacket)		{	return g_pPacketCODEC->GetDecodedPacketLength( pPacket );					}
	bool IsHacking (char *szDesc, char *szFile, int iLine);

	WORD			m_wGSID;
	BYTE			m_btChannelNO;
//	short			m_nWaitZoneNO;
	short			m_nLevel;

public :
	CStrVAR			m_Name;
	t_HASHKEY		m_HashCHAR;

	CWS_Account	   *m_pAccount;
	t_HASHKEY		m_HashACCOUNT;

	DWORD			m_dwPayFLAG;
	DWORD			m_dwRIGHT;
	DWORD			m_dwDBID;
    DWORD           m_dwMD5Password[ 8 ];

	DWORD			m_dwGuildID;

	int				m_iChatRoomID;
	CDLList<CWS_Client*>::tagNODE *m_pNodeChatROOM;
	CDLList<CWS_Client*>::tagNODE *m_pNodeSockLIST;

	tag_MY_CLAN		m_CLAN;
	CStrVAR			m_ClanNAME;

	short			m_nPlatinumCharCNT;

	void	ClanINIT(DWORD dwClanID=0, int iClanPOS=0)
	{
		m_CLAN.m_btClanLEVEL = 1;
		m_CLAN.m_dwClanID	 = dwClanID;
		m_CLAN.m_dwClanMARK  = 0;
		m_CLAN.m_iClanSCORE  = 0;
		m_CLAN.m_iClanRATE   = 0;
		m_CLAN.m_biClanMONEY = 0;
		m_CLAN.m_nMemberCNT  = 0;
		m_CLAN.m_iClanRATE	 = 0;

		m_CLAN.m_btClanPOS   = (BYTE)iClanPOS;
		m_CLAN.m_iClanCONT   = 0;

		m_ClanNAME.Set( NULL );
	}
	char*	GetClanNAME()				{	return m_ClanNAME.Get();					}
	void	SetClanNAME(char*szName)	{	m_ClanNAME.Set( szName );					}

	DWORD	GetClanID()					{	return m_CLAN.m_dwClanID;					}
	void	SetClanID(DWORD dwClanID)	{	m_CLAN.m_dwClanID = dwClanID;				}

	DWORD	GetClanMARK()				{	return m_CLAN.m_dwClanMARK;					}
	void	SetClanMARK(DWORD dwMark)	{	m_CLAN.m_dwClanMARK = dwMark;				}

	BYTE	GetClanPOS()				{	return m_CLAN.m_btClanPOS;					}
	void	SetClanPOS(int iPos)		{	m_CLAN.m_btClanPOS = (BYTE)iPos;			}

	BYTE	GetClanLEVEL()				{	return m_CLAN.m_btClanLEVEL;				}
	void	SetClanLEVEL(short nLev)	{	m_CLAN.m_btClanLEVEL=(BYTE)nLev;			}
	void	IncClanLEVEL ();

	int		GetClanSCORE()				{	return m_CLAN.m_iClanSCORE;					}
	void	SetClanSCORE(int iScore)	{	m_CLAN.m_iClanSCORE = iScore;				}
	void	AddClanSCORE(int iScore);

	int		GetClanCONTRIBUTE()			{	return m_CLAN.m_iClanCONT;					}
	void	SetClanCONTRIBUTE(int iCon)	{	m_CLAN.m_iClanCONT = iCon;					}
	void	AddClanCONTRIBUTE(int iCon);

	__int64 GetClanMONEY ()				{	return m_CLAN.m_biClanMONEY;				}
	void	SetClanMONEY (__int64 biM)	{	m_CLAN.m_biClanMONEY = biM;					}
	void	AddClanMONEY(int iMoney);

	short	GetClanUserCNT ()			{	return m_CLAN.m_nMemberCNT;					}
	void	SetClanUserCNT (short nC)	{	m_CLAN.m_nMemberCNT = nC;					}

	int		GetClanRATE()				{	return m_CLAN.m_iClanRATE;					}
	void	SetClanRATE(int iR)			{	m_CLAN.m_iClanRATE = iR;					}

	bool	Is_ClanMASTER(void)			{	return this->GetClanPOS() >= GPOS_MASTER;	}

	CWS_Client ();
	~CWS_Client ();

	void Init ()
	{
		m_iRecvSeqNO = 0;

		m_dwGuildID = 0;
		m_iChatRoomID = 0;

		m_dwDBID = 0;
		m_wGSID = 0;

		m_dwPayFLAG = 0;
		m_dwRIGHT = 0;

		m_HashCHAR  = 0;
		m_HashACCOUNT = 0;
		m_pAccount = NULL;

		m_btChannelNO = MAX_CHANNEL_SERVER;
		m_nPlatinumCharCNT = 99;
	}
	void Free ()
	{
		_ASSERT( 0 == this->m_iSocketIDX );
		this->Clear_LIST ();
	}

	bool Recv_cli_JOIN_SERVER_REQ( t_PACKET *pPacket );
	bool Recv_cli_CHAR_LIST( t_PACKET *pPacket );
	bool Recv_cli_SELECT_CHAR( t_PACKET *pPacket );
	bool Recv_cli_CREATE_CHAR( t_PACKET *pPacket );
	bool Recv_cli_DELETE_CHAR( t_PACKET *pPacket );
	bool Recv_cli_JOIN_ZONE ( t_PACKET *pPacket );

	bool Recv_cli_MEMO( t_PACKET *pPacket );
	bool Recv_cli_MESSENGER( t_PACKET *pPacket );
	bool Recv_cli_MESSENGER_CHAT( t_PACKET *pPacket );

	bool Send_wsv_MOVE_SERVER( short nZoneNO );
	bool Send_srv_ERROR( WORD wErrCODE );

	bool Send_HEADER ( WORD wPacketTYPE )
	{
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = wPacketTYPE;
		pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );

		this->Send_Start( pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}

	bool Send_wsv_CHATROOM (BYTE btCMD, WORD wUserID, char *szSTR);

	bool Send_srv_JOIN_SERVER_REPLY (BYTE btResult, DWORD dwRecvSeqNO, DWORD dwPayFlag);

	bool Send_gsv_GM_COMMAND( char *szAccount, BYTE btCMD, WORD wBlockTIME=1);
	bool Send_gsv_WHISPER (char *szFromAccount, char *szMessage);
	bool Recv_cli_WHISPER( t_PACKET* pPacket );
	void Send_wsv_CHAR_CHANGE ();

	bool Send_tag_MCMD_HEADER( BYTE btCMD, char *szStr );
	bool Recv_cli_MCMD_APPEND_REQ( t_PACKET *pPacket );

	bool Recv_mon_SERVER_LIST_REQ( t_PACKET *pPacket );
	bool Recv_mon_SERVER_STATUS_REQ( t_PACKET *pPacket );

	short GuildCMD (char *szCMD);
	bool Recv_cli_CLAN_COMMAND( t_PACKET *pPacket );
	bool Recv_cli_GUILD_CHAT( t_PACKET *pPacket );
	bool Send_wsv_CLAN_COMMAND( BYTE btCMD, ... );

	bool Recv_cli_CLANMARK_SET( t_PACKET *pPacket );
	bool Recv_cli_CLANMARK_REQ( t_PACKET *pPacket );
	bool Recv_cli_CLANMARK_REG_TIME( t_PACKET *pPacket );

	bool Send_wsv_CLANMARK_REPLY( DWORD dwClanID, WORD wMarkCRC, BYTE *pMarkData, short nMarkDataLen );

	char*		Get_ACCOUNT()					{	return m_pAccount->m_Account.Get();		}

	DWORD		Get_LSID()						{	return m_pAccount->m_dwLSID;			}
	DWORD		Get_LoginTIME()					{	return m_pAccount->m_dwLoginTIME;		}

	t_HASHKEY	Get_HashACCOUNT()				{	return m_HashACCOUNT;					}

	char*		Get_NAME()						{	return m_Name.Get();		}

	WORD		Get_WSID()						{	return (WORD)m_iSocketIDX;	}

	DWORD		Get_RIGHT()						{	return m_dwRIGHT;			}
	WORD		Get_GSID()						{	return m_wGSID;				}
	void		Set_GSID(DWORD dwID)			{	m_wGSID = (WORD)dwID;		}
	BYTE		Get_ChannelNO()					{	return m_btChannelNO;		}

	void		Set_RecvSeqNO(DWORD dwSEQ)		{	m_iRecvSeqNO = dwSEQ;		}
	// ------------------ inherited from CMessenger
	DWORD	Get_DBID ()							{	return m_dwDBID;			}
    bool	SendPacket( classPACKET *pCPacket )	{   return this->Send_Start( pCPacket );	}


	CWS_Server *GetSERVER();
} ;



//-------------------------------------------------------------------------------------------------
class CWS_ListCLIENT : public IOCPSocketSERVER, public CDataPOOL< CWS_Client >
{
private :
	CCriticalSection		 m_csHashACCOUNT;
	classHASH< CWS_Account*>*m_pHashACCOUNT;

	CCriticalSection		 m_csHashCHAR;
	classHASH< CWS_Client* >*m_pHashCHAR;

	CCriticalSection		m_CS;
	CDLList< CWS_Client* >	m_SockLIST;

public :
	// Worker Thread 갯수 = CPU * 2 + 2
	CWS_ListCLIENT( UINT uiInitDataCNT, UINT uiIncDataCNT )
								: IOCPSocketSERVER( "CWS_ClientSOCKET", 2, 2, true ), 
								  CDataPOOL< CWS_Client > ("CClientPOOL", uiInitDataCNT, uiIncDataCNT ),
								  m_CS( 4000 )
	{
		m_pHashACCOUNT = new classHASH< CWS_Account* >( 1024 * 3 );
		m_pHashCHAR = new classHASH< CWS_Client* >( 1024 * 3 );
	}
	~CWS_ListCLIENT()
	{
		SAFE_DELETE( m_pHashCHAR );
		SAFE_DELETE( m_pHashACCOUNT );
	}

	int  Get_AccountCNT ()				{	return m_pHashACCOUNT->GetCount();		}

	bool Add_ACCOUNT (int iSocketIDX, t_PACKET *pRecvPket, char *szAccount);
	BYTE Del_ACCOUNT (char *szAccount, BYTE btDelLoginBIT, CWS_Server *pClosingServer=NULL);
	CWS_Client* Find_ACCOUNT (char *szAccount);
	bool Check_DupACCOUNT (char *szAccount, t_HASHKEY HashKEY);

	bool Add_CHAR (CWS_Client *pCLIENT, char *szCharName);
	void Del_CHAR (CWS_Client *pCLIENT);
	CWS_Client* Find_CHAR (char *szAvatar);

	bool SendPacketToSocketIDX (int iClientSocketIDX, classPACKET *pCPacket);
	void Send_wsv_CREATE_CHAR (int iSocketIDX, BYTE btResult, BYTE btIsPlatinumCHAR=0);
	void Send_wsv_MEMO (int iSocketIDX, BYTE btTYPE, short nRecvCNT=-1);
	void Send_wls_ACCOUNT_LIST ();

	void Check_SocketALIVE ();

	//-------------------------------------------------------------------------------------------------
	/// Inherited from IOCPSocketSERVER
	iocpSOCKET*	AllocClientSOCKET()
	{
		// 메모리할당
		CWS_Client *pSOCKET = this->Pool_Alloc ();
		if ( pSOCKET ) {
			pSOCKET->Init ();

			this->m_CS.Lock ();
			pSOCKET->m_pNodeSockLIST = m_SockLIST.AllocNAppend( pSOCKET );
			this->m_CS.Unlock ();
		}
		return (iocpSOCKET*)pSOCKET;
	}
	void FreeClientSOCKET( iocpSOCKET *pSOCKET )
	{
		//LogString (LOG_DEBUG, ">>> Delete CLIENT socket : %s, %s UsedCnt: %d \n", 
		//				pSOCKET->m_IP.Get(), 
		//				this->GetPoolNAME(), 
		//				this->GetUsedCNT()-1 );
		CWS_Client *pClient = (CWS_Client*)pSOCKET;

		this->m_CS.Lock ();
		if ( pClient->m_pNodeSockLIST )
			m_SockLIST.DeleteNFree( pClient->m_pNodeSockLIST );
		this->m_CS.Unlock ();

		// 검증없이 메모리 해제
		pClient->Free ();
		this->Pool_Free( pClient );
	}
	void ClosedClientSOCKET( iocpSOCKET *pSOCKET );
} ;
extern CWS_ListCLIENT   *g_pUserLIST;


//-------------------------------------------------------------------------------------------------
#endif