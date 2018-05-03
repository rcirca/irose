#ifndef	__CPARTYLIST_H
#define	__CPARTYLIST_H
#include "CDLList.h"
#include "classSyncOBJ.h"
#include "classINDEX.h"

#include "CWS_Client.h"
//-------------------------------------------------------------------------------------------------

#define	MAX_PARTY_BUFF	8162

struct tagWSPartyUSER {
	DWORD		m_dwDisconnectTIME;
	int			m_iDBCharID;
	CWS_Client *m_pClient;
} ;


class CWSParty {
public :
	WORD	m_wPartyWSID;

	char	m_cMemberCNT;
	BYTE	m_btPartyLEV;
	int		m_iPartyEXP;

	CCriticalSection		m_ListCS;
	CDLList< tagPartyUSER >	m_ListUSER;
} ;


class CWSPartyBUFF : public CIndexARRAY< CWSParty* > {
private:
	CCriticalSection	 m_csBUFF;
	CChatROOM			*m_pBUFFs;

public :
	CWSPartyBUFF( WORD wMaxPartyCNT=MAX_PARTY_BUFF );
	~CWSPartyBUFF ();
} ;

//-------------------------------------------------------------------------------------------------
#endif