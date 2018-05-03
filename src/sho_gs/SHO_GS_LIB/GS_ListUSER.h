/**
 * \ingroup SHO_GS
 * \file	GS_ListUSER.h
 * \brief	서버에 접속된 사용자 관리
 */
#ifndef GS_ListUSERH
#define GS_ListUSERH

#include "classHASH.h"
#include "classSYNCOBJ.h"
#include "GS_USER.h"
#include "CDataPOOL.h"
#include "iocpSocketSERVER.h"


//---------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	CUserLIST
 * \author	wookSang.Jo
 * \brief	접속되어 있는 전체 사용자를 관리하는 클래스
 */
class CUserLIST : public IOCPSocketSERVER, public CDataPOOL< classUSER >
{
public:
	CCriticalSection		 m_csHashACCOUNT;
	CCriticalSection		 m_csHashCHAR;
	CCriticalSection		 m_csNullZONE;	// m_csSOCKET;

	classHASH< classUSER *>	*m_pHashACCOUNT;
	classHASH< classUSER *>	*m_pHashCHAR;

    classDLLIST< CGameOBJ* > m_NullZoneLIST;	// 존이 없이 떠도는 유저~
	int						 m_iUserCNT;

	void InitData (classUSER *pData);
	void FreeData (classUSER *pData);

	t_HASHKEY*				 m_HashLevelUpTRIGGER;

public :
	CUserLIST( UINT uiInitDataCNT, UINT uiIncDataCNT );
	~CUserLIST();

    void DeleteUSER (classUSER *pUSER, BYTE btLogOutMODE /* =LOGOUT_MODE_LEFT */);

	bool SendPacketToSocketIDX (int iClientSocketIDX, classPACKET *pCPacket);

	void Send_wsv_CREATE_CHAR (int iSocketIDX, BYTE btResult);
	void Send_wsv_MEMO (int iSocketIDX, BYTE btTYPE, short nMemoCNT=-1);
	void Send_wsv_GUILD_COMMAND (int iSocketIDX, BYTE btResult, char *szStr=NULL);

	classUSER* Find_CHAR (char *szCharName);
	bool Add_CHAR (classUSER *pUSER);

	classUSER* Find_ACCOUNT (char *szAccount);
	bool Add_ACCOUNT (int iSocketIDX, t_PACKET *pRecvPket, char *szAccount);

	classUSER * Find_IP( char * szIP );

	void Send_zws_ACCOUNT_LIST (CClientSOCKET *pSrvSocket, bool bSendIP);

	void Send_cli_STRESS_TEST ( classPACKET *pCPacket );
	void Save_AllUSER ();

	int  Get_AccountCNT()		{	return m_pHashACCOUNT->GetCount();	}
	bool Kick_ACCOUNT( char *szAccount );

	t_HASHKEY Get_LevelUpTRIGGER( short nLevel ) 
	{
		return 	m_HashLevelUpTRIGGER[ nLevel ];
	}

	void Add_NullZONE( classDLLNODE< CGameOBJ* > *pUserNODE )
	{
		m_csNullZONE.Lock ();
			m_NullZoneLIST.AppendNode( pUserNODE );
		m_csNullZONE.Unlock ();
	}
	void Sub_NullZONE( classDLLNODE< CGameOBJ* > *pUserNODE ) 
	{
		m_csNullZONE.Lock ();
			m_NullZoneLIST.DeleteNode( pUserNODE );
		m_csNullZONE.Unlock ();
	}
	void Check_SocketALIVE ();

	/// Inherited from IOCPSocketSERVER
	iocpSOCKET*	AllocClientSOCKET()
	{
		// 메모리할당
		classUSER *pCLIENT = this->Pool_Alloc ();
		if ( pCLIENT )
			this->InitData( pCLIENT );
		return pCLIENT;
	}
	void FreeClientSOCKET( iocpSOCKET *pCLIENT )
	{
		// 검증없이 메모리 해제
		this->FreeData( (classUSER*)pCLIENT );
		this->Pool_Free( (classUSER*)pCLIENT );
	}
	void ClosedClientSOCKET( iocpSOCKET *pCLIENT )
	{
		// 소켓이 삭제됐다.. 알아서 메모리 해제할것...
		if ( 0 == ( (classUSER*)pCLIENT )->Get_INDEX() ) {
			// 존에 들어가 있지 않거나 존에 들어가기 위해 대기중인 상태 바로 삭제 등록...
			this->DeleteUSER( (classUSER*)pCLIENT, LOGOUT_MODE_NET_ERROR );
		} 
		/*
		else
			// 존에 들어 있으면 존에서 삭제후 직접 this->DeleteUSER 호출...
		*/
	}
} ;

extern CUserLIST *g_pUserLIST;

void Save_AllUSER ();

//---------------------------------------------------------------------------
#endif
