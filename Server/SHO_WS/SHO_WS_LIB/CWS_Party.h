#ifndef	__CWS_PARTY_H
#define	__CWS_PARTY_H
/*
#include "CDLList.h"
#include "classSyncOBJ.h"
#include "classINDEX.h"

//-------------------------------------------------------------------------------------------------

class CWS_Client;

struct tagWSPartyUSER {
	DWORD		m_dwDisconnectTIME;
	int			m_iDBCharID;
	WORD		m_wWSID;
	CWS_Client *m_pClient;
	CStrVAR		m_Name;
} ;


class CWS_Party : public CCriticalSection {
private:
	void Del( CDLList< tagWSPartyUSER >::tagNODE *pNode);

public :
	CDLList< tagWSPartyUSER >	m_listUSER;

	WORD	m_wPartyWSID;

	short	m_nMemberCNT;
	BYTE	m_btPartyLEV;
	int		m_iPartyEXP;

	int		m_iAverageLEV;		// 평균 파티원 레벨
	int		m_iTotalLEV;		// 모든 파티원 레벨 합

	CWS_Party () : CCriticalSection( 4000 )		{	;	}

	void InitPARTY ()
	{
		m_nMemberCNT = 1;
		m_iPartyEXP = 0;
		m_btPartyLEV = 1;

		m_iTotalLEV = 0;
	}

	void Destroy ();

	bool IsOWNER (CWS_Client *pClient);

	BYTE GetPartyLEV()					{	return	this->m_btPartyLEV;		}
	int	 GetAverageLEV()				{	return	this->m_iAverageLEV;	}

	void Send_wsv_PARTY_CMD( BYTE btOP, CWS_Client *pUSER, CWS_Client *pMember=NULL );

	bool SendToPartyMembers( classPACKET *pCPacket );

	bool SendPartyLEVEL (CWS_Client *pUSER);
	void Set_LEVnEXP (short nLevel, int iEXP);

	void Change_UserLEVEL (CWS_Client *pClient, short nNewLEV);
	bool Change_PartyOWNER (CWS_Client *pNewOWNER);

	BYTE Add_PartyUSER (CWS_Client *pUSER);
	bool Sub_PartyUSER (CWS_Client *pUSER);

	void AddNODE( CWS_Client *pClient);
	void DelNODE (CWS_Client *pClient);

	void Connect (CWS_Client *pClient);
	void Disconnect (CWS_Client *pClient);
} ;


class CWSPartyBUFF : public CIndexARRAY< CWS_Party* > {
private:
	WORD				 m_wMaxPartyCNT;
	CCriticalSection	 m_csPartyBUFF;
	CWS_Party			*m_pPartyBUFF;

public :
	CWSPartyBUFF( WORD wMaxPartyCNT=MAX_PARTY_BUFF );
	~CWSPartyBUFF ();

	WORD GetMaxPartyCNT()			{	return m_wMaxPartyCNT;	}

	void Delete ( WORD wPartyID );

	inline CWS_Party *Get_PARTY( WORD wPartyWSID )
	{
		return &m_pPartyBUFF[ wPartyWSID ];
	}

	int		CreatePARTY ( CWS_Client *pOwner, CWS_Client *pMember );
	void	DeletePARTY ( CWS_Party *pCParty );
} ;
extern CWSPartyBUFF	*g_pPartyBUFF;
*/
//-------------------------------------------------------------------------------------------------
#endif